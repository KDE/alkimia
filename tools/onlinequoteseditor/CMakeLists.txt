add_subdirectory(icons)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

set(SOURCES
    applicationsettings.cpp
    main.cpp
    mainwindow.cpp
)

set(HEADERS
    applicationsettings.h
    mainwindow.h
)

set(UI
    mainwindow.ui
)

ki18n_wrap_ui(SOURCES ${UI} )

ecm_add_app_icon(SOURCES
    ICONS
    icons/16-apps-onlinequoteseditor5.png
    icons/22-apps-onlinequoteseditor5.png
    icons/32-apps-onlinequoteseditor5.png
    icons/48-apps-onlinequoteseditor5.png
    icons/64-apps-onlinequoteseditor5.png
    icons/128-apps-onlinequoteseditor5.png
)

ecm_add_executable(onlinequoteseditor ${SOURCES} ${HEADERS})
if(QT_MAJOR_VERSION GREATER_EQUAL 5)
    add_definitions(-DTRANSLATION_DOMAIN=\"onlinequoteseditor\")
endif()

target_link_libraries(onlinequoteseditor
    alkimia
    alkimia-internal
    Qt${QT_MAJOR_VERSION}::Core
    KF${QT_MAJOR_VERSION}::CoreAddons
    KF${QT_MAJOR_VERSION}::I18n
    Qt${QT_MAJOR_VERSION}::Network
    Qt${QT_MAJOR_VERSION}::Widgets
    KF${QT_MAJOR_VERSION}::XmlGui
)
set_target_properties(onlinequoteseditor PROPERTIES OUTPUT_NAME onlinequoteseditor${TARGET_SUFFIX})

install(TARGETS onlinequoteseditor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(org.kde.onlinequoteseditor.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.onlinequoteseditor${TARGET_SUFFIX}.desktop)
if (NOT XDG_APPS_INSTALL_DIR)
    set(XDG_APPS_INSTALL_DIR ${KDE_INSTALL_APPDIR})
endif()
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.onlinequoteseditor${TARGET_SUFFIX}.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

configure_file(org.kde.onlinequoteseditor.appdata.xml.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.onlinequoteseditor${TARGET_SUFFIX}.appdata.xml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.onlinequoteseditor${TARGET_SUFFIX}.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

if(APPSTREAMCLI)
    add_test(
        NAME appstreamtest-onlinequoteseditor
        COMMAND ${APPSTREAMCLI} validate ${CMAKE_CURRENT_BINARY_DIR}/org.kde.onlinequoteseditor${TARGET_SUFFIX}.appdata.xml
    )
else()
    message(STATUS "Could not set up the appstream test. appstreamcli is missing.")
endif()
