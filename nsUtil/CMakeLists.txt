#-------------------------------------------------------
# Copyright (c) NSDM
# All rights reserved.
# History:
# 		<Author>	<Time>		<Version>	  <Des>
#      guoyou		2019-7-26	1.0
#-------------------------------------------------------
PROJECT(nsUtil)

#-------------------------------------------------------------------------------------------------------------------
file(GLOB ROOT_HEADER 									inc/*h;inc/*.rc)
file(GLOB ROOT_SOURCE 									src/*.cpp;src/*.c)

file(GLOB COMMON_SOURCE     						${PROJDIR}/3rdParty/common/src/Utils.cpp)
file(GLOB COMMON_BASE_SOURCE     				${PROJDIR}/3rdParty/common/base/encrypt/modp_b64/modp_b64.cpp
                                        ${PROJDIR}/3rdParty/common/base/encrypt/base64.cpp
                                        ${PROJDIR}/3rdParty/common/base/encrypt/md5.cpp
                                        ${PROJDIR}/3rdParty/common/base/string/string_util.cpp
                                        ${PROJDIR}/3rdParty/common/base/system/minfo/cpuid.cpp
                                        ${PROJDIR}/3rdParty/common/base/system/minfo/hdbios.cpp
                                        ${PROJDIR}/3rdParty/common/base/system/minfo/mid.cpp
                                        ${PROJDIR}/3rdParty/common/base/system/system_util.cpp)

file(GLOB NSIS_SOURCE     					    ${PROJDIR}/3rdParty/nsis/include/output_debug.cpp
                                        ${PROJDIR}/3rdParty/nsis/include/pluginapi.c)

source_group("inc" 								      FILES 	${ROOT_HEADER})
source_group("src"								      FILES 	${ROOT_SOURCE})

source_group("src\\common"							FILES 	${COMMON_SOURCE})
source_group("src\\common\\base"				FILES 	${COMMON_BASE_SOURCE})
source_group("src\\nsis"							  FILES 	${NSIS_SOURCE})
                              
#-------------------------------------------------------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc
                    )                              
INCLUDE_DIRECTORIES(${PROJDIR}/3rdParty/easy_libcurl/include
                    ${PROJDIR}/3rdParty/nsis/include
                    ${PROJDIR}/3rdParty/common/include
                    ${PROJDIR}/3rdParty/common/
                    ${PROJDIR}/3rdParty/common/base/encrypt
                    ${PROJDIR}/3rdParty/common/base/encrypt/modp_b64
                    ${PROJDIR}/3rdParty/common/base/string
                    ${PROJDIR}/3rdParty/common/base/system
                    ${PROJDIR}/3rdParty/common/base/system/minfo
                    )

#-------------------------------------------------------------------------------------------------------------------
add_definitions(-DUNICODE -D_UNICODE)  # unicode版本

# MT模式
SET(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")# 直接抄gui的默认，然后 改成mtd
SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /D NDEBUG")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

ADD_LIBRARY(nsUtil SHARED
			${ROOT_HEADER}
      ${ROOT_SOURCE}
      ${COMMON_SOURCE}
      ${COMMON_BASE_SOURCE}
      ${NSIS_SOURCE}
      )
add_custom_command(TARGET nsUtil 
      POST_BUILD 
      COMMAND call ${PROJDIR}/Tools/CMake/PostBuild.cmd $(TargetDir)
      COMMENT "This command will be executed after build target nsCurl"
      )
SET_TARGET_PROPERTIES(nsUtil PROPERTIES OUTPUT_NAME "nsUtil")
