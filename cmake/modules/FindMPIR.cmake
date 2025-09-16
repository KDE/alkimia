# Try to find the MPIR libraries
#  MPIR_FOUND - system has MPIR lib
#  MPIR_INCLUDE_DIR - the MPIR include directory
#  MPIR_LIBRARIES - Libraries needed to use MPIR

# SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
# SPDX-FileCopyrightText: 2018 Thomas Baumgart tbaumgart @kde.org
#
# SPDX-License-Identifier: BSD-3-Clause

if (MPIR_INCLUDE_DIR AND MPIR_LIBRARIES)
    # Already in cache, be silent
    set(MPIR_FIND_QUIETLY TRUE)
endif (MPIR_INCLUDE_DIR AND MPIR_LIBRARIES)

find_path(MPIR_INCLUDE_DIR NAMES mpir.h )
find_library(MPIR_LIBRARIES NAMES mpir libmpir)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MPIR DEFAULT_MSG MPIR_INCLUDE_DIR MPIR_LIBRARIES)

mark_as_advanced(MPIR_INCLUDE_DIR MPIR_LIBRARIES)
