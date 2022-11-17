#include "../rotten-window-internal.h"

//
// Define the static handles for the different windowing systems
// Ideally, I'd like these to stay internal. ensure to set the handles to nullpointers
//
#ifdef __linux__

#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
static wayland_dispatch s_wl = {
  .wayland_handle = NULL,  // linux native shared lib handle
  .display_connect = NULL  //
};
#endif  //! Wayland

#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
static xcb_dispatch s_xcb = {
  .libary_handle = NULL,  // linux native shared lib handle
  .connect = NULL         //
};
#endif  //! XCB

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
rotten_success_code connect_test_xcb(rotten_window_connection* connection)
{
    // Same as above try to open the dynamic library for xcb on the host machine
    if (s_xcb.libary_handle == NULL) {
        rotten_log_debug("Attempting to open libxcb.so", e_rotten_log_verbose);
        s_xcb.libary_handle = dlopen("libxcb.so", RTLD_LAZY);
        if (s_xcb.libary_handle == NULL) {
            rotten_log("Failed to open libxcb.so", e_rotten_log_warning);
            return e_rotten_library_not_present;
        }

        rotten_log("Successfully opened libxcb.so", e_rotten_log_info);
    }

    // xcb library handle is opened, find the function pointer to connect to the screen
    if (s_xcb.connect == NULL) {
        s_xcb.connect = dlsym(s_xcb.libary_handle, "xcb_connect");

        // This should never happen unless the user is making their own xcb implementation or something?
        if (s_xcb.connect == NULL) {
            rotten_log_debug(
              "Somehow couldn't find the function pointer for xcb_connect despite openeing xcb library. How "
              "did this happen??",
              e_rotten_log_error);

            return e_rotten_unclassified_error;
        }
    }

    // Now check if the connection can be made using this function pointer
    if (s_xcb.connection_t == NULL) {
        rotten_log_debug("Attempting to connect to xcb display", e_rotten_log_verbose);
        s_xcb.connection_t = s_xcb.connect(NULL, NULL);

        if (s_xcb.connection_t == NULL) {
            rotten_log("Failed to connect to xcb display", e_rotten_log_warning);
            return e_rotten_library_not_present;
        }

        // The xcb library is valid, and we can connect to an xcb server. It is now worth collecting the
        // function pointers and getting the details about the root window
        s_xcb.get_setup = dlsym(s_xcb.libary_handle, "xcb_get_setup");
        s_xcb.setup_roots_iterator = dlsym(s_xcb.libary_handle, "xcb_setup_roots_iterator");
        s_xcb.generate_id = dlsym(s_xcb.libary_handle, "xcb_generate_id");
        s_xcb.create_window = dlsym(s_xcb.libary_handle, "xcb_create_window");
        s_xcb.map_window = dlsym(s_xcb.libary_handle, "xcb_map_window");
        s_xcb.flush = dlsym(s_xcb.libary_handle, "xcb_flush");

        // Get the information about the screen from the root node by iterating over all of the root windows
        // and just selecting the first one
        s_xcb.screen_t = s_xcb.setup_roots_iterator(s_xcb.get_setup(s_xcb.connection_t)).data;
    }

    // We got here, so we know that we can make a valid xcb connection which is nice! I'm not sure, but I
    // think this should remain valid for the runtime of the program? What happens if the session changes? or
    // a monitor gets disconnected? I have no idea? I'm gonna assume this stays valid
    //
    // TODO: Check this assumption
    rotten_log("Successfully opened xcb connection", e_rotten_log_info);
    return e_rotten_success;
}
#endif  // ! XCB
#endif  // ! __linux__