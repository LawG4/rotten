############################################################################
# Rotten Retro Graphics Simulation
# Builds the simulation layers for desktop
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# When rotten_retro_target is defined as a cmake variable then we don't build
# any of the simuation layers. When this cmake variable is not defined then 
# we add all of the subdirectories, and let the project option decide.
if(DEFINED rotten_retro_target)
	return()
endif()

# Add in the Wii/Gamecube simulation layer
add_subdirectory(src/gekko)
