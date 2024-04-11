add_definitions(-DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
set(TEST_LIB
    Qt${QT_MAJOR_VERSION}::Core
    ${QT_BROWSER_LIB}
    ${QT_QML_LIB}
    alkimia-internal
    alkimia
)

add_executable(qmlalkonlinequotetest
    qmlalkonlinequotetest.cpp
)
target_link_libraries(qmlalkonlinequotetest ${TEST_LIB})

add_executable(alkonlinequoteswidgettest
    alkonlinequoteswidgettest.cpp
)
target_link_libraries(alkonlinequoteswidgettest ${TEST_LIB})

add_executable(alknewstuffwidgettest
    alknewstuffwidgettest.cpp
)
target_link_libraries(alknewstuffwidgettest ${TEST_LIB} ${KF_NEWSTUFF_LIB} KF${QT_MAJOR_VERSION}::I18n)

if(BUILD_WITH_WEBKIT)
    add_executable(alkwebviewwidget-webkit-test
        alkwebviewwidgettest.cpp
    )
    target_link_libraries(alkwebviewwidget-webkit-test ${TEST_LIB})
elseif(BUILD_WITH_WEBENGINE)
    add_executable(alkwebviewwidget-webengine-test
        alkwebviewwidgettest.cpp
    )
    target_link_libraries(alkwebviewwidget-webengine-test ${TEST_LIB})
else()
    add_executable(alkwebviewwidget-qtextbrowser-test
        alkwebviewwidgettest.cpp
    )
    target_link_libraries(alkwebviewwidget-qtextbrowser-test ${TEST_LIB})
endif()
