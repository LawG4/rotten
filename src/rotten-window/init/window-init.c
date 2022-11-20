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
#endif  // !linux

    // Got to the end without finding a supported backend
    rotten_log_debug("This rotten window backend isn't supported!", e_rotten_log_error);
    return 0;
}

//
// Initialising the window memory block
//

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

rotten_success_code rotten_window_init_xcb(rotten_window* window, rotten_window_connection* connection,
                                           rotten_window_definition* definition)
{
    // TODO: Handling xcb errors, when are they? Can a window id be 0? who knows? ^.^
    rotten_window_xcb* xcb = (rotten_window_xcb*)window;
    xcb->dispatch = connection->backend_handle;

    // We now have a pointer to the xcb dispatch and the xcb specfic variables, we can now start initialisng
    // the xcb window. This begins with registering a new id with the X server
    xcb->window_id = xcb->dispatch->generate_id(xcb->dispatch->connection_t);

    // Now that the window has been registered in the xserver, we can create a window object associated to
    // this id. oh mama is this a doozey
    xcb->dispatch->create_window(xcb->dispatch->connection_t, XCB_COPY_FROM_PARENT, xcb->window_id,
                                 xcb->dispatch->screen_t->root, 0, 0, definition->width, definition->height,
                                 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb->dispatch->screen_t->root_visual,
                                 xcb_property_mask, xcb_property_values);

    // Chane the name of the window by changing the XCB window name property
    xcb->dispatch->change_property(xcb->dispatch->connection_t, XCB_PROP_MODE_REPLACE, xcb->window_id,
                                   XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(definition->window_title),
                                   definition->window_title);

    // In order to catch events from the window manager, such as someone pressing the X button, we need to get
    // access to the WM_PROTOOCOLS property of the window we just created.

    // In order for us to recieve the messages from the window manager, we need to register our interest with
    // the XServer which window manager events we're going to listen for. Specifically we're going to append
    // "WM_DELETE_WINDOW" to the "WM_PROTOCOLS"
    xcb_intern_atom_cookie_t wm_cookie =
      xcb->dispatch->intern_atom(xcb->dispatch->connection_t, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_cookie_t close_cookie =
      xcb->dispatch->intern_atom(xcb->dispatch->connection_t, 0, 16, "WM_DELETE_WINDOW");

    // We have those cookies, however the server doesn't send these messages as cookies, they send them as
    // atoms which is a uint32_t. So we get the reply from the server for these cookies which contain the
    // atoms we're looking for
    xcb_intern_atom_reply_t* wm_reply =
      xcb->dispatch->intern_atom_reply(xcb->dispatch->connection_t, wm_cookie, NULL);
    xcb_intern_atom_reply_t* close_reply =
      xcb->dispatch->intern_atom_reply(xcb->dispatch->connection_t, close_cookie, NULL);

    // Register the interest for the wm_delete_window attom, by appending the delete atom onto the window
    // mnager protocols atoms. 32, 1 means we're sending one 32 bit value.
    xcb->dispatch->change_property(xcb->dispatch->connection_t, XCB_PROP_MODE_REPLACE, xcb->window_id,
                                   wm_reply->atom, XCB_ATOM_ATOM, 32, 1, &(close_reply->atom));

    // Now when the wm wants to close the window it sends an XCM_CLIENT_MESSAGE to xcb, and in the extra data
    // for the event it will contain the close atom. That means we need to save the close atom for comparisson
    // later. However the replies have had a block of memory dynamically allocated from within XCB, but it is
    // the callers responsibility to free the memory
    xcb->window_manager_close_atom = close_reply->atom;
    free(close_reply);
    free(wm_reply);

    rotten_log_debug("Created XCB window", e_rotten_log_info);
    return e_rotten_success;
}
#endif  //! XCB

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
        return rotten_window_init_xcb(window, connection, definition);
#endif  //! XCB
#endif  //! linux

    // Got this far without exiting, looks like we're out of luck
    rotten_log_debug("This rotten window backend isn't supported!", e_rotten_log_error);
    return e_rotten_unimplemented;
}

//
// Showing the window
//
rotten_success_code rotten_window_show(rotten_window* window)
{
    // convert pointer to base for translation. Now that the window should be shown we can tell the base that
    // it should now be shown
    rotten_window_base* base = (rotten_window_base*)window;
    base->remain_open = 1;

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    if (base->backend == e_rotten_window_xcb) {
        // Go onto map and flush
        // TODO: Handle error codes from xcb?
        rotten_window_xcb* xcb = (rotten_window_xcb*)window;
        xcb->dispatch->map_window(xcb->dispatch->connection_t, xcb->window_id);
        xcb->dispatch->flush(xcb->dispatch->connection_t);
        return e_rotten_success;
    }
#endif  // !xcb
#endif  // !linux
    return e_rotten_unimplemented;
}