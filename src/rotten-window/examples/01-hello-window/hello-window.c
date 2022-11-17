#include <stdio.h>
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

    return 0;
}
