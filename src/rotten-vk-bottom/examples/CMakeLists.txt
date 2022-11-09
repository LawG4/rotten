############################################################################
# Adds in the VK bottom level examples
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET rotten-vk-bottom-01-hello-instance-c)
	return()
endif() 
message(STATUS "Adding rotten vk bottom : examples")

add_executable(rotten-vk-bottom-01-hello-instance-c 01-hello-instance/instance.c)
add_executable(rotten-vk-bottom-01-hello-instance-cpp 01-hello-instance/instance.cpp)

set(examples_list 
	rotten-vk-bottom-01-hello-instance-c
	rotten-vk-bottom-01-hello-instance-cpp)

foreach(example ${examples_list})
	target_link_libraries(${example} PRIVATE rotten-vk-bottom)
	set_target_properties(${example} PROPERTIES FOLDER "rotten/examples")
endforeach()