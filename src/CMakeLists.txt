############################################################################
# Adds in all the subprojects 
# The options which control the projects being added will be handelled in 
# the project itself
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################

set(rotten_projects
	rotten-core
	rotten-gfx
	rotten-gfx-vk
	rotten-retro-gfx-examples
	rotten-retro-gfx-sim)

foreach(project ${rotten_projects})
	add_subdirectory(${project})
endforeach()

