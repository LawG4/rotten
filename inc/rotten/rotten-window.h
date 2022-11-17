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
 * @brief Connects to the host operating system to interogate it for the window's features. This does not
 * launch a window yet, an exmaple use case is a user who wants to check which Vulkan instance extension they
 * should enable at runtime.
 * @param window Pointer to the window to make the operating system connection to
 * @returns Sucess code if rotten was able to connect to the host operating system
 */
rotten_success_code rotten_window_connect(rotten_window_connection* connection);

ROTTEN_CPP_GUARD_END
#endif