# SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de
#
# SPDX-License-Identifier: BSD-3-Clause

# Qt > 5 compatibility layer for Qt4
# - provides same component names as Qt >= 5

# prepend Qt prefix
list(TRANSFORM Qt4_FIND_COMPONENTS PREPEND Qt)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules)
include(FindQt4)

if(Qt4_FOUND)
    set(Qt4_COMPONENTS
        Core
        Gui
        DBus
        Declarative
        # not available on mingw
        #Designer
        #DesignerComponents
        Help
        Multimedia
        Network
        OpenGL
        Script
        ScriptTools
        Sql
        Svg
        Test
        #UiTools
        WebKit
        Xml
        XmlPatterns
    )

    #QtAssistant
    #QtAssistantClient
    #QAxContainer
    #QAxServer
    #Qt3Support
    #QtMotif
    #QtNsPLugin

    # add targets Qt4::<component>
    foreach(COMPONENT ${Qt4_COMPONENTS})
        if(TARGET Qt4::Qt${COMPONENT})
            add_library(Qt4::${COMPONENT} ALIAS Qt4::Qt${COMPONENT})
        endif()
    endforeach()

    # wrapper for Qt > 4 targets
    add_library(Qt4::Qml ALIAS Qt4::QtDeclarative)
    add_library(Qt4::WebKitWidgets ALIAS Qt4::QtWebKit)
    add_library(Qt4::Widgets ALIAS Qt4::QtGui)

    set(QT4_INSTALL_CMAKECONFIG_FILES ${CMAKE_CURRENT_LIST_DIR}/FindQt4.cmake)
endif()
