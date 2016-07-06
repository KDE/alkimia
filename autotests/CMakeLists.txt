include(ECMMarkAsTest)

set(alkvaluetest_SRCS alkvaluetest.cpp)
add_executable(alkvaluetest ${alkvaluetest_SRCS})
ecm_mark_as_test(alkvaluetest)
add_test(alkvaluetest alkvaluetest)
target_link_libraries(alkvaluetest PRIVATE alkimia Qt5::Test)

set(alkquoteitemtest_SRCS alkquoteitemtest.cpp)
add_executable(alkquoteitemtest ${alkquoteitemtest_SRCS})
ecm_mark_as_test(alkquoteitemtest)
add_test(alkquoteitemtest alkquoteitemtest)
target_link_libraries(alkquoteitemtest PRIVATE alkimia Qt5::Test)

set(alkcompanytest_SRCS alkcompanytest.cpp)
add_executable(alkcompanytest ${alkcompanytest_SRCS})
ecm_mark_as_test(alkcompanytest)
add_test(alkcompanytest alkcompanytest)
target_link_libraries(alkcompanytest PRIVATE alkimia Qt5::Test)
