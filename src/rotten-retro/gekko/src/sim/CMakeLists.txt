############################################################################
# Rotten Retro - Gekko
# Builds the simulation layers for Wii and Gamecube 
#
# LICENSE  : MIT
# Copyrite : Rotten
# Authors  : Lawrence G,
############################################################################
cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# The rotten sim target has already been defined in the above folder
get_target_property(rotten_root_dir rotten SOURCE_DIR)


target_sources(rotten-retro-gekko-sim PRIVATE
	${rotten_root_dir}/inc/rotten/rotten-retro/gekko/gccore.h
	${rotten_root_dir}/inc/rotten/rotten-retro/gekko/platform.h
	${rotten_root_dir}/inc/rotten/rotten-retro/gekko/ogc/gx.h
	${rotten_root_dir}/inc/rotten/rotten-retro/gekko/ogc/system.h
	${rotten_root_dir}/inc/rotten/rotten-retro/gekko/ogc/video.h
	${CMAKE_CURRENT_LIST_DIR}/ogc-internal.h
	${CMAKE_CURRENT_LIST_DIR}/gx.c
	${CMAKE_CURRENT_LIST_DIR}/system.c
	${CMAKE_CURRENT_LIST_DIR}/video.c)