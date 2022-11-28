#include "../generated/xdg-shell-client-protocol.h"
#include "rotten-wayland.h"

static struct xdg_surface *rotten_wl_wm_base_get_xdg_surface(rotten_library_wayland *way,
                                                             struct xdg_wm_base *xdg_wm_base,
                                                             struct wl_surface *surface)
{
    return (struct xdg_surface *)way->proxy_marshal_flags(
      (struct wl_proxy *)xdg_wm_base, XDG_WM_BASE_GET_XDG_SURFACE, way->xdg_surface_interface,
      way->proxy_get_version((struct wl_proxy *)xdg_wm_base), 0, NULL, surface);
}

static struct xdg_toplevel *rotten_wl_surface_get_toplevel(rotten_library_wayland *way,
                                                           struct xdg_surface *surface)
{
    return (struct xdg_toplevel *)way->proxy_marshal_flags(
      (struct wl_proxy *)surface, XDG_SURFACE_GET_TOPLEVEL, way->xdg_toplevel_interface,
      way->proxy_get_version((struct wl_proxy *)surface), 0, NULL);
}

static void rotten_wl_toplevel_set_title(rotten_library_wayland *way, struct xdg_toplevel *xdg_toplevel,
                                         const char *title)
{
    way->proxy_marshal_flags((struct wl_proxy *)xdg_toplevel, XDG_TOPLEVEL_SET_TITLE, NULL,
                             way->proxy_get_version((struct wl_proxy *)xdg_toplevel), 0, title);
}

void rotten_wl_xdg_atach_dispatch_function_pointers(rotten_library_wayland *way)
{
    // Attaches the xdg function pointers to the wayland library handle
    way->xdg_wm_base_get_xdg_surface = &rotten_wl_wm_base_get_xdg_surface;
    way->xdg_surface_get_toplevel = &rotten_wl_surface_get_toplevel;
    way->xdg_toplevel_set_title = &rotten_wl_toplevel_set_title;
}