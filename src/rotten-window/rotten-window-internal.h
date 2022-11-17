#include "rotten-window.h"
#ifndef __ROTTEN_INTERNAL_WINDOW_SHARED_H__
#define __ROTTEN_INTERNAL_WINDOW_SHARED_H__ (1)

#ifdef __linux__

// On linux there are multiple common different display servers. And the user might not want to build for all
// of them. So we give them the ability to exclude each one individually
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
#include <wayland-client-core.h>

// Dynamic funciton dispatch table for wayland
typedef struct wayland_dispatch {
    void* wayland_handle;        // Handle to linux shared library
    struct wl_display* display;  // Connection to the wayland compositer

    // Function pointers for wayland
    struct wl_display* (*display_connect)(const char*);
    void (*display_disconnect)(struct wl_display*);
} wayland_dispatch;

/***
 * @brief Attempts to connect to a wayland server and load all of the function pointers from the hosts
 * wayland-client library
 * @param window Pointer to the window to hande the
 * @returns success code, success only if wayland is the chosen window backend
 */
rotten_success_code connect_test_wayland(rotten_window_connection* connection);
#endif

#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
#include <xcb/xcb.h>
#include <xcb/xcbext.h>

typedef struct xcb_dispatch {
    void* libary_handle;
    xcb_connection_t* connection_t;

    // Function pointers
    xcb_connection_t* (*connect)(const char* display_name, int* p_screen_num);
} xcb_dispatch;

rotten_success_code connect_test_xcb(rotten_window_connection* connection);
#endif

// For allowing the user to dynamically open the display server libraries
#include <dlfcn.h>  // Requires linking with libdl
#endif

#endif