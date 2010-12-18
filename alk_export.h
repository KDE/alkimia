/***************************************************************************
 *   Copyright 2010  Thomas Baumgart  ipwizard@users.sourceforge.net       *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

#ifndef ALK_EXPORT_H
#define ALK_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <kdemacros.h>

#ifndef ALK_EXPORT
# if defined(MAKE_ALK_LIB)
   /* We are building this library */ 
#  define ALK_EXPORT KDE_EXPORT
# else
   /* We are using this library */ 
#  define ALK_EXPORT KDE_IMPORT
# endif
#endif

# ifndef ALK_EXPORT_DEPRECATED
#  define ALK_EXPORT_DEPRECATED KDE_DEPRECATED ALK_EXPORT
# endif

#endif
