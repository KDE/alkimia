if (NOT ICON_INSTALL_DIR)
    set(ICON_INSTALL_DIR ${KDE_INSTALL_ICONDIR})
endif()
ecm_install_icons(
    ICONS 16-apps-onlinequoteseditor5.png
          22-apps-onlinequoteseditor5.png
          32-apps-onlinequoteseditor5.png
          48-apps-onlinequoteseditor5.png
          64-apps-onlinequoteseditor5.png
          128-apps-onlinequoteseditor5.png
          sc-apps-onlinequoteseditor5.svgz
    THEME hicolor
    DESTINATION ${ICON_INSTALL_DIR}
)
