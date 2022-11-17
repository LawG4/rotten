#include "../rotten-window-internal.h"

void rotten_window_poll_events(rotten_window* window) {}

uint8_t rotten_window_remain_open(rotten_window* window)
{
    // This can be done without worrying about the different platforms since it's contained in the base window
    return ((rotten_window_base*)window)->remain_open;
}