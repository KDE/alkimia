# The CMake version we require (must be first)
cmake_minimum_required(VERSION 3.0.2)
if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

project(alkimia VERSION 8.0.70)

option(BUILD_QT4 "Build for Qt4" OFF)
option(BUILD_DOXYGEN_DOCS "Build api docs" ON)
option(BUILD_APPLETS "Build plasma applets" ON)

if(WIN32)
    set(ENABLE_FINANCEQUOTE_DEFAULT 0)
else()
    set(ENABLE_FINANCEQUOTE_DEFAULT 1)
endif()
option(ENABLE_FINANCEQUOTE "Enable finance quote support (requires perl)" ENABLE_FINANCEQUOTE_DEFAULT)
if(ENABLE_FINANCEQUOTE)
    add_definitions(-DENABLE_FINANCEQUOTE)
    set(PACKAGE_COMPILE_OPTIONS "-DHAVE_ALK_FINANCEQUOTE")
endif()

find_package(ECM 0.0.11 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(ECMAddAppIcon)
include(ECMInstallIcons)
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
    set(QT_BROWSER_LIB ${QT_USE_LIBSPREFIX}WebKit)
    add_definitions(-DBUILD_WITH_WEBKIT)
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

    macro(ecm_install_icons)
        kde4_install_icons(${ICON_INSTALL_DIR})
    endmacro()

    add_definitions(-DQStringLiteral=QLatin1String)
    if(NOT SHARE_INSTALL_DIR)
        set(SHARE_INSTALL_DIR ${DATA_INSTALL_DIR})
    endif()
else()
    option(BUILD_WITH_WEBKIT "Build with Qt Web Kit library" ON)
    option(BUILD_WITH_WEBENGINE "Build with Qt Web Engine instead of WebKit (experimental)" OFF)
    if (BUILD_WITH_WEBENGINE)
        add_definitions(-DBUILD_WITH_WEBENGINE)
        set(QT5_OPTIONAL_COMPONENTS WebEngineWidgets)
        message(WARNING "Qt Web Engine support is currently incomplete")
    elseif(BUILD_WITH_WEBKIT)
        add_definitions(-DBUILD_WITH_WEBKIT)
        set(QT5_OPTIONAL_COMPONENTS WebKitWidgets)
    else()
        set(QT5_OPTIONAL_COMPONENTS Widgets)
    endif()
    find_package(Qt5 REQUIRED COMPONENTS
        Core
        Test
        ${QT5_OPTIONAL_COMPONENTS}
        Qml
    )
    if(BUILD_APPLETS)
        set(PLASMA_COMPONENT Package Plasma)
    endif()
    # search packages used by KDE
    find_package(KF5 REQUIRED COMPONENTS
        Config
        CoreAddons
        Completion
        I18n
		${PLASMA_COMPONENT}
        NewStuff
        KIO
        IconThemes
        TextWidgets
        XmlGui
        OPTIONAL_COMPONENTS ${OPT_KF5_COMPONENTS}
    )

    set(QT_USE_LIBSPREFIX Qt5::)
    set(ALKIMIA_LIB_SUFFIX "5")
    set(ALKIMIA_INCLUDE_SUFFIX "Qt5")
    set(ALKIMIA_PATH_SUFFIX 5)
    set(PC_TARGET_QTPREFIX Qt5)
    set(PC_TARGET_SUFFIX 5)

    if(BUILD_WITH_WEBENGINE)
        set(QT_BROWSER_LIB ${QT_USE_LIBSPREFIX}WebEngineWidgets)
    elseif(BUILD_WITH_WEBKIT)
        set(QT_BROWSER_LIB ${QT_USE_LIBSPREFIX}WebKitWidgets)
    else()
        set(QT_BROWSER_LIB ${QT_USE_LIBSPREFIX}Widgets)
    endif()

    macro(ecm_add_executable)
        add_executable(${ARGN})
    endmacro(ecm_add_executable)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()
endif()

set(TARGET_SUFFIX ${PC_TARGET_SUFFIX})
add_definitions(-DTARGET_SUFFIX=\"${TARGET_SUFFIX}\")

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
if(BUILD_DOXYGEN_DOCS)
    find_package(Doxygen)
endif()

feature_summary(WHAT ALL)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
    ${MP_INCLUDE_DIR}
)

add_subdirectory(src)
add_subdirectory(autotests)
add_subdirectory(qml)
if(BUILD_APPLETS)
    if(BUILD_QT4)
        add_subdirectory(plasma/applets/onlinequote)
    else()
        add_subdirectory(plasma/applets/ForeignCurrencies)
    endif()
endif()
add_subdirectory(tests)
add_subdirectory(tools)
