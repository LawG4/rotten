############################################################################
# Adds in the wii graphics api examples
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################

# Check that this cmake list should be added, which is only if it hasn't 
# already been added 
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET gekko-01-clear-color)
	return()
endif()

# Add all the examples
add_executable(gekko-01-clear-color 01-clear-color/clear-color.c)

# On Native Wii builds the devkitPro toolchain has automatically  added all
# the compilation flags, linker flags and include directories. So we only 
# need to wory about adding the simulation layer on desktop builds.
if(NOT DEFINED rotten_retro_target)
	add_subdirectory(../../../rotten-retro-gfx-sim/src/gekko gfx-sim)

	set(gekko_examples 
		gekko-01-clear-color)

	foreach(example ${gekko_examples})
		target_link_libraries(${example} PRIVATE rotten-retro-gfx-sim-gekko)
	endforeach()

endif()
