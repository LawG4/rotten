############################################################################
# Adds in the wii graphics api examples
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
message(STATUS "Adding rotten retro gfx examples for wii")

# Add all the examples
add_executable(wii-01-clear-color 01-clear-color/clear-color.c)

# On Native Wii builds the devkitPro toolchain has automatically  added all
# the compilation flags, linker flags and include directories. So we only 
# need to wory about adding the simulation layer on desktop builds.
if(NOT DEFINED rotten_retro_target)
	add_subdirectory(../rotten-retro-gfx-sim gfx-sim)

	set(wii_examples 
		wii-01-clear-color)

	foreach(example ${wii_examples})
		target_link_libraries(${example} PRIVATE rotten-wii)
	endforeach()

endif()
