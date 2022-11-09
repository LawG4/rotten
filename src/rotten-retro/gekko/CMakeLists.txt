############################################################################
# Rotten Retro - Gekko
# Builds the simulation layers for Wii and Gamecube, or can build native 
# gekko targets when the rotten_retro_target is defined 
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Ensure this only gets added once
if(TARGET rotten-retro-gekko)
	return()
endif()
project(rotten-retro-gekko VERSION 0.1.0 LANGUAGES C)
add_library(rotten-retro-gekko INTERFACE)

# If we have defined the rotten_retro_target variable then we are 
# specifically on native hardware, so we only need to add the examples 
# and exit
if(DEFINED rotten_retro_target)
	if(ROTTEN_PROJECT_EXAMPLES)
		add_subdirectory(examples)
	endif()
	return()
endif() 

# If we got this far then we know we are building the rotten gekko simulator
message(STATUS "Adding rotten retro gekko : sim")
add_subdirectory(../../rotten-core core)

# define the sources for the gekko sim target
add_library(rotten-retro-gekko-sim 
	${CMAKE_CURRENT_LIST_DIR}/ogc-internal.h
	${CMAKE_CURRENT_LIST_DIR}/gx.c
	${CMAKE_CURRENT_LIST_DIR}/system.c
	${CMAKE_CURRENT_LIST_DIR}/video.c)

# Group the sources and link to core
rotten_group_sources(rotten-retro-gekko-sim rotten)
target_link_libraries(rotten-retro-gekko-sim PRIVATE rotten-core)

# Add the public include directories
set(public_headers 
	rotten-retro/gekko/gccore.h
	rotten-retro/gekko/platform.h
	rotten-retro/gekko/ogc/gx.h
	rotten-retro/gekko/ogc/system.h
	rotten-retro/gekko/ogc/video.h)
rotten_add_public_headers( rotten-retro-gekko-sim "${public_headers}")

# We need to update the include paths for this project an extra time
# to keep the include paths in line with the native gekko projects
get_target_property(rotten_root_dir rotten SOURCE_DIR)
target_include_directories(rotten-retro-gekko-sim PUBLIC 
	${rotten_root_dir}/inc/rotten/rotten-retro/gekko)

# Add in the examples at the request of the user 
if(ROTTEN_PROJECT_EXAMPLES)
	add_subdirectory(examples)
endif()
