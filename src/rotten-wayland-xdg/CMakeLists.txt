############################################################################
# Adds in the dynamic xdg component by compiling it into a shared library 
# so rotten window can launch without directly linking to wayland
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten-wayland-xdg OR NOT TARGET rotten-wayland)
	return()
endif()
project(rotten-wayland-xdg VERSION 0.0.1 LANGUAGES C)
message(STATUS "Adding rotten wayland xdg support")

# Check if the private xdg sources have been created or not.
if(NOT EXISTS ${CMAKE_CURRENT_LIST_DIR}/generated/xdg-shell-protocol.c)
	# Check if wayland scanner exists 
	find_program(WL_SCANNER "wayland-scanner")
	if(NOT WL_SCANNER)
		message(FATAL_ERROR "Unable to find wayland scanner to generate " 
			"wayland xdg source files, either install wayland-scanner or "
			"disable rotten wayland with -DROTTEN_PROJECT_WAYLAND=OFF")
	endif()

	# Ensure the protocols exist 
	if(NOT EXISTS "/usr/share/wayland-protocols")
		message(FATAL_ERROR "Unable to find wayland protocols, either "
			"download them or skip wayland support for rotten window "
			"with -DRotten_PROJECT_WAYLAND=OFF")
	endif()

	# Now ensure the generated directory exists 
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/generated")

	# Now use wayland scanner to generate the source files
	execute_process(COMMAND ${WL_SCANNER} 
		"private-code"
		"/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml"
		"xdg-shell-protocol.c"
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/generated"
		RESULT_VARIABLE WL_RESULT)
	if(NOT ${WL_RESULT} EQUAL 0)
		message(FATAL_ERROR "Failed to generated source")
	endif()

	execute_process(COMMAND ${WL_SCANNER} 
		"client-header"
		"/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml"
		"xdg-shell-client-protocol.h"
		WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/generated"
		RESULT_VARIABLE WL_RESULT)	
	if(NOT ${WL_RESULT} EQUAL 0)
		message(FATAL_ERROR "Failed to generated header")
	endif()
endif()

# Okay! now we have generated the files add them to a build system!
add_library(rotten-wayland-xdg SHARED 
	generated/xdg-shell-protocol.c
	generated/xdg-shell-client-protocol.h)

target_link_libraries(rotten-wayland-xdg PRIVATE "wayland-client")