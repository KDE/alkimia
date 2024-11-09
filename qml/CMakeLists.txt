if(QT_MAJOR_VERSION GREATER_EQUAL 5)
    set_package_properties(Qt${QT_MAJOR_VERSION}Qml PROPERTIES  URL "git://gitorious.org/qt/qtdeclarative.git"
        DESCRIPTION "QML support for Alkimia"
        TYPE OPTIONAL
        PURPOSE "Allows to use Alkimiain QML code"
    )
    if(NOT Qt${QT_MAJOR_VERSION}Qml_FOUND)
        message(STATUS "Qt${QT_MAJOR_VERSION}Qml not found, qml imports will not be built.")
        return()
    endif()
endif()

# Qt6
if(NOT QML_INSTALL_DIR)
    set(QML_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt${QT_MAJOR_VERSION}/qml)
endif()

set(IMPORTS_DIR ${CMAKE_BINARY_DIR}/bin/org/kde/alkimia${ALKIMIA_LIB_SUFFIX})
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IMPORTS_DIR})
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${IMPORTS_DIR})
endif()

configure_file(qmldir.cmake ${IMPORTS_DIR}/qmldir)
add_library(qmlalkimia qmlalkimiaplugin.cpp)
target_link_libraries(qmlalkimia ${QT_QML_LIB} alkimia)
set_target_properties(qmlalkimia PROPERTIES OUTPUT_NAME qmlalkimia${ALKIMIA_LIB_SUFFIX})

install(TARGETS qmlalkimia
    RUNTIME DESTINATION ${QML_INSTALL_DIR}/org/kde/alkimia${ALKIMIA_LIB_SUFFIX}
    LIBRARY DESTINATION ${QML_INSTALL_DIR}/org/kde/alkimia${ALKIMIA_LIB_SUFFIX})
install(FILES ${IMPORTS_DIR}/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/alkimia${ALKIMIA_LIB_SUFFIX})

# run qml file with
