set(TEST_LIB
    alkimia
    alkimia-internal
    ${QT_USE_LIBSPREFIX}Core
    ${QT_USE_LIBSPREFIX}WebKit
)

if(BUILD_QT4)
    list(APPEND TEST_LIB ${KDE_LIBRARIES})
endif()

if(BUILD_QT4)
    list(APPEND TEST_LIB ${QT_USE_LIBSPREFIX}Declarative)
    set(RESOURCES alkonlinequoteqmltest.qrc)
    message("FIXME: alkonlinequoteqmltest need to be implemented")
else()
    list(APPEND TEST_LIB ${QT_USE_LIBSPREFIX}Qml)
    find_package(Qt5QuickCompiler)
    qtquick_compiler_add_resources(RESOURCES alkonlinequoteqmltest.qrc)
    add_executable(alkonlinequoteqmltest
        alkonlinequoteqmltest.cpp
        ${RESOURCES}
    )
    target_link_libraries(alkonlinequoteqmltest ${TEST_LIB})
endif()
