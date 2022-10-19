############################################################################
# Rotten CMake Toolchain Common
# Provides a useful set of tools to be used in all the different toolchain
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################

# Add in the common file, from this point we won't need to use absoute paths
# for rotten's cmake modules
include("${CMAKE_CURRENT_LIST_DIR}/rotten-common.cmake")

# Visual studio doesn't support most toolchains, so provide a function to fail
# when the user is using visual studio.
function(rotten_no_visual_studio)
	if(CMAKE_GENERATOR MATCHES "Visual Studio")
		message(FATAL_ERROR "Rotten : Visual studio doesn't work with this "
			"toolchain, your best bet is to use ninja and VS Code")
	endif()
endfunction()

# A set of logging function. The toolchain runs for each language discovered 
# this is annoying as it produces double the toolchain output. So provide a 
# function which will log info and warnings. It won't provide an error 
# equivelant as they should always fire.
if(NOT DEFINED rotten_toolchain_msg) 
	set(rotten_toolchain_msg ON CACHE BOOL "Controls when toolchain messages show")
endif()

function(rotten_toolchain_msg_off)
	set(rotten_toolchain_msg OFF CACHE BOOL "Controls when toolchain messages show" FORCE)
endfunction()

function(rotten_toolchain_info msg) 
	if(${rotten_toolchain_msg})
		message(STATUS ${msg})
	endif()
endfunction()

function(rotten_toolchain_warn msg) 
	if(${rotten_toolchain_msg})
		message(STATUS ${msg})
	endif()
endfunction()