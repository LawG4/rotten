/***
 * Rotten - Rotten-Enum.h
 * Provides all of the enum used in rotten
 *
 * License   : MIT
 * Copyright : Rotten
 * Authors   : Lawrence G,
 */

#ifndef __ROTTEN_ENUM_EXTERNAL_H__
#define __ROTTEN_ENUM_EXTERNAL_H__ (1)

typedef enum rotten_success_code {

    e_rotten_success = 0,              // All good
    e_rotten_unclassified_error = 1,   // Not yet clasified error
    e_rotten_library_not_present = 2,  // A dependant library could not be loaded
    e_rotten_feature_not_present = 3,  // OS, software, or hardware can't use this feature
    e_rotten_unimplemented = 4         // The functionality is not yet iimplemented
} rotten_success_code;

typedef enum rotten_log_level {
    e_rotten_log_verbose = 0,
    e_rotten_log_info = 1,
    e_rotten_log_warning = 2,
    e_rotten_log_error = 3
} rotten_log_level;

typedef enum rotten_vk_features {
    e_rotten_vkf_none = 0,
    e_rotten_vkf_presenting = 1 << 0,  // The device can display to the screen

} rotten_vk_features;

typedef enum rotten_window_backend {
    e_rotten_window_none = 0,       // There is no window backend, no pixels can be drawn at all
    e_rotten_window_offscreen = 1,  // Can render to a screen which can't be displayed, like a png
    e_rotten_window_direct = 2,     // Direct framebuffer access through the kernel
    e_rotten_window_xcb = 4,        // There is an active xcb connection
    e_rotten_window_wayland = 8,    // There is an active wayland server
} rotten_window_backend;

#endif  // !__ROTTEN_ENUM_EXTERNAL_H__
