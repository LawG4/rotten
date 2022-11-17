#include <stdio.h>
#include "rotten-window.h"

void application_rotten_log(const char* msg, rotten_log_level lvl)
{
    (void)lvl;
    printf("rotten log: %s\n", msg);
}

int main()
{
    // Register logging function to track debug information
    rotten_log_set_callback(application_rotten_log);

    // Attempt to open a connection to the operating systems windowing system
    rotten_window window;
    rotten_success_code err = rotten_window_connect(&window);

    return 0;
}
