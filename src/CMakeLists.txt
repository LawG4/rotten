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
	rotten-retro
	rotten-vk-bottom
	rotten-vk-top)

foreach(project ${rotten_projects})
	add_subdirectory(${project})
endforeach()

