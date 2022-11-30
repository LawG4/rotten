#include "../generated/xdg-shell-client-protocol.h"
#include "rotten-wayland.h"

void rotten_wl_xdg_atach_dispatch_function_pointers(rotten_library_wayland* lib);
void rotten_wl_xdg_attach_interface_pointers(rotten_library_wayland* lib);
void rotten_wl_xdg_attach_listener_pointers(rotten_library_wayland* lib);

void rotten_wl_ext_fill_struct(rotten_library_wayland* lib)
{
    rotten_wl_xdg_attach_interface_pointers(lib);
    rotten_wl_xdg_atach_dispatch_function_pointers(lib);
    rotten_wl_xdg_attach_listener_pointers(lib);
}