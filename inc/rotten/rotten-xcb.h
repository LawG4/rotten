/************************************************************************************************************
 * @file Rotten window xcb specific header
 * @brief Allows the user to extract the native xcb handles used by the rotten backend, if they wish to
 * perform anything outside the bounds of rotten window, but wants to take advantage of the convinence that
 * rotten supplies
 * @note A good example use case would be starting up the rotten window and then extracting the handles to
 * change an xcb property directly
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/
#ifndef __ROTTEN_EXTERNAL_WINDOW_XCB_H__
#define __ROTTEN_EXTERNAL_WINDOW_XCB_H__ (1)
#include <xcb/xcb.h>
#include <xcb/xcbext.h>
#include "rotten-core.h"
#include "rotten-dynamic-loading.h"
ROTTEN_CPP_GUARD

typedef struct rotten_library_xcb {
    // Native linux handle for libxcb.so
    rotten_dynamic_library* xcb_lib;

    //
    // function pointer table
    //

    /**
     * @brief Connects the the xcb server
     * @param display_name The name of the display, or null if that doesn't matter
     * @param screen_num Pointer to the integer represetnting the screen index, or null if not important
     * @returns Pointer to the connection to the xcb server, or null if the connection failed
     */
    xcb_connection_t* (*connect)(const char* display_name, int* screen_num);

    /**
     * @brief Terminates an xcb connection
     */
    void (*disconnect)(xcb_connection_t* connection);

    /**
     * @brief
     * @param
     * @returns
     */
    const struct xcb_setup_t* (*get_setup)(xcb_connection_t* connection);

    /**
     * @brief
     * @param
     * @returns
     */
    xcb_screen_iterator_t (*setup_roots_iterator)(const xcb_setup_t* root_setup);

    /**
     * @brief
     * @param
     * @returns
     */
    xcb_window_t (*generate_id)(xcb_connection_t* xcb_connection);

    /**
     * @brief Creates an xcb window
     * @param
     * @param
     * @param
     */
    xcb_void_cookie_t (*create_window)(xcb_connection_t* connection, uint8_t depth, xcb_window_t window_id,
                                       xcb_window_t parent_window_id, int16_t xpos, int16_t ypos,
                                       uint16_t width, uint16_t height, uint16_t border_width,
                                       uint16_t _class, xcb_visualid_t visual, uint32_t value_mask,
                                       const uint32_t* value_list);

    /**
     * @brief Maps the window onto the screen
     */
    xcb_void_cookie_t (*map_window)(xcb_connection_t* connection, xcb_window_t window_id);

    /**
     * @brief flushes the window settings and parameters over to the hosting xcb server
     */
    int (*flush)(xcb_connection_t* connection);

    /**
     * @brief
     */
    xcb_intern_atom_cookie_t (*intern_atom)(xcb_connection_t* connection, uint8_t only_if_exists,
                                            uint16_t name_length, const char* name);

    /**
     * @brief
     */
    xcb_intern_atom_reply_t* (*intern_atom_reply)(xcb_connection_t* connection,
                                                  xcb_intern_atom_cookie_t cookie, xcb_generic_error_t** e);
    /**
     * @brief
     */
    xcb_void_cookie_t (*change_property)(xcb_connection_t* connection, uint8_t mode, xcb_window_t window,
                                         xcb_atom_t property, xcb_atom_t type, uint8_t format,
                                         uint32_t data_length, const void* data);

    /**
     * @brief
     */
    xcb_generic_event_t* (*poll_for_event)(xcb_connection_t* connection, int* error);

} rotten_library_xcb;

/**
 * @brief Represents the extra information that the rotten window handle contains in order to communicate with
 * xcb specifically. Doesn't contain private variables like the width and height.
 */
typedef struct rotten_window_xcb_extra {
    xcb_connection_t* connection;
    xcb_screen_t* root_window;
    xcb_window_t window_id;
    xcb_atom_t window_manager_close_atom;
} rotten_window_xcb_extra;

/**
 * @brief Loads the absolute minimum ammount from libxcb.so on the client computer to check if there is a
 * valid XServer running to connect to.
 * @param lib Pointer to the library to have minimum initialisation happen to, contains the native linux
 * handle and the function pointer table.
 * @note It is the callers responsibility to ensure that this handle hasn't been initialised already or else
 * we will overwrite the contents
 * @returns success code
 */
rotten_success_code rotten_library_xcb_load_min(rotten_library_xcb* lib);

/**
 * @brief Tests to see if the xcb library can connect to an xcb server, can be used on a minimum initalised
 * library
 * @param lib Pointer to the library to use to check the server. Can be minimumally loaded
 * @note It is the callers responsibility to ensure that thus library has been minimally initialised befire
 * checking if the connection is valid
 * @returns success code, rotten_success means that xcb can be used as the display client
 */
rotten_success_code rotten_library_xcb_valid_session(rotten_library_xcb* lib);

/**
 * @brief Loads the entirety of the xcb library with the function pointers being initialised
 * @param lib Pointer to the library to be fully initialised
 * @note It is the callers responsibility to ensure that this library has been minimally initalised first, or
 * else there will be no native library to extract functions from
 * @returns success code
 */
rotten_success_code rotten_library_xcb_load_full(rotten_library_xcb* lib);

/**
 * @brief Closes the native xcb handle for when the user is finished with xcb
 */
rotten_success_code rotten_library_xcb_close(rotten_library_xcb* lib);
ROTTEN_CPP_GUARD_END
#endif