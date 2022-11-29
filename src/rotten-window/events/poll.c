/************************************************************************************************************
 * @file Rotten window polling
 * @brief Allows the user to poll for what events have happened since the last time the window was polled. The
 * user attaches the function callbacks so that the events can be called automatically in order when the poll
 * happens, instead of polling events and then checking what events happen and then calling them manually.
 * @note public functions defined : rotten_window_poll_events, rotten_window_remain_open
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/
#include <stdlib.h>
#include "../rotten-window-internal.h"

uint8_t rotten_window_remain_open(rotten_window* window)
{
    // This can be done without worrying about the different platforms since it's contained in the base window
    return ((rotten_window_base*)window)->remain_open;
}

//
// Polling per platform events
//

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
void rotten_window_poll_events_xcb(rotten_window_xcb* window)
{
    rotten_library_xcb* xcb = window->xcb;
    rotten_window_xcb_extra* extra = &window->extra;

    // How do we process events in XCB? Well the server will store a list of events. The next event can be
    // retrieved from the wait_for_event, but this actually blocks the current thread until XCB gets an event.
    // Using this in an event loop won't let us exit! But poll_event simply deques any existing events but
    // does not wait for a new one
    //
    // event will be a null pointer if there is no event in the queue
    int xcb_err = 0;
    xcb_generic_event_t* event = xcb->poll_for_event(extra->connection, &xcb_err);

    while (event != NULL) {
        // What type of event did we just recieve from the server? There is a bit which represents if the
        // event came from the server or a client (including this one or the other ones currently connected to
        // the server) However we don't care where the event came from, so clear that bit flag so that it can
        // be matched regardless

        switch (event->response_type & ~0x80) {
            // Got a message from the window manager, usually a close request from the x button
            case XCB_CLIENT_MESSAGE: {
                // Compare the message from the wm to the close request, if so set the window to close
                xcb_client_message_event_t* client = (xcb_client_message_event_t*)event;
                if (client->data.data32[0] == extra->window_manager_close_atom) {
                    rotten_log_debug("Window close request recieved from WM", e_rotten_log_info);
                    window->base.remain_open = 0;
                }
                break;
            }

            // Expose happens when the window is first mapped or maybe when it's been uncovered etc
            case XCB_EXPOSE: {
                // TODO: Do something useful with this!
                xcb_expose_event_t* expose = (xcb_expose_event_t*)event;
                break;
            }

            // Resize has happened
            case XCB_CONFIGURE_NOTIFY: {
                // TODO: Tell user when this happens, perhaps with a callback?
                break;
            }

            // unhandled event type, just ignore it
            default:
                rotten_log_debug("Unhandled event", e_rotten_log_warning);
                break;
        }

        // XCB internally allocates memory block for the event, it's our responsibility to free it after
        // dequeing the event.
        free(event);

        // dequeue the next event
        event = xcb->poll_for_event(extra->connection, &xcb_err);
    }
}

#endif  // | XCB
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
void rotten_window_poll_events_wayland(rotten_window_wayland* window)
{
    // TODO: This operation is deadlocking!! Fix this by peaking for events instead
    while (window->way->display_dispatch(window->extra.display)) {
    };
}
#endif  // !Wayland
#endif  // ! linux

//
// Call the correct platform function for polling events
//
void rotten_window_poll_events(rotten_window* window)
{
    // Convert into a window base
    rotten_window_base* base = (rotten_window_base*)window;

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    // Poll the xcb events for the window
    if (base->backend == e_rotten_window_xcb) {
        rotten_window_poll_events_xcb((rotten_window_xcb*)window);
        return;
    }
#endif  // ! XCB
#ifndef ROTTEN_WINDOW_EXCLUDE_WAYLAND
    if (base->backend == e_rotten_window_wayland) {
        rotten_window_poll_events_wayland((rotten_window_wayland*)window);
        return;
    }
#endif  // !Wayland
#endif  // !_linux
}
