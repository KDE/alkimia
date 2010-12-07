
# The CMake version we require
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.4)

SET(ALKIMIA_LIB_VERSION "4.3.0")
SET(ALKIMIA_LIB_SOVERSION "4.3.0")

# check for PkgConfig, KDE and GMP
FIND_PACKAGE(PkgConfig REQUIRED)
FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(GMP REQUIRED)

include(${QT_USE_FILE})

SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

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


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.pc IMMEDIATE @ONLY)

# If no build type is set, use "Release with Debug Info"
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
  "Choose the type of build.
Possible values are: 'Release' 'RelWithDebInfo' 'Debug' 'Debugfull' 'Profile'
The default value is: 'RelWithDebInfo'" FORCE)


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

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.pc
        DESTINATION ${KDE4_LIB_DIR}/pkgconfig)

########### documentation ###################

# check for Doxygen
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
  SET(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

  MAKE_DIRECTORY(${APIDOC_DIR})

  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen IMMEDIATE)

  ADD_CUSTOM_TARGET(apidoc ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen)
ENDIF(DOXYGEN_FOUND)

########### tests ###################

IF( KDE4_BUILD_TESTS )
  # INCLUDE(CTest) # (makes sense only with a ctest online dashboard)
  ENABLE_TESTING()


  # alkvaluetest
  set( alkvaluetest_SRCS alkvaluetest.cpp )

  kde4_add_unit_test( alkvaluetest TESTNAME alkimia-alkvalue ${alkvaluetest_SRCS} )
  ADD_DEPENDENCIES( alkvaluetest alkimia )

  target_link_libraries( alkvaluetest alkimia ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} )

ENDIF( KDE4_BUILD_TESTS )
