
option(TEST_DEVELOP_HOST "Use development host for testing" OFF)
if(TEST_DEVELOP_HOST)
    add_definitions(-DTEST_DEVELOP_HOST)
endif()

include(ECMAddTests)
set(TEST_LIB
    Qt${QT_MAJOR_VERSION}::Test
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::DBus
    ${MP_LIBRARIES}
    alkimia-internal
    alkimia
)

# setup test environment
list(APPEND TEST_ENV "KDE_DEBUG=1")
if(BUILD_WITH_WEBENGINE)
    list(APPEND TEST_ENV "QTWEBENGINE_DISABLE_SANDBOX=1")
endif()
if(TEST_IN_DOCKER)
    list(APPEND TEST_ENV "QT_QPA_PLATFORM=offscreen")
endif()

if(BUILD_WITH_WEBKIT)
    set(TEST_SUFFIX "-webkit-")
elseif(BUILD_WITH_WEBENGINE)
    set(TEST_SUFFIX "-webengine-")
else(BUILD_WITH_WEBKIT)
    set(TEST_SUFFIX "")
endif()

ecm_add_tests(
    alkonlinequotesourcetest.cpp
    alkvaluetest.cpp
    alkquoteitemtest.cpp
    alkcompanytest.cpp
    alkdomtest.cpp
    alkexceptiontest.cpp
    ENV ${TEST_ENV}
    LINK_LIBRARIES
    ${TEST_LIB}
)

ecm_add_test(
    alkonlinequotetest.cpp
    ../src/alkquotereceiver.cpp
    ENV ${TEST_ENV}
    LINK_LIBRARIES ${TEST_LIB}
    TEST_NAME alkonlinequotes${TEST_SUFFIX}test
)

ecm_add_test(
    alkdateformattest.cpp
    ENV ${TEST_ENV}
    LINK_LIBRARIES ${TEST_LIB}
    TEST_NAME alkdateformattest
)

if(BUILD_WITH_WEBENGINE)
    ecm_add_test(
        test-qwebengine-offscreen.cpp
        ENV ${TEST_ENV}
        LINK_LIBRARIES ${TEST_LIB}
        TEST_NAME test-qwebengine-offscreen
    )
endif()

if(BUILD_WITH_WEBENGINE)
    ecm_add_test(
        test-qwebengine-cli.cpp
        ENV ${TEST_ENV}
        LINK_LIBRARIES ${TEST_LIB}
        TEST_NAME test-qwebengine-cli
    )
endif()

ecm_add_test(
    alkonlinequoteprivatetest.cpp
    alkonlinequoteprivatetest.qrc
    ENV ${TEST_ENV}
    LINK_LIBRARIES ${TEST_LIB} KF${QT_MAJOR_VERSION}::CoreAddons
    TEST_NAME alkonlinequoteprivatetest
)

ecm_add_test(
    alkdownloadenginetest.cpp
    alkdownloadenginetest.h
    COMPILE_DEFINITIONS -DUSE_QTNETWORK
    ENV ${TEST_ENV}
    LINK_LIBRARIES ${TEST_LIB}
    TEST_NAME alkdownloadengine-qt-test
)

if(BUILD_WITH_WEBKIT)
    ecm_add_test(
        alkdownloadenginetest.cpp
        alkdownloadenginetest.h
        COMPILE_DEFINITIONS -DUSE_WEBKIT
        ENV ${TEST_ENV}
        LINK_LIBRARIES ${TEST_LIB}
        TEST_NAME alkdownloadengine-webkit-test
    )
endif()

if(BUILD_WITH_WEBENGINE)
    ecm_add_test(
        alkdownloadenginetest.cpp
        alkdownloadenginetest.h
        COMPILE_DEFINITIONS -DUSE_WEBENGINE
        ENV ${TEST_ENV}
        LINK_LIBRARIES ${TEST_LIB}
        TEST_NAME alkdownloadengine-webengine-test
    )
endif()

ecm_add_test(
    alknewstuffenginetest.cpp
    ENV ${TEST_ENV}
    LINK_LIBRARIES alkimia alkimia-internal ${KF_NEWSTUFF_LIB}
    TEST_NAME alknewstuffenginetest
)

ecm_add_test(
    alkwebpagetest.cpp
    ENV ${TEST_ENV}
    LINK_LIBRARIES ${TEST_LIB}
    TEST_NAME alkwebpage${TEST_SUFFIX}test
)

#
# cmake install test
#
set(TEMP_INSTALL_DIR ${CMAKE_BINARY_DIR}/tmp)

if(CMAKE_CROSSCOMPILING)
    if(NOT TARGET_SYSROOT)
        execute_process(
            COMMAND ${CMAKE_C_COMPILER} --print-sysroot
            OUTPUT_VARIABLE TARGET_SYSROOT
            ERROR_VARIABLE ERROR
            RESULT_VARIABLE RESULT
        )
        if(RESULT)
            set(ENABLE_CLIENT_PACKAGE_TEST 0)
            message(WARNING "disabled client package build, because sysroot could not be fetched from compiler - error is ${ERROR_VARIABLE}")
        endif()
        string(REPLACE "\n" "/${TARGET_PREFIX}" TARGET_SYSROOT "${TARGET_SYSROOT}")
    endif()
    set(FIND_ROOT_PATH "-DCMAKE_FIND_ROOT_PATH:PATH=${TARGET_SYSROOT}/mingw")
endif()

if(ENABLE_CLIENT_PACKAGE_TEST)
    # clean local install directories used by client build/install test
    add_test(
        NAME test-install-local-clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${TEMP_INSTALL_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # install dbus in local directory for client build/install test
    add_test(
        NAME test-install-local
        COMMAND ${CMAKE_MAKE_PROGRAM} install DESTDIR=${TEMP_INSTALL_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # build/install a client package using dbus cmake package support from this build
    add_test(
        NAME test-install-build-client-package
        COMMAND ${CMAKE_CTEST_COMMAND}
            --build-and-test
            ${CMAKE_CURRENT_SOURCE_DIR}/client-package
            ${CMAKE_CURRENT_BINARY_DIR}/client-package
            --build-config "${CMAKE_BUILD_TYPE}"
            --build-generator ${CMAKE_GENERATOR}
            --build-makeprogram ${CMAKE_MAKE_PROGRAM}
            --build-target install
            --build-options
                #--trace-expand
                -DALKIMIA_PATH_SUFFIX=${ALKIMIA_PATH_SUFFIX}
                -DLibAlkimia${ALKIMIA_PATH_SUFFIX}_DIR=${TEMP_INSTALL_DIR}${CMAKE_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR}
                -DCHECK_VERSION=${PROJECT_VERSION}
                -DCMAKE_CROSSCOMPILING=${CMAKE_CROSSCOMPILING}
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_INSTALL_PREFIX:PATH=${TEMP_INSTALL_DIR}${CMAKE_INSTALL_PREFIX}
                ${FIND_ROOT_PATH}
                -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER
                -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY
                -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY
                -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                -DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}
        )

    set_property(TEST test-install-build-client-package APPEND PROPERTY DEPENDS test-install-local-clean test-install-local)
endif()
