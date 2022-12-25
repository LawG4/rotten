#ifndef __ROTTEN_WAYLAND_EXT_INTERNAL_H__
#define __ROTTEN_WAYLAND_EXT_INTERNAL_H__ (1)

#include "rotten-wayland-ext.h"
#include "rotten-wayland.h"
#include "rotten-window.h"

#include "generated/xdg-decoration-protocol.h"
#include "generated/xdg-shell-client-protocol.h"

/**
 * @brief Attaches all of the function pointers used
 */
void rotten_wl_xdg_attach_listener_pointers(rotten_library_wayland_ext *lib);

#endif