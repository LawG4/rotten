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

VkResult vk_create_surface(VkInstance* instance, const char* const* extension_list, uint32_t extension_count);
VkResult vk_create_device(VkInstance instance, VkPhysicalDevice physical, VkSurfaceKHR surface,
                          VkDevice* device, VkQueue* queue);
int main()
{
    // Start the rotten window without showing it to the screen
    rotten_window_connection window_conection;
    rotten_window_definition window_definition;
    rotten_window* window;
    if (rotten_window_startup(&window_conection, &window_definition, &window) != 0) {
        printf("Failed to do rotten window startup\n");
        return -1;
    }

    // Create the Vulkan instance with the required instacne extensions. We know we'll always need the khronos
    // Vulkan extension and the platform specific extension which we can infer can get from rotten
    VkInstance instance;
    const char* instance_extensions[] = {"VK_KHR_surface", rotten_window_vk_surface_ext_name(window)};
    uint32_t instance_extension_count = sizeof(instance_extensions) / sizeof(char*);
    if (vk_create_surface(&instance, instance_extensions, instance_extension_count) != VK_SUCCESS) {
        printf("Failed to create a vulkan instance\n");
        return -1;
    }
    printf("Created the vulkan instance!\n");

    // Now in order to create the surface, the window has to be shown first and then use the rotten window api
    // to actually create the surface
    VkSurfaceKHR surface;
    rotten_window_show(window);
    if (rotten_window_vk_surface_create(window, &instance, &surface) != e_rotten_success) {
        printf("Failed to create VkSurfaceKHR via rotten window\n");
        return -1;
    }

    // Just select the first physical device and create a logical device with dynamic rendering enabled
    VkPhysicalDevice physical;
    uint32_t device_count = 1;
    vkEnumeratePhysicalDevices(instance, &device_count, &physical);

    VkDevice device;
    VkQueue queueGraphicsPresent;
    if (vk_create_device(instance, physical, surface, &device, &queueGraphicsPresent) != VK_SUCCESS) {
        printf("Failed to create a vulkan device!\n");
        return -1;
    }
    printf("Created the Vulkan device!\n");
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

VkResult vk_create_surface(VkInstance* instance, const char* const* extension_list, uint32_t extension_count)
{
    // Now move onto the Vulkan instnce
    // There are two instance extensnions we need to enable, which are the Vk_surface_khr which is the base
    // surface extension, along with the platform specific instance extension
    VkApplicationInfo app_info = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                  .apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0),
                                  .engineVersion = 0,
                                  .pApplicationName = "Rotten window example",
                                  .pEngineName = "Rotten",
                                  .pNext = NULL};

    VkInstanceCreateInfo instance_info = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                          .pApplicationInfo = &app_info,
                                          .ppEnabledExtensionNames = extension_list,
                                          .enabledExtensionCount = extension_count,
                                          .ppEnabledLayerNames = NULL,
                                          .enabledLayerCount = 0,
                                          .flags = 0,
                                          .pNext = NULL};

    return vkCreateInstance(&instance_info, NULL, instance);
}

VkResult vk_create_device(VkInstance instance, VkPhysicalDevice physical, VkSurfaceKHR surface,
                          VkDevice* device, VkQueue* queue)
{
    // Retrieve all of the queue families from the physical device
    uint32_t queue_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &queue_count, NULL);
    VkQueueFamilyProperties* queues = malloc(queue_count * sizeof(VkQueueFamilyProperties));
    if (queues == NULL) return -1;
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &queue_count, queues);

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                          .pQueuePriorities = &priority,
                                          .queueCount = 1,
                                          .flags = 0,
                                          .pNext = NULL};

    // Select the queue which has graphics and presentation support
    for (uint32_t i = 0; i < queue_count; i++) {
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface, &present_support);

        // Found good enough queue, break out loop
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && present_support == VK_TRUE) {
            queue_info.queueFamilyIndex = i;
            break;
        }

        // Gotten to the end without finding a good enough queue? error
        if (i == queue_count - 1) {
            printf("Failed to find a suitable queue\n");
            return -1;
        }
    }

    // List of device extensions
    const char* extensions[] = {"VK_KHR_dynamic_rendering"};
    uint32_t extension_count = sizeof(extensions) / sizeof(char*);

    // Create the vulkan device with dynamic rendering enabled
    VkDeviceCreateInfo device_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                      .ppEnabledExtensionNames = extensions,
                                      .enabledExtensionCount = extension_count,
                                      .pQueueCreateInfos = &queue_info,
                                      .queueCreateInfoCount = 1,
                                      .ppEnabledLayerNames = NULL,
                                      .enabledLayerCount = 0,
                                      .flags = 0,
                                      .pNext = NULL};

    VkResult res = vkCreateDevice(physical, &device_info, NULL, device);
    if (res != VK_SUCCESS) return res;

    vkGetDeviceQueue(*device, queue_info.queueFamilyIndex, 0, queue);
    if (queue != VK_NULL_HANDLE) return VK_SUCCESS;
    return -1;
}