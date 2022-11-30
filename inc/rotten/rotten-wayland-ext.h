/************************************************************************************************************
 * @file Rotten window wayland extensions header
 * @brief Provides functionality and helper functions for protocols which aren't yet in the core protocol,
 * also provides some extra helpers which may be useful during regular wayland operation
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 *
 * @note Unlike rotten-wayland this library links directly to libwayland-client.so, it needs to inorder to
 * build the extra protocols. This means linking to rotten-wayland-ext will cause a runtime error if the user
 * doesn't have wayland installed. The workaround is to have a function inside rotten-wayland which loads all
 * the symbols from this project dynamically.
 *
 * This results in the following control flow. directly link to rotten-wayland -> use rotten wayland to
 * determine wayland support at runtime -> dynamically load wayland symbols using rotten-wayland
 * -> dynamically load rotten-wayland-ext symbols using rotten-wayland.
 ************************************************************************************************************/
#ifndef __ROTTEN_EXTERNAL_WINDOW_WAYLAND_EXT_H__
#define __ROTTEN_EXTERNAL_WINDOW_WAYLAND_EXT_H__ (1)
#include "rotten-wayland.h"
#include "rotten-window.h"
ROTTEN_CPP_GUARD

/**
 * @brief Fills in a rotten library wayland library structure with function pointers for the extended
 * protocols and pointers to the interface structs.
 * @param lib Pointer to the rotten wayland library handle to fill
 *
 * @note If you link to rotten wayland ext explicitly, i.e when you're linking, then there's no real need for
 * this function as all of the symbols will be availble through the linker. The point of this is to
 * dynamically open this library, but then you only need fetch this one symbol
 */
void rotten_wl_ext_fill_struct(rotten_library_wayland* lib);
typedef void (*rotten_wl_ext_fill_struct_fp)(rotten_library_wayland* lib);

typedef struct rotten_wl_shm_pool {
    struct wl_shm_pool* pool;
    size_t pool_size;
    int file_descriptor;
    uint8_t* ptr;
} rotten_wl_shm_pool;

rotten_wl_shm_pool rotten_wl_shm_create_pool(struct wl_shm* shm, uint32_t size);
void rotten_wl_shm_destroy_pool(rotten_wl_shm_pool* pool);

ROTTEN_CPP_GUARD_END
#endif