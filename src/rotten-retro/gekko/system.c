#include "ogc-internal.h"

void* SYS_AllocateFramebuffer(GXRModeObj* rmode)
{
    (void)rmode;
    rotten_log_debug("Error : Not implemented yet", e_rotten_log_error);
    return NULL;
}

void* MEM_K0_TO_K1(void* src)
{
    rotten_log_debug("Error : Not implemented yet", e_rotten_log_error);
    return NULL;
}

#ifdef WIN32
void* memalign(size_t size, size_t alignment)
{
    rotten_log_debug("Error : Not implemented yet", e_rotten_log_error);
    return NULL;
}
#endif
