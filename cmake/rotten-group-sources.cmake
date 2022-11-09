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

	# Iterate over all of the sources 
	foreach(source ${target_sources})
		
		# If the path is relative, then make it absolute
		if(NOT IS_ABSOLUTE ${source})
			set(source ${target_root_dir}/${source})
		endif()
		# Actually group the files
		source_group(TREE "${target_root_dir}" PREFIX source FILES ${source})
		
	endforeach()

endfunction()

# The headers take such an seperate direction from the rest of the source files so 
# we make sure to add them seperatley, this way the ide can still group them without 
# too much hassle from us trying to sort everything int src folders etc..
function(rotten_add_public_headers target_name sources)
	# Use the interface library at the root of the repository to get the path there 
	get_target_property(rotten_root_dir rotten SOURCE_DIR)

	# Group them all into the same folder, requires the user gives the path relative
	# to /include/rotten
	foreach(source ${sources})
		set(full_source ${rotten_root_dir}/inc/rotten/${source})
		target_sources(${target_name} PRIVATE ${full_source})
		source_group(TREE "${rotten_root_dir}/inc/rotten" PREFIX "public-include" 
			FILES ${full_source})
	endforeach()
endfunction()