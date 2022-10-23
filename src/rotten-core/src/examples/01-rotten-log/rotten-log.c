/***
 * Rotten Core Example - Logging
 * an example showing users how to use the logging functions in rotten
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */

#include <stdio.h>
#include "../../../../../inc/rotten/rotten-core.h"

void example_log(const char* msg, rotten_log_level level)
{
    if (level <= e_rotten_log_info) return;
    printf("%s", msg);
}

int main()
{
    // We haven't given rotten a callback function yet, so we shouldn't get any logs
    rotten_log("Error: This should not show up, there is no callback\n", e_rotten_log_error);
    rotten_log_debug("Error: This should not show up, there is no callback\n", e_rotten_log_error);

    // Now give rotten a function to callback, and test if it shows us a high warning level log
    rotten_log_set_callback(example_log);
    rotten_log("Success! You should see this\n", e_rotten_log_error);
#ifndef NDEBUG
    rotten_log_debug("Success! You should see this\n", e_rotten_log_error);
#else
    rotten_log_debug("Error: You should not see this in release builds\n", e_rotten_log_error);
#endif  // !NDEBUG

    // Now check that the log doesn't report low level warnings
    rotten_log("Error, log level should be too low to show this\n", e_rotten_log_verbose);
    rotten_log_debug("Error, log level should be too low to show this\n", e_rotten_log_verbose);

    // Now turn off the logging function by setting it to null
    rotten_log_set_callback(NULL);
    rotten_log("Error: This should not show up, there is no callback\n", e_rotten_log_error);
    rotten_log_debug("Error: This should not show up, there is no callback\n", e_rotten_log_error);

#ifndef NDEBUG
    printf("You should see 2 debug logs\n");
#else
    printf("You should see 1 release log\n");
#endif  // !NDEBUG
}
