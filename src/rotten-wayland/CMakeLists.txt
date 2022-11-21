############################################################################
# Adds in the dynamic loader for wayland, used by the rotten window.
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten-wayland OR NOT ${ROTTEN_PROJECT_WAYLAND} OR NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	return()
endif()

project(rotten-wayland VERSION 0.1.0 LANGUAGES C)
message(STATUS "Adding rotten wayland loader")
add_library(rotten-wayland
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten/rotten-wayland.h
	wayland.c)

# Add in rotten core
add_subdirectory(../rotten-core core)
target_link_libraries(rotten-wayland PRIVATE rotten-core)
target_include_directories(rotten-wayland PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten)

# No examples for rotten wayland