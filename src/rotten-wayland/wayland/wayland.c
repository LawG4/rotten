#include <string.h>
#include "rotten-core.h"
#include "rotten-wayland.h"

#define LOAD_WAY_FN(X) lib->X = rotten_dynamic_library_fetch(lib->lib, "wl_" #X)

// Now this is an interesting one, there is a const struct exported as a symbol for the wayland compositor
// interface, how do we get it? I guess just fetch a pointer and pray?
#define LOAD_WAY_INTERFACE(X) lib->X = (struct wl_interface*)rotten_dynamic_library_fetch(lib->lib, "wl_" #X)

rotten_success_code rotten_library_wayland_load(rotten_library_wayland* lib)
{
    // Is the native handle null if so load it
    if (lib->lib == NULL) {
        rotten_dynamic_library_open("libwayland-client.so");
        if (lib->lib == NULL) {
            return e_rotten_library_not_present;
        }
    }

    // Registry functions
    LOAD_WAY_FN(display_dispatch);
    LOAD_WAY_FN(display_roundtrip);

    // Proxy functions
    LOAD_WAY_INTERFACE(registry_interface);
    LOAD_WAY_FN(proxy_get_version);
    LOAD_WAY_FN(proxy_marshal_flags);
    LOAD_WAY_FN(proxy_add_listener);

    // Compositor functions
    LOAD_WAY_INTERFACE(compositor_interface);
    LOAD_WAY_FN(compositor_create_surface);

    // Surface functions
    LOAD_WAY_INTERFACE(surface_interface);

    // Shared memory functions
    LOAD_WAY_INTERFACE(shm_interface);

    return e_rotten_success;
}
