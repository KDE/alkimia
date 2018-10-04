/***************************************************************************
 *   Copyright 2000-2002 by Michael Edwardes mte@users.sourceforge.net     *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
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

#ifndef ALKEXCEPTION_H
#define ALKEXCEPTION_H

#include <alkimia/alk_export.h>

#include <QString>

/**
  * @file
  * @author Thomas Baumgart
  */

/**
  * This class describes an exception that is thrown by the engine
  * in case of a failure.
  */
class ALK_EXPORT AlkException
{
public:

    /**
      * @def ALKEXCEPTION(text)
      * This is the preferred constructor to create a new exception
      * object. It automatically inserts the filename and the source
      * code line into the object upon creation.
      *
      * It is equivilant to AlkException(text, __FILE__, __LINE__)
      */
#define ALKEXCEPTION(what) AlkException(what, __FILE__, __LINE__)

    /**
      * The constructor to create a new ALKEXCEPTION object.
      *
      * @param msg reference to QString containing the message
      * @param file reference to QString containing the name of the sourcefile where
      *             the exception was thrown
      * @param line unsigned long containing the line number of the line where
      *             the exception was thrown in the file.
      *
      * An easier way to use this constructor is to use the macro
      * ALKEXCEPTION(text) instead. It automatically assigns the file
      * and line parameter to the correct values.
      */
    AlkException(const QString &msg, const QString &file, const unsigned long line);

    ~AlkException();

    /**
      * This method is used to return the message that was passed
      * during the creation of the exception object.
      *
      * @return reference to QString containing the message
      */
    const QString &what() const
    {
        return m_msg;
    }

    /**
      * This method is used to return the filename that was passed
      * during the creation of the exception object.
      *
      * @return reference to QString containing the filename
      */
    const QString &file() const
    {
        return m_file;
    }

    /**
      * This method is used to return the linenumber that was passed
      * during the creation of the exception object.
      *
      * @return long integer containing the line number
      */
    unsigned long line() const
    {
        return m_line;
    }

private:
    /**
      * This member variable holds the message
      */
    QString m_msg;

    /**
      * This member variable holds the filename
      */
    QString m_file;

    /**
      * This member variable holds the line number
      */
    unsigned long m_line;
};

#endif
