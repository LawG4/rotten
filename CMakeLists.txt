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

# Add in all the cmake module paths and functions used by rotten 
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(rotten-common)

# Make the cmake predefined targets like ALL_BUILD and ZERO_CHECK into a folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

message("Rotten monorepo build system")
message("Target : ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message("Host   : ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_PROCESSOR}")

# Allow the users to configure which rotten projects are built
option(ROTTEN_PROJECT_EXAMPLES   "Build the rotten examples for all projecsts" ON)
option(ROTTEN_PROJECT_GFX        "Build the rotten graphics abstraction layer" ON)
option(ROTTEN_PROJECT_RETRO      "Build the rotten retro targets" ON)
option(ROTTEN_PROJECT_VK_BOTTOM  "Build the rotten vulkan low level helpers" ON)
option(ROTTEN_PROJECT_VK_RUNTIME "Build the rotten vulkan runtime loader" ON)
option(ROTTEN_PROJECT_VK_TOP     "Build the rotten vulkan high level helpers" ON)
option(ROTTEN_PROJECT_WINDOW   "Build the rotten windowing system abstraction" ON)

# Enable the compile commands.json to be automatically generated
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "" FORCE)

# Jump into the source directory to add in all of the different subprojects
add_subdirectory(src) 