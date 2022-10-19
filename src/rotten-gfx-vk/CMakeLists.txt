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
if(TARGET rotten-gfx-vk OR NOT ${ROTTEN_PROJECT_GFX_VK})
	return()
endif()
add_library(rotten-gfx-vk INTERFACE)
message(STATUS "Adding rotten gfx vulkan backend")