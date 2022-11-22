/************************************************************************************************************
 * @file Rotten window connect
 * @brief Connects the application to the host Os' windowing system
 * @note Public functions defined : rotten_window_connect
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/

#include "../rotten-window-internal.h"

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB

// Keep an internal reference to the xcb library which will be dynamically loaded at runtime
// TODO: This is not going to be threadsafe - and I think there must be a better way to do this, I could give
// each window it's own copy of the library and it's function pointers, but that seems excessive. For now I
// will give each window a pointer to this static library
static rotten_library_xcb s_xcb = {.xcb_lib = NULL, .connect = NULL, .create_window = NULL};

// Actually attempt to connect to the library
rotten_success_code connect_test_xcb(rotten_window_connection* connection)
{
    // Is the native library handle null? If so then attempt to open it
    if (s_xcb.xcb_lib == NULL) {
        rotten_success_code err = rotten_library_xcb_load_min(&s_xcb);
        if (err != e_rotten_success) return err;
    }

    // We got here so the xcb native library is openable, so now lets check if we can use it to connect to the
    // server, we have already ensured the minimum functions have been loaded above
    rotten_success_code err = rotten_library_xcb_valid_session(&s_xcb);
    if (err != e_rotten_success) {
        return err;
    }

    // Now load the rest of the functions
    if (s_xcb.create_window == NULL) {
        err = rotten_library_xcb_load_full(&s_xcb);
        if (err != e_rotten_success) return err;
    }

    rotten_log("Successfully opened xcb connection", e_rotten_log_info);
    return e_rotten_success;
}

#endif  //! XCB
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
static wayland_dispatch s_wl = {
  .wayland_handle = NULL,  // linux native shared lib handle
  .display_connect = NULL  //
};
#endif  //! Wayland

#endif  //! linux

rotten_success_code rotten_window_connect(rotten_window_connection* connection)
{
    // This section is about setting the window backend flag. so we have to clear it first
    connection->supported_window_backends = 0;
    connection->selected_backend = e_rotten_window_none;
    connection->backend_handle = NULL;

#ifdef __linux__

    // Connect to the linux os screen server.
    rotten_log_debug("Rotten window connecting to linux", e_rotten_log_info);

// Attempt to connect to wayland display server
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    if (connect_test_wayland(connection) == e_rotten_success) {
        connection->supported_window_backends |= e_rotten_window_wayland;

        // by default prefer wayland
        connection->selected_backend = e_rotten_window_wayland;
        connection->backend_handle = &s_wl;
    }
#endif  // ! Wayland connect

// Attempt to connect to XCB display server
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    if (connect_test_xcb(connection) == e_rotten_success) {
        connection->supported_window_backends |= e_rotten_window_xcb;

        // Set the default to backend to XCB only if wayland hasn't already been selected
        if (connection->selected_backend == e_rotten_window_none) {
            connection->selected_backend = e_rotten_window_xcb;
            connection->backend_handle = &s_xcb;
        }
    }
#endif  // ! XCB connect

#else
    rotten_log_debug("Rotten window is not implemented for this os atm", e_rotten_log_error);
    return e_rotten_unimplemented;
#endif  // ! End of OS specific

    // Check that at least one of the window connections was enabled
    if (connection->supported_window_backends == 0 || connection->backend_handle == NULL ||
        connection->selected_backend == e_rotten_window_none) {
        rotten_log("No supported window backend were found", e_rotten_log_error);
        return e_rotten_library_not_present;
    }

    // Got this far so everything was all good.
    return e_rotten_success;
}

//
// Implementation specific functions
//

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
rotten_success_code connect_test_wayland(rotten_window_connection* connection)
{
    // Has the client library on this computer been opened yet?
    if (s_wl.wayland_handle == NULL) {
        rotten_log_debug("Attempting to open libwayland-client.so", e_rotten_log_verbose);
        void* wayland_handle = dlopen("libwayland-client.so", RTLD_LAZY);

        // Store the wayland handle or exit the wayland process.
        if (wayland_handle == NULL) {
            rotten_log("Failed to open libwayland-client.so", e_rotten_log_warning);
            return e_rotten_library_not_present;
        }
        rotten_log("Successfully opened libwayland-client.so", e_rotten_log_info);
        s_wl.wayland_handle = wayland_handle;
    }

    // We know that the native linux library is opened, check if  we have loaded the first function pointer
    // needed from the dynamic library which is used to connect to the wayland compositor
    if (s_wl.display_connect == NULL) {
        // Try and fetch the connection function from the dynamic library
        s_wl.display_connect = dlsym(s_wl.wayland_handle, "wl_display_connect");
        if (s_wl.display_connect == NULL) {
            rotten_log_debug(
              "Somehow opened wayland library but couldn't find core library function pointer. How did this "
              "even happen?",
              e_rotten_log_error);

            // This should almost never happen unless the user is making their own wayland implementation. For
            // now assume wayland doesn't work, so close the library and reset the state, maybe we can try
            // again later?
            dlclose(s_wl.wayland_handle);
            s_wl.wayland_handle = NULL;
            s_wl.display_connect = NULL;

            return e_rotten_unclassified_error;
        }
    }

    // We now know that we have a valid function pointer into the wayland client library which can be used to
    // make a connection to the compositor. The connection to the compositor should remain constant over the
    // runtime of the application. Maybe if the user swaps wayland client mid runtime? but that's pretty
    // rediculous
    //
    // TODO: Check assumption that compositor connection is constant
    if (s_wl.display == NULL) {
        // The host computer might have the library present, however it might not be the active session. So we
        // have to attempt to make a connection, now I think if there's X session this should return NULL
        //
        // TODO : Check this assumption!!!!!
        rotten_log_debug("Attempting to connect to wayland compositor", e_rotten_log_verbose);
        s_wl.display = s_wl.display_connect(NULL);

        if (s_wl.display == NULL) {
            rotten_log("Failed to connect to wayland compositor", e_rotten_log_warning);
            return e_rotten_library_not_present;
        }
        rotten_log("Connected to wayland compositor", e_rotten_log_info);

        // Now that we know the user has connected to the wayland compositor, it's finally worth us building
        // up the rest of the function pointer table used by wayland
        rotten_log_debug("Fetching function pointers from wayland client library", e_rotten_log_verbose);
        s_wl.display_disconnect = dlsym(s_wl.wayland_handle, "wl_display_disconnect");
    }

    return e_rotten_success;
}
#endif  // ! Wayland

#ifndef ROTTEN_WINDOW_EXCLUDE_XCB

#endif  // ! XCB
#endif  // ! __linux__