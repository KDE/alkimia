include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

########### target alkimia-internal ###########

set(alkimia_INTERNAL_SRCS
    alkapplication.cpp
    alkdateformat.cpp
    alkexception.cpp
    alknewstuffwidget.cpp
    alkonlinequoteprocess.cpp
    alkquoteitem.cpp
    alkquotereceiver.cpp
)

set(alkimia_INTERNAL_HEADERS
    alkapplication.h
    alkdateformat.h
    alkexception.h
    alknewstuffwidget.h
    alkonlinequoteprocess.h
    alkquoteitem.h
    alkquotereceiver.h
)

set(alkimia_INTERNAL_UI
)

ki18n_wrap_ui(alkimia_INTERNAL_SRCS ${alkimia_INTERNAL_UI} )
add_library(alkimia-internal STATIC ${alkimia_INTERNAL_SRCS} ${alkimia_INTERNAL_HEADERS})
if (QT_MAJOR_VERSION GREATER_EQUAL 5)
    list(APPEND ALKIMIA_INTERNAL_LIBS
        PRIVATE
        ${QT_DBUS_LIB}
        KF${QT_MAJOR_VERSION}::Completion
        KF${QT_MAJOR_VERSION}::CoreAddons
        KF${QT_MAJOR_VERSION}::I18n
    )
endif()

target_link_libraries(alkimia-internal PRIVATE
    ${ALKIMIA_INTERNAL_LIBS}
    ${KF_NEWSTUFF_LIB}
    KF${QT_MAJOR_VERSION}::ConfigCore
)
kde_target_enable_exceptions(alkimia-internal PUBLIC)

########### target alkimia ###########

set(ALKIMIA_LIB_VERSION ${alkimia_VERSION})
set(ALKIMIA_LIB_SOVERSION "${alkimia_VERSION_MAJOR}")

set(alkimia_LIB_SRCS
    alkcompany.cpp
    alkenvironment.cpp
    alknewstuffengine.cpp
    alknewstuffstore.cpp
    alknewstuffwidget.cpp
    alkonlinequote.cpp
    alkonlinequote_p.cpp
    alkonlinequotesource.cpp
    alkonlinequotesprofile.cpp
    alkonlinequotesprofile_p.cpp
    alkonlinequotesprofilemanager.cpp
    alkonlinequoteswidget.cpp
    alkonlinequoteuploaddialog.cpp
    alkvalue.cpp
    alkutils.cpp
    alkwebpage.cpp
)

set(alkimia_LIB_HEADERS
    alkcompany.h
    alkenvironment.h
    alknewstuffengine.h
    alknewstuffstore.h
    alkonlinequote.h
    alkonlinequotesource.h
    alkonlinequotesprofile.h
    alkonlinequotesprofilemanager.h
    alkonlinequoteswidget.h
    alkwebpage.h
)

set(alkimia_LIB_PRIVATE_HEADERS
    alkonlinequote_p.h
    alkonlinequotesource_p.h
    alkonlinequotesprofile_p.h
    alkonlinequoteuploaddialog.h
    alkpushbutton.h
    alkutils.h
)

if(ENABLE_FINANCEQUOTE)
    list(APPEND alkimia_LIB_SRCS alkfinancequoteprocess.cpp)
    list(APPEND alkimia_LIB_HEADERS alkfinancequoteprocess.h)
endif()

set(alkimia_UI
    alkonlinequotedetails.ui
    alkonlinequotes.ui
    alkonlinequotesdebug.ui
    alkonlinequotesprofiledetails.ui
    alkonlinequotesprofiles.ui
    alkonlinequoteuploaddialog.ui
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/alkversion.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/alkimia/alkversion.h
    IMMEDIATE
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/alkvalue.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/alkimia/alkvalue.h
    IMMEDIATE
)

foreach(header ${alkimia_LIB_HEADERS})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${header}
        ${CMAKE_CURRENT_BINARY_DIR}/alkimia/${header}
        COPYONLY
    )
endforeach()

ki18n_wrap_ui(alkimia_LIB_SRCS ${alkimia_UI} )
add_library(alkimia ${alkimia_LIB_SRCS} ${alkimia_LIB_HEADERS} ${alkimia_LIB_PRIVATE_HEADERS})

kde_target_enable_exceptions(alkimia PUBLIC)
target_link_libraries(alkimia PRIVATE
    ${QT_NETWORK_LIB}
    Qt${QT_MAJOR_VERSION}::DBus
    ${KF_NETWORK_LIB}
    ${KF_NEWSTUFF_LIB}
    KF${QT_MAJOR_VERSION}::Codecs
    KF${QT_MAJOR_VERSION}::Completion
    KF${QT_MAJOR_VERSION}::I18n
    KF${QT_MAJOR_VERSION}::IconThemes
    KF${QT_MAJOR_VERSION}::TextWidgets
    KF${QT_MAJOR_VERSION}::WidgetsAddons
    PUBLIC
    KF${QT_MAJOR_VERSION}::ConfigCore
)

add_definitions(-DTRANSLATION_DOMAIN=\"alkimia\")

if (QT_MAJOR_VERSION GREATER_EQUAL 6 AND NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR include)
endif()

target_include_directories(alkimia
    INTERFACE
        ${MP_INCLUDE_DIR}
        $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/alkimia/${ALKIMIA_INCLUDE_SUFFIX}>
)

target_link_libraries(alkimia
    PRIVATE
    alkimia-internal
    Qt${QT_MAJOR_VERSION}::Core
    PUBLIC
    ${MP_LIBRARIES}
    ${QT_BROWSER_LIB}
)

if(QT_MAJOR_VERSION GREATER_EQUAL 5 AND MSVC AND BUILD_WITH_WEBKIT)
    message(WARNING "Applying fix for broken Qt WebKit package (see https://phabricator.kde.org/T10146 for details)")
    target_link_libraries(alkimia
        PRIVATE
        Qt${QT_MAJOR_VERSION}::Test
    )
endif()

if (NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR})
endif()
install(
    DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/alkimia
    DESTINATION ${INCLUDE_INSTALL_DIR}/alkimia/${ALKIMIA_INCLUDE_SUFFIX}
    COMPONENT Devel
)

generate_export_header(alkimia BASE_NAME alk EXPORT_FILE_NAME alkimia/alk_export.h)

if(BUILD_SHARED_LIBS)
    if(WIN32)
        set_target_properties(alkimia PROPERTIES SUFFIX "-${ALKIMIA_LIB_SOVERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}")
    else()
        set_target_properties(alkimia PROPERTIES VERSION ${ALKIMIA_LIB_VERSION} SOVERSION ${ALKIMIA_LIB_SOVERSION})
    endif()
endif()
set_target_properties(alkimia PROPERTIES OUTPUT_NAME alkimia${ALKIMIA_LIB_SUFFIX})
set(INCLUDE_INSTALL_DIR include/alkimia/${ALKIMIA_INCLUDE_SUFFIX})

if(BUILD_SHARED_LIBS)
    set(alkimia_export_targets alkimia)
else()
    set(alkimia_export_targets alkimia alkimia-internal)
endif()

install(TARGETS ${alkimia_export_targets}
    EXPORT  alkimiaTargets
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

if(QT_MAJOR_VERSION LESS 5)
    if(WIN32)
        set(KDE_INSTALL_KNSRCDIR ${SHARE_INSTALL_DIR}/config)
    else()
        set(KDE_INSTALL_KNSRCDIR ${SHARE_INSTALL_DIR}/kde4/config)
    endif()
    set(APPDATA_INSTALL_DIR ${SHARE_INSTALL_DIR}/kde4/apps)
    set(FQ_LOCAL_INSTALL_PATH ${CMAKE_BINARY_DIR}/share/apps/alkimia${ALKIMIA_PATH_SUFFIX}/misc)
else()
    if(KF5NewStuff_VERSION VERSION_GREATER "5.56.0")
    else()
        set(KDE_INSTALL_KNSRCDIR ${CONFIG_INSTALL_DIR})
    endif()
    # TODO
    if(KF6NewStuffCore_VERSION)
        set(KDE_INSTALL_KNSRCDIR etc/knsrc)
    endif()
    set(APPDATA_INSTALL_DIR ${SHARE_INSTALL_DIR})
    set(FQ_LOCAL_INSTALL_PATH ${CMAKE_BINARY_DIR}/share/alkimia${ALKIMIA_PATH_SUFFIX}/misc)
endif()

install(FILES
    alkimia-quotes.knsrc
    kmymoney-quotes.knsrc
    skrooge-quotes.knsrc
    DESTINATION ${KDE_INSTALL_KNSRCDIR}
)

# install perl script in expected location of build dir
# KDE4: set KDEHOME to build dir
# KF5: ???
if(ENABLE_FINANCEQUOTE)
    make_directory(${FQ_LOCAL_INSTALL_PATH})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/financequote.pl ${FQ_LOCAL_INSTALL_PATH}/financequote.pl IMMEDIATE COPYONLY)
    install(PROGRAMS financequote.pl
      DESTINATION ${APPDATA_INSTALL_DIR}/alkimia${ALKIMIA_PATH_SUFFIX}/misc
    )
endif()

if (NOT WIN32)
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia${ALKIMIA_PATH_SUFFIX}.pc IMMEDIATE @ONLY)
endif(NOT WIN32)

########### create package configuration file ###########

ecm_setup_version(${alkimia_VERSION} VARIABLE_PREFIX ALKIMIA
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/LibAlkimiaConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}Config.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

########### install files ###############

export(TARGETS ${alkimia_export_targets}
    FILE "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}Targets.cmake"
    NAMESPACE Alkimia::
)

install(EXPORT alkimiaTargets
    FILE LibAlkimia${ALKIMIA_PATH_SUFFIX}Targets.cmake
    NAMESPACE Alkimia::
    DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}ConfigVersion.cmake"
    "../cmake/modules/Find${MP_CMAKE_MODULE}.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

if (NOT WIN32)
    if (LIB_INSTALL_DIR)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalkimia${ALKIMIA_PATH_SUFFIX}.pc
            DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
    else()
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libalkimia${ALKIMIA_PATH_SUFFIX}.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
    endif()
endif(NOT WIN32)

########### documentation ###################

if(DOXYGEN_FOUND)
    set(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

    make_directory(${APIDOC_DIR})

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libalkimia.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen IMMEDIATE)

    add_custom_target(libalkimia_apidoc ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/libalkimia.doxygen)
endif(DOXYGEN_FOUND)

