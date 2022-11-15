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
    e_rotten_success = 0,
    e_rotten_unclassified_error = 1,
    e_rotten_library_not_present = 2
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
#endif  // !__ROTTEN_ENUM_EXTERNAL_H__
