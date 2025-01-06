# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

cmake_minimum_required(VERSION 3.16.0)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0074 NEW)
endif()

project(alkimiatest)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(LibAlkimia${QT_MAJOR_VERSION} CONFIG)

add_executable(mytest test.cpp)
target_link_libraries(mytest Alkimia::alkimia)
install(TARGETS mytest DESTINATION bin)
