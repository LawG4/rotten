#include <string.h>
#include "rotten-core.h"
#include "rotten-wayland.h"

//
// Checkout wayland-client-protocol for details
//

struct wl_registry* rotten_wl_display_get_registry(rotten_library_wayland* lib, struct wl_display* display)
{
    struct wl_proxy* registry =
      lib->proxy_marshal_flags((struct wl_proxy*)display, WL_DISPLAY_GET_REGISTRY, lib->registry_interface,
                               lib->proxy_get_version((struct wl_proxy*)display), 0, NULL);

    return (struct wl_registry*)registry;
}

int rotten_wl_registry_add_listener(rotten_library_wayland* lib, struct wl_registry* registry,
                                    const struct wl_registry_listener* listener, void* data)
{
    // Real quick, what the hell is that awfull void function pointer class
    return lib->proxy_add_listener((struct wl_proxy*)registry, (void (**)(void))listener, data);
}

void* rotten_wl_registry_bind(rotten_library_wayland* lib, struct wl_registry* registry, uint32_t id,
                              const struct wl_interface* interface, uint32_t version)
{
    return (void*)lib->proxy_marshal_flags((struct wl_proxy*)registry, WL_REGISTRY_BIND, interface, version,
                                           0, id, interface->name, version, NULL);
}

struct wl_surface* rotten_wl_compositor_create_surface(rotten_library_wayland* lib,
                                                       struct wl_compositor* compositor)
{
    struct wl_proxy* id;

    id = lib->proxy_marshal_flags((struct wl_proxy*)compositor, WL_COMPOSITOR_CREATE_SURFACE,
                                  lib->surface_interface,
                                  lib->proxy_get_version((struct wl_proxy*)compositor), 0, NULL);

    return (struct wl_surface*)id;
}