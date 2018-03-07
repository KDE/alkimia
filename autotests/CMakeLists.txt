include(ECMAddTests)

if(BUILD_QT4)
    set(TEST_LIB ${QT_QTTEST_LIBRARY})
else()
    set(TEST_LIB Qt5::Test)
endif()

ecm_add_tests(
    alkvaluetest.cpp
    alkquoteitemtest.cpp
    alkcompanytest.cpp
  LINK_LIBRARIES
    alkimia ${TEST_LIB}
)
