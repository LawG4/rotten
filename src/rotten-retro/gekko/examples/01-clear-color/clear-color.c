#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#include "gccore.h"

#define FIFO_SIZE (256 * 1024)

int main()
{
    // Startup the video subsystem
    VIDEO_Init();
    GXRModeObj* screen_mode = VIDEO_GetPreferredMode(NULL);

    // From the video output settings allocate enough memory for double buffered framebuffers
    void* framebuffers[2] = {MEM_K0_TO_K1(SYS_AllocateFramebuffer(screen_mode)),
                             MEM_K0_TO_K1(SYS_AllocateFramebuffer(screen_mode))};
    uint8_t frame_index = 0;

    // Configure the video subsystem to expect the next framebuffer and flush those changes out to the
    // hardware registers
    VIDEO_Configure(screen_mode);
    VIDEO_SetNextFramebuffer(framebuffers[frame_index]);
    VIDEO_Flush();

    // Wait for these register updates to be completed. If we're using interlace mode, then we need to wait
    // for a second VSync
    VIDEO_WaitVSync();
    if (screen_mode->viTVMode & VI_NON_INTERLACE) {
        VIDEO_WaitVSync();
    }

    // Allocate enough memory for the command buffer and give ownership of the memory backing the buffer over
    // to GX. The memory has to be aligned to 32 bytes to be visible to the gpu. The command buffer needs to
    // be set to 0s to avoid garbage commands being read
    void* cmd_buffer = memalign(32, FIFO_SIZE);
    memset(cmd_buffer, 0, FIFO_SIZE);
    GX_Init(cmd_buffer, FIFO_SIZE);

    // Set up the rendering area for the enternal framebuffer, which is where all draws are made during a
    // frame. Define the viewport and scissor and finally the region of the embedded framebuffer which will be
    // copied once a frame is finished.
    GX_SetViewport(0, 0, screen_mode->fbWidth, screen_mode->efbHeight, 0, 1);
    GX_SetScissor(0, 0, screen_mode->fbWidth, screen_mode->efbHeight);
    GX_SetDispCopySrc(0, 0, screen_mode->fbWidth, screen_mode->efbHeight);

    // Set up the region of the external framebuffer that will recieve the copied embedded framebuffer. In
    // this case the region will have the same width as the embedded one, but there may be more horizontal
    // rows. Therefore we need to fetch the scale between these two framebuffers.
    uint32_t xfb_height =
      GX_SetDispCopyYScale(GX_GetYScaleFactor(screen_mode->efbHeight, screen_mode->xfbHeight));
    GX_SetDispCopyDst(screen_mode->fbWidth, xfb_height);

    // Set the parameters for the copy operation, such as which color the embedded framebuffer will be cleared
    // to after the copy operation has taken place, then perform the actual copy. We have to do two copies as
    // the embedded framebuffer hasn't had a clear color set yet, so when it gets copied to the external
    // framebuffer, we might be copying garbage
    GXColor clear_color = {50, 50, 50, 0xff};
    GX_SetCopyClear(clear_color, 0x00ffffff);
    GX_CopyDisp(framebuffers[frame_index], true);
    GX_CopyDisp(framebuffers[frame_index], true);

    // We know know that the next frame buffer has a good clear value, so stop blacking out the video output
    // port. This prevents us from displaying whatever was in the ram on boot.
    VIDEO_SetBlack(false);
    VIDEO_Flush();

    while (1) {
        // Tell GX what color to clear the embedded framebuffer to after the next copy. This means we won't
        // see this clear color until next frame when the embedded framebuffer is copied again. We're
        // incrementing the clear color so that we know the display hasn't been locked up
        clear_color.b = clear_color.b == 0xff ? 0 : clear_color.b + 1;
        GX_SetCopyClear(clear_color, 0x00ffffff);

        // Copy the current embedded framebuffer out to the framebuffer about to be displayed, while clearing
        // the embedded framebuffer to the value we just set
        frame_index = !frame_index;
        GX_CopyDisp(framebuffers[frame_index], true);

        // Tell the video output system where the next framebuffer is going to be, flush those changes out to
        // the video hardware, and wait for the next VSync to take place, once that has happend the video
        // output will start displaying the new framebuffer
        VIDEO_SetNextFramebuffer(framebuffers[frame_index]);
        VIDEO_Flush();
        VIDEO_WaitVSync();
    }
}
