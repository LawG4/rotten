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

# Add rotten core as a dependency
add_subdirectory(../../rotten-core core)

# Only include the simulator if the rotten_retro_target is not defined
if(NOT DEFINED rotten_retro_target)
	message(STATUS "Adding rotten retro gekko : sim")
	add_library(rotten-retro-gekko-sim)
	add_subdirectory(src/sim)  

	# Group the sources and link to core
	rotten_group_sources(rotten-retro-gekko-sim rotten)
	target_link_libraries(rotten-retro-gekko-sim PRIVATE rotten-core)
else()
	message(STATUS "Adding rotten retro gekko : native")
endif()

# Add in the examples at the request of the user 
if(ROTTEN_PROJECT_EXAMPLES)
	add_subdirectory(src/examples)
endif()
