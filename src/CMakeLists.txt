include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)

set(ALKIMIA_LIB_VERSION ${alkimia_VERSION})
set(ALKIMIA_LIB_SOVERSION "${alkimia_VERSION_MAJOR}")

set(alkimia_LIB_SRCS
  alkvalue.cpp
  alkquoteitem.cpp
  alkcompany.cpp
  )

set(alkimia_HEADERS
  ${CMAKE_CURRENT_BINARY_DIR}/alkvalue.h
  alkquoteitem.h
  alkcompany.h
  ${CMAKE_CURRENT_BINARY_DIR}/alk_export.h
  )

add_library(alkimia SHARED ${alkimia_LIB_SRCS})

kde_target_enable_exceptions(alkimia PUBLIC)

target_link_libraries(alkimia PUBLIC ${QT_USE_LIBSPREFIX}Core ${QT_USE_LIBSPREFIX}DBus ${MP_LIBRARIES})
generate_export_header(alkimia BASE_NAME alk)

if(WIN32)
    set_target_properties(alkimia PROPERTIES SUFFIX "${ALKIMIA_LIB_SOVERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}")
else()
    set_target_properties(alkimia PROPERTIES VERSION ${ALKIMIA_LIB_VERSION} SOVERSION ${ALKIMIA_LIB_SOVERSION})
endif()
set_target_properties(alkimia PROPERTIES OUTPUT_NAME alkimia${ALKIMIA_LIB_SUFFIX})
set(INCLUDE_INSTALL_DIR include/alkimia/${ALKIMIA_INCLUDE_SUFFIX})

if (NOT WIN32)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia${ALKIMIA_PATH_SUFFIX}.pc IMMEDIATE @ONLY)
endif(NOT WIN32)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/alkvalue.h.in ${CMAKE_CURRENT_BINARY_DIR}/alkvalue.h IMMEDIATE)

########### create package configuration file ###########

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/LibAlkimia${ALKIMIA_PATH_SUFFIX}-${alkimia_VERSION_MAJOR}.${alkimia_VERSION_MINOR}")

ecm_setup_version(${alkimia_VERSION} VARIABLE_PREFIX ALKIMIA
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}ConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/LibAlkimiaConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}Config.cmake"
  INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR
)

########### install files ###############

install(TARGETS alkimia
        EXPORT  alkimiaTargets
        ${INSTALL_TARGETS_DEFAULT_ARGS}
        )

export(TARGETS alkimia
       FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}Targets.cmake"
       NAMESPACE Alkimia::
      )

install(EXPORT alkimiaTargets
        FILE LibAlkimia${ALKIMIA_PATH_SUFFIX}Targets.cmake
        NAMESPACE Alkimia::
        DESTINATION ${CMAKECONFIG_INSTALL_DIR}
       )

install(FILES ${alkimia_HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/alkimia
        COMPONENT Devel)

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}ConfigVersion.cmake"
          "../cmake/modules/Find${MP_CMAKE_MODULE}.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

if (NOT WIN32)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalkimia${ALKIMIA_PATH_SUFFIX}.pc
          DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif(NOT WIN32)

########### documentation ###################

if(DOXYGEN_FOUND)
  set(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

  make_directory(${APIDOC_DIR})

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen IMMEDIATE)

  add_custom_target(libalkimia_apidoc ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen)
endif(DOXYGEN_FOUND)

