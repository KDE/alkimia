# The CMake version we require
cmake_minimum_required(VERSION 2.6.4)

######################### libalkimia Version ##########################
# The current version of libalkimia (used for packages and config.h)
set(VERSION_MAJOR "4")
set(VERSION_MINOR "3")
set(VERSION_PATCH "0")

# Determine the SVN revision number (if we're based on SVN)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.svn)
  # make sure we don't use translated messages here
  set(LANG $ENV{LANG})
  set(ENV{LANG} "C")
  execute_process(COMMAND svn info ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_VARIABLE VERSION_SUFFIX)
  string(REGEX MATCH "Revision: ([0-9]+)" VERSION_SUFFIX "${VERSION_SUFFIX}") 
  string(REPLACE "Revision: " "" VERSION_SUFFIX "${VERSION_SUFFIX}")
  set(VERSION_SUFFIX "-svn${VERSION_SUFFIX}")
  # switch back to the original language setting
  set(ENV{LANG} ${LANG})
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.svn)

set(ALKIMIA_LIB_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(ALKIMIA_LIB_SOVERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# check for PkgConfig, KDE and GMP
find_package(PkgConfig REQUIRED)
find_package(KDE4 REQUIRED)
find_package(GMP REQUIRED)

include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DMAKE_ALK_LIB)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

set(alkimia_LIB_SRCS
  alkvalue.cpp
  )

set(alkimia_HEADERS
  alkvalue.h
  alk_export.h
  )

kde4_add_library(alkimia SHARED ${alkimia_LIB_SRCS})
 
target_link_libraries(alkimia ${QT_QTCORE_LIBRARY} ${GMP_LIBRARIES})

set_target_properties(alkimia PROPERTIES VERSION ${ALKIMIA_LIB_VERSION} SOVERSION ${ALKIMIA_LIB_SOVERSION})

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${KDE4_INSTALL_DIR}" CACHE PATH
    "Alkimia install prefix defaults to the KDE4 install prefix: ${KDE4_INSTALL_DIR}" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.pc IMMEDIATE @ONLY)


########### install files ###############

install(TARGETS alkimia ${INSTALL_TARGETS_DEFAULT_ARGS} )

install(FILES ${alkimia_HEADERS}
        DESTINATION ${KDE4_INCLUDE_DIR}/alkimia COMPONENT Devel)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.pc
        DESTINATION ${KDE4_LIB_DIR}/pkgconfig)

########### documentation ###################

# check for Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

  make_directory(${APIDOC_DIR})

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen IMMEDIATE)

  add_custom_target(apidoc ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen)
endif(DOXYGEN_FOUND)

########### tests ###################

set( alkvaluetest_SRCS alkvaluetest.cpp )
kde4_add_unit_test( alkvaluetest TESTNAME alkimia-alkvalue ${alkvaluetest_SRCS} )
target_link_libraries( alkvaluetest alkimia ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} )

