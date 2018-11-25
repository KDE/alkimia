# The CMake version we require (must be first)
cmake_minimum_required(VERSION 3.0.2)

project(alkimia VERSION 7.0.80)

option(BUILD_QT4 "Build for Qt4" ON)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

# check for PkgConfig, Qt and MPIR/GMP
find_package(PkgConfig)
if(BUILD_QT4)
    find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtDBus QtTest QtWebKit QtDeclarative)
    set(QT_USE_LIBSPREFIX Qt4::Qt)
    set(ALKIMIA_LIB_SUFFIX "")
    set(ALKIMIA_INCLUDE_SUFFIX "Qt4")
    set(ALKIMIA_PATH_SUFFIX)
    set(PC_TARGET_QTPREFIX Qt)
    set(PC_TARGET_SUFFIX)
    set(_kde4_uninstall_rule_created 1)
    find_package(KDE4 REQUIRED)
    include(KDE4Defaults)
    set(KDE_LIBRARIES ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS})
    include_directories(${KDE4_INCLUDES})
    macro(ki18n_wrap_ui)
        kde4_add_ui_files(${ARGN})
    endmacro(ki18n_wrap_ui)
    macro(kconfig_add_kcfg_files)
        kde4_add_kcfg_files(${ARGN})
    endmacro(kconfig_add_kcfg_files)
    macro(ecm_add_executable)
        kde4_add_executable(${ARGN})
    endmacro(ecm_add_executable)

    macro(ecm_mark_nongui_executable)
        foreach(_target ${ARGN})
            set_target_properties(${_target}
                                  PROPERTIES
                                  WIN32_EXECUTABLE FALSE
                                  MACOSX_BUNDLE FALSE
                                 )
        endforeach()
    endmacro(ecm_mark_nongui_executable)
else()
    find_package(Qt5 REQUIRED COMPONENTS
        Core
        Quick
        Test
        WebKitWidgets
        Qml
    )
    # search packages used by KDE
    find_package(KF5 REQUIRED COMPONENTS
        Config
        CoreAddons
        KDELibs4Support
        #I18n
        #WebKit
        Package
        Plasma
        NewStuff
    )
    set(QT_USE_LIBSPREFIX Qt5::)
    set(ALKIMIA_LIB_SUFFIX "5")
    set(ALKIMIA_INCLUDE_SUFFIX "Qt5")
    set(ALKIMIA_PATH_SUFFIX 5)
    set(PC_TARGET_QTPREFIX Qt5)
    set(PC_TARGET_SUFFIX 5)
    macro(ecm_add_executable)
        add_executable(${ARGN})
    endmacro(ecm_add_executable)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()
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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(
    ${MP_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(qml)
if(BUILD_QT4)
    add_subdirectory(plasma/applets/onlinequote)
else()
    add_subdirectory(plasma/applets/ForeignCurrencies)
endif()
add_subdirectory(tests)
add_subdirectory(tools)
