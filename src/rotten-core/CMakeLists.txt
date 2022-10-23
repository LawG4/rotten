############################################################################
# Adds in the rotten core library
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################

# Check if this cmake list should be added. Rotten core is always added unless
# the target has already been added
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten-core)
	return()
endif()

# Define the rotten core project
project(rotten-core VERSION 0.1.0 LANGUAGES C)
message(STATUS "Adding rotten core")
add_library(rotten-core
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten/rotten-core.h
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten/rotten-enum.h
	${CMAKE_CURRENT_LIST_DIR}/src/rotten-core-internal.h
	${CMAKE_CURRENT_LIST_DIR}/src/log.c)

target_include_directories(rotten-core PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/../../inc/rotten)

set_target_properties(rotten-core PROPERTIES LINKER_LANGUAGE C)

if(${ROTTEN_PROJECT_EXAMPLES})
	add_subdirectory(src/examples)
endif()