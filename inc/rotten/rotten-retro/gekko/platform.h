/***
 * Rotten Retro - Platform.h
 * Aims to resolve some of the differences that certain platforms have. For example aligning a memory
 * allocation to 32 bytes makes it GPU visible on gekko. But that doesn't do anything on desktop, and on
 * windows the memalloc command isn't availble. There's _aligned_malloc on win32 but this will need some extra
 * work arounds.
 *
 * For example you have the macros for moving memory from virual or cached to uncached etc. These works by
 * manipulating the pointer addresses themselves. Which obviously won't work on desktop
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */
#ifndef __ROTTEN_RETRO_GEKKO_EXTERNAL_PLATFORM_H__
#define __ROTTEN_RETRO_GEKKO_EXTERNAL_PLATFORM_H__ (1)
#include "../../rotten-core.h"
ROTTEN_CPP_GUARD

void* MEM_K0_TO_K1(void* src);

#ifdef WIN32
void* memalign(size_t size, size_t alignment);
#endif

ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_RETRO_GEKKO_EXTERNAL_PLATFORM_H__
