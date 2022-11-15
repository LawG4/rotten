#include "rotten-vk-runtime.h"

#ifdef WIN32
#include <Windows.h>
#endif  // WIN32

/**
 * @brief Opens the dynamic library handle to vulkan, this is reference counted per process id by the
 * operating system, so it's fine if the user and us open it multiple times
 * @returns pointer to the handle, or NULL on failure
 */
void* open_os_handle()
{
    rotten_log_debug("Attempting to open Vulkan dynamic library", e_rotten_log_verbose);

#ifdef WIN32
    // Load the library with LoadLibraryA instead of LoadLibraryEX, as the vulkan library does some opening of
    // it's own in dllmain. Windows also directly returns null if it can't find the vulkan dll
    return LoadLibraryA(TEXT("vulkan-1.dll"));

#else
    rotten_log_debug("Unhandled operating system attempting to open vulkan dynamic library",
                     e_rotten_log_error);
    return NULL;
#endif  // OS DEPENDENT
}

PFN_vkGetInstanceProcAddr fetch_library_entry_handle(void* handle)
{
    rotten_log_debug("Attempting to find the vkInstanceGetProcAddress from the handle", e_rotten_log_verbose);

#ifdef WIN32
    return (PFN_vkGetInstanceProcAddr)GetProcAddress((HMODULE)handle, "vkGetInstanceProcAddr");

#else
    rotten_log_debug(
      "Operating system opened a Vulkan library handle, but didn't have an implementation to fetch get "
      "procaddress? How did this even happen???",
      e_rotten_log_error);
    return NULL;
#endif  // OS DEPENDENT
}

rotten_success_code rotten_vk_start_runtime(rotten_vkr_library* lib)
{
    // First try to open the library, this will depend on the os
    lib->os_library_handle = open_os_handle();
    if (lib->os_library_handle == NULL) {
        rotten_log("Failed to open the Vulkan dynamic library", e_rotten_log_error);
        return e_rotten_library_not_present;
    }

    // Now get the instance procaddress function which can be used to get all the other functions
    lib->get_instance_proc_addr = fetch_library_entry_handle(lib->os_library_handle);
    if (lib->get_instance_proc_addr == NULL) {
        rotten_log("Failed to get an instance proc address after successfully opening the vulkan library",
                   e_rotten_log_error);
        return e_rotten_library_not_present;
    }

    // Use the instance procaddress to load all the other functions
    lib->enumerate_instance_extension_properties =
      (PFN_vkEnumerateInstanceExtensionProperties)lib->get_instance_proc_addr(
        VK_NULL_HANDLE, "vkEnumerateInstanceExtensionProperties");

    lib->enumerate_instance_layer_properties =
      (PFN_vkEnumerateInstanceLayerProperties)lib->get_instance_proc_addr(
        VK_NULL_HANDLE, "vkEnumerateInstanceLayerProperties");

    lib->enumerate_instance_version = (PFN_vkEnumerateInstanceVersion)lib->get_instance_proc_addr(
      VK_NULL_HANDLE, "vkEnumerateInstanceVersion");

    lib->create_instance =
      (PFN_vkCreateInstance)lib->get_instance_proc_addr(VK_NULL_HANDLE, "vkCreateInstance");

    // Get the maximum instance version supported by the loader
    if (lib->enumerate_instance_version == NULL) {
        lib->instance_loader_version = VK_MAKE_API_VERSION(0, 1, 0, 0);
    } else {
        lib->enumerate_instance_version(&lib->instance_loader_version);
    }

    return e_rotten_success;
}
