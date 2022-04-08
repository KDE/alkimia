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
if(BUILD_QT4)
    set(LIBS
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${QT_USE_LIBSPREFIX}Core
    )
else()
    set(LIBS
        Qt5::Widgets
        ${QT_BROWSER_LIB}
        KF5::CoreAddons
        KF5::I18n
        KF5::XmlGui
    )
    add_definitions(-DTRANSLATION_DOMAIN=\"onlinequoteseditor\")
endif()

target_link_libraries(onlinequoteseditor
    alkimia
    alkimia-internal
    ${LIBS}
)
set_target_properties(onlinequoteseditor PROPERTIES OUTPUT_NAME onlinequoteseditor${TARGET_SUFFIX})

install(TARGETS onlinequoteseditor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

configure_file(org.kde.onlinequoteseditor.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/org.kde.onlinequoteseditor${TARGET_SUFFIX}.desktop)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.onlinequoteseditor${TARGET_SUFFIX}.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
