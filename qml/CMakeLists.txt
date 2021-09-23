if(BUILD_QT4)
    set(LIBS ${QT_USE_LIBSPREFIX}Declarative)
    set(QML_INSTALL_DIR ${LIB_INSTALL_DIR}/qt4/plugins/imports)
else()
    set(LIBS ${QT_USE_LIBSPREFIX}Qml)
    set_package_properties(Qt5Qml PROPERTIES  URL "git://gitorious.org/qt/qtdeclarative.git"
        DESCRIPTION "QML support for Alkimia"
        TYPE OPTIONAL
        PURPOSE "Allows to use Alkimiain QML code"
    )
    if(NOT Qt5Qml_FOUND)
        message(STATUS "Qt5Qml not found, qml imports will not be built.")
        return()
    endif()
endif()

set(IMPORTS_DIR ${CMAKE_BINARY_DIR}/bin/org/kde/alkimia)
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${IMPORTS_DIR})
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${IMPORTS_DIR})
endif()

configure_file(qmldir ${IMPORTS_DIR}/qmldir)
add_library(qmlalkimia qmlalkimiaplugin.cpp)
target_link_libraries(qmlalkimia ${LIBS} alkimia)

install(TARGETS qmlalkimia
    RUNTIME DESTINATION ${QML_INSTALL_DIR}/org/kde/alkimia
    LIBRARY DESTINATION ${QML_INSTALL_DIR}/org/kde/alkimia)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/alkimia)

# run qml file with
