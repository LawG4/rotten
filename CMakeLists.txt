############################################################################
# Rotten MonoRepo CMake 
# Builds the entirety of the rotten toolsets
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten)
	return()
endif()

project(rotten-monorepo VERSION 0.0.1 LANGUAGES C)
add_library(rotten INTERFACE)

message("Rotten monorepo build system")
message("Target : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message("Host   : ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# Allow the users to configure which rotten projects are built
option(ROTTEN_PROJECT_EXAMPLES           "Build the rotten examples for all projecsts" ON)
option(ROTTEN_PROJECT_GFX                "Build the rotten graphics abstraction layer" ON)
option(ROTTEN_PROJECT_GFX_VK             "Build the rotten graphics vulkan helpers" ON)
option(ROTTEN_PROJECT_RETRO_GFX_EXAMPLES "Build the rotten retro graphics api examples" ON)
option(ROTTEN_PROJECT_RETRO_GFX_SIM      "Build the rotten graphics simulators for retro apis" ON)

# Jump into the source directory to add in all of the different subprojects
add_subdirectory(src)