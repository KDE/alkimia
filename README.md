# Building instruction

## Requirements

* C++ compiler supported by cmake
* GMP
* Qt/KDE frameworks development packages (see below)
* cmake
* doxygen (optional for api documentation)
* git

## Required Qt/KDE Frameworks development components

### with Qt6 and KF6

    KF6::Codecs
    KF6::Completion
    KF6::Config
    KF6::CoreAddons
    KF6::I18n
    KF6::IconThemes
    KF6::NewStuff
    KF6::Package
    KF6::TextWidgets
    KF6::WidgetsAddons
    KF6::XmlGui
    Qt6::Core
    Qt6::Network
    Qt6::Qml
    Qt6::Test
    kf6-extra-cmake-modules
    qt6-sql-private-devel

Optional packages:

    Plasma
    Qt6::DBus
    Qt6::WebEngineWidgets

### with Qt5 and KF5

    KF5::Completion
    KF5::Config
    KF5::CoreAddons
    KF5::I18n
    KF5::IconThemes
    KF5::NewStuff
    KF5::TextWidgets
    KF5::XmlGui
    Qt5::Core
    Qt5::Network
    Qt5::Test
    Qt5::WebKitWidgets or Qt5::WebEngineWidgets
    Qt5::Qml
    extra-cmake-modules

Optional packages:

    KF5::Package
    KF5::Plasma

### with Qt4 and KF4

Building with KF4/Qt4 is still possible for online services comparison purpose

    extra-cmake-modules
    gmp-devel
    KDE4-devel

## Distribution packages

The names of the development packages that contain these components
are often distribution-specific, some examples are given below:

### KF6 - openSUSE - native
    
    doxygen
    cmake(KF6Codecs)
    cmake(KF6Completion)
    cmake(KF6Config)
    cmake(KF6CoreAddons)
    cmake(KF6I18n)
    cmake(KF6IconThemes)
    cmake(KF6NewStuff)
    cmake(KF6Package)
    cmake(KF6TextWidgets)
    cmake(KF6WidgetsAddons)
    cmake(KF6XmlGui)
    cmake(Plasma)
    cmake(Qt6Core)
    cmake(Qt6DBus)
    cmake(Qt6Qml)
    cmake(Qt6Test)
    cmake(Qt6WebEngineWidgets)
    cmake(Qt6Widgets)
    gmp-devel
    kf6-extra-cmake-modules

### KF5 - openSUSE - native

    doxygen
    extra-cmake-modules
    gmp-devel
    kcompletion-devel
    kconfig-devel
    kcoreaddons-devel
    ki18n-devel
    kiconthemes-devel
    knewstuff-devel
    kpackage-devel
    ktextwidgets-devel
    kxmlgui-devel
    libQt5Core-devel
    libQt5DBus-devel
    libQt5Test-devel
    libQt5WebKit-devel
    plasma-framework-devel

### KF5 - openSUSE - mingw32 (cross compile on https://build.opensuse.org)

    doxygen
    mingw32(cmake:KF5Completion)
    mingw32(cmake:KF5Config)
    mingw32(cmake:KF5CoreAddons)
    mingw32(cmake:KF5I18n)
    mingw32(cmake:KF5IconThemes)
    mingw32(cmake:KF5NewStuff)
    mingw32(cmake:KF5Package)
    mingw32(cmake:KF5TextWidgets)
    mingw32(cmake:Qt5Core)
    mingw32(cmake:Qt5DBus)
    mingw32(cmake:Qt5Qml)
    mingw32(cmake:Qt5Test)
    mingw32(cmake:Qt5WebKit)
    mingw32(cmake:Qt5Widgets)
    mingw32-extra-cmake-modules
    mingw32-gmp-devel

### KF5 - openSUSE - mingw64 (cross compile on https://build.opensuse.org)

    doxygen
    mingw64(cmake:KF5Completion)
    mingw64(cmake:KF5Config)
    mingw64(cmake:KF5CoreAddons)
    mingw64(cmake:KF5I18n)
    mingw64(cmake:KF5IconThemes)
    mingw64(cmake:KF5NewStuff)
    mingw64(cmake:KF5Package)
    mingw64(cmake:KF5TextWidgets)
    mingw64(cmake:Qt5Core)
    mingw64(cmake:Qt5DBus)
    mingw64(cmake:Qt5Qml)
    mingw64(cmake:Qt5Test)
    mingw64(cmake:Qt5WebKit)
    mingw64(cmake:Qt5Widgets)
    mingw64-extra-cmake-modules
    mingw64-gmp-devel

### KF5 - msvc (craft):

    libs/qt5/qtbase
    libs/qt5/qtwebkit
    libs/mpir
    kde/frameworks/extra-cmake-modules
    kde/frameworks/tier3/knewstuff
    kde/frameworks/.../kconfig
    kde/frameworks/.../kcoreaddons
    kde/frameworks/.../kcompletion
    kde/frameworks/.../ki18n
    kde/frameworks/.../kiconthemes
    kde/frameworks/.../knewstuff
    kde/frameworks/.../kpackage
    kde/frameworks/.../ktextwidgets
    kde/frameworks/.../kxmlgui
    kde/frameworks/tier3/plasma-framework

### KDE4 - openSUSE - native

    doxygen
    extra-cmake-modules
    gmp-devel
    libkde4-devel

### KDE4 - openSUSE - mingw32 (cross compile on https://build.opensuse.org):

    doxygen
    mingw32-extra-cmake-modules
    mingw32-gmp-devel
    mingw32-libkde4-devel

### KDE4 - openSUSE - mingw64 (cross compile on https://build.opensuse.org):    

    doxygen
    mingw64-extra-cmake-modules
    mingw64-gmp-devel
    mingw64-libkde4-devel

## Getting source

Fetch alkimia source package from git repo and unpack into a subdirectory with

    cd <root-dir>
    git clone <alkimia-git-repo-url>

## For building run the following shell commands

### with KF6/Qt6

    mkdir <root-dir>/alkimia-build
    cd <root-dir>/alkimia-build
    cmake <root-dir>/alkimia -DBUILD_WITH_QT6=1
    make
    sudo make install

### with KF5/Qt5

    mkdir <root-dir>/alkimia-build
    cd <root-dir>/alkimia-build
    cmake <root-dir>/alkimia
    make
    sudo make install

### with Qt4

    cmake <root-dir>/alkimia -DBUILD_QT4=1

## To use alkimia in client applications or libraries

Add the following line to the top level CMakeLists.txt:

### with cmake and KF6/Qt6

    find_package(LibAlkimia6)


### with cmake and Qt5/KF5

    find_package(LibAlkimia5)


### with Qt4/KF4

    find_package(LibAlkimia)


## On creating targets add the library with

    add_executable(<target> ...)
    target_link_libraries(<target>  Alkimia::alkimia)


## In source code include header file for example with

    #include <alkimia/alkvalue.h>

    ...

## Use "Finance:Quote" support

The alkimia library can be created with support for the external perl package "Finance:Quote".

On Linux hosts this is automatically enabled, on Windows hosts the parameter -DENABLE_FINANCEQUOTE=1 has to be added to the CMake call.

Packages using alkimia can check and use this support with a CPP condition.

    #ifdef HAVE_ALK_FINANCEQUOTE
    #include <alkimia/alkfinancequoteprocess.h>
    #endif

    #ifdef HAVE_ALK_FINANCEQUOTE
    ...
    #endif
