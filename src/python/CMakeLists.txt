
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
else()
    set(_version ${PROJECT_VERSION})
    set(_gdb_libname ${_prefix}${_name}${_suffix}.${_version}-gdb.py)
endif()

# used by gdb when debugging executables from the build dir
configure_file(${_srcfile} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_gdb_libname} COPYONLY)

set(_libdir ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR})
install(
    FILES libalkimia-gdb.py
    DESTINATION ${DATA_INSTALL_DIR}/gdb/auto-load/${_libdir}
    RENAME ${_gdb_libname}
)
