/***
 * Rotten - Rotten-Core.h
 * Provides functions which are shared in most rotten projects like the logging functionality. Including
 * common macros which will export function symbols
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */
#ifndef __ROTTEN_EXTERNAL_CORE_H__
#define __ROTTEN_EXTERNAL_CORE_H__ (1)
#include "rotten-enum.h"

// Cpp header guards, so that cpp projects can use our c libraries
#ifdef __cplusplus
#define ROTTEN_CPP_GUARD extern "C" {
#define ROTTEN_CPP_GUARD_END }
#else
#define ROTTEN_CPP_GUARD
#define ROTTEN_CPP_GUARD_END
#endif  // !__cplusplus
ROTTEN_CPP_GUARD

/**
 * @brief A function pointer definition which explains to the user the expected paramarers and return type of
 * a function which can be given to rotten core to be used as a function callback whenever a log function is
 * called. Rotten expects the function to have a void return type, and take two parameters, a const char
 * followed by a rotten log level enum
 */
typedef void (*rotten_log_function)(const char*, rotten_log_level);

/**
 * @brief Sets the user function to be called whenever an attempt at logging is made
 * @param callback The user defined function pointer to be called
 */
void rotten_log_set_callback(rotten_log_function callback);

/**
 * @brief Calls the currently set log callback
 * @param msg C-string with the message to be logged
 * @param level The severity of the message to be logged
 */
void rotten_log(const char* const msg, rotten_log_level level);

#ifndef NDEBUG
/**
 * @brief Performs a log call only in debug mode (aligns with asserts)
 * @param msg C-string with the message to be logged
 * @param level The severity of the message to be logged
 */
#define rotten_log_debug(msg, level) rotten_log((msg), (level));
#else
#define rotten_log_debug(msg, level) ;
#endif  // !NDEBUG

ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_EXTERNAL_CORE_H__
