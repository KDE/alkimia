include(ECMAddTests)
set(TEST_LIB
    alkimia
    alkimia-internal
    ${QT_USE_LIBSPREFIX}Test
    ${QT_USE_LIBSPREFIX}Core
    ${QT_USE_LIBSPREFIX}DBus
    ${QT_USE_LIBSPREFIX}WebKit
    ${MP_LIBRARIES}
)

if(BUILD_QT4)
    list(APPEND TEST_LIB ${KDE_LIBRARIES})
else()
    list(APPEND TEST_LIB ${QT_USE_LIBSPREFIX}WebKitWidgets)
endif()

ecm_add_tests(
    alkonlinequotesourcetest.cpp
    alkvaluetest.cpp
    alkquoteitemtest.cpp
    alkcompanytest.cpp
    alkexceptiontest.cpp
  LINK_LIBRARIES
    alkimia ${TEST_LIB}
)

ecm_add_test(
    alkonlinequotetest.cpp
    ../src/alkquotereceiver.cpp
    LINK_LIBRARIES ${TEST_LIB}
    TEST_NAME alkonlinequotestest
)

ecm_add_test(
    alkdateformattest.cpp
    LINK_LIBRARIES ${TEST_LIB}
    TEST_NAME alkdateformattest
)
