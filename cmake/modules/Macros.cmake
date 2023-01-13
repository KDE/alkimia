# SPDX-FileCopyrightText: 2020,2023 Ralf Habacker
#
# SPDX-License-Identifier: BSD-3-Clause

macro(feature_note text state)
    if(${state})
        message(STATUS "${text} is supported")
    else()
        message(WARNING "${text} is not supported")
    endif()
endmacro()

macro(feature_notes a b c d)
    feature_note("Fetching pages with javascript" ${a})
    feature_note("Preview of fetched pages" ${b})
    feature_note("Multilingual preview of web pages" ${c})
    feature_note("Inspecting of web pages" ${c})
endmacro()

if(BUILD_QT4)
# SPDX-FileCopyrightText: 2006 Alexander Neundorf <neundorf@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

set(_ki18n_build_pofiles_script ${CMAKE_CURRENT_LIST_DIR}/build-pofiles.cmake)
set(_ki18n_build_tsfiles_script ${CMAKE_CURRENT_LIST_DIR}/build-tsfiles.cmake)
find_program(KI18N_PYTHON_EXECUTABLE python3)
find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

# KI18N_INSTALL(po) does the following:
# - Compiles kfoo.po into kfoo.mo and installs it in
#   ${LOCALE_INSTALL_DIR}/fr/LC_MESSAGES or share/locale/fr/LC_MESSAGES if
#   ${LOCALE_INSTALL_DIR} is not set.
# - Installs kfoo.js in ${LOCALE_INSTALL_DIR}/fr/LC_SCRIPTS/kfoo
#
# KI18N_INSTALL_TS_FILES() is deprecated, use KI18N_INSTALL()
#
function(KI18N_INSTALL podir)
    if (NOT LOCALE_INSTALL_DIR)
        set(LOCALE_INSTALL_DIR share/locale)
    endif()

    if(NOT KI18N_PYTHON_EXECUTABLE)
        message(FATAL_ERROR "KI18N_PYTHON_EXECUTABLE was not found")
    endif()

    if(NOT GETTEXT_MSGFMT_EXECUTABLE)
        message(FATAL_ERROR "GETTEXT_MSGFMT_EXECUTABLE was not found")
    endif()

    # First try to find the po directory in the source directory, where the release scripts copy them before making the tarballs
    get_filename_component(absolute_podir ${podir} ABSOLUTE)

    # we try to find the po directory in the binary directory, in case it was downloaded
    # using ECM
    if (NOT (EXISTS "${absolute_podir}" AND IS_DIRECTORY "${absolute_podir}"))
            get_filename_component(absolute_podir ${CMAKE_CURRENT_BINARY_DIR}/${podir} ABSOLUTE)
    endif()

    if (NOT (EXISTS "${absolute_podir}" AND IS_DIRECTORY "${absolute_podir}"))
        # Nothing to do if there's no podir and it would create an empty
        # LOCALE_INSTALL_DIR in that case.
        return()
    endif()

    get_filename_component(dirname ${LOCALE_INSTALL_DIR} NAME)
    get_filename_component(destname ${LOCALE_INSTALL_DIR} DIRECTORY)
    string(MD5 pathmd5 ${absolute_podir})

    add_custom_target(pofiles-${pathmd5} ALL
        COMMENT "Generating mo..."
        COMMAND ${CMAKE_COMMAND}
                -DGETTEXT_MSGFMT_EXECUTABLE=${GETTEXT_MSGFMT_EXECUTABLE}
                -DCOPY_TO=${CMAKE_CURRENT_BINARY_DIR}/${dirname}
                -DPO_DIR=${absolute_podir}
                -P ${_ki18n_build_pofiles_script}
    )
    add_custom_target(tsfiles-${pathmd5} ALL
        COMMENT "Generating ts..."
        COMMAND ${CMAKE_COMMAND}
                -DPYTHON_EXECUTABLE=${KI18N_PYTHON_EXECUTABLE}
                -D_ki18n_pmap_compile_script=${_ki18n_pmap_compile_script}
                -DCOPY_TO=${CMAKE_CURRENT_BINARY_DIR}/${dirname}
                -DPO_DIR=${absolute_podir}
                -P ${_ki18n_build_tsfiles_script}
    )
    if (NOT TARGET pofiles)
        add_custom_target(pofiles)
    endif()
    if (NOT TARGET tsfiles)
        add_custom_target(tsfiles)
    endif()
    add_dependencies(pofiles pofiles-${pathmd5})
    add_dependencies(tsfiles tsfiles-${pathmd5})

    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dirname})
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${dirname} DESTINATION ${destname})
endfunction()

endif()
