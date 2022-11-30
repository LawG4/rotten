// Things get pretty wild here. We need to be able to create shared memory between ourselves and the wayland
// compositor. Which either requires that we make an anonymous file descriptor by making a syscall, or we can
// repeatedly attempt to make a temp file with a random name... I think the syscall route is the least insane,
// but still pretty insane.
//
// TODO: How does this effect are runtime loading? I assume since we have libc symbols we have kernel symbols
// at runtime too?
#include <string.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "rotten-wayland-ext.h"

rotten_wl_shm_pool rotten_wl_shm_create_pool(struct wl_shm* shm, uint32_t size)
{
    rotten_wl_shm_pool pool;

    // Create an annoynmous file descriptor with 0 bytes written to it. This way we won't get any naming
    // conflics. So we don't have to try over and over again
    pool.file_descriptor = syscall(SYS_memfd_create, "buffer", 0);
    ftruncate(pool.file_descriptor, size);

    // Then map this file descriptor to addressable memory, with read and write access
    uint8_t* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, pool.file_descriptor, 0);
    if (ptr == MAP_FAILED) {
        // Unhandled!!!
        // TODO: handle this >:)
    }

    // Tell the server to map the same file pointer into addressable memory to share the memory
    pool.pool = wl_shm_create_pool(shm, pool.file_descriptor, size);
    pool.pool_size = size;
    return pool;
}

void rotten_wl_shm_destroy_pool(rotten_wl_shm_pool* pool)
{
    // Tell the server to unmap the memory and deallocate the pool handle
    wl_shm_pool_destroy(pool->pool);
    pool->pool = NULL;

    // Unmap the pointer
    munmap(pool->ptr, pool->pool_size);  // TODO: Handle potential errors
    pool->ptr = 0;
    pool->pool_size = 0;

    // Close the file descriptor
    close(pool->file_descriptor);
    pool->file_descriptor = 0;
}