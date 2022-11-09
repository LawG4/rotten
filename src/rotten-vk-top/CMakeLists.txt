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
if(TARGET rotten-vk-top OR NOT ${ROTTEN_PROJECT_VK_TOP})
	return()
endif()
project(rotten-vk-top VERSION 0.1.0 LANGUAGES C)
message(STATUS "Adding rotten vulkan top level")

# The vulkan top level depends on the vulkan bottom level, so ensure to 
# add that as a dependency - through CMake library inheritence, this will
# also become linked with rotten core 
add_subdirectory(../rotten-vk-bottom vk-bottom)

# define the sources for the vulkan top library
add_library(rotten-vk-top 
	${CMAKE_CURRENT_LIST_DIR}/startup/instance.c)

# Link the dependencies 
rotten_group_sources(rotten-vk-top rotten)
target_link_libraries(rotten-vk-top PUBLIC rotten-vk-bottom)