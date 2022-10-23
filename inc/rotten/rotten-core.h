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
ROTTEN_CPP_GUARD_END
#endif  // !__ROTTEN_EXTERNAL_CORE_H__
