#include <string.h>
#include "rotten-wayland.h"

// Wayland has global handles for certain resources like the compositor etc. These resources can be created
// and and destroyed by the compositor, the server will notify us when this happens, but we first have to
// register the funcition pointers to handle these events. These are the static functions below, when we
// register the function pointers with the server we can also give it a user data pointer, this way we can
// avoid using shared static data and instead recieve a pointer to the rotten-window as a param to these
// functons.
static void s_notify_registry(void* data, struct wl_registry* registry, uint32_t id, const char* interface,
                              uint32_t version)
{
    // Cast the user data pointer into our own window
    rotten_window_wayland* window = (rotten_window_wayland*)data;

    // Create the compositor proxy
    if (!strcmp(interface, "wl_compositor")) {
        window->extra.compositor = rotten_wl_registry_bind(window->way, window->extra.registry, id,
                                                           window->way->compositor_interface, 1);
        return;
    } else if (!strcmp(interface, "xdg_wm_base")) {
        window->extra.wm_base = rotten_wl_registry_bind(window->way, window->extra.registry, id,
                                                        window->way->fetch_wm_base_interface(), 1);
        return;
    }
}

static void s_notify_registry_remove(void* data, struct wl_registry* registry, uint32_t id)
{
    // left blank because what am i supposed to do with this -.-
}

static const struct wl_registry_listener s_way_listener = {.global = s_notify_registry,
                                                           .global_remove = s_notify_registry_remove};

rotten_success_code rotten_wl_attach_interface_listeners(rotten_window_wayland* window)
{
    // Since this registry proxy is unique to the window, attach the static function pointers called to to
    // notify us, give it the pointer to the window itself, the statatic function pointers above can then
    // recieve that window handle as a parameter
    int err = rotten_wl_registry_add_listener(window->way, window->extra.registry, &s_way_listener, window);
    if (err != 0) {
        return e_rotten_unclassified_error;
    }

    return e_rotten_success;
}