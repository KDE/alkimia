######################### libalkimia Version ##########################
# The current version of libalkimia (used for packages and config.h)
set(VERSION_MAJOR "6")
set(VERSION_MINOR "0")
set(VERSION_PATCH "90")

# The CMake version we require
cmake_minimum_required(VERSION 2.8.12)
include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
# TODO: add when QStrings are no longer constructed from ASCII strings
#include(KDEFrameworkCompilerSettings)

set(ALKIMIA_LIB_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(ALKIMIA_LIB_SOVERSION "${VERSION_MAJOR}")

include( GenerateExportHeader )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE_ENABLE_EXCEPTIONS}")

set(alkimia_LIB_SRCS
  alkvalue.cpp
  alkquoteitem.cpp
  alkcompany.cpp
  )

set(alkimia_HEADERS
  alkvalue.h
  alkquoteitem.h
  alkcompany.h
  ${CMAKE_CURRENT_BINARY_DIR}/alk_export.h
  )

add_library(alkimia SHARED ${alkimia_LIB_SRCS})

kde_target_enable_exceptions(alkimia PUBLIC)

target_include_directories(alkimia PUBLIC ${GMP_INCLUDE_DIR})
target_link_libraries(alkimia PUBLIC Qt5::Core Qt5::DBus ${GMP_LIBRARIES})
generate_export_header(alkimia BASE_NAME alk)

set_target_properties(alkimia PROPERTIES VERSION ${ALKIMIA_LIB_VERSION} SOVERSION ${ALKIMIA_LIB_SOVERSION})

if (NOT WIN32)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.pc IMMEDIATE @ONLY)
endif(NOT WIN32)

########### create package configuration file ###########

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/LibAlkimia")

ecm_setup_version(${ALKIMIA_LIB_VERSION} VARIABLE_PREFIX ALKIMIA
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimiaConfigVersion.cmake"
)

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/LibAlkimiaConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimiaConfig.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR
)

########### install files ###############

install(TARGETS alkimia
        EXPORT  alkimiaTargets
        ${INSTALL_TARGETS_DEFAULT_ARGS}
        )

export(TARGETS alkimia
       FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimiaTargets.cmake"
       NAMESPACE Alkimia::
      )

install(EXPORT alkimiaTargets
        FILE LibAlkimiaTargets.cmake
        NAMESPACE Alkimia::
        DESTINATION ${CMAKECONFIG_INSTALL_DIR}
       )

install(FILES ${alkimia_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/alkimia
        COMPONENT Devel)

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimiaConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimiaConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

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
include(ECMMarkAsTest)

set(alkvaluetest_SRCS alkvaluetest.cpp)
set(alkquoteitemtest_SRCS alkquoteitemtest.cpp)
set(alkcompanytest_SRCS alkcompanytest.cpp)

add_executable(alkvaluetest ${alkvaluetest_SRCS})
ecm_mark_as_test(alkvaluetest)
add_test(alkvaluetest alkvaluetest)

add_executable(alkquoteitemtest ${alkquoteitemtest_SRCS})
ecm_mark_as_test(alkquoteitemtest)
add_test(alkquoteitemtest alkquoteitemtest)

add_executable(alkcompanytest ${alkcompanytest_SRCS})
ecm_mark_as_test(alkcompanytest)
add_test(alkcompanytest alkcompanytest)

target_link_libraries(alkvaluetest PRIVATE alkimia Qt5::Core Qt5::DBus Qt5::Test ${GMP_LIBRARIES})
target_link_libraries(alkquoteitemtest PRIVATE alkimia Qt5::Core Qt5::DBus Qt5::Test)
target_link_libraries(alkcompanytest PRIVATE alkimia Qt5::Core Qt5::Test)

