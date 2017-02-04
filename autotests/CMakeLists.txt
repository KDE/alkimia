include(ECMAddTests)

ecm_add_tests(
    alkvaluetest.cpp
    alkquoteitemtest.cpp
    alkcompanytest.cpp
  LINK_LIBRARIES
    alkimia Qt5::Test
)
