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

VkResult vk_create_instance(VkInstance* instance, VkDebugUtilsMessengerEXT* msg,
                            const char* const* extension_list, uint32_t extension_count);
VkResult vk_create_device(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice* physical,
                          VkDevice* device, VkQueue* queue, uint32_t* queue_index);
VkResult vk_create_swapchain(VkDevice device, VkPhysicalDevice physical, VkSurfaceKHR surface,
                             VkSwapchainKHR* swap, uint32_t* swap_length, VkExtent2D* extent,
                             VkImage** images, VkImageView** views);
VkResult vk_create_sync_objects(VkDevice device, uint32_t swap_length, VkSemaphore** image_renderable,
                                VkSemaphore** image_presentable, VkFence** fences);
VkResult vk_create_command_buffers(VkDevice, uint32_t swap_length, VkCommandPool* pool,
                                   VkCommandBuffer** cmd);
VkResult vk_do_work(VkDevice device, VkSwapchainKHR swap, VkImage* swap_images, VkImageView* swap_view,
                    VkExtent2D render_area, VkQueue queue, VkSemaphore image_renderable,
                    VkSemaphore image_presentable, VkFence fence, VkCommandBuffer* cmds);

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
    VkDebugUtilsMessengerEXT messenger;
    const char* instance_extensions[] = {"VK_KHR_surface", rotten_window_vk_surface_ext_name(window)};
    uint32_t instance_extension_count = sizeof(instance_extensions) / sizeof(char*);
    if (vk_create_instance(&instance, &messenger, instance_extensions, instance_extension_count) !=
        VK_SUCCESS) {
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

    // Select the first physical device assuming that it is capable of dynamic rendering
    VkDevice device;
    VkPhysicalDevice physical;
    VkQueue queue_gp;  // graphics & present queues
    uint32_t queue_index_gp;
    if (vk_create_device(instance, surface, &physical, &device, &queue_gp, &queue_index_gp) != VK_SUCCESS) {
        printf("Failed to create a vulkan device!\n");
        return -1;
    }
    printf("Created the Vulkan device!\n");

    // Now fetch the swapchain and the associated image views
    VkExtent2D swap_extent = {.width = window_definition.width, .height = window_definition.height};
    VkSwapchainKHR swap;
    VkImage* swap_images;
    VkImageView* swap_views;
    uint32_t swap_length;

    if (vk_create_swapchain(device, physical, surface, &swap, &swap_length, &swap_extent, &swap_images,
                            &swap_views) != VK_SUCCESS) {
        printf("Failed to create the swapchain\n");
        return -1;
    }
    printf("Created the Vulkan swapchain!\n");

    // Create the sync objects
    VkSemaphore* image_renderables;
    VkSemaphore* image_presentables;
    VkFence* fences;
    if (vk_create_sync_objects(device, swap_length, &image_renderables, &image_presentables, &fences) !=
        VK_SUCCESS) {
        printf("Failed to create the sync objects\n");
        return -1;
    }
    printf("Created the Vulkan sync objects!\n");

    // Create all the command buffers and the pool
    VkCommandPool pool;
    VkCommandBuffer* cmds;
    if (vk_create_command_buffers(device, swap_length, &pool, &cmds) != VK_SUCCESS) {
        printf("Failed to create command buffers\n");
        return -1;
    }
    printf("Created the Vulkan command buffers!\n");

    // Now enter into the windowing loop
    uint32_t sync_object_index = 0;
    while (rotten_window_remain_open(window)) {
        rotten_window_poll_events(window);

        // Record the command buffer for the current frame and submit it
        vk_do_work(device, swap, swap_images, swap_views, swap_extent, queue_gp,
                   image_renderables[sync_object_index], image_presentables[sync_object_index],
                   fences[sync_object_index], cmds);

        // Advance to the next set of sync objects
        sync_object_index = (sync_object_index + 1) % swap_length;
        // printf("On sync index %d\n", sync_object_index);
    }
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

static VkBool32 s_debug_messenger(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                  VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    printf("VALIDATION ERROR!!!\n\n%s\n\n", pCallbackData->pMessage);
    return VK_FALSE;  // Don't do anything after this function ends
}

VkResult vk_create_instance(VkInstance* instance, VkDebugUtilsMessengerEXT* msg,
                            const char* const* extension_list, uint32_t extension_count)
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

#ifndef NDEBUG
    // If we're in debug mode then enable validation layers and attach the debug messenger
    const char** debug_extension_list = malloc((extension_count + 1) * sizeof(char*));
    if (debug_extension_list == NULL) return -1;
    memcpy(debug_extension_list, extension_list, extension_count * sizeof(char*));
    debug_extension_list[extension_count] = "VK_EXT_debug_utils";
    instance_info.enabledExtensionCount = extension_count + 1;
    instance_info.ppEnabledExtensionNames = debug_extension_list;

    const char* debug_layer_list[] = {"VK_LAYER_KHRONOS_validation"};
    instance_info.enabledLayerCount = 1;
    instance_info.ppEnabledLayerNames = debug_layer_list;
#endif

    if (vkCreateInstance(&instance_info, NULL, instance) != VK_SUCCESS) {
        printf("Vulkan instance creation failed\n");
        return -1;
    }

#ifndef NDEBUG
    // Now if we're debugging we need to actually create the debug messenger and also free the memory we
    // allocated for the extension list
    free(debug_extension_list);

    PFN_vkCreateDebugUtilsMessengerEXT create_messenger =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*instance, "vkCreateDebugUtilsMessengerEXT");
    if (create_messenger == NULL) return -1;

    VkDebugUtilsMessengerCreateInfoEXT messenger_info = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pfnUserCallback = s_debug_messenger,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
      .pUserData = NULL,
      .flags = 0,
      .pNext = NULL};

    return create_messenger(*instance, &messenger_info, NULL, msg);
#else
    *msg = VK_NULL_HANDLE;
    return VK_SUCCESS;
#endif
}

VkResult vk_create_device(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice* physical,
                          VkDevice* device, VkQueue* queue, uint32_t* queue_index)
{
    // The way you're supposed to do this is to querey how many devices their are first and allocate how many,
    // however if the pPhysicalDevice handle is not null, then loader will just fetch the that many devices,
    // so we can fetch 1 physical device, this requires the assumption that the user definitley has one
    uint32_t device_count = 1;
    vkEnumeratePhysicalDevices(instance, &device_count, physical);

    // Retrieve all of the queue families from the physical device
    uint32_t queue_count;
    vkGetPhysicalDeviceQueueFamilyProperties(*physical, &queue_count, NULL);
    VkQueueFamilyProperties* queues = malloc(queue_count * sizeof(VkQueueFamilyProperties));
    if (queues == NULL) return -1;
    vkGetPhysicalDeviceQueueFamilyProperties(*physical, &queue_count, queues);

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info = {.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                          .pQueuePriorities = &priority,
                                          .queueCount = 1,
                                          .flags = 0,
                                          .pNext = NULL};

    // Select the queue which has graphics and presentation support
    for (uint32_t i = 0; i < queue_count; i++) {
        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(*physical, i, surface, &present_support);

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
    const char* extensions[] = {"VK_KHR_swapchain", "VK_KHR_dynamic_rendering"};
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

    // Enable dynamic rendering
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature = {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
      .dynamicRendering = VK_TRUE,
    };
    device_info.pNext = &dynamic_rendering_feature;

    VkResult res = vkCreateDevice(*physical, &device_info, NULL, device);
    if (res != VK_SUCCESS) return res;

    *queue_index = queue_info.queueFamilyIndex;
    vkGetDeviceQueue(*device, queue_info.queueFamilyIndex, 0, queue);
    if (queue != VK_NULL_HANDLE) return VK_SUCCESS;
    return -1;
}

VkResult vk_create_swapchain(VkDevice device, VkPhysicalDevice physical, VkSurfaceKHR surface,
                             VkSwapchainKHR* swap, uint32_t* swap_length, VkExtent2D* extent,
                             VkImage** images, VkImageView** views)
{
    // For the swapchain just take the first format availble (Note this always assumes there is a format) And
    // for the presentation mode FIFO is always availble
    uint32_t format_count = 1;
    VkSurfaceFormatKHR format;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &format_count, &format);

    // Get the swapchain length, this is how many per swapchain resources we'll need to create.
    VkSurfaceCapabilitiesKHR cap;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &cap);
    *swap_length = cap.minImageCount;

    // Bound the swapchain's dimensions by the maximum supported, we rely on the caller to fill in the extent
    // with the suggested size of the window, for example the title bar might shrink the framebuffer
    if (extent->width < cap.minImageExtent.width) extent->width = cap.minImageExtent.width;
    if (extent->height < cap.minImageExtent.height) extent->height = cap.minImageExtent.height;
    if (extent->width > cap.maxImageExtent.width) extent->width = cap.maxImageExtent.width;
    if (extent->height > cap.maxImageExtent.height) extent->height = cap.maxImageExtent.height;

    // TODO pass the actual queue family index
    uint32_t queue_family = 0;

    VkSwapchainCreateInfoKHR swap_info = {.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                          .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                          .surface = surface,
                                          .minImageCount = *swap_length,
                                          .imageFormat = format.format,
                                          .imageColorSpace = format.colorSpace,
                                          .presentMode = VK_PRESENT_MODE_FIFO_KHR,
                                          .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                          .pQueueFamilyIndices = &queue_family,
                                          .queueFamilyIndexCount = 1,
                                          .imageExtent = *extent,
                                          .oldSwapchain = VK_NULL_HANDLE,
                                          .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                                          .preTransform = cap.currentTransform,
                                          .imageArrayLayers = 1,
                                          .clipped = VK_TRUE,
                                          .flags = 0,
                                          .pNext = NULL};

    VkResult err = vkCreateSwapchainKHR(device, &swap_info, NULL, swap);
    if (err != VK_SUCCESS) {
        printf("Unable to create a swapchain\n");
        return -1;
    }

    // Retrieve the swapchain image handles while getting the actual number of swap images
    vkGetSwapchainImagesKHR(device, *swap, swap_length, NULL);
    *images = malloc(*swap_length * sizeof(VkImage));
    if (*images == NULL) return -1;
    if (vkGetSwapchainImagesKHR(device, *swap, swap_length, *images) != VK_SUCCESS) {
        printf("Failed while retriving swapchain images\n");
        return -1;
    }

    // Reserve enough space for the image views and initialise the views from the image
    *views = malloc(*swap_length * sizeof(VkImageView));
    if (*views == NULL) return -1;

    for (uint32_t i = 0; i < *swap_length; i++) {
        VkImageViewCreateInfo view_info = {.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                           .image = (*images)[i],
                                           .format = format.format,
                                           .viewType = VK_IMAGE_VIEW_TYPE_2D,
                                           .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                           .subresourceRange.baseMipLevel = 0,
                                           .subresourceRange.levelCount = 1,
                                           .subresourceRange.baseArrayLayer = 0,
                                           .subresourceRange.layerCount = 1,
                                           .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
                                           .flags = 0,
                                           .pNext = NULL};

        if (vkCreateImageView(device, &view_info, NULL, &(*views)[i]) != VK_SUCCESS) {
            printf("Failed to create image view\n");
            return -1;
        }
    }

    return VK_SUCCESS;
}

VkResult vk_create_sync_objects(VkDevice device, uint32_t swap_length, VkSemaphore** image_renderable,
                                VkSemaphore** image_presentable, VkFence** fences)
{
    // Create the create infos for the semaphore
    VkSemaphoreCreateInfo semaphore_info = {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, .flags = 0, .pNext = NULL};
    VkFenceCreateInfo fence_info = {
      .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, .flags = VK_FENCE_CREATE_SIGNALED_BIT, .pNext = NULL};

    // Allocate memory for each frame in the swapchain
    *image_renderable = malloc(swap_length * sizeof(VkSemaphore));
    if (*image_renderable == NULL) return -1;
    *image_presentable = malloc(swap_length * sizeof(VkSemaphore));
    if (*image_presentable == NULL) return -1;
    *fences = malloc(swap_length * sizeof(VkFence));
    if (*fences == NULL) return -1;

    for (uint32_t i = 0; i < swap_length; i++) {
        if (vkCreateSemaphore(device, &semaphore_info, NULL, &(*image_renderable)[i]) != VK_SUCCESS)
            return -1;
        if (vkCreateSemaphore(device, &semaphore_info, NULL, &(*image_presentable)[i]) != VK_SUCCESS)
            return -1;
        if (vkCreateFence(device, &fence_info, NULL, &(*fences)[i]) != VK_SUCCESS) return -1;
    }

    return VK_SUCCESS;
}

VkResult vk_create_command_buffers(VkDevice device, uint32_t swap_length, VkCommandPool* pool,
                                   VkCommandBuffer** cmd)
{
    VkCommandPoolCreateInfo pool_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                         .queueFamilyIndex = 0,  // TODO pass on actual queue index
                                         .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                         .pNext = NULL};

    if (vkCreateCommandPool(device, &pool_info, NULL, pool) != VK_SUCCESS) {
        printf("Failed to create command pool\n");
        return -1;
    }

    // Allocate the command buffers from the pool
    VkCommandBufferAllocateInfo alloc_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                              .commandPool = *pool,
                                              .commandBufferCount = swap_length,
                                              .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                              .pNext = NULL};
    *cmd = malloc(swap_length * sizeof(VkCommandBuffer));
    if (*cmd == NULL) return -1;
    return vkAllocateCommandBuffers(device, &alloc_info, *cmd);
}

VkResult vk_do_work(VkDevice device, VkSwapchainKHR swap, VkImage* swap_images, VkImageView* swap_view,
                    VkExtent2D render_area, VkQueue queue, VkSemaphore image_renderable,
                    VkSemaphore image_presentable, VkFence fence, VkCommandBuffer* cmds)
{
    // Perform a wait on the CPU via a fence. This action performs a syncronisation between the cpu and gpu
    // and is very slow. The aim is to make sure that it is now safe for the CPU to start recording commands
    // into the command buffer, and that can only be the case when the GPU is finished with it. Once we have
    // waited on this fence we can reset it
    if (vkWaitForFences(device, 1, &fence, VK_TRUE, (uint64_t)-1) != VK_SUCCESS) printf("Failed to wait\n");
    vkResetFences(device, 1, &fence);

    // Fetch which image in the swapchain we're about to use. This can then be used to index all other
    // resources tied to the swapchain like the command buffer and image view indexes. This doesn't perform
    // any waiting but it will tell the command buffer to singal the semaphore we pass when the image is
    // acquired from the swapchain
    uint32_t swap_index;
    if (vkAcquireNextImageKHR(device, swap, (uint64_t)-1, image_renderable, VK_NULL_HANDLE, &swap_index) !=
        VK_SUCCESS)
        printf("Failed to acquire next image");

    // Use this to get the current command buffer to use and reset it
    VkCommandBuffer cmd = cmds[swap_index];
    if (vkResetCommandBuffer(cmd, 0) != VK_SUCCESS) printf("Failed to reset\n");
    VkCommandBufferBeginInfo begin_info = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                           .pInheritanceInfo = NULL,
                                           .flags = 0,
                                           .pNext = NULL};
    if (vkBeginCommandBuffer(cmd, &begin_info) != VK_SUCCESS) printf("Failed to begin cmd\n");

    // Transition the image from surface usage to colour output
    VkImageMemoryBarrier renderable_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                               .image = swap_images[swap_index],
                                               .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                               .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                               .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                               .subresourceRange =
                                                 {
                                                   .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                   .baseMipLevel = 0,
                                                   .levelCount = 1,
                                                   .baseArrayLayer = 0,
                                                   .layerCount = 1,
                                                 },
                                               .pNext = NULL};

    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, NULL, 0, NULL, 1,
                         &renderable_barrier);

    // Now rather than beginning a render pass we can use the dynamic rendering device extension to package
    // all of that for us. The aim of this is to start a "render pass" so that the framebuffer is cleared and
    // that's litterally it
    VkClearValue clear = {1.0f, 0.0f, 0.0f, 1.0f};
    VkRenderingAttachmentInfoKHR attachment_info = {.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                                                    .clearValue = clear,
                                                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                                    .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
                                                    .resolveImageLayout = 0,  //?
                                                    .imageView = swap_view[swap_index],
                                                    .resolveImageView = 0,  //?
                                                    .pNext = NULL};
    VkRenderingInfoKHR rendering_info = {.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
                                         .pColorAttachments = &attachment_info,
                                         .colorAttachmentCount = 1,
                                         .renderArea = {.extent = render_area, .offset = {.x = 0, .y = 0}},
                                         .pDepthAttachment = NULL,
                                         .pStencilAttachment = NULL,
                                         .layerCount = 1,  // ?
                                         .flags = 0,
                                         .pNext = NULL};

    vkCmdBeginRendering(cmd, &rendering_info);
    vkCmdEndRendering(cmd);

    // Transition the render target back to presentable format using the
    VkImageMemoryBarrier presentable_barrier = {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                .image = swap_images[swap_index],
                                                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                .subresourceRange = renderable_barrier.subresourceRange,
                                                .pNext = NULL};
    vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &presentable_barrier);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        printf("Failed while attempting to end the command buffer\n");
        return -1;
    }

    // Now submit this command buffer, ensuring that we set the gpu to signal the image present ready
    // semaphore when the command buffer is finished, this is because all the rendering commands are done and
    // the image is ready to be shown on screen. However, we also want to prevent the writing to the render
    // target until the image is ready to be rendered to. This semaphore is the one signalled by the aquire
    // next image
    uint32_t wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pCommandBuffers = &cmd,
                                .commandBufferCount = 1,
                                .pSignalSemaphores = &image_presentable,
                                .signalSemaphoreCount = 1,
                                .pWaitSemaphores = &image_renderable,
                                .waitSemaphoreCount = 1,
                                .pWaitDstStageMask = &wait_stage,
                                .pNext = NULL};

    // Tell the GPU which fence to signal when it is done reading the command buffer, and the CPU can safely
    // write to the command buffer
    if (vkQueueSubmit(queue, 1, &submit_info, fence) != VK_SUCCESS) {
        printf("Failed to submit the command buffer to the graphics queue\n");
        return -1;
    }

    // Now that we have commands submitted to the graphics queue and it is churning away at that work, let's
    // queue up the image to be presented to the screen once the image is ready. Once again this doesn't
    // perform any waits on the CPU, but tells the presentation queue to wait until the semaphore is signalled
    // (The one which is signalled by the graphics queue finishing the command buffer)
    VkPresentInfoKHR present_info = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                     .pSwapchains = &swap,
                                     .swapchainCount = 1,
                                     .pWaitSemaphores = &image_presentable,
                                     .waitSemaphoreCount = 1,
                                     .pImageIndices = &swap_index,
                                     .pResults = NULL,
                                     .pNext = NULL};
    if (vkQueuePresentKHR(queue, &present_info) != VK_SUCCESS) {
        printf("Failed to present the image\n");
    }

    return VK_SUCCESS;
}