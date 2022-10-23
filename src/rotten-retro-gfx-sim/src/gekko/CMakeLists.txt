############################################################################
# Rotten Retro Graphics Simulation - Gekko
# Builds the simulation layers for Wii and Gamecube
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Check if this project should be added, since the parent directory checked
# for the rotten_retro_target define, then we only need to check if this 
# specific project was enabled or if it has already been added
if(TARGET rotten-retro-gfx-sim-gekko OR NOT ${ROTTEN_PROJECT_RETRO_GFX_SIM_GEKKO})
	return()
endif()

if(DEFINED rotten_retro_target)
	message(FATAL_ERROR "Added in a retro simulation layer while performing a "
		"native build for retro target \"${rotten_retro_target}\"")
endif()

project(rotten-retro-gfx-gekko VERSION 0.1.0 LANGUAGES CXX C)
message(STATUS "Adding rotten retro gfx sim : gekko")
add_library(rotten-retro-gfx-sim-gekko)
target_include_directories(rotten-retro-gfx-sim-gekko PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}/../../../../inc/rotten/rotten-retro-gfx-sim/gekko)

target_sources(rotten-retro-gfx-sim-gekko PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/../../../../inc/rotten/rotten-retro-gfx-sim/gekko/gccore.h
	${CMAKE_CURRENT_LIST_DIR}/../../../../inc/rotten/rotten-retro-gfx-sim/gekko/ogc/video.h
	${CMAKE_CURRENT_LIST_DIR}/src/ogc-internal.h
	${CMAKE_CURRENT_LIST_DIR}/src/video.c)

# Add the examples
if(${ROTTEN_PROJECT_EXAMPLES})
	add_subdirectory(../../../rotten-retro-gfx-examples/src/gekko examples)
endif()