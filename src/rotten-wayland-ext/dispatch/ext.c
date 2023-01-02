/************************************************************************************************************
 * @file Rotten wayland ext loader
 * @brief Loads all of the function pointers for this library into a dispatch table
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/
#include "../rotten-wayland-ext-internal.h"

void rotten_wl_ext_dispatch_load(rotten_library_wayland_ext* lib)
{
    //
    // XDG
    //

    // window manager base function pointers
    lib->xdg_wm_base_interface = &xdg_wm_base_interface;
    lib->xdg_wm_base_add_listener = xdg_wm_base_add_listener;
    lib->xdg_wm_base_get_xdg_surface = xdg_wm_base_get_xdg_surface;
    // surface
    lib->xdg_surface_interface = &xdg_surface_interface;
    lib->xdg_surface_get_toplevel = xdg_surface_get_toplevel;
    // Top level
    lib->xdg_toplevel_interface = &xdg_toplevel_interface;
    lib->xdg_toplevel_set_title = xdg_toplevel_set_title;
    // Listeners
    rotten_wl_xdg_attach_listener_pointers(lib);

    //
    // ZXDG
    //

    // Decoration manager
    lib->zxdg_decoration_manager_interface = &zxdg_decoration_manager_v1_interface;
    lib->zxdg_decoration_manager_get_toplevel_decoration = zxdg_decoration_manager_v1_get_toplevel_decoration;
    // Top level decorationer
}