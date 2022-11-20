#include <stdio.h>
#include <stdlib.h>
#include "rotten-window.h"

void application_rotten_log(const char* msg, rotten_log_level lvl)
{
    switch (lvl) {
        case e_rotten_log_error:
            printf("rotten error   : ");
            break;
        case e_rotten_log_warning:
            printf("rotten warning : ");
            break;
        case e_rotten_log_info:
            printf("rotten info    : ");
            break;
        case e_rotten_log_verbose:
            return;
            // printf("rotten verbose : ");
            // break;
        default:
            return;
    }

    printf("%s\n", msg);
}

int main()
{
    // Register logging function to track debug information
    rotten_log_set_callback(application_rotten_log);

    // Attempt to open a connection to the operating systems windowing system
    rotten_window_connection connection;
    rotten_success_code err = rotten_window_connect(&connection);
    if (err != e_rotten_success) {
        printf("Failed to open a connection to the operating systems windowing system\n");
        return -1;
    }

    // Got a connection to a windowing system, display to the user which one was selected
    switch (connection.selected_backend) {
        case e_rotten_window_wayland:
            printf("Selected windowing system : Wayland\n");
            break;
        case e_rotten_window_xcb:
            printf("Selected windowing system: XCB\n");
            break;
        default:
            printf("Unknown windowing system? value \"%d\"\n", connection.selected_backend);
            break;
    }

    // Try to allocate space for a new window. The amount of memory we'll need might change depending on the
    // backend and the window size, etc. For that reason we leave it to the user to dynamically allocate a
    // large enough memory block for the window
    rotten_window_definition definition = {
      .window_title = "Hello rotten", .fullscreen = 0, .width = 920, .height = 360};
    rotten_window* window = malloc(rotten_window_block_size(&connection, &definition));
    if (window == NULL) {
        printf("Failed to allocate space for a rotten window\n");
        return -1;
    }

    // Initialise the window using the connection and the definition. Note how we're not taking a reference to
    // the window, as it is already an address
    err = rotten_window_init(window, &connection, &definition);
    if (err != e_rotten_success) {
        printf("Failed to initialise a rotten window with error code \"%d\"\n", err);
        return -1;
    }

    // A lot of windowing systems don't actually show the window on initialisation, instead they specifically
    // wait for the user to request to show the window. After this we need to handle the events from the
    // windowing system, for example moving the window around, or pressing the close button. Since we're not
    // doing anything we just poll for the events as fast as possible, until the user presses the close button
    rotten_window_show(window);
    while (rotten_window_remain_open(window)) {
        rotten_window_poll_events(window);
    }
    printf("Exiting windowing loop\n");
    // We're done!
    return 0;
}
