############################################################################
# Rotten CMake Common
# Provides a useful set of tools to be whenever using CMake
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################

# Whenever this file is included, add the directory containing it to the 
# CMake module path, that makes it waaaay easier for users to include any
# of the other cmake tools provided by rotten
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} CACHE INTERNAL "")

# Sometimes programs on cmake won't work if they don't have a .exe file 
# suffix, I'm gonna add a function to do that automatically
# WARNING This function does not care if you actually need an exe extension
# for your platform 
function(rotten_append_exe var_name)
	if(NOT DEFINED ${var_name})
		return()
	endif()

	set(var_value ${${var_name}})
	if(NOT ${var_value} MATCHES ".*\\.exe")
		set(var_value "${var_value}.exe")
	endif()

	# Return the value out to the user 
	set(${var_name} ${var_value} PARENT_SCOPE)
endfunction()