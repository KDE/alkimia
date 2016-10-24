######################### libalkimia Version ##########################
# The current version of libalkimia (used for packages and config.h)
set(VERSION_MAJOR "6")
set(VERSION_MINOR "0")
set(VERSION_PATCH "90")

include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)

set(ALKIMIA_LIB_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(ALKIMIA_LIB_SOVERSION "${VERSION_MAJOR}")

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
target_link_libraries(alkimia PUBLIC ${QT_LIBSPREFIX}Core ${QT_LIBSPREFIX}DBus ${GMP_LIBRARIES})
generate_export_header(alkimia BASE_NAME alk)

set_target_properties(alkimia PROPERTIES VERSION ${ALKIMIA_LIB_VERSION} SOVERSION ${ALKIMIA_LIB_SOVERSION})
if(BUILD_QT4)
    set_target_properties(alkimia PROPERTIES OUTPUT_NAME alkimia${ALKIMIA_LIB_POSTFIX})
endif()

if (NOT WIN32)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia${CMAKE_DIR_PREFIX}.pc IMMEDIATE @ONLY)
endif(NOT WIN32)

########### create package configuration file ###########

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/LibAlkimia${CMAKE_DIR_PREFIX}-${VERSION_MAJOR}.${VERSION_MINOR}")

ecm_setup_version(${ALKIMIA_LIB_VERSION} VARIABLE_PREFIX ALKIMIA
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${CMAKE_DIR_PREFIX}ConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
)

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/LibAlkimiaConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${CMAKE_DIR_PREFIX}Config.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR
)

########### install files ###############

install(TARGETS alkimia
        EXPORT  alkimiaTargets
        ${INSTALL_TARGETS_DEFAULT_ARGS}
        )

export(TARGETS alkimia
       FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${CMAKE_DIR_PREFIX}Targets.cmake"
       NAMESPACE Alkimia::
      )

install(EXPORT alkimiaTargets
        FILE LibAlkimia${CMAKE_DIR_PREFIX}Targets.cmake
        NAMESPACE Alkimia::
        DESTINATION ${CMAKECONFIG_INSTALL_DIR}
       )

install(FILES ${alkimia_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/alkimia/${CMAKE_DIR_PREFIX}/alkimia
        COMPONENT Devel)

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${CMAKE_DIR_PREFIX}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${CMAKE_DIR_PREFIX}ConfigVersion.cmake"
          "../cmake/modules/FindGMP.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

if (NOT WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalkimia${CMAKE_DIR_PREFIX}.pc
          DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

########### documentation ###################

if(DOXYGEN_FOUND)
  set(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

  make_directory(${APIDOC_DIR})

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen IMMEDIATE)

  add_custom_target(libalkimia_apidoc ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen)
endif(DOXYGEN_FOUND)

