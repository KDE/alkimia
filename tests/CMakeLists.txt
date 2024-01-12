add_definitions(-DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
set(TEST_LIB
    alkimia
    alkimia-internal
    Qt${QT_MAJOR_VERSION}::Core
    ${QT_BROWSER_LIB}
    ${QT_QML_LIB}
)

add_executable(qmlalkonlinequotetest
    qmlalkonlinequotetest.cpp
)
target_link_libraries(qmlalkonlinequotetest ${TEST_LIB})

add_executable(alkonlinequoteswidgettest
    alkonlinequoteswidgettest.cpp
)
target_link_libraries(alkonlinequoteswidgettest ${TEST_LIB})

add_executable(alknewstufftest
    alknewstufftest.cpp
)
target_link_libraries(alknewstufftest ${TEST_LIB} ${KF_NEWSTUFF_LIB})
