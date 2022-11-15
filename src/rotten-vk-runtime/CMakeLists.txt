############################################################################
# Adds the Vulkan dynamic loader to make things more portable
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Check that this project should be added, when the project is enabled and 
# hasn't already been added by another project
if(TARGET rotten-vk-runtime OR NOT ${ROTTEN_PROJECT_VK_RUNTIME})
	return()
endif()
project(rotten-vk-runtime VERSION 0.1.0 LANGUAGES C)
message(STATUS "Adding rotten vulkan runtime loader")

# Add rotten core as a dependency - for debug logging etc
add_subdirectory(../rotten-core core)

# Define the source files which make up the library
add_library(rotten-vk-runtime 
	startup/library.c)

# Link the rotten dependencies
target_link_libraries(rotten-vk-runtime PRIVATE rotten-core)
rotten_group_sources(rotten-vk-runtime rotten)
rotten_add_public_headers(rotten-vk-runtime "rotten-vk-runtime.h")
target_include_directories(rotten-vk-runtime PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../../inc/")

# TODO : Remove this 
# Find and link to the external dependencies 
# The end goal is to remove all of these eventually for example the Vulkan 
# headers can be generated from the registry, from there we could even 
# provide our own version of the headers, then dynamically link to Vulkan 
find_package(Vulkan QUIET REQUIRED) 
target_link_libraries(rotten-vk-runtime PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(rotten-vk-runtime PUBLIC ${Vulkan_INCLUDE_DIRS})