add_definitions(-DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
set(TEST_LIB
    alkimia
    alkimia-internal
    ${QT_USE_LIBSPREFIX}Core
)

if(BUILD_QT4)
    list(APPEND TEST_LIB
        ${QT_USE_LIBSPREFIX}Declarative
        ${KDE_LIBRARIES}
    )
else()
    list(APPEND TEST_LIB
        ${QT_USE_LIBSPREFIX}Qml
        KF5::KDELibs4Support
     )
endif()
add_executable(qmlalkonlinequotetest
    qmlalkonlinequotetest.cpp
)
target_link_libraries(qmlalkonlinequotetest ${TEST_LIB})

add_executable(alkonlinequoteswidgettest
    alkonlinequoteswidgettest.cpp
)
target_link_libraries(alkonlinequoteswidgettest ${TEST_LIB})
