############################################################################
# Adds in the examples for the rotten core library
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
if(TARGET 01-rotten-core-log-c)
	return()
endif()

project(rotten-core-examples VERSION 0.1.0 LANGUAGES CXX C)
add_executable(01-rotten-core-log-c 01-rotten-log/rotten-log.c)
add_executable(01-rotten-core-log-cpp 01-rotten-log/rotten-log.cpp)

set(examples 
	01-rotten-core-log-c
	01-rotten-core-log-cpp)

foreach(example ${examples})
	target_link_libraries(${example} PRIVATE rotten-core)
endforeach()