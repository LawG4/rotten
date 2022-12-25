############################################################################
# Adds in the dynamic loader for xcb, used by the rotten window.
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten-xcb OR NOT ${ROTTEN_PROJECT_XCB} OR NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	return()
endif()
project(rotten-xcb VERSION 0.1.0 LANGUAGES C)
message(STATUS "Adding rotten xcb loader")
add_library(rotten-xcb
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten/rotten-xcb.h
	xcb.c)

# Add in rotten core
add_subdirectory(../rotten-core core)
target_link_libraries(rotten-xcb PRIVATE rotten-core)
target_include_directories(rotten-xcb PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten)

# No examples for rotten xcb 