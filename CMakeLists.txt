# The CMake version we require (must be first)
cmake_minimum_required(VERSION 2.8.12)

project(alkimia)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

# check for PkgConfig, KDE and GMP
find_package(PkgConfig)
# Xml Network PrintSupport needed by KDELibs4Support
find_package(Qt5 REQUIRED COMPONENTS Core DBus Test Xml Network PrintSupport)
# KDELibs4Support needed to find 'GMP'
find_package(KF5 REQUIRED COMPONENTS CoreAddons KDELibs4Support)
find_package(GMP REQUIRED)

add_subdirectory(libalkimia)
