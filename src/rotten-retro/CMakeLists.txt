############################################################################
# Rotten Retro Graphics Simulation
# Builds the simulation layers for desktop
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Ensure that this build only gets added once when requested by the user 
if(TARGET rotten-retro OR NOT ${ROTTEN_PROJECT_RETRO})
	return()
endif()
project(rotten-retro VERSION 0.1.0 LANGUAGES C)
add_library(rotten-retro INTERFACE)

# Select when to build the simulation layers or not. When building for a 
# native retro target, the rotten cmake toolchains define a rotten_retro_target
# cmake variable. If this variable isn't defined then we're building for 
# desktop, so add in the simualation layers, which means adding in all of the 
# possible retro targets.
#
# If the rotten_retro_target is defined then we're trying to build for a native
# platform, so only add on that one project  
if(DEFINED rotten_retro_target)
	add_subdirectory(${rotten_retro_target})
else()
	add_subdirectory(gekko)
endif()


