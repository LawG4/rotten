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

# Since these are examples, if the user has switched off the examples option
# exit out of any attempts to add this directory. If the rotten_retro_target
# is not defined by cmake then we're on desktop and which samples are added 
# is decided by which simulation layers are added
if(NOT DEFINED rotten_retro_target OR NOT ROTTEN_PROJECT_EXAMPLES)
	return()
endif()

# Now we know that the rotten retro target has been defined and the examples
# option is switched on, so add in the corrosponding platforms examples
if(${rotten_retro_target} STREQUAL "WII" OR ${rotten_retro_target} STREQUAL "CUBE")
	add_subdirectory(src/gekko)
else()
	message(FATAL_ERROR "Unknown retro target \"${rotten_retro_target}\"")
endif()
