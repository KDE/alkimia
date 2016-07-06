# The CMake version we require (must be first)
cmake_minimum_required(VERSION 2.8.12)

project(alkimia)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

# check for PkgConfig, Qt and GMP
find_package(PkgConfig)
find_package(Qt5 REQUIRED COMPONENTS Core DBus Test)
find_package(GMP REQUIRED)

# check for Doxygen (for API documentation)
find_package(Doxygen)

feature_summary(WHAT ALL)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

add_subdirectory(libalkimia)
add_subdirectory(autotests)
