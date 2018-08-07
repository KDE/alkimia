# We add our source code here
set(SOURCES plasma-onlinequote.cpp)

set(UI configwidget.ui)

ki18n_wrap_ui(SOURCES ${UI})

# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_onlinequote ${SOURCES})
target_link_libraries(plasma_applet_onlinequote
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDEUI_LIBS}
    alkimia
)

set(KDE4_PLUGIN_INSTALL_DIR "${KDE4_LIB_INSTALL_DIR}/kde4")
# qt5: ${PLUGIN_INSTALL_DIR}

install(TARGETS plasma_applet_onlinequote
        DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-onlinequote.desktop
        DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
    # qt5: ${SERVICES_INSTALL_DIR}
