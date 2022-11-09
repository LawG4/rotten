/************************************************************************************************************
 * Rotten - Rotten-Vk-Bottom.h
 * The bottom level helper for Vulkan within rotten, specifically aims to help users handle the more annoying
 * and finically stuff. I don't think this library should actually not use any vk calls inside, instead should
 * allow users to fill up the vk create infos easier.
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 ************************************************************************************************************/
#ifndef __ROTTEN_VK_BOTTOM_EXTERNAL_H__
#define __ROTTEN_VK_BOTTOM_EXTERNAL_H__ (1)
#include "rotten-core.h"
#include "rotten-enum.h"
ROTTEN_CPP_GUARD

// TODO Remove this part, replacing it with a generated header at some point!
// Externally add in the vulkan headers. This does mean that in order to use this library people are going to
// have to add vulkan to their include directory search paths
#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES (1)
#define ROTTEN_POP_NO_PROTOTYPES (1)
#endif  // !VK_NO_PROTOTYPES
#include "vulkan/vulkan.h"

/**
 * Group together all of the structures that a vulkan program would need in order to connect to the api and
 * start searching for devices
 */
typedef struct rotten_vk_instance_startup {
    VkApplicationInfo app_info;
    VkInstanceCreateInfo instance_info;
    char** found_instance_extensions;
} rotten_vk_instance_startup;

void rotten_vk_insinstance_startup_factory(rotten_vk_instance_startup* startup,
                                           uint32_t required_feature_mask, uint32_t potental_feature_mask);

// Remember to pop the no prototypes rule if the user didn't define it themselves
#ifdef ROTTEN_POP_NO_PROTOTYPES
#undef VK_NO_PROTOTYPES
#undef ROTTEN_POP_NO_PROTOTYPES
#endif

ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_VK_BOTTOM_EXTERNAL_H__
