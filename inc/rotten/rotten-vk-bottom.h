/************************************************************************************************************
 * Rotten - Rotten-Vk-Bottom.h
 * Handles the more annoying parts of working with Vulkan, it does this by linking to Vulkan at runtime
 * internally so that it has access to all the vulkan calls needed. Since dynamic library handles are
 * reference counted it doesn't matter if we open a library that the user has already opened.
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 ************************************************************************************************************/
#ifndef __ROTTEN_VK_BOTTOM_EXTERNAL_H__
#define __ROTTEN_VK_BOTTOM_EXTERNAL_H__ (1)
#include "rotten-core.h"
#include "rotten-vk-runtime.h"
ROTTEN_CPP_GUARD

// Remember to pop the no prototypes rule if the user didn't define it themselves
#ifdef ROTTEN_POP_NO_PROTOTYPES
#undef VK_NO_PROTOTYPES
#undef ROTTEN_POP_NO_PROTOTYPES
#endif

ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_VK_BOTTOM_EXTERNAL_H__
