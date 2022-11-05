############################################################################
# Rotten CMake Common
# Groups the rotten projects into folders which will be useful 
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Takes the name of a rotten target and sorts all of the files into 
# hopefully cool source groups, it is recommended you only use this on 
# internal rotten projects as it uses rotten root project and the expected 
# include and source path splitting  
#
# params :
#    target_name - The name of the target to source
#    folder_path - Folder to group the folders into  
function(rotten_group_sources target_name folder_path)
	# First ensure that the target actually exists 
	if(NOT TARGET ${target_name})
		message(FATAL_ERROR "Error : Target named \"${target_name}\" does "
			"not exist. So it cannot be sorted into source groups")
	endif()

	# Now place that target inside the folder 
	set_target_properties(${target_name} PROPERTIES FOLDER ${folder_path})

	# Fetch the source directory (i.e. the directory where the add_lib/exe)
	# was called for this target, this can be used to cut off the execess
	# file paths on the sources
	get_target_property(target_root_dir ${target_name} SOURCE_DIR)
	get_target_property(target_sources ${target_name} SOURCES)
	get_target_property(rotten_root_dir rotten SOURCE_DIR)


	# Iterate over all of the sources so they can be placed into the right 
	# source group by parsing the file path 
	foreach(source ${target_sources})
		# Find file path local to the cmake list folder
		file(RELATIVE_PATH relative_source ${target_root_dir} ${source})
		
		# If it begins with the src folder then we can add it directly 
		# else we asume it starts with ../ and refers to the include files
		if(${relative_source} MATCHES "^src/")
			source_group(TREE "${target_root_dir}/src" PREFIX source FILES ${source})
		else()
			source_group(TREE "${rotten_root_dir}/inc/rotten" PREFIX "public-include" FILES ${source})
		endif()
		
	endforeach()

endfunction()