#include "../rotten-window-internal.h"

rotten_success_code rotten_window_connect(rotten_window* window)
{
    // This section is about setting the window backend flag. so we have to clear it first
    window->supported_window_backends = 0;

#ifdef __linux__

    // Connect to the linux os screen server.
    rotten_log_debug("Rotten window connecting to linux", e_rotten_log_info);

// Attempt to connect to wayland display server
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    if (connect_wayland(window) == e_rotten_success)
        window->supported_window_backends |= e_rotten_window_wayland;

#endif  // ! Wayland connect

// Attempt to connect to XCB display server
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    rotten_log("Attempting to connect to XCB server", e_rotten_log_info);

#endif  // ! XCB connect

#else
    rotten_log_debug("Rotten window is not implemented for this os atm", e_rotten_log_error);
    return e_rotten_unimplemented;
#endif

    // Check that at least one of the window connections was enabled
    if (window->supported_window_backends == 0) {
        rotten_log("No supported window backend were found", e_rotten_log_error);
        return e_rotten_library_not_present;
    }

    return e_rotten_success;
}

//
// Implementation specific functions
//

// Linux
#ifdef __linux__

#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND

static wayland_dispatch s_wl = {.wayland_handle = NULL, .display_connect = NULL};

rotten_success_code connect_wayland(rotten_window* window)
{
    rotten_log_debug("Attempting to open wayland connection", e_rotten_log_info);

    // Has the client library on this computer been opened yet?
    if (s_wl.wayland_handle == NULL) {
        rotten_log_debug("Attempting to open wayland-client library", e_rotten_log_info);
        void* wayland_handle = dlopen("libwayland-client.so", RTLD_LAZY);

        // Store the wayland handle or exit the wayland process.
        if (wayland_handle == NULL) {
            rotten_log("Failed to open libwayland-client.so", e_rotten_log_warning);
            return e_rotten_library_not_present;
        }
        rotten_log("libwayland-client.so opened successfully", e_rotten_log_info);
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
        rotten_log_debug("Attempting to connect to wayland compositor", e_rotten_log_info);
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