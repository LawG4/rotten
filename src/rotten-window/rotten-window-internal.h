#include "rotten-window.h"
#ifndef __ROTTEN_INTERNAL_WINDOW_SHARED_H__
#define __ROTTEN_INTERNAL_WINDOW_SHARED_H__ (1)

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

// TODO: Look into thread saftey of embedding these handles instead of giving the user a pointer
extern rotten_library_wayland g_wl;
extern rotten_library_wayland_ext g_wlext;

#endif  //! wayland
#endif  // !Linux

#endif