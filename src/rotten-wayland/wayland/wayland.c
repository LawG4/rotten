#include <string.h>
#include "rotten-core.h"
#include "rotten-wayland.h"

#define LOAD_WAY_FN(X) lib->X = rotten_dynamic_library_fetch(lib->way_lib, "wl_" #X)

// Now this is an interesting one, there is a const struct exported as a symbol for the wayland compositor
// interface, how do we get it? I guess just fetch a pointer and pray?
#define LOAD_WAY_INTERFACE(X) \
    lib->X = (struct wl_interface*)rotten_dynamic_library_fetch(lib->way_lib, "wl_" #X)

// For xdg functions
#define LOAD_XDG_FN(X) lib->X = rotten_dynamic_library_fetch(lib->ext_lib, "xdg_" #X)
#define LOAD_XDG_INTERFACE(X) \
    lib->X = (struct wl_interface*)rotten_dynamic_library_fetch(lib->ext_lib, "xdg_" #X)

rotten_success_code rotten_library_wayland_load_min(rotten_library_wayland* lib)
{
    // Get memset mofo >:(
    memset(lib, 0, sizeof(rotten_library_wayland));

    // Load the shared library from the system
    rotten_log_debug("Attempting to open libwayland-client.so", e_rotten_log_verbose);
    lib->way_lib = rotten_dynamic_library_open("libwayland-client.so");
    if (lib->way_lib == NULL) {
        rotten_log("Failed to open libwayland-client.so", e_rotten_log_warning);
        return e_rotten_library_not_present;
    }

    // Open librotten-wayland-xdg.so
    lib->ext_lib = rotten_dynamic_library_open("./librotten-wayland-ext.so");
    if (lib->ext_lib == NULL) {
        rotten_log("Failed to open librotten-wayland-ext.so", e_rotten_log_warning);
        return e_rotten_library_not_present;
    }

    // Got here, so we have a valid shared library so extract some function pointer.
    LOAD_WAY_FN(display_connect);
    LOAD_WAY_FN(display_disconnect);

    rotten_log_debug("Opened libwayland-client.so and librotten-wayland-ext.so", e_rotten_log_info);
    return e_rotten_success;
}

rotten_success_code rotten_library_wayland_valid_session(rotten_library_wayland* lib)
{
    // Assumes the library is atleast minimally loaded so use connect and disconnect to grab the compositor
    struct wl_display* temporary_display = lib->display_connect(NULL);  // Don't care about the name
    if (temporary_display == NULL) {
        rotten_log("Failed to connect to a wayland display", e_rotten_log_warning);
    } else {
        rotten_log_debug("Connection to wayland display!", e_rotten_log_info);
        lib->display_disconnect(temporary_display);
        return e_rotten_success;
    }
}

rotten_success_code rotten_library_wayland_load_full(rotten_library_wayland* lib)
{
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

    // Get the xdg functions, we have one function inside the rotten-wayland-xdg library which fills in the
    // function pointers for you, fetch that function from the library, use it to fill the library and close
    // it.
    void (*local_ext_fill_struct)(rotten_library_wayland * way_lib) =
      dlsym(lib->ext_lib, "rotten_wl_ext_fill_struct");

    if (local_ext_fill_struct == NULL) {
        rotten_log("Failed to fetch function pointers from ext helper", e_rotten_log_warning);
        return e_rotten_library_not_present;
    }

    local_ext_fill_struct(lib);
    return e_rotten_success;
}

rotten_success_code rotten_library_wayland_close(rotten_library_wayland* lib)
{
    // Clear everything else
    rotten_dynamic_library_close(lib->way_lib);
    memset(&lib->way_lib, 0, sizeof(rotten_library_wayland));
    return e_rotten_success;
}
