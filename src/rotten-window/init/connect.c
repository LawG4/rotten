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
    if (err != e_rotten_success) return err;

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

rotten_library_wayland g_wl = {.lib = NULL};
rotten_library_wayland_ext g_wlext = {.lib = NULL};

rotten_success_code connect_test_wayland(rotten_window_connection* connection)
{
    // If there is no wayland client library present then we have no way to use wayland.
    if (g_wl.lib == NULL) {
        g_wl.lib = rotten_dynamic_library_open("libwayland-client.so");
        if (g_wl.lib == NULL) {
            rotten_log("Failed to open libwayland-client.so", e_rotten_log_warning);
            return e_rotten_library_not_present;
        }

        rotten_log("Opened libwayland-client.so", e_rotten_log_info);
    }

    // The next library we rely on is the dynamic rotten-wayland-ext, now although you can do wayland without
    // this helper library, the core protocol doesn't go much beyond a blank square you can memcpy pixel data
    // to, we want more features and so need ext, so we'll drop out of wayland support if there is no
    // waylandext
    if (g_wlext.lib == NULL) {
        // TODO: This is a local path, if launching from a different working directory, you're fucked
        g_wlext.lib = rotten_dynamic_library_open("./librotten-wayland-ext.so");
        if (g_wlext.lib == NULL) {
            rotten_log("Failed to load librotten-wayland-ext.so", e_rotten_log_warning);
            return e_rotten_library_not_present;
        }

        rotten_log("Opened librotten-wayland-ext.so", e_rotten_log_info);

        // Fetch the loading helper functions from the extension library
        g_wl.lib_load = rotten_dynamic_library_fetch(g_wlext.lib, "rotten_wl_core_dispatch_load");
        g_wlext.lib_load = rotten_dynamic_library_fetch(g_wlext.lib, "rotten_wl_ext_dispatch_load");

        if (g_wl.lib_load == NULL || g_wlext.lib_load == NULL) {
            rotten_log("Failed to fetch required function pointers from wayland libraries",
                       e_rotten_log_warning);
            return e_rotten_feature_not_present;
        }

        // Use these function pointers to fill their own dispatch tables
        g_wl.lib_load(&g_wl);
        g_wlext.lib_load(&g_wlext);
    }

    // Use those dynamically loaded functions to test if it's possible for us to connect to the display
    struct wl_display* temp_display = g_wl.display_connect(NULL);
    if (temp_display == NULL) {
        rotten_log("Failed to connect to a wayland display", e_rotten_log_warning);
        return e_rotten_feature_not_present;
    }

    g_wl.display_disconnect(temp_display);
    rotten_log("Successfully connected to wayland display", e_rotten_log_info);
    return e_rotten_success;
}
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
        // connection->backend_handle = &s_way;
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
    if (connection->supported_window_backends == 0 || connection->selected_backend == e_rotten_window_none) {
        rotten_log("No supported window backend were found", e_rotten_log_error);
        return e_rotten_library_not_present;
    }

    // Got this far so everything was all good.
    return e_rotten_success;
}