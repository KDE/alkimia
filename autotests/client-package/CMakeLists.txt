#SPDX-License-Identifier: BSD-2-Clause
#SPDX-FileCopyrightText: 2023 Ralf Habacker ralf.habacker @freenet.de

cmake_minimum_required(VERSION 3.16.0)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0074 NEW)
endif()

project(alkimiatest)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(LibAlkimia${ALKIMIA_PATH_SUFFIX} CONFIG)

if(NOT LibAlkimia${ALKIMIA_PATH_SUFFIX}_VERSION OR NOT CHECK_VERSION OR NOT LibAlkimia${ALKIMIA_PATH_SUFFIX}_VERSION VERSION_EQUAL CHECK_VERSION)
    message(FATAL_ERROR "version mismatch actual '${LibAlkimia${ALKIMIA_PATH_SUFFIX}_VERSION}' expected '${CHECK_VERSION}'")
endif()

add_executable(mytest test.cpp)
target_link_libraries(mytest Alkimia::alkimia)
install(TARGETS mytest DESTINATION bin)
