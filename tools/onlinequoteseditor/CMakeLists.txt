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

ki18n_wrap_ui(SOURCES ${UI} )

#kde4_add_app_icon(SOURCES "${KDE4_INSTALL_DIR}/share/icons/oxygen/*/actions/application-exit.png")

ecm_add_executable(onlinequoteseditor ${SOURCES} ${HEADERS})
if(BUILD_QT4)
    set(LIBS ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS})
else()
    #set(LIBS Qt5::Widgets)
endif()

target_link_libraries(onlinequoteseditor
    alkimia
    ${LIBS}
)

install(TARGETS onlinequoteseditor ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})