/***
 * Rotten Core Example - Logging
 * Using the logging functionality from cpp. For more advanced example check the C version
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */
#include <iostream>
#include "../../../../inc/rotten/rotten-core.h"

int main()
{
    auto log_function = [](const char* msg, rotten_log_level level) { std::cout << msg; };
    rotten_log_set_callback(log_function);
    rotten_log("Logging from cpp lambda!\n", e_rotten_log_info);
}
