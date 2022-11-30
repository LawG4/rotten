############################################################################
# Adds in the examples for the rotten window library
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten-window-01-hello)
	return()
endif()

project(rotten-window-examples VERSION 0.1.0 LANGUAGES C)
add_executable(rotten-window-01-hello 01-hello-window/hello-window.c)

set(examples 
    rotten-window-01-hello)

foreach(example ${examples})
	target_link_libraries(${example} PRIVATE rotten-window)
	set_target_properties(${example} PROPERTIES FOLDER "rotten/examples")

	# If the file has a rotten-wayland component then we need to copy the rotten-wayland-xdg
	# shared library into the binary output directory of the example so it can be loaded at runtime
	if(TARGET rotten-wayland-xdg)
		add_custom_command(TARGET ${example} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:rotten-wayland-xdg>"
			"$<TARGET_FILE_DIR:${example}>"
			COMMENT "Copying rotten-wayland-xdg to ${example}"
			DEPENDS rotten-wayland-xdg)
	endif()
endforeach()