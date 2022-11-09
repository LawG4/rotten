############################################################################
# Adds the Vulkan backend for Rotten GFX and the Vulkan helper library as 
# seperate projects
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Check that this project should be added, when the project is enabled and 
# hasn't already been added by another project
if(TARGET rotten-vk-bottom OR NOT ${ROTTEN_PROJECT_VK_BOTTOM})
	return()
endif()
project(rotten-vk-bottom VERSION 0.1.0 LANGUAGES C)
message(STATUS "Adding rotten vulkan bottom level")

# Add rotten core as a dependency - for debug logging etc
add_subdirectory(../rotten-core core)

# Define the source files which make up the library
add_library(rotten-vk-bottom
	${CMAKE_CURRENT_LIST_DIR}/startup/instance.c)

# Link the rotten dependencies
rotten_group_sources(rotten-vk-bottom rotten)
target_link_libraries(rotten-vk-bottom PUBLIC rotten_core)

# TODO : Remove this 
# Find and link to the external dependencies 
# The end goal is to remove all of these eventually for example the Vulkan 
# headers can be generated from the registry, from there we could even 
# provide our own version of the headers, then dynamically link to Vulkan 
find_package(Vulkan QUIET REQUIRED) 
target_link_libraries(rotten-vk-bottom PUBLIC ${Vulkan_LIBRARIES})
target_include_directories(rotten-vk-bottom PUBLIC ${Vulkan_INCLUDE_DIRS})