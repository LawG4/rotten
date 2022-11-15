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
endforeach()