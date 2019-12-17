/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
 *                                                                            *
 *   This file is part of pgl.                                                *
 *                                                                            *
 *   pgl is free software: you can redistribute it and/or modify              *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation, either version 3 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 *   pgl is distributed in the hope that it will be useful,                   *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with pgl.  If not, see <http://www.gnu.org/licenses/>.             *
 *****************************************************************************/


#ifndef OPTION_PRIVATE_H
#define OPTION_PRIVATE_H

#include <QVariant>
#include <QString>

/**
*
* @short Class representing an option in the program.
*
*/

class OptionPrivate {

        public:
        explicit OptionPrivate();
        virtual ~OptionPrivate();
        bool operator==(const OptionPrivate&);

public:
    bool removed;
    bool enabled;
    QVariant value;
    QString name;
};

#endif //OPTION_PRIVATE_H

