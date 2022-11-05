/***
 * Rotten Retro - Video.h
 * Provides the function declarations for the simulated video output system for gekko
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */
#ifndef __ROTTEN_RETRO_GEKKO_EXTERNAL_VIDEO_H__
#define __ROTTEN_RETRO_GEKKO_EXTERNAL_VIDEO_H__ (1)
#include <stdbool.h>
#include <stdint.h>
#include "../platform.h"
ROTTEN_CPP_GUARD

typedef struct GXRModeObj {
    uint32_t viTVMode;
    uint16_t fbWidth;
    uint16_t efbHeight;
    uint16_t xfbHeight;
    uint16_t viXOrigin;
    uint16_t viYOrigin;
    uint16_t viWidth;
    uint16_t viHeight;
    uint32_t xfbMode;
    uint8_t field_rendering;
    uint8_t aa;
    uint8_t sample_pattern[12][2];
    uint8_t vFilter[7];
} GXRModeObj;

void VIDEO_Init(void);

GXRModeObj *VIDEO_GetPreferredMode(GXRModeObj *mode);

void VIDEO_Flush(void);

void VIDEO_SetBlack(bool black);

void VIDEO_Configure(GXRModeObj *rmode);

void VIDEO_SetNextFramebuffer(void *fb);

void VIDEO_WaitVSync(void);

/*
 * Defines and enums go here! The original version in devkitpro uses defines, and I'm sure there's probably a
 * reason for that which I have yet to discover. However, I will be using enums until I encounter something
 * which causes problems.
 */

typedef enum VI_INTERLACE_MODE {
    VI_INTERLACE = 0,
    VI_NON_INTERLACE = 1,
    VI_PROGRESSIVE = 2
} VI_INTERLACE_MODE;

ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_RETRO_GEKKO_EXTERNAL_VIDEO_H__
