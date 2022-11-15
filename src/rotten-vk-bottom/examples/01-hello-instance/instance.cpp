#include <iostream>
#include <vector>
#include "rotten/rotten-vk-bottom.h"
#include "vulkan/vulkan.h"

int main()
{
    // Dynamically load vulkan
    rotten_vkr_library vulkan_library{};
    rotten_success_code err = rotten_vk_start_runtime(&vulkan_library);
    if (err != e_rotten_success) {
        std::cout << "Failed to connect to the vulkan library" << std::endl;
        return -1;
    } else {
        std::cout << "Opened vulkan library with loader version ("
                  << VK_API_VERSION_MAJOR(vulkan_library.instance_loader_version) << ", "
                  << VK_API_VERSION_MINOR(vulkan_library.instance_loader_version) << ", "
                  << VK_API_VERSION_PATCH(vulkan_library.instance_loader_version) << ")" << std::endl;
    }
}
