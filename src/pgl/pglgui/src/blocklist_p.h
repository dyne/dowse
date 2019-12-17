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


#ifndef BLOCKLIST_PRIVATE_H
#define BLOCKLIST_PRIVATE_H

#include <QString>

#include "option_p.h"

/**
*
* @short Class representing a blocklist, either local or remote.
*
*/

class BlocklistPrivate : public OptionPrivate
{

public:
    explicit BlocklistPrivate() {};
    virtual ~BlocklistPrivate() {};

public:
    bool local;
    bool valid;
    bool isSymLink;
    QString targetLocation;
};

#endif //BLOCKLIST_H

