
#
# install gdb pretty printer
#
set(_srcfile libalkimia-gdb.py)
set(_prefix ${CMAKE_SHARED_LIBRARY_PREFIX})
set(_suffix ${CMAKE_SHARED_LIBRARY_SUFFIX})
get_target_property(_name alkimia OUTPUT_NAME)

if(WIN32)
    set(_version ${PROJECT_VERSION_MAJOR})
    set(_gdb_libname ${_prefix}${_name}-${_version}${_suffix}-gdb.py)
    set(_gdb_install_dir ${CMAKE_INSTALL_BINDIR)
else()
    set(_version ${PROJECT_VERSION})
    set(_gdb_libname ${_prefix}${_name}${_suffix}.${_version}-gdb.py)
    set(_gdb_libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
    set(_gdb_install_dir ${CMAKE_INSTALL_DATADIR}/gdb/auto-load/${_gdb_libdir})
endif()

# used by gdb when debugging executables from the build dir
configure_file(${_srcfile} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_gdb_libname} COPYONLY)

install(
    FILES libalkimia-gdb.py
    DESTINATION ${_gdb_install_dir}
    RENAME ${_gdb_libname}
)
