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
    icons/16-apps-onlinequoteseditor.png
    icons/22-apps-onlinequoteseditor.png
    icons/32-apps-onlinequoteseditor.png
    icons/48-apps-onlinequoteseditor.png
    icons/64-apps-onlinequoteseditor.png
    icons/128-apps-onlinequoteseditor.png
)

ecm_add_executable(onlinequoteseditor ${SOURCES} ${HEADERS})
if(BUILD_QT4)
    set(LIBS
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${QT_USE_LIBSPREFIX}Core
        ${QT_USE_LIBSPREFIX}Network
        ${QT_USE_LIBSPREFIX}WebKit
    )
else()
    set(LIBS
        Qt5::Widgets
        Qt5::WebKitWidgets
        KF5::CoreAddons
        KF5::I18n
    )
    add_definitions(-DTRANSLATION_DOMAIN=\"onlinequoteseditor\")
endif()

target_link_libraries(onlinequoteseditor
    alkimia
    alkimia-internal
    ${LIBS}
)

install(TARGETS onlinequoteseditor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES org.kde.onlinequoteeditor.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
