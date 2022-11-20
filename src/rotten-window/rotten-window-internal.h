#include "rotten-window.h"
#ifndef __ROTTEN_INTERNAL_WINDOW_SHARED_H__
#define __ROTTEN_INTERNAL_WINDOW_SHARED_H__ (1)

//
// The internal window always has the first member being the backend type, that way the pointer can be casted
// into a baseline struct, and then that can be used to select the correct function path ways
//
typedef struct rotten_window_base {
    rotten_window_backend backend;
    uint8_t remain_open;
    uint32_t width;
    uint32_t height;
    char* title;
} rotten_window_base;

//
// Per OS configuration for the header
//

#ifdef __linux__
// On linux there are multiple common different display servers. And the user might not want to build for all
// of them. So we give them the ability to exclude each one individually
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
#include <wayland-client-core.h>

// Dynamic funciton dispatch table for wayland
typedef struct wayland_dispatch {
    void* wayland_handle;        // Handle to linux shared library
    struct wl_display* display;  // Connection to the wayland compositer

    // Function pointers for wayland
    struct wl_display* (*display_connect)(const char*);
    void (*display_disconnect)(struct wl_display*);
} wayland_dispatch;

/***
 * @brief Attempts to connect to a wayland server and load all of the function pointers from the hosts
 * wayland-client library
 * @param window Pointer to the window to hande the
 * @returns success code, success only if wayland is the chosen window backend
 */
rotten_success_code connect_test_wayland(rotten_window_connection* connection);
#endif

#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
#include <xcb/xcb.h>
#include <xcb/xcbext.h>

typedef struct xcb_dispatch {
    void* libary_handle;
    xcb_connection_t* connection_t;  // connection handle
    xcb_screen_t* screen_t;          // information about the root window

    // Function pointers for connection
    xcb_connection_t* (*connect)(const char* display_name, int* p_screen_num);
    const struct xcb_setup_t* (*get_setup)(xcb_connection_t* connection);
    xcb_screen_iterator_t (*setup_roots_iterator)(const xcb_setup_t* root_setup);

    // Function pointers for creating
    xcb_window_t (*generate_id)(xcb_connection_t* xcb_connection);
    xcb_void_cookie_t (*create_window)(xcb_connection_t* connection, uint8_t depth, xcb_window_t window_id,
                                       xcb_window_t parent_window_id, int16_t xpos, int16_t ypos,
                                       uint16_t width, uint16_t height, uint16_t border_width, uint16_t class,
                                       xcb_visualid_t visual, uint32_t value_mask,
                                       const uint32_t* value_list);
    xcb_void_cookie_t (*map_window)(xcb_connection_t* connection, xcb_window_t window_id);
    int (*flush)(xcb_connection_t* connection);

    // Function pointers for events
    xcb_intern_atom_cookie_t (*intern_atom)(xcb_connection_t* connection, uint8_t only_if_exists,
                                            uint16_t name_length, const char* name);
    xcb_intern_atom_reply_t* (*intern_atom_reply)(xcb_connection_t* connection,
                                                  xcb_intern_atom_cookie_t cookie, xcb_generic_error_t** e);
    xcb_void_cookie_t (*change_property)(xcb_connection_t* connection, uint8_t mode, xcb_window_t window,
                                         xcb_atom_t property, xcb_atom_t type, uint8_t format,
                                         uint32_t data_length, const void* data);
    xcb_generic_event_t* (*poll_for_event)(xcb_connection_t* connection, int* error);

} xcb_dispatch;

rotten_success_code connect_test_xcb(rotten_window_connection* connection);

typedef struct rotten_window_xcb {
    rotten_window_base base;
    xcb_dispatch* dispatch;

    xcb_window_t window_id;
    xcb_atom_t window_manager_close_atom;
} rotten_window_xcb;
#endif

// For allowing the user to dynamically open the display server libraries
#include <dlfcn.h>  // Requires linking with libdl
#endif

#endif