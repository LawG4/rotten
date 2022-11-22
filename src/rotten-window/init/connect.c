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

// Introduce the wayland specific code path for connection. remember that we're currently holding an internal
// static library and giving each window a pointer to the library. TLDR this sucks
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
rotten_success_code connect_test_wayland(rotten_window_connection* connection) { return e_rotten_success; }
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

#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    // Attempt to connect to wayland display server. We check wayland first because wayland servers can run
    // xorg on wayland, and we want to avoid using a weird translation layer.
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