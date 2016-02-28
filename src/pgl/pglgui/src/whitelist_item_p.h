/******************************************************************************
 *   Copyright (C) 2013 by Carlos Pais <fr33mind@users.sourceforge.net>       *
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


#ifndef WHITELIST_ITEM_P_H
#define WHITELIST_ITEM_P_H

#include <QMap>
#include <QString>
#include <QStringList>

#include "option_p.h"

class WhitelistItemPrivate : public OptionPrivate
{
    public:
        WhitelistItemPrivate();
        ~WhitelistItemPrivate();
        bool operator ==(const WhitelistItemPrivate&);
        bool containsPort(const QString&);

        QStringList values;
        QString connection; //Incoming, Outgoing or Forward
        int type; //Ip or Port
        QString protocol; //TCP, UDP or IP
        QString group;
};

#endif
