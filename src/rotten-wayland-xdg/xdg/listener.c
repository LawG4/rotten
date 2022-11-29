#include "../generated/xdg-shell-client-protocol.h"
#include "rotten-wayland.h"

// Exposing the listener struct has multiple members it's not an opaque pointer or incomplete struct class so
// we can't pass the xdg listner struct between libraries. Instead we have a static listner which will do all
// you need plus a function to attach this default listener to the window

//
// WM PING PONG
// Basically a heart beat to tell the window manager we haven't locked up
//

// Define the function used to send the reply
static void rotten_wl_xdg_wm_base_pong(rotten_library_wayland *way, struct xdg_wm_base *xdg_wm_base,
                                       uint32_t serial)
{
    way->proxy_marshal_flags((struct wl_proxy *)xdg_wm_base, XDG_WM_BASE_PONG, NULL,
                             way->proxy_get_version((struct wl_proxy *)xdg_wm_base), 0, serial);
}

static int rotten_wl_xdg_wm_base_add_listener(rotten_library_wayland *way, struct xdg_wm_base *xdg_wm_base,
                                              const struct xdg_wm_base_listener *listener, void *data)
{
    return way->proxy_add_listener((struct wl_proxy *)xdg_wm_base, (void (**)(void))listener, data);
}

// Function which actually sends the reply - Relies on the window being given as the data pointer
static void s_xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    rotten_window_wayland *window = (rotten_window_wayland *)data;
    window->way->xdg_wm_base_pong(window->way, xdg_wm_base, serial);
}

// Struct to contain the ping pong.
static const struct xdg_wm_base_listener s_xdg_wm_base_listener = {
  .ping = s_xdg_wm_base_ping,
};

// Finally a function which attaches this static listener to the window
void rotten_wl_xdg_attach_listener_pointers(rotten_library_wayland *lib)
{
    lib->xdg_wm_base_pong = &rotten_wl_xdg_wm_base_pong;
    lib->xdg_wm_base_listener = &s_xdg_wm_base_listener;

    lib->xdg_wm_base_add_listener = &rotten_wl_xdg_wm_base_add_listener;
}