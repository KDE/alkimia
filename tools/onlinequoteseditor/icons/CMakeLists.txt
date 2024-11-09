if (NOT ICON_INSTALL_DIR)
    set(ICON_INSTALL_DIR ${KDE_INSTALL_ICONDIR})
endif()
set(ICONS
    16-apps-onlinequoteseditor5.png
    22-apps-onlinequoteseditor5.png
    32-apps-onlinequoteseditor5.png
    48-apps-onlinequoteseditor5.png
    64-apps-onlinequoteseditor5.png
    128-apps-onlinequoteseditor5.png
    sc-apps-onlinequoteseditor5.svgz
)
foreach(i ${ICONS})
    string(REPLACE "5" "${QT_MAJOR_VERSION}" ic ${i})
    configure_file(${i} ${CMAKE_CURRENT_BINARY_DIR}/${ic} COPYONLY)
    list(APPEND _icons ${CMAKE_CURRENT_BINARY_DIR}/${ic})
endforeach()
ecm_install_icons(
    ICONS ${_icons}
    THEME hicolor
    DESTINATION ${ICON_INSTALL_DIR}
)
