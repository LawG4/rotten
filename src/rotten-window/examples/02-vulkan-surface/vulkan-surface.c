/************************************************************************************************************
 * @file Vulkan Surface example
 * @brief Shows the user how to use rotten-window in tandem with Vulkan to create and present to a surface
 * using the cross platform API
 * @copyright Rotten, MIT.
 * @authors Lawrence G,
 *
 * @note This example is not a good example for Vulkan, I verfiy no features or anything, I am going as
 * quickly as possible to just put ANYTHING on the screen, do NOT use this as a Vulkan reference, instead use
 * this as a rotten-window reference, that's why this example is in rotten window not rotten vulkan.
 *
 * Oh this sample also shows users that despite the fact that Vulkan is loaded dynamically by the helper
 * libraries, you can link directly to vulkan, meaning you can use rotten window without using rotten VK!
 ************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>
#include "rotten-window.h"

// Create the window - at the bottom, see example 01 for more information
int rotten_window_startup(rotten_window_connection* con, rotten_window_definition* def,
                          rotten_window** window);

int main()
{
    rotten_window_connection window_conection;
    rotten_window_definition window_definition;
    rotten_window* window;
    if (rotten_window_startup(&window_conection, &window_definition, &window) != 0) {
        printf("Failed to do rotten window startup\n");
        return -1;
    }

    // Now move onto the Vulkan instnce
    // There are two instance extensnions we need to enable, which are the Vk_surface_khr which is the base
    // surface extension, along with the platform specific instance extension
    const char* instance_extensions[] = {"VK_KHR_surface", rotten_window_vk_surface_ext_name(window)};
    uint32_t instance_extension_count = sizeof(instance_extensions) / sizeof(char*);

    VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0),
                                  .engineVersion = 0,
                                  .pApplicationName = "Rotten window example",
                                  .pEngineName = "Rotten",
                                  .pNext = NULL};

    VkInstanceCreateInfo instance_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                          .pApplicationInfo = &app_info,
                                          .ppEnabledExtensionNames = instance_extensions,
                                          .enabledExtensionCount = instance_extension_count,
                                          .ppEnabledLayerNames = NULL,
                                          .enabledLayerCount = 0,
                                          .flags = 0,
                                          .pNext = NULL};
    VkInstance instance;
    if (vkCreateInstance(&instance_info, NULL, &instance) != VK_SUCCESS) {
        printf("Failed to create a vulkan instance\n");
        return -1;
    }
    printf("Created the Vulkan instance!\n");
}

int rotten_window_startup(rotten_window_connection* con, rotten_window_definition* def,
                          rotten_window** window)
{
    // Attempt to open a connection to the operating systems windowing system
    rotten_success_code err = rotten_window_connect(con);
    if (err != e_rotten_success) {
        printf("Failed to open a connection to the operating systems windowing system\n");
        return -1;
    }

    // Allocate space for a window
    rotten_window_definition definition = {
      .window_title = "Vulkan window example", .width = 720, .height = 360, .fullscreen = 0};
    memcpy(def, &definition, sizeof(rotten_window_definition));

    *window = malloc(rotten_window_block_size(con, def));
    if (*window == NULL) {
        printf("Failed to allocate space for a window\n");
        return -1;
    }

    //  Initialise the window but don't show it, keep it hidden for as long as possible
    err = rotten_window_init(*window, con, def);
    if (err != e_rotten_success) {
        printf("Failed to initialise a rotten window\n");
        return -1;
    }

    // All good
    return 0;
}