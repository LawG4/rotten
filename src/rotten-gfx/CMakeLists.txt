############################################################################
# Adds the Rotten GFX sub project into the cmake build system
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Check that this project should be added, when the project is enabled and 
# hasn't already been added by another project
if(TARGET rotten-gfx OR NOT ${ROTTEN_PROJECT_GFX})
	return()
endif()
add_library(rotten-gfx INTERFACE)
message(STATUS "Adding rotten gfx")