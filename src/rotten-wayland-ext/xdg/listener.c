#include "../generated/xdg-shell-client-protocol.h"
#include "rotten-wayland-ext.h"
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

//
// XDG Surface configuration listener, your roll is to tell the window manager that we exist
// and that we acknowledge any configuration requests
//

// When events happen to the surface, the window manager calls this function. Relies on rotten window being
// the handle being parsed via the data pointer for context. For now just aknowledge the fact we got a
// configure request. This tells the window manager that we exist
static void s_xdg_surface_listener_configure(void *data, struct xdg_surface *surface, uint32_t serial)
{
    // Inform the server that we've recieved their configuring request
    xdg_surface_ack_configure(surface, serial);
    rotten_window_wayland *way = data;

    // Create a buffer which is large enough to hold the current screen.
    // For now we create a pool which is exactly the same size as this one buffer, get a buffer handle from
    // that pool and then create that buffer
    //
    // TODO : We can do this better, we don't need to remake a pool every time
    size_t line_stride = way->base.width * 4;  // The format we select uses 4 bytes per pixel
    size_t buff_size = way->base.height * line_stride;

    rotten_wl_shm_pool temp_pool = rotten_wl_shm_create_pool(way->extra.shared_mem, buff_size);
    struct wl_buffer *temp_buff = wl_shm_pool_create_buffer(
      temp_pool.pool, 0, way->base.width, way->base.height, line_stride, WL_SHM_FORMAT_XRGB8888);
    rotten_wl_shm_destroy_pool(&temp_pool);

    // TODO: We're leaving this temparary buffer handle dangling, the way to avoid this is to attach a buffer
    // listener, which the compositor will run when it's done with the buffer

    // Attach this buffer to the surface  and flush changes out to hardware.
    wl_surface_attach(way->extra.surface, temp_buff, 0, 0);
    wl_surface_commit(way->extra.surface);
}

static struct xdg_surface_listener s_xdg_surface_listener = {.configure = &s_xdg_surface_listener_configure};

//
// XDG top level listeners so that we can respond to changes requested to the top level
//

static void s_xdg_toplevel_handle_configure(void *data, struct xdg_toplevel *xdg_toplevel, int32_t w,
                                            int32_t h, struct wl_array *states)
{
    // no window geometry event, ignore
    if (w == 0 && h == 0) return;

    rotten_window_wayland *window = (rotten_window_wayland *)data;

    // window resized
    if (w != window->base.width || h != window->base.height) {
        window->base.width = w;
        window->base.height = h;

        wl_surface_commit(window->extra.surface);
    }
}

static void s_xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
    //  Left blank
    ((rotten_window_wayland *)data)->base.remain_open = 0;
}

struct xdg_toplevel_listener s_xdg_toplevel_listener = {
  .configure = &s_xdg_toplevel_handle_configure,
  .close = &s_xdg_toplevel_handle_close,
};

//
// Expose these functions to a wayland library handle
//

// Finally a function which attaches this static listener to the window
void rotten_wl_xdg_attach_listener_pointers(rotten_library_wayland *lib)
{
    // For listening to wm_base stuff
    lib->xdg_wm_base_pong = &rotten_wl_xdg_wm_base_pong;
    lib->xdg_wm_base_listener = &s_xdg_wm_base_listener;
    lib->xdg_wm_base_add_listener = &rotten_wl_xdg_wm_base_add_listener;

    // For listening to surface configurations
    lib->xdg_surface_listener = &s_xdg_surface_listener;
    lib->xdg_surface_add_listener = &xdg_surface_add_listener;

    // For top level listening
    lib->xdg_toplevel_listener = &s_xdg_toplevel_listener;
    lib->xdg_toplevel_add_listener = &xdg_toplevel_add_listener;
}