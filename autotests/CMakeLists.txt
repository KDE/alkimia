include(ECMAddTests)
set(TEST_LIB
    alkimia
    alkimia-internal
    Qt${QT_MAJOR_VERSION}::Test
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::DBus
    ${MP_LIBRARIES}
)

if(BUILD_QT4)
    list(APPEND TEST_LIB ${KDE4_KDECORE_LIBS})
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
set_property(TEST alkonlinequotestest PROPERTY ENVIRONMENT "QTWEBENGINE_DISABLE_SANDBOX=1")

ecm_add_test(
    alkdateformattest.cpp
    LINK_LIBRARIES ${TEST_LIB}
    TEST_NAME alkdateformattest
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
endif()

if(ENABLE_CLIENT_PACKAGE_TEST)
    # clean local install directories used by client build/install test
    add_test(
        NAME test-install-local-clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${TEMP_INSTALL_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # install dbus in local directoy for client build/install test
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
                -DCMAKE_FIND_ROOT_PATH=${TARGET_SYSROOT};${TEMP_INSTALL_DIR}${CMAKE_INSTALL_PREFIX}
                -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER
                -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY
                -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY
                -DCMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME}
                -DCMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}
        )

    set_property(TEST test-install-build-client-package APPEND PROPERTY DEPENDS test-install-local-clean test-install-local)
endif()
