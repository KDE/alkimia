# We add our source code here
set(SOURCES plasma-onlinequote.cpp)

set(UI configwidget.ui)

ki18n_wrap_ui(SOURCES ${UI})

# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_onlinequote ${SOURCES})
target_link_libraries(plasma_applet_onlinequote
    alkimia
)
if(BUILD_QT4)
    target_link_libraries(plasma_applet_onlinequote
        ${KDE4_PLASMA_LIBS}
        ${KDE4_KDEUI_LIBS}
    )
    set(PLUGIN_INSTALL_DIR "${KDE4_LIB_INSTALL_DIR}/kde4")
    set(SERVICES_INSTALL_DIR ${KDE4_SERVICES_INSTALL_DIR})
else()
    target_link_libraries(plasma_applet_onlinequote
        KF5::Package
        KF5::Plasma
    )
    add_definitions(-DTRANSLATION_DOMAIN=\"alkimia-plasma-onlinequote\")
endif()

install(TARGETS plasma_applet_onlinequote
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-onlinequote.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
