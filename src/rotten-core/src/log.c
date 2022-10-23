/***
 * Rotten Core - log.c
 * Provides VERY basic logging functionality to rotten
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */
#include <stdlib.h>
#include "rotten-core-internal.h"
static rotten_log_function s_internal_logger = NULL;

void rotten_log_set_callback(rotten_log_function callback)
{
    // Store the callback function
    s_internal_logger = callback;
}

void rotten_log(const char* const msg, rotten_log_level level)
{
    // Check if the internal logger is null, if not call the user defined callback
    if (s_internal_logger) s_internal_logger(msg, level);
}
