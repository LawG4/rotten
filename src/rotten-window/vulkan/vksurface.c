#include "../rotten-window-internal.h"

//
// Include the right surface extension
//

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
#define VK_USE_PLATFORM_XCB_KHR (1)
#endif  // xcb
#ifndef ROTTEN_WINDOE_EXCLUDE_WAYLAND
#define VK_USE_PLATFORM_WAYLAND_KHR (1)
#endif  // wayland
#endif  // linux

// Add in vulkan without any declarations
#include "rotten-vulkan.h"

// Store a static pointer to the system's vulkan library.
// TODO : Investigate if it is safe to leave this open or do I have to add a shutdown subsystem which closes
// this handle. I assume it's okay as libraries which are linked to start with a ref count of 1. But maybe
// that's popped at close by the crt?
static rotten_library_wayland* s_vk_handle = NULL;
static PFN_vkGetInstanceProcAddr s_vk_proc = NULL;

const char* rotten_window_vk_surface_ext_name(rotten_window* window)
{
    rotten_window_base* base = (rotten_window_base*)window;
#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    if (base->backend == e_rotten_window_xcb) return "VK_KHR_xcb_surface";
#endif  // xcb
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    if (base->backend == e_rotten_window_wayland) return "VK_KHR_wayland_surface";
#endif  // wayland
#endif  // linux

    // Got to the end of the backend selections without exiting out so there's no supported platform
    rotten_log_debug("There isn't yet a Vulkan surface extension implemented for this rotten window backend",
                     e_rotten_log_error);
    return NULL;  // Null to tell user it went wrong
}

//
// Per platform implementations of creating a surface!
//

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
rotten_success_code vk_surface_create_xcb(rotten_window_xcb* window, const VkInstance* instance,
                                          VkSurfaceKHR* surface)
{
    // The instance used to create the vksurface can change between calls and unlike getProcAddr, the create
    // surface function pointer can change between instances, consider of course one instance with the
    // extension enabled and one with out
    PFN_vkCreateXcbSurfaceKHR create_xcb_surface =
      (PFN_vkCreateXcbSurfaceKHR)s_vk_proc(*instance, "vkCreateXcbSurfaceKHR");

    // If this function pointer is null then the instance extension isn't loaded or something else went wrong
    if (create_xcb_surface == NULL) {
        rotten_log(
          "Failed to load a function pointer for creating an xcb surface, I can only assume you haveb't "
          "enabled the required instance extension",
          e_rotten_log_error);
        return e_rotten_feature_not_present;
    }

    // Go onto create the surface
    VkXcbSurfaceCreateInfoKHR info = {.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
                                      .connection = window->extra.connection,
                                      .window = window->extra.window_id,
                                      .flags = 0,
                                      .pNext = NULL};
    VkResult err = create_xcb_surface(*instance, &info, NULL, surface);

    if (err != VK_SUCCESS) {
        rotten_log("Failed when attempting to create a Vksurface with VkCreateXcbSurfaceKHR",
                   e_rotten_log_error);
        return e_rotten_feature_not_present;
    }

    // Got this far so return a success
    return e_rotten_success;
}
#endif  // xcb
#endif  // linux

rotten_success_code rotten_window_vk_surface_create(rotten_window* window, const void* instance,
                                                    void* surface)
{
    // User is trying to create the window, make sure we have a valid vulkan library handle
    if (s_vk_handle == NULL) {
        // Don't use the standard min, max load situation. since we only want a couple functions
        s_vk_handle = rotten_dynamic_library_open(ROTTEN_LIBRARY_VULKAN_NAME);
        if (s_vk_handle == NULL) {
            rotten_log("Couldn't open internal vulkan handle", e_rotten_log_error);
            return e_rotten_library_not_present;
        }
    }

    // Now we have to check we have a get instance proc address function pointer. It's used to fetch function
    // pointers from vulkan loader. It's a function which doesn't depend on the instance
    if (s_vk_proc == NULL) {
        s_vk_proc = rotten_dynamic_library_fetch(s_vk_handle, "vkGetInstanceProcAddr");
        if (s_vk_proc == NULL) {
            rotten_log(
              "Couldn't find function pointer for vkGetInstanceProcAddr, "
              "from a supposedly valid vulkan library, what??",
              e_rotten_log_error);
            return e_rotten_feature_not_present;
        }
    }

    // Got here so we have a valid vulkan library handle and a valid getInstanceProcAddress function pointer
    // se we can hand over things to the platform specific implementations
    rotten_window_base* base = (rotten_window_base*)window;

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    if (base->backend == e_rotten_window_xcb)
        return vk_surface_create_xcb((rotten_window_xcb*)window, instance, surface);
#endif  // xcb
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    if (base->backend == e_rotten_window_wayland) return e_rotten_unimplemented;
#endif  // wayland
#endif  // linux

    // Got to the end without being able to match to an implementation
    rotten_log(
      "This windowing system backend does not have functionality for creating a vulkan surface within rotten "
      "window (yet)",
      e_rotten_log_error);
    return e_rotten_unimplemented;
}