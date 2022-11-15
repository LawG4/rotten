/************************************************************************************************************
 * Rotten - Rotten-Vk-Runtime.h
 * Handles the dynamic loading of the vulkan library so that users can handle the host machine not having
 * vulkan, while also grouping the function tables together. Meaning that they can be passed between helper
 * libraries more easily.
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 ************************************************************************************************************/
#ifndef __ROTTEN_VK_RUNTIME_EXTERNAL__
#define __ROTTEN_VK_RUNTIME_EXTERNAL__ (1)
#include "rotten-core.h"
ROTTEN_CPP_GUARD

// We need the function pointers declarations from Vulkan, but that is all we need. Users may want to not use
// Vulkan prototypes or they might want to use them, the important thing is that this header file doesn't
// ulter that status. We do that by defining the Vulkan no prototypes variable if the user hasn't defined it,
// and then undefing it at the end of the file
#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES (1)
#define ROTTEN_POP_NO_PROTOTYPES (1)
#endif  // !VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

typedef struct rotten_vkr_library {
    void* os_library_handle;
    uint32_t instance_loader_version;
    PFN_vkGetInstanceProcAddr get_instance_proc_addr;
    PFN_vkEnumerateInstanceVersion enumerate_instance_version;
    PFN_vkEnumerateInstanceExtensionProperties enumerate_instance_extension_properties;
    PFN_vkEnumerateInstanceLayerProperties enumerate_instance_layer_properties;
    PFN_vkCreateInstance create_instance;
} rotten_vkr_library;

rotten_success_code rotten_vk_start_runtime(rotten_vkr_library* lib);

// Remember to pop the no prototypes rule if the user didn't define it themselves
#ifdef ROTTEN_POP_NO_PROTOTYPES
#undef VK_NO_PROTOTYPES
#undef ROTTEN_POP_NO_PROTOTYPES
#endif

ROTTEN_CPP_GUARD_END
#endif  //!__ROTTEN_VK_RUNTIME_EXTERNAL__
