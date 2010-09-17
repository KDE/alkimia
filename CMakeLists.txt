
# The CMake version we require
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

SET(ALKIMIA_LIB_VERSION "4.3.0")
SET(ALKIMIA_LIB_SOVERSION "4.3.0")

FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(GMP REQUIRED)
include(${QT_USE_FILE})

SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions" )

SET(alkimia_LIB_SRCS
  alkvalue.cpp
  )

SET(alkimia_HEADERS
  alkvalue.h
  )

ADD_LIBRARY(alkimia SHARED ${alkimia_LIB_SRCS})
 
TARGET_LINK_LIBRARIES(alkimia ${QT_LIBRARIES} ${GMP_LIBRARIES})

SET_TARGET_PROPERTIES(alkimia PROPERTIES VERSION ${ALKIMIA_LIB_VERSION} SOVERSION ${ALKIMIA_LIB_SOVERSION})

IF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  SET(CMAKE_INSTALL_PREFIX "${KDE4_INSTALL_DIR}" CACHE PATH
    "Alkimia install prefix defaults to the KDE4 install prefix: ${KDE4_INSTALL_DIR}" FORCE)
ENDIF(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# the RPATH to be used when installing
# (cf. http://www.vtk.org/Wiki/CMake_RPATH_handling)
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


IF(NOT WIN32)
  # use position independent code
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(NOT WIN32)
IF(NOT MSVC)
  # be more pedantic about common symbols
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-common")

  # Debug, Debugfull, Profile
  SET(CMAKE_CXX_FLAGS_DEBUG
    "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
  SET(CMAKE_CXX_FLAGS_DEBUGFULL
    "-g3 -fno-inline")
  SET(CMAKE_CXX_FLAGS_PROFILE
    "-g3 -fno-inline -ftest-coverage -fprofile-arcs")

  # be pedantic about undefined symbols when linking shared libraries
  IF(CMAKE_SYSTEM_NAME MATCHES Linux)
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined -Wl,--as-needed")
  ELSE(CMAKE_SYSTEM_NAME MATCHES Linux)
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl")
  ENDIF(CMAKE_SYSTEM_NAME MATCHES Linux)
ENDIF(NOT MSVC)

########### install files ###############

INSTALL(TARGETS alkimia ${INSTALL_TARGETS_DEFAULT_ARGS} )

INSTALL(FILES ${alkimia_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/alkimia COMPONENT Devel)

