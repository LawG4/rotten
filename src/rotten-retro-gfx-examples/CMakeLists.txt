############################################################################
# Adds in all of the retro graphics samples
# When building on desktop this will add in the simulation layers for all of 
# the samples. When building natively for the retro system this will select
# the original graphics api and only build for that system.
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Check that this project should be added, when the project is enabled and 
# hasn't already been added by another project
if(TARGET rotten-retro-gfx-examples OR NOT ${ROTTEN_PROJECT_RETRO_GFX_EXAMPLES})
	return()
endif()
add_library(rotten-retro-gfx-examples INTERFACE)
message(STATUS "Adding rotten retro gfx examples")

# Defining the rotten retro string is what is used to differentiate which 
# retro platform is being used. It is used in every toolchain file added by 
# rotten. If it not defined then assume desktop
if(NOT DEFINED rotten_retro_target)
	add_subdirectory(../rotten-retro-gfx-sim sim)
	add_subdirectory(src/gx-wii)
else()

	# Select the correct rotten retro platform
	if(${rotten_retro_target} STREQUAL "WII")
		add_subdirectory(src/gx-wii)
	else()
		message(FATAL_ERROR "Unknown retro target \"${rotten_retro_target}\"")
	endif()

endif()

