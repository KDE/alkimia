# Building instruction

Requirements
* cmake
* git
* cmake supported C++ compiler including linker
* Qt4 or Qt5 development packages (see below)
* KDELibs4 or KF5 development packages (see below)
* libMPIR or libGMP



Required development components to build Alkimia with Qt4 and KDELibs4

    extra-cmake-modules
    gmp-devel
    KDE4-devel

Required development components to build Alkimia5 with Qt5 and KF5

    Qt5::Core
    Qt5::Test
    Qt5::WebKitWidgets
    Qt5::Qml

    KF5::Config
    KF5::CoreAddons
    KF5::Completion
    KF5::I18n
    KF5::Package
    KF5::Plasma
    KF5::NewStuff
    KF5::KIO
    KF5::IconThemes
    KF5::TextWidgets

Packages

    The names of the development packages that contain these components
    are often distribution-specific, some examples are given below:

    - KDE4:
        - opensuse
            extra-cmake-modules
            gmp-devel
            libkde4-devel
        - mingw32 cross compile on obs:
            mingw32-extra-cmake-modules
            mingw32-gmp-devel
            mingw32-libkde4-devel
        - mingw64 cross compile on obs:
            mingw64-extra-cmake-modules
            mingw64-gmp-devel
            mingw64-libkde4-devel
    - KF5:
        - opensuse
            extra-cmake-modules
            gmp-devel
            libQt5Core-devel
            libQt5Test-devel
            libQt5DBus-devel
            kconfig-devel
            kcoreaddons-devel
            kcompletion-devel
            ki18n-devel
            kiconthemes-devel
            kio-devel
            knewstuff-devel
            kpackage-devel
            ktextwidgets-devel
            plasma-framework-devel
        - msvc (craft):
            libs/qt5/qtbase
            kde/frameworks/extra-cmake-modules
            kde/frameworks/extra-cmake-modules
            kde/frameworks/tier3/knewstuff
            kde/frameworks/.../kconfig
            kde/frameworks/.../kcoreaddons
            kde/frameworks/.../kcompletion
            kde/frameworks/.../ki18n
            kde/frameworks/.../kiconthemes
            kde/frameworks/.../kio
            kde/frameworks/.../knewstuff
            kde/frameworks/.../kpackage
            kde/frameworks/.../ktextwidgets
            kde/frameworks/tier3/plasma-framework
            libs/mpir


Fetch alkimia source package from git repo and unpack into a subdirectory with

    cd <root-dir>
    git clone <alkimia-git-repo-url>


For building with Qt5 run the following shell commands

    mkdir <root-dir>/alkimia-build
    cd <root-dir>/alkimia-build
    cmake <root-dir>/alkimia
    make
    sudo make install


In case you are building for Qt4 add -DBUILD_QT4=1 to the cmake command line e.g.

    cmake -DBUILD_QT4=1 <root-dir>/alkimia


To use alkimia in client applications or libraries with cmake and Qt5 add the following line to the top level CMakeLists.txt

    find_package(LibAlkimia5)


for application or libraries with Qt4 add the following line to the top level CMakeLists.txt

    find_package(LibAlkimia)


On creating targets add the library with

    add_executable(<target> ...)
    target_link_libraries(<target>  Alkimia::alkimia)


In source code include header file for example with

    #include <alkimia/alkvalue.h>

    ...

The alkimia library can be created with support for the external perl package "Finance:Quote".

On Linux hosts this is automatically enabled, on Windows hosts the parameter -DENABLE_FINANCEQUOTE=1 has to be added to the CMake call.

Packages using alkimia can check and use this support with a CPP condition.

    #ifdef HAVE_ALK_FINANCEQUOTE
    #include <alkimia/alkfinancequoteprocess.h>
    #endif

    #ifdef HAVE_ALK_FINANCEQUOTE
    ...
    #endif
