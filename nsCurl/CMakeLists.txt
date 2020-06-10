#-------------------------------------------------------
# Copyright (c) NSDM
# All rights reserved.
# History:
# 		<Author>	<Time>		<Version>	  <Des>
#      guoyou		2019-7-26	1.0
#-------------------------------------------------------
PROJECT(nsCurl)

#-------------------------------------------------------------------------------------------------------------------
file(GLOB ROOT_HEADER 									inc/*h;inc/*.rc)
file(GLOB ROOT_SOURCE 									src/*.cpp;src/*.c)

file(GLOB COMMON_SOURCE     						${PROJDIR}/3rdParty/common/src/Utils.cpp)
file(GLOB NSIS_SOURCE     				${PROJDIR}/3rdParty/nsis/pluginapi.c)

source_group("inc" 								      FILES 	${ROOT_HEADER})
source_group("src"								      FILES 	${ROOT_SOURCE})

source_group("src\\common"							FILES 	${COMMON_SOURCE})
source_group("src\\nsis"							  FILES 	${NSIS_SOURCE})

#-------------------------------------------------------------------------------------------------------------------
use_precompiled_header(nsCurl "${CMAKE_CURRENT_SOURCE_DIR}/inc/StdAfx.h"
								              "${CMAKE_CURRENT_SOURCE_DIR}/src/StdAfx.cpp")
                              

#-------------------------------------------------------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc
                    )                              
INCLUDE_DIRECTORIES(${PROJDIR}/3rdParty/easy_libcurl/include
                    ${PROJDIR}/3rdParty/
                    ${PROJDIR}/3rdParty/common/include)

#-------------------------------------------------------------------------------------------------------------------
add_definitions(-DUNICODE -D_UNICODE)  # unicode版本

# MT模式
SET(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")# 直接抄gui的默认，然后 改成mtd
SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

set_source_files_properties(${NSIS_SOURCE} PROPERTIES COMPILE_FLAGS "/Y-")

ADD_LIBRARY(nsCurl SHARED
			${ROOT_HEADER}
      ${ROOT_SOURCE}
      ${COMMON_SOURCE}
      ${NSIS_SOURCE}
      )
add_custom_command(TARGET nsCurl 
      POST_BUILD 
      COMMAND call ${PROJDIR}/Tools/CMake/PostBuild.cmd $(TargetDir)
      COMMENT "This command will be executed after build target nsCurl"
      )
SET_TARGET_PROPERTIES(nsCurl PROPERTIES OUTPUT_NAME "nsCurl")
