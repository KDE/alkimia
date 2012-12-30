# The CMake version we require
cmake_minimum_required(VERSION 2.6.4)

# setup default CMAKE_INSTALL_PREFIX before calling FIND_PACKAGE(KDE4)
# see https://bugs.kde.org/show_bug.cgi?id=261132
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  execute_process(COMMAND kde4-config --prefix COMMAND tr -d "\n" OUTPUT_VARIABLE KDE4_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${KDE4_INSTALL_PREFIX}" CACHE PATH
    "Alkimia install prefix defaults to the KDE4 install prefix: ${KDE4_INSTALL_DIR}" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# check for PkgConfig, KDE and GMP
find_package(PkgConfig REQUIRED)
find_package(KDE4 REQUIRED)
find_package(GMP REQUIRED)

include(KDE4Defaults)

add_subdirectory(libalkimia)
