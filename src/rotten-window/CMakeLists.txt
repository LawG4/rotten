############################################################################
# Adds in the rotten window abstraction layer to connect to the ui
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten-window OR NOT ${ROTTEN_PROJECT_WINDOW})
	return()
endif()

project(rotten-window VERSION 0.1.0 LANGUAGES C)
message(STATUS "Adding rotten window")
add_library(rotten-window 
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten/rotten-window.h
	init/connect.c
	init/window-init.c
	init/show.c
	events/poll.c
	vulkan/vksurface.c) 

# Add in rotten core
add_subdirectory(../rotten-core core)
target_link_libraries(rotten-window PRIVATE rotten-core)
target_include_directories(rotten-window PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten)

# Add in the windowing system loaders for linux. Each individual windowing system can be 
# excluded, if they have been excluded then set the corrosponding compile definition
add_subdirectory(../rotten-xcb xcb)
add_subdirectory(../rotten-wayland wayland)
if(TARGET rotten-xcb)
	target_link_libraries(rotten-window PUBLIC rotten-xcb)
else()
	target_compile_definitions(rotten-window PRIVATE ROTTEN_WINDOW_EXCLUDE_XCB)
endif()

if(TARGET rotten-wayland)
	target_link_libraries(rotten-window PUBLIC rotten-wayland)
else()
	target_compile_definitions(rotten-window PRIVATE ROTTEN_WINDOW_EXCLUDE_WAYLAND)
endif()

# Add the examples directory 
if(${ROTTEN_PROJECT_EXAMPLES})
	add_subdirectory(examples)
endif()