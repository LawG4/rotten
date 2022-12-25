#include "../generated/xdg-shell-client-protocol.h"
#include "rotten-wayland.h"

void rotten_wl_xdg_attach_interface_pointers(rotten_library_wayland_ext* lib)
{
    lib->xdg_wm_base_interface = &xdg_wm_base_interface;
    lib->xdg_surface_interface = &xdg_surface_interface;
    lib->xdg_toplevel_interface = &xdg_toplevel_interface;
}