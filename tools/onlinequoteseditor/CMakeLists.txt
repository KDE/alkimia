include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_BINARY_DIR}/src
)

set(SOURCES
  main.cpp
  mainwindow.cpp
)

set(HEADERS
  mainwindow.h
)

set(UI
 mainwindow.ui
)

kde4_add_ui_files(SOURCES ${UI} )

#kde4_add_app_icon(SOURCES "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/actions/application-exit.png")

kde4_add_executable(onlinequoteseditor ${SOURCES} ${HEADERS})

if(BUILD_QT4)
  target_link_libraries(onlinequoteseditor
    alkimia
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
 )
else()
  target_link_libraries(onlinequoteseditor
    alkimia
    Qt5::Widgets
    KF5::CoreAddons
    KF5::I18n
    KF5::XmlGui
    KF5::TextWidgets
  )
endif()

install(TARGETS onlinequoteseditor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
