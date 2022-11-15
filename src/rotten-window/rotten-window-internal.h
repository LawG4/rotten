#include "rotten-window.h"
#ifndef __ROTTEN_INTERNAL_WINDOW_SHARED_H__
#define __ROTTEN_INTERNAL_WINDOW_SHARED_H__ (1)

#ifdef __linux__

// On linux there are multiple common different display servers. And the user might not want to build for all
// of them. So we give them the ability to exclude each one individually
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
#include <xcb/xcb.h>
#include <xcb/xcbext.h>
#endif

#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
#include <wayland-client-core.h>
#endif

// For allowing the user to dynamically open the display server libraries
#include <dlfcn.h>  // Requires linking with libdl
#endif

#endif