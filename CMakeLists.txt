#-------------------------------------------------------
# Copyright (c) NSDM
# All rights reserved.
# History:
# 		<Author>	<Time>		<Version>	  <Des>
#     		
#-------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(NSDM)  
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)

#-------------------------------------------------------------------------------------------------------------------
set(PROJDIR 										${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH  						${NSDM_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  							${NSDM_BINARY_DIR}/bin)
set(LIBS 											${LIBRARY_OUTPUT_PATH};${PROJDIR}/3rdParty/lib)
link_directories(${LIBS})

#-------------------------------------------------------------------------------------------------------------------
option(USE_ALLMT_ "Compile all projects as mt"      ON)
option(USE_DMDLL_ "Compile DmMain as dll"  			OFF) 
if(USE_DMDLL_)
	add_definitions(-DDLL_DMMAIN)
endif()

#-------------------------------------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${PROJDIR}/Tools/CMake)
include(PrecompiledHeader)# 添加CMake的模块文件的查找路径

#-------------------------------------------------------------------------------------------------------------------
set(ZLIB_BUILD_SHARED OFF CACHE BOOL "Build zlib shared library" FORCE)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "Build zlib examples" FORCE)
set(ZLIB_INSTALL OFF CACHE BOOL "Create installation targets (chain builders may want to disable this)" FORCE)
ADD_SUBDIRECTORY(${PROJDIR}/3rdParty/zlib)
add_subdirectory(${PROJDIR}/DMMain)
add_subdirectory(${PROJDIR}/nsCurl)
add_subdirectory(${PROJDIR}/nsUtil)
add_subdirectory(${PROJDIR}/nsDM)