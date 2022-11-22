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
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
#include "rotten-xcb.h"

typedef struct rotten_window_xcb {
    rotten_window_base base;        // Default information
    rotten_window_xcb_extra extra;  // XCB specific information
    rotten_library_xcb* xcb;        // handle to xcb library functions
} rotten_window_xcb;
#endif  // !xcb

#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
#include "rotten-wayland.h"

typedef struct rotten_window_wayland {
    rotten_window_base base;            // Default information
    rotten_window_wayland_extra extra;  // Wayland specific information for the window
    rotten_library_wayland way;         // handle to the wayland library functions
} rotten_window_wayland;
#endif  //! wayland
#endif  // !Linux

#endif