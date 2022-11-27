#include "../generated/xdg-shell-client-protocol.h"
#include "rotten-wayland.h"

const struct wl_interface* rotten_wl_fetch_wm_base_interface() { return &xdg_wm_base_interface; }