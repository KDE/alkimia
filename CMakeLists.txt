# The CMake version we require (must be first)
cmake_minimum_required(VERSION 3.0.2)

project(alkimia VERSION 7.0.1)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

# check for PkgConfig, Qt and MPIR/GMP
find_package(PkgConfig)
if(BUILD_QT4)
    find_package(Qt4 REQUIRED COMPONENTS QtCore QtDBus QtTest)
    set(QT_USE_LIBSPREFIX Qt4::Qt)
    set(ALKIMIA_LIB_SUFFIX "")
    set(ALKIMIA_INCLUDE_SUFFIX "Qt4")
    set(ALKIMIA_PATH_SUFFIX)
    set(PC_TARGET_QTPREFIX Qt)
    set(PC_TARGET_SUFFIX)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core DBus Test)
    set(QT_USE_LIBSPREFIX Qt5::)
    set(ALKIMIA_LIB_SUFFIX "5")
    set(ALKIMIA_INCLUDE_SUFFIX "Qt5")
    set(ALKIMIA_PATH_SUFFIX 5)
    set(PC_TARGET_QTPREFIX Qt5)
    set(PC_TARGET_SUFFIX 5)
endif()

# figure out which multi-precision library to use
# MPIR is preferred over GMP
find_package(MPIR)
if(MPIR_FOUND)
  set(MP_INCLUDE_DIR ${MPIR_INCLUDE_DIR})
  set(MP_LIBRARIES ${MPIR_LIBRARIES})
  set(MP_HEADER mpirxx.h)
  set(MP_CMAKE_MODULE "MPIR")
  set(PC_LIB mpir)
else()
  find_package(GMP REQUIRED)
  set(MP_INCLUDE_DIR ${GMP_INCLUDE_DIR})
  set(MP_LIBRARIES ${GMP_LIBRARIES})
  set(MP_HEADER gmpxx.h)
  set(MP_CMAKE_MODULE "GMP")
  set(PC_LIB gmp)
endif()

# check for Doxygen (for API documentation)
find_package(Doxygen)

feature_summary(WHAT ALL)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

add_subdirectory(src)
add_subdirectory(autotests)
