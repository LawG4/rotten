/************************************************************************************************************
 * @file Rotten wayland ext loader
 * @brief Loads all of the function pointers for this library into a dispatch table
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/
#include "../rotten-wayland-ext-internal.h"

void rotten_wl_ext_dispatch_load(rotten_library_wayland_ext* lib)
{
    // XDG
    // Attaches the xdg function pointers to the wayland library handle
    lib->xdg_wm_base_get_xdg_surface = xdg_wm_base_get_xdg_surface;
    lib->xdg_surface_get_toplevel = xdg_surface_get_toplevel;
    lib->xdg_toplevel_set_title = xdg_toplevel_set_title;
}