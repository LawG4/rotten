/************************************************************************************************************
 * @file Rotten window show
 * @brief Manages the displaying of a window after it has been created
 * @note Public functions defined: rotten_window_show
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/
#include "../rotten-window-internal.h"

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
rotten_success_code rotten_window_show_xcb(rotten_window_xcb* window)
{
    // Go onto map and flush
    // TODO: Handle error codes from xcb?
    window->xcb->map_window(window->extra.connection, window->extra.window_id);
    window->xcb->flush(window->extra.connection);
    return e_rotten_success;
}
#endif  //! xcb

#endif  //! linux

rotten_success_code rotten_window_show(rotten_window* window)
{
    // convert pointer to base for translation. Now that the window should be shown we can tell the base that
    // it should now be shown
    rotten_window_base* base = (rotten_window_base*)window;
    base->remain_open = 1;

#ifdef __linux__
#ifndef ROTTEN_WINDOW_EXCLUDE_XCB
    if (base->backend == e_rotten_window_xcb) return rotten_window_show_xcb((rotten_window_xcb*)window);

#endif  // !xcb
#endif  // !linux
    return e_rotten_unimplemented;
}