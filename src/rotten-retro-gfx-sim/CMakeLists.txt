############################################################################
# Rotten Retro Graphics Simulation
# Builds the simulation layers for desktop
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Check that this project should be added, when the project is enabled and 
# hasn't already been added by another project
if(TARGET rotten-retro-gfx-sim OR NOT ${ROTTEN_PROJECT_RETRO_GFX_SIM})
	return()
endif()

add_library(rotten-retro-gfx-sim INTERFACE)
