#include "rotten-window.h"
#ifndef __ROTTEN_INTERNAL_WINDOW_SHARED_H__
#define __ROTTEN_INTERNAL_WINDOW_SHARED_H__ (1)

//
// The internal window always has the first member being the backend type, that way the pointer can be casted
// into a baseline struct, and then that can be used to select the correct function path ways
//
typedef struct rotten_window_base {
    rotten_window_backend backend;
    uint8_t remain_open;
    uint32_t width;
    uint32_t height;
    char* title;
} rotten_window_base;

//
// Per OS configuration for the header
//

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
#include "rotten-xcb.h"

typedef struct rotten_window_xcb {
    rotten_window_base base;        // Default information
    rotten_window_xcb_extra extra;  // XCB specific information
    rotten_library_xcb* xcb;        // handle to xcb library functions
} rotten_window_xcb;
#endif

// For allowing the user to dynamically open the display server libraries
#include <dlfcn.h>  // Requires linking with libdl
#endif

#endif