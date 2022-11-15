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
    rotten_log("Attempting to open wayland connection", e_rotten_log_info);

    // First we have to connect to the dynamic library, we do a lazy open of the library to detect if it even
    // exists and if not we move onto the next display server
    void* lazy_wayland = dlopen("libwayland-client.so", RTLD_LAZY);
    if (lazy_wayland != NULL) {
        // Now that we know the library exists on the machine, try to find the symbol to connect to the
        // display server. This should hopefully always exists!
        struct wl_display* (*display_connect)(const char*) = dlsym(lazy_wayland, "wl_display_connect");
        void (*display_disconnect)(struct wl_display*) = dlsym(lazy_wayland, "wl_display_disconnect");

        if (display_connect != NULL && display_disconnect != NULL) {
            // We've retrieved a valid funtion pointer to connect to a display, so attempt to connect, but
            // what if the user has a propper wayland install, they just happen to be running an x session at
            // the moment? I think this command should then return a null pointer
            //
            // TODO : Check this assumption
            struct wl_display* display = display_connect(NULL);
            if (display != NULL) {
                // Valid display set the feature flag, and disconnect
                window->supported_window_backends |= e_rotten_window_wayland;
                display_disconnect(display);
                rotten_log("Found an active running wayland server", e_rotten_log_info);

            } else {
                rotten_log(
                  "Was able to open wayland client library, but failed to connect to a wayland server. Are "
                  "you sure you're not running X?",
                  e_rotten_log_warning);
            }

        } else {
            rotten_log_debug(
              "Somehow managed to open wayland library on host but couldn't find core function "
              "wl_display_connect",
              e_rotten_log_error);
        }

        // Finished with the temporary lazy loaded wayland library, so close it
        dlclose(lazy_wayland);

    } else {
        rotten_log("Wayland library not detected on system", e_rotten_log_warning);
    }

#endif  // ! Wayland connect

#else
    rotten_log_debug("Rotten window is not implemented for this os atm", e_rotten_log_error);
    return e_rotten_unimplemented;
#endif
    return e_rotten_success;
}
