/************************************************************************************************************
 * @file Rotten dynamic loading
 * @brief Provides a header only implementation for attempting to load dynamic libraries at runtime.
 * @note Uses macros to define rotten_dynamic_library, rotten_dynamic_library_open,
 * rotten_dynamic_library_close, rotten_dynamic_library_fetch
 * @note On linux requires users add libdl
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 ************************************************************************************************************/

#if defined __linux__
#include <dlfcn.h>  // Requires -ldl
// So that rotten_dynamic_library* == void*
typedef struct rotten_dynamic_library rotten_dynamic_library;

#define rotten_dynamic_library_open(X) dlopen(X, RTLD_LAZY)
#define rotten_dynamic_library_close(X) dlclose(X)
#define rotten_dynamic_library_fetch(X, Y) dlsym(X, Y)

#elif defined _WIN32

#endif