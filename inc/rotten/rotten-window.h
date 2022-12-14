/************************************************************************************************************
 * Rotten - Rotten-Window.h
 * Provides fplatform independant ish window system interactions, eases the burden on the user for handling
 * all of the different platforms and os calls
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 ***********************************************************************************************************/
#ifndef __ROTTEN_EXTERNAL_WINDOW_H__
#define __ROTTEN_EXTERNAL_WINDOW_H__ (1)
#include "rotten-core.h"
ROTTEN_CPP_GUARD

/**
 * @brief Represents the user's connection to the operating system's windoing system. Must be initialised
 * before creating any windows as it is used to determine which backend to use
 */
typedef struct rotten_window_connection {
    // A bit mask representing which window backends rotten has connected to, each bit is represented by a
    // member in the rotten_window_backend enum.
    uint32_t supported_window_backends;

    // A bit flag representing the currently selected backend. When the connection is first made we select one
    // for you, but you can try to override it.
    //
    // TODO: Implement some kind of backend selection
    rotten_window_backend selected_backend;

    // Opaque pointer to the library handle so that it can be passed around internally, without forcing users
    // of the library to add the dependant libraries
    void* backend_handle;
} rotten_window_connection;

/**
 * @brief An opaque pointer used to represent a rotten window handle, so that user's don't have access to the
 * internal elements. For example, it doesn't make sense for a user to have access to the width, height
 * members of a window, as setting those variables won't update the window size without being flushed to the
 * operating system.
 *
 * The user can't infer how large a memory block is required to store the window, so they have to ask
 * rotten-window api. This is resloved using the rotten_window_definition struct.
 */
typedef struct rotten_window rotten_window;

/**
 * @brief Data type used by the user to tell the rotten api how they would like the window to be created. This
 * can be used by the api to tell the user how much memory they need to allocate. Then once the memory block
 * is large enough, it can then be used to initialise the window struct internally.
 */
typedef struct rotten_window_definition {
    char* window_title;
    uint32_t width;
    uint32_t height;
    uint8_t fullscreen;
} rotten_window_definition;

/**
 * @brief A base struct which holds the information about a window shared between all of the different
 * windowing system implementations
 * @note Don't create the struct you're sell it is exclusively used as a member struct for rotten windows and
 * not designed to be accessed by the user. For example setting the width and height in these structs won't
 * actually set the size of the window
 */
typedef struct rotten_window_base {
    rotten_window_backend backend;
    uint8_t remain_open;
    uint32_t width;
    uint32_t height;
    char* title;
} rotten_window_base;

/**
 * @brief Connects to the host operating system to interogate it for the window's features. This does not
 * launch a window yet, an exmaple use case is a user who wants to check which Vulkan instance extension they
 * should enable at runtime.
 * @param window Pointer to the window to make the operating system connection to
 * @returns Sucess code if rotten was able to connect to the host operating system
 */
rotten_success_code rotten_window_connect(rotten_window_connection* connection);

/**
 * @brief Calculates the number of bytes the user would have to allocate for a rotten window with the given
 * definition. May alter parameters contained in the definition when appropriate
 * @param connection Pointer to the operating system's connection to the windowing system
 * @param definition Pointer to the users desired settings for the window. members may get altered.
 * @returns size in bytes of the memory block to allocate for a rotten window corresponding to the definition.
 * Or 0 if an error took place
 */
size_t rotten_window_block_size(rotten_window_connection* connection, rotten_window_definition* definition);

/**
 * @brief Initialises a rotten window once the memory block for it has already been allocated.
 * @param window Pointer to the preallocated window to be initialised.
 * @param connection Pointer to the operating system's connection to the windowing system
 * @param definition Pointer to the user's desired settings for the window
 * @returns success code.
 */
rotten_success_code rotten_window_init(rotten_window* window, rotten_window_connection* connection,
                                       rotten_window_definition* definition);

/**
 * @brief Attempts to make the initialised window visible to the user
 * @param window Pointer to the window to make visible
 * @returns success code
 */
rotten_success_code rotten_window_show(rotten_window* window);

/**
 * @brief Should the current window remain open, or has the operating system, user or api requested that it
 * should close. Examples that cause the window to close are things like pressing the cross button etc.
 * @param window Pointer to the window which we are checking if it should remain open
 * @returns boolean value, 1 if the window should remain open or 0 if the window should close
 */
uint8_t rotten_window_remain_open(rotten_window* window);

/**
 * @brief The operating system usually stores up all the events which has happened since the last poll, that
 * means we don't need to keep it ourselves as a buffer. So check all of the events which has happened since
 * the last time, doing this once per frame works well
 * @param window Pointer to the window to poll the events for.
 */
void rotten_window_poll_events(rotten_window* window);

/**
 * @brief Fetches the corrosponding instance extension used to enable vulkan to create a VkSurface object for
 * the current rotten window
 * @param window Pointer to a rotten window which will eventually have the surface created.
 * @returns C string which represents which Vulkan extension to enable
 *
 * @note This function needs to be called after the window is initialised, however it can be called before the
 * window is actually shown as the only decision needed is for which backend has been selected
 */
const char* rotten_window_vk_surface_ext_name(rotten_window* window);

/**
 * @brief Attempts to create a vksurface object for a rotten window, requires a pointer to the vkinstance
 * which has the correct surface extension enabled.
 * @param window Pointer to the rotten window which will have
 * @param instance Pointer to the VkInstance object which is the parent object of the surface to be created
 * @param surface Pointer to the VkSurface object to be initialised
 * @returns success code, I wish we could do double return types to get the VkResult too.
 *
 * @note Before you can create a surface, the underlying OS component which is passed to Vulkan needs to be
 * initialised, that means the window needs to be open! There's all sorts of reasons as to why, such as
 * getting the true size of the window etc.
 */
rotten_success_code rotten_window_vk_surface_create(rotten_window* window, const void* instance,
                                                    void* surface);
ROTTEN_CPP_GUARD_END
#endif