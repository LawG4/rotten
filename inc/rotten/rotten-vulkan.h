/************************************************************************************************************
 * @file Rotten vulkan dynamic loader header
 * @brief Provides helper functions for dynamically loading the systems vulkan library
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 *
 * @note This header file includes vulkan.h so any source files which use rotten vulkan will need their
 * include search paths updated to point to the user's vulkan sdk
 ************************************************************************************************************/
#ifndef __ROTTEN_LIBRARY_VULKAN_H__
#define __ROTTEN_LIBRARY_VULKAN_H__ (1)
#include "rotten-core.h"
#include "rotten-dynamic-loading.h"
ROTTEN_CPP_GUARD

/**
 * @brief Opaque handle used to represent the Vulkan dynamic library which we load the function pointers from
 */
typedef struct rotten_library_vulkan rotten_library_vulkan;

/**
 * @brief Select the file which is searched for in the dynamic library search path. That file will then be
 * used to load the function pointers. This is determined on a per os basis.
 */
#ifdef __linux__
#define ROTTEN_LIBRARY_VULKAN_NAME "libvulkan.so"
#endif

// We need the function pointers declarations from Vulkan, but that is all we need. Users may want to not use
// Vulkan prototypes or they might want to use them, the important thing is that this header file doesn't
// ulter that status. We do that by defining the Vulkan no prototypes variable if the user hasn't defined it,
// and then undefing it at the end of the file
#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES (1)
#define ROTTEN_POP_NO_PROTOTYPES (1)
#endif  // !VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

// Remember to pop the no prototypes rule if the user didn't define it themselves
#ifdef ROTTEN_POP_NO_PROTOTYPES
#undef VK_NO_PROTOTYPES
#undef ROTTEN_POP_NO_PROTOTYPES
#endif

ROTTEN_CPP_GUARD_END
#endif