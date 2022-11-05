############################################################################
# Rotten CMake Toolchain Wii
# More easily spins up the wii cmake toolchain provided by devkit pro
# Will search in the following places in order :
#     ${CMAKE_DEVKITPRO_DIR}
#     $ENV{PATH}
#     $ENV{DEVKITPRO}     
#     /opt/devkitpro     
#     C:/devkitpro/
#     C:/msys64/opt/devkitpro
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################

# Turn off the projects which don't make sense on the wii
set(ROTTEN_PROJECT_GFX OFF CACHE BOOL "Build the rotten graphics abstraction layer")
set(ROTTEN_PROJECT_GFX_VK OFF CACHE BOOL "Build the rotten graphics vulkan helpers")
set(ROTTEN_PROJECT_RETRO_GFX_SIM OFF CACHE BOOL "Build the rotten graphics simulators for retro apis")

# Add in the toolchain common, from this point on we won't need to use 
# absolute paths for rotten's cmake
include("${CMAKE_CURRENT_LIST_DIR}/toolchain-common.cmake")

# Tell Rotten build system we're building for either the wii or the gamecube
# The rotten_retro_target string has to match the folder structure. So we'll 
# define an extra cmake variable called rotten_retro_enable_wii
set(rotten_retro_target "gekko" CACHE STRING "Target platform being built for")
set(rotten_retro_enable_wii TRUE CACHE BOOL "Enable wii builds for gekko targets")

# Visual studio doesn't like wii dev toolchain
rotten_no_visual_studio()

# First check the variable that the user has parsed, have they parsed one?
# if they have then ensure that the directory exists before we use it!
if(DEFINED CMAKE_DEVKITPRO_DIR) 
	if(NOT EXISTS ${CMAKE_DEVKITPRO_DIR})
		rotten_toolchain_warn("Parsed a nonexisting path to devkitpro??")
	elseif(NOT EXISTS "${CMAKE_DEVKITPRO_DIR}/cmake/devkitPPC.cmake")
		rotten_toolchain_warn("Parsed an existing path to devkitpro, "
			"however you don't have a cmake module installed")
	else()
		set(DEVKITPRO_CMAKE_DIR "${CMAKE_DEVKITPRO_DIR}/cmake")
	endif()
endif()

# If the user has not defined a devkit path, or if they did pass one but 
# it wasn't valid
if(NOT DEFINED DEVKITPRO_CMAKE_DIR)
	find_file(PPC_TOOLCHAIN_FILE
		"cmake/devkitPPC.cmake"
		PATHS $ENV{DEVKITPRO} "/opt/devkitpro" "C:/devkitPro" 
		"C:/msys64/opt/devkitPro")

	if(${PPC_TOOLCHAIN_FILE} STREQUAL "PPC_TOOLCHAIN_FILE-NOTFOUND")
		message(FATAL_ERROR "Failed to find a devkitPro ppc cmake toolchain")
	endif()

	# If we got here then we found the fantastic cmake file
	get_filename_component(DEVKITPRO_CMAKE_DIR ${PPC_TOOLCHAIN_FILE} DIRECTORY)
endif()

# Just quickly check that the cmake dir is all good
if(NOT EXISTS ${DEVKITPRO_CMAKE_DIR}/devkitPPC.cmake)
	message(FATAL_ERROR "The found cmake folder \"${DEVKITPRO_CMAKE_DIR}\" "
		"doesn't exist or it doesn't contain a devkitPPC.cmake file")
endif()

# From the devkitPro Cmake directory, find the DevkitPro folder 
get_filename_component(DEVKITPRO_DIR ${DEVKITPRO_CMAKE_DIR} DIRECTORY)
list(APPEND CMAKE_PREFIX_PATH ${DEVKITPRO_DIR}/tools/bin)
rotten_toolchain_info("Found devkitPro toolchain at \"${DEVKITPRO_DIR}\"")

# We need to add this folder to the cmake module path so that common files can 
# be included 
list(APPEND CMAKE_MODULE_PATH ${DEVKITPRO_CMAKE_DIR})
include("${DEVKITPRO_CMAKE_DIR}/Wii.cmake")

# turn off the toolchain messages to avoid repeated info logs
rotten_toolchain_msg_off()