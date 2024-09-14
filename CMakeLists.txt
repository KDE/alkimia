# The CMake version we require (must be first)
cmake_minimum_required(VERSION 3.16.0)
if(POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()

project(alkimia VERSION 8.1.76)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

# adds option BUILD_WITH_QT6
include(QtVersionOption OPTIONAL)

option(BUILD_QT4 "Build for Qt4" OFF)
option(BUILD_TOOLS "Build tools" ON)
option(BUILD_WITH_QTNETWORK "Build with Qt Network library for downloading" WIN32)
option(BUILD_WITH_WEBKIT "Build with Qt Web Kit library" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(ENABLE_CLIENT_PACKAGE_TEST "enable test for building a client package" OFF)

# No Plasma on Windows, so no applets
include(CMakeDependentOption)
cmake_dependent_option(BUILD_APPLETS "Build plasma applets" ON "NOT WIN32" OFF)

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

if(BUILD_QT4)
    set(QT_MAJOR_VERSION 4)
endif()

include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEInstallDirs)

# provides option BUILD_TESTING
include(KDECMakeSettings)

include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

include(Macros)

if(NOT QT_MAJOR_VERSION)
    message(FATAL_ERROR "No Qt version defined")
endif()

set(QT_USE_LIBSPREFIX Qt${QT_MAJOR_VERSION}::)

# select network module
if(NOT BUILD_WITH_QTNETWORK)
    message(WARNING "No download backend selected, use Qt Network")
    set(BUILD_WITH_QTNETWORK 1)
endif()
if(BUILD_WITH_QTNETWORK)
    add_definitions(-DBUILD_WITH_QTNETWORK)
    set(QT_NETWORK_COMPONENT Network)
    set(QT_NETWORK_LIB ${QT_USE_LIBSPREFIX}Network)
    message(STATUS "Using Qt Network as download backend")
endif()

# check for PkgConfig, Qt and MPIR/GMP
find_package(PkgConfig)
if(QT_MAJOR_VERSION EQUAL 4)
    set(ALKIMIA_LIB_SUFFIX "")
    set(ALKIMIA_PATH_SUFFIX)
    set(PC_TARGET_QTPREFIX Qt)
    set(PC_TARGET_SUFFIX)

    # select browser engine
    if(BUILD_WITH_WEBKIT)
        add_definitions(-DBUILD_WITH_WEBKIT)
        set(QT_BROWSER_COMPONENT WebKit)
        set(QT_BROWSER_LIB ${QT_USE_LIBSPREFIX}WebKit)
        message(STATUS "Using Qt WebKit as download backend with javascript support")
        feature_notes(TRUE TRUE TRUE TRUE)
    else()
        feature_notes(FALSE TRUE FALSE FALSE)
    endif()

    # select qml engine
    set(QT_QML_COMPONENT Declarative)
    set(QT_QML_LIB ${QT_USE_LIBSPREFIX}${QT_QML_COMPONENT})

    # setup Qt components
    set(QT_PRIVATE_COMPONENTS Core Gui DBus ${QT_NETWORK_COMPONENT} Test ${QT_BROWSER_COMPONENT} ${QT_QML_COMPONENT})
    set(QT_PUBLIC_COMPONENTS)
    find_package(Qt${QT_MAJOR_VERSION} COMPONENTS ${QT_PRIVATE_COMPONENTS} ${QT_PUBLIC_COMPONENTS})

    # setup KDE Frameworks components
    find_package(KF${QT_MAJOR_VERSION} REQUIRED)
    set(KF_NEWSTUFF_LIB KF${QT_MAJOR_VERSION}::NewStuff knewstuff)
    set(QML_INSTALL_DIR ${LIB_INSTALL_DIR}/qt4/plugins/imports)
    set(BUILD_WITH_WEBKIT 1)

    # install custom cmake files
    set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}-${alkimia_VERSION_MAJOR}.${alkimia_VERSION_MINOR}")
    install(
        FILES ${QT4_INSTALL_CMAKECONFIG_FILES} ${KF4_INSTALL_CMAKECONFIG_FILES}
        DESTINATION ${CMAKECONFIG_INSTALL_DIR}
        COMPONENT Devel
    )
    add_subdirectory(3rdparty/KF${QT_MAJOR_VERSION}/knewstuff)
else()
    if (QT_MAJOR_VERSION EQUAL 5)
        set(QT_MIN_VERSION 5.12.2)
        set(KF_MIN_VERSION 5.76)
    endif()
    set(ALKIMIA_LIB_SUFFIX "${QT_MAJOR_VERSION}")
    set(ALKIMIA_PATH_SUFFIX ${QT_MAJOR_VERSION})
    set(PC_TARGET_QTPREFIX Qt${QT_MAJOR_VERSION})
    set(PC_TARGET_SUFFIX ${QT_MAJOR_VERSION})

    # select browser engine
    find_package(Qt${QT_MAJOR_VERSION}WebEngineWidgets ${QT_MIN_VERSION})

    option(BUILD_WITH_WEBENGINE "Build with Qt Web Engine (experimental)" ${Qt${QT_MAJOR_VERSION}WebEngineWidgets_FOUND})
    if (BUILD_WITH_WEBENGINE AND NOT BUILD_WITH_WEBKIT)
        add_definitions(-DBUILD_WITH_WEBENGINE)
        set(QT_BROWSER_COMPONENT WebEngineWidgets)
        message(STATUS "Using Qt WebEngine as download backend with javascript support")
        feature_notes(UNSTABLE TRUE FALSE TRUE)
    elseif(BUILD_WITH_WEBKIT)
        add_definitions(-DBUILD_WITH_WEBKIT)
        set(QT_BROWSER_COMPONENT WebKitWidgets)
        message(STATUS "Using Qt WebKit as download backend with javascript support")
        feature_notes(TRUE TRUE TRUE TRUE)
    else()
        set(QT_BROWSER_COMPONENT Widgets)
        feature_notes(FALSE TRUE FALSE FALSE)
    endif()
    set(QT_BROWSER_LIB ${QT_USE_LIBSPREFIX}${QT_BROWSER_COMPONENT})

    set(QT_QML_COMPONENT Qml)
    set(QT_QML_LIB ${QT_USE_LIBSPREFIX}${QT_QML_COMPONENT})

    # setup Qt components
    set(QT_PRIVATE_COMPONENTS Core DBus ${QT_NETWORK_COMPONENT} Test ${QT_QML_COMPONENT})
    set(QT_PUBLIC_COMPONENTS ${QT_BROWSER_COMPONENT})
    find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS ${QT_PRIVATE_COMPONENTS} ${QT_PUBLIC_COMPONENTS})

    # setup KDE Frameworks components
    if(BUILD_APPLETS)
        set(PLASMA_COMPONENT Package Plasma)
    endif()
    if(QT_MAJOR_VERSION GREATER_EQUAL 6)
        set(QT_DBUS_LIB Qt${QT_MAJOR_VERSION}::DBus)
        set(KF_NEWSTUFF_COMPONENT NewStuffCore NewStuff)
        set(KF_NEWSTUFF_LIB KF${QT_MAJOR_VERSION}::NewStuffWidgets)
        set(KF_PRIVATE_COMPONENTS Codecs WidgetsAddons)
        set(KF_PRIVATE_LIBS KF${QT_MAJOR_VERSION}::Codecs KF${QT_MAJOR_VERSION}::WidgetsAddons)
    else()
        set(QT_DBUS_LIB)
        set(KF_NEWSTUFF_COMPONENT NewStuff)
        set(KF_NEWSTUFF_LIB KF${QT_MAJOR_VERSION}::${KF_NEWSTUFF_COMPONENT})
        set(KF_PRIVATE_COMPONENTS)
    endif()

    set(KF_PUBLIC_COMPONENTS Config)
    list(APPEND KF_PRIVATE_COMPONENTS
        CoreAddons
        Completion
        I18n
        ${KF_NETWORK_COMPONENT}
        ${KF_NEWSTUFF_COMPONENT}
        IconThemes
        ${PLASMA_COMPONENT}
        TextWidgets
        XmlGui
    )
    find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS ${KF_PRIVATE_COMPONENTS} ${KF_PUBLIC_COMPONENTS})

    macro(ecm_add_executable)
        add_executable(${ARGN})
    endmacro(ecm_add_executable)

    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
    endif()

    set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/LibAlkimia${ALKIMIA_PATH_SUFFIX}-${alkimia_VERSION_MAJOR}.${alkimia_VERSION_MINOR}")

    add_definitions(
        -DKF_DEPRECATED_WARNINGS_SINCE=0x60000
        -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x56200C
        -DQT_DEPRECATED_WARNINGS_SINCE=0x60000
        -DQT_DISABLE_DEPRECATED_BEFORE=0x50f02
        -DQT_NO_KEYWORDS
    )
endif()

# setup common variables
set(ALKIMIA_INCLUDE_SUFFIX "Qt${QT_MAJOR_VERSION}")

# setup components for use in cmake support files
set(CMAKECONFIG_QT_COMPONENTS ${QT_PUBLIC_COMPONENTS})
set(CMAKECONFIG_KF_COMPONENTS ${KF_PUBLIC_COMPONENTS})

# static builds need also private components
if(NOT BUILD_SHARED_LIBS)
    list(APPEND CMAKECONFIG_QT_COMPONENTS ${QT_PRIVATE_COMPONENTS})
    list(APPEND CMAKECONFIG_KF_COMPONENTS ${KF_PRIVATE_COMPONENTS})
endif()

if(NOT SHARE_INSTALL_DIR)
    set(SHARE_INSTALL_DIR ${DATA_INSTALL_DIR})
endif()

set(TARGET_SUFFIX ${PC_TARGET_SUFFIX})

find_package(Git QUIET)
set(BUILD_KEY "${PROJECT_VERSION}")
if(Git_FOUND)
    set(GITCOMMAND ${GIT_EXECUTABLE} -C ${CMAKE_CURRENT_SOURCE_DIR})
    execute_process(
        COMMAND ${GITCOMMAND} rev-parse --short HEAD
        OUTPUT_VARIABLE COMMIT_ID
        ERROR_VARIABLE ERROR
        RESULT_VARIABLE RESULT
    )
    if(NOT RESULT AND COMMIT_ID)
        string(STRIP ${COMMIT_ID} COMMIT_ID)
        message(STATUS "Using git commit id '${COMMIT_ID}'")
        string(APPEND BUILD_KEY "-${COMMIT_ID}")
    endif()
endif()
if(BUILD_WITH_WEBKIT)
    string(APPEND BUILD_KEY "-webkit")
elseif(BUILD_WITH_WEBENGINE)
    string(APPEND BUILD_KEY "-webengine")
endif()
if(BUILD_WITH_QTNETWORK)
    string(APPEND BUILD_KEY "-qtnetwork")
endif()
if(ENABLE_FINANCEQUOTE)
    string(APPEND BUILD_KEY "-financequote")
endif()

add_definitions(
    -DTARGET_SUFFIX=\"${TARGET_SUFFIX}\"
    -DKNSRC_DIR="${CMAKE_SOURCE_DIR}/src"
    -DBUILD_KEY="${BUILD_KEY}"
)

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

include_directories(${MP_INCLUDE_DIR})

# check for Doxygen (for API documentation)
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

  make_directory("${APIDOC_DIR}")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/alkimia.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/alkimia.doxygen IMMEDIATE)

  add_custom_target(apidoc "${DOXYGEN}" "${CMAKE_CURRENT_BINARY_DIR}/alkimia.doxygen")
endif(DOXYGEN_FOUND)

feature_summary(WHAT ALL)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

add_subdirectory(src)
add_subdirectory(qml)

if(BUILD_APPLETS)
    add_subdirectory(plasma/applets)
endif()

if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()

if (BUILD_TOOLS)
    add_subdirectory(tools)
endif()

ki18n_install(po)
