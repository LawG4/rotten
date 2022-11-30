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
	wayland/wayland.c
	wayland/wl-inline-replacement.c
	wayland/interface-proxies.c)

# Add in rotten core
add_subdirectory(../rotten-core core)
target_link_libraries(rotten-wayland PRIVATE rotten-core)
target_include_directories(rotten-wayland PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten)

# Add in the rotten-wayland-xdg
# But we don't link to it, we just need it in the build system
add_subdirectory(../rotten-wayland-ext wayland-ext)

# No examples for rotten wayland