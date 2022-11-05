/**
 * Rotten Retro GX.h
 * Defines the functions and types for the GX graphics API, which runs of gekko systems, game cube and the
 * nintendo Wii. Heavilly based on the one from DevkitPro, although not completley compatable
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */
#ifndef __ROTTEN_RETRO_GEKKO_EXTERNAL_GX_H__
#define __ROTTEN_RETRO_GEKKO_EXTERNAL_GX_H__ (1)
#include <stdint.h>
#include "../platform.h"
ROTTEN_CPP_GUARD

#define GX_FIFO_OBJSIZE (128)

typedef struct GXColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} GXColor;

typedef struct {
    uint8_t pad[GX_FIFO_OBJSIZE];
} GXFifoObj;

GXFifoObj* GX_Init(void* base, uint32_t size);

/**
 * Functions for controlling how the copy from the embedded framebuffer to the external framebuffer takes
 * place
 */

float GX_GetYScaleFactor(uint16_t embedded_fb_height, uint16_t external_fb_height);

uint32_t GX_SetDispCopyYScale(float scale);

void GX_SetDispCopySrc(uint16_t left, uint16_t top, uint16_t width, uint16_t height);

void GX_SetDispCopyDst(uint16_t width, uint16_t height);

void GX_SetCopyClear(GXColor color, uint32_t z_value);

void GX_CopyDisp(void* destination_fb, uint8_t clear);

/**
 * Functions that control the clipping of the embedded framebuffer
 */

void GX_SetViewport(float origin_x, float origin_y, float width, float height, float near_z, float far_z);
void GX_SetScissor(uint32_t origin_x, uint32_t origin_y, uint32_t width, uint32_t height);

ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_RETRO_GEKKO_EXTERNAL_GX_H__
