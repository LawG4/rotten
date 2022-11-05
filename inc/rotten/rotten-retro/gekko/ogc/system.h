/**
 * Rotten Retro - System.h
 * Provides the simulated system calls for the gekko systems
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */
#ifndef __ROTTEN_RETRO_GEKKO_EXTERNAL_SYSTEM_H__
#define __ROTTEN_RETRO_GEKKO_EXTERNAL_SYSTEM_H__ (1)
#include "../platform.h"
#include "video.h"
ROTTEN_CPP_GUARD

void* SYS_AllocateFramebuffer(GXRModeObj* rmode);

ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_RETRO_GEKKO_EXTERNAL_SYSTEM_H__
