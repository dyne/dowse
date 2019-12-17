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


#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include "settings.h"
#include "option.h"
#include "blocklist_p.h"

/**
*
* @short Class representing a blocklist, either local or remote.
*
*/

class BlocklistPrivate;

class Blocklist : public Option
{

public:
    Blocklist(const QString&, bool active=false, bool enabled=true);
    Blocklist(const Blocklist&);
    virtual ~Blocklist();

    bool isSymLink() const;
    bool isLocal() const;
    bool isValid() const;
    bool exists() const;
    QString location() const;
    QString targetLocation()const;
    static bool isValid(const QString&);

protected:
    QString parseName(const QString&);
    QString parseUrl(const QString&);

private:
    BlocklistPrivate *d_active_ptr;
    BlocklistPrivate *d_ptr;

};



#endif //BLOCKLIST_H

