######################### libalkimia Version ##########################
# The current version of libalkimia (used for packages and config.h)
set(VERSION_MAJOR "4")
set(VERSION_MINOR "3")
set(VERSION_PATCH "2")

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
set(ALKIMIA_LIB_SOVERSION "${VERSION_MAJOR}")

include_directories(${KDE4_INCLUDES})
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} -DMAKE_ALK_LIB)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")

set(alkimia_LIB_SRCS
  alkvalue.cpp
  alkquoteitem.cpp
  alkcompany.cpp
  )

set(alkimia_HEADERS
  alkvalue.h
  alkquoteitem.h
  alkcompany.h
  alk_export.h
  )

set(alkimia_CMAKE_MODULES
  FindLibAlkimia.cmake
  )

kde4_add_library(alkimia SHARED ${alkimia_LIB_SRCS})
 
target_link_libraries(alkimia ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${GMP_LIBRARIES})

set_target_properties(alkimia PROPERTIES VERSION ${ALKIMIA_LIB_VERSION} SOVERSION ${ALKIMIA_LIB_SOVERSION})

if (NOT WIN32)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.pc IMMEDIATE @ONLY)
endif(NOT WIN32)

########### install files ###############

install(TARGETS alkimia ${INSTALL_TARGETS_DEFAULT_ARGS} )

install(FILES ${alkimia_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/alkimia COMPONENT Devel)

install(FILES ${alkimia_CMAKE_MODULES}
        DESTINATION ${DATA_INSTALL_DIR}/cmake/modules COMPONENT Devel)

if (NOT WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.pc
          DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)
########### documentation ###################

# check for Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

  make_directory(${APIDOC_DIR})

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen IMMEDIATE)

  add_custom_target(libalkimia_apidoc ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen)
endif(DOXYGEN_FOUND)

########### tests ###################

set( alkvaluetest_SRCS alkvaluetest.cpp )
set( alkquoteitemtest_SRCS alkquoteitemtest.cpp )
set( alkcompanytest_SRCS alkcompanytest.cpp )
kde4_add_unit_test( alkvaluetest TESTNAME alkimia-alkvalue ${alkvaluetest_SRCS} )
kde4_add_unit_test( alkquoteitemtest TESTNAME alkimia-alkquoteitem ${alkquoteitemtest_SRCS} )
kde4_add_unit_test( alkcompanytest TESTNAME alkimia-alkcompany ${alkcompanytest_SRCS} )
target_link_libraries( alkvaluetest alkimia ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTTEST_LIBRARY} ${GMP_LIBRARIES})
target_link_libraries( alkquoteitemtest alkimia ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTTEST_LIBRARY} )
target_link_libraries( alkcompanytest alkimia ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} )

