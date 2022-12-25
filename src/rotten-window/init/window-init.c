/**
 * Performs the initialisation for a rotten window object, including how large the handle should be in bytes,
 * followed by initialising that memory block and then showing the window
 */
#include <stdlib.h>
#include <string.h>
#include "../rotten-window-internal.h"
//
// Finding how large the window block should be
//

size_t rotten_window_block_size(rotten_window_connection* connection, rotten_window_definition* definition)
{
// Iterate through each of the backends built for the platform, check the current window is using this backend
// then return that backend's size
#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    if (connection->selected_backend == e_rotten_window_xcb) return sizeof(rotten_window_xcb);
#endif  //! xcb
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    if (connection->selected_backend == e_rotten_window_wayland) return sizeof(rotten_window_wayland);
#endif  //! Wayland
#endif  // !linux

    // Got to the end without finding a supported backend
    rotten_log_debug("This rotten window backend isn't supported!", e_rotten_log_error);
    return 0;
}

//
// Initialising the window memory block
//
#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB

// The way we give the window extra attributes is having a mask, that tells properties we're setting. Each
// property has it's own values to set via a bit mask, so we pass an array of bitmasks for each property. Take
// for example the events we want the window to listen for, we set the mask to contain the events bit on, and
// then in the values array, we have an element with a bitmask of all the events we want the window to listen
// for
static const uint32_t xcb_property_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
static const uint32_t xcb_property_values[] = {
  // Value for the pixel to be cleared to. Needs to screens white pixel value
  0,
  // Events to listen for
  XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_STRUCTURE_NOTIFY};

rotten_success_code rotten_window_init_xcb(rotten_window_xcb* window, rotten_window_connection* connection,
                                           rotten_window_definition* definition)
{
    // The connection contains a pointer to the xcb library which is statically allocated inside the connect.c
    // file. I have my doubts about this method to be honest
    window->xcb = (rotten_library_xcb*)connection->backend_handle;
    rotten_window_xcb_extra* extra = &window->extra;
    rotten_library_xcb* xcb = window->xcb;

    // Make a new xcb conection for the window so that other windows can't snoop on the connection
    // TODO: is this neccasary?
    extra->connection = xcb->connect(NULL, NULL);
    if (extra->connection == NULL) {
        rotten_log_debug("Somehow unable to make a connection when we could before?", e_rotten_log_error);
        return e_rotten_log_error;
    }

    // The window now needs to be registered with the xcb server, and by doing this we get a unique id
    // TODO: Handling xcb errors, when are they? Can a window id be 0? who knows? ^.^
    extra->window_id = xcb->generate_id(extra->connection);

    // Before creating the window, we need to get the information about the root window, which usually
    // represents the screen. Is this always the base? I don't know, but you can iterate over multiple values,
    // I'll just assume the first value in this iterator is the root window?
    // TODO: Lookup what the contents of this iterator actually is
    extra->root_window = xcb->setup_roots_iterator(xcb->get_setup(extra->connection)).data;

    // Now that the window has been registered in the xserver, we can create a window object associated to
    // this id. While retireving some of the base settings from the root window.
    // oh mama is this a doozey!!
    xcb->create_window(extra->connection, XCB_COPY_FROM_PARENT, extra->window_id, extra->root_window->root, 0,
                       0, definition->width, definition->height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
                       extra->root_window->root_visual, xcb_property_mask, xcb_property_values);

    // Chane the name of the window by changing the XCB window name property
    xcb->change_property(extra->connection, XCB_PROP_MODE_REPLACE, extra->window_id, XCB_ATOM_WM_NAME,
                         XCB_ATOM_STRING, 8, strlen(definition->window_title), definition->window_title);

    // In order to catch events from the window manager, such as someone pressing the X button, we need to get
    // access to the WM_PROTOOCOLS property of the window we just created.
    //
    // In order for us to recieve the messages from the window manager, we need to register our interest with
    // the XServer which window manager events we're going to listen for. Specifically we're going to append
    // "WM_DELETE_WINDOW" atom to the "WM_PROTOCOLS" atom.
    //
    // In order to do this we need to know that atoms, which we get by creating a cookie, sending it to the
    // server and reading the reply, I think? TODO: Is this correct?
    xcb_intern_atom_cookie_t wm_cookie = xcb->intern_atom(extra->connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_cookie_t close_cookie = xcb->intern_atom(extra->connection, 0, 16, "WM_DELETE_WINDOW");

    xcb_intern_atom_reply_t* wm_reply = xcb->intern_atom_reply(extra->connection, wm_cookie, NULL);
    xcb_intern_atom_reply_t* close_reply = xcb->intern_atom_reply(extra->connection, close_cookie, NULL);

    // Register the interest for the wm_delete_window attom, by appending the delete atom onto the window
    // mnager protocols atoms. 32, 1 means we're sending one 32 bit value.
    xcb->change_property(extra->connection, XCB_PROP_MODE_REPLACE, extra->window_id, wm_reply->atom,
                         XCB_ATOM_ATOM, 32, 1, &(close_reply->atom));

    // Now when the wm wants to close the window it sends an XCM_CLIENT_MESSAGE to xcb, and in the extra data
    // for the event it will contain the close atom. That means we need to save the close atom for comparisson
    // later. However the replies have had a block of memory dynamically allocated from within XCB, but it is
    // the callers responsibility to free the memory
    extra->window_manager_close_atom = close_reply->atom;
    free(close_reply);
    free(wm_reply);

    rotten_log_debug("Created XCB window", e_rotten_log_info);
    return e_rotten_success;
}
#endif  //! XCB

#ifndef ROTTEN_WAINDOW_EXCLUDE_WAYLAND

rotten_success_code rotten_window_init_wayland(rotten_window_wayland* window,
                                               rotten_window_connection* connection,
                                               rotten_window_definition* definition)
{
    // Connect the window handle to the wayland library handle and connect to the display
    window->way = &g_wl;
    window->ext = &g_wlext;
    rotten_library_wayland* way = window->way;

    window->core_state.display = way->display_connect(NULL);
    if (window->core_state.display == NULL) {
        rotten_log_debug(
          "Somehow couldn't connect to wayland display after previously verifying that it works",
          e_rotten_log_error);
        return e_rotten_unclassified_error;
    }

    // Wayland contains a registry of all the higher level objects, we need to fetch a handle to that registry
    // first. The resources for the registry object can only be released on client disconnect, not on the
    // destruction of the registry pointer. So each client should store the registry pointer to minimise mem
    //
    // TODO: Can this be null? error handling. There was a typo in this before but it still gave me a pointer,
    // but the resulting device has no events and so hanged on the dispatch
    window->core_state.registry = rotten_wl_display_get_registry(way, window->core_state.display);
    rotten_success_code err = rotten_wl_attach_interface_listeners(window);
    if (err != e_rotten_success) return err;

    // Perform a thread blocking round trip to get the notifications for all currently attached devices
    way->display_dispatch(window->core_state.display);
    way->display_roundtrip(window->core_state.display);
    if (window->core_state.compositor == NULL || window->ext_state.wm_base == NULL) {
        rotten_log("Failed to fetch proxy handle for a required interface", e_rotten_log_error);
        return e_rotten_unclassified_error;
    }

    // Now that we have a proxy handle to the global wayland compositor, create a surface which is the
    // rectangular area of pixels which we actually have control over
    window->core_state.surface = rotten_wl_compositor_create_surface(way, window->core_state.compositor);
    if (window->core_state.surface == NULL) {
        rotten_log("Failed to fetch a proxy to required interface", e_rotten_log_error);
        return e_rotten_unclassified_error;
    }

    // From the surface now derive the xdg structs and attach a listener to the surface which will allocate a
    // buffer large enough to contain all of the pixels in the window
    window->ext_state.xdg_surface =
      window->ext->xdg_wm_base_get_xdg_surface(window->ext_state.wm_base, window->core_state.surface);
    window->ext->xdg_surface_add_listener(window->ext_state.xdg_surface, window->ext->xdg_surface_listener,
                                          window);
    window->ext_state.xdg_toplevel = window->ext->xdg_surface_get_toplevel(window->ext_state.xdg_surface);
    window->ext->xdg_toplevel_add_listener(window->ext_state.xdg_toplevel, window->ext->xdg_toplevel_listener,
                                           window);

    rotten_log("Created wayland window", e_rotten_log_info);
    return e_rotten_success;
}

#endif  //! wayland
#endif  //! Linux

rotten_success_code rotten_window_init(rotten_window* window, rotten_window_connection* connection,
                                       rotten_window_definition* definition)
{
    // Setup the base information
    rotten_window_base* base = (rotten_window_base*)window;
    base->backend = connection->selected_backend;
    base->remain_open = 0;                   // Not showing the window yet, no point to stay open
    base->title = definition->window_title;  // Maybe take a memcopy?
    base->width = definition->width;
    base->height = definition->height;

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    if (connection->selected_backend == e_rotten_window_xcb)
        return rotten_window_init_xcb((rotten_window_xcb*)window, connection, definition);
#endif  //! XCB
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    if (connection->selected_backend == e_rotten_window_wayland)
        return rotten_window_init_wayland((rotten_window_wayland*)window, connection, definition);
#endif  //! wayland
#endif  //! linux

    // Got this far without exiting, looks like we're out of luck
    rotten_log_debug("This rotten window backend isn't supported!", e_rotten_log_error);
    return e_rotten_unimplemented;
}