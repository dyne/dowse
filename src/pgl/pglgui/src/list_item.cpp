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


#include "list_item.h"

#include <QFile>
#include <QFileInfo>
#include <QStringList>

ListItem::ListItem( const QString &itemRawLine ) {

    QString itemLine = itemRawLine.simplified();

    mValue = itemLine;
    mode = COMMENT_ITEM;
    mBlocklist = 0;

    if ( itemLine.isEmpty() ) {
        mode = COMMENT_ITEM;
    }
    else if (itemLine.startsWith('#')){
        if ( Blocklist::isValid(itemLine) )
        {
            mode = DISABLED_ITEM;
            mBlocklist = new Blocklist(itemLine, true, false);
        }
        else
            mode = COMMENT_ITEM;
    }
    else if(Blocklist::isValid(itemLine)){
        mode = ENABLED_ITEM;
        mBlocklist = new Blocklist(itemLine, true);
    }
}

bool ListItem::isDisabled()
{
    return (mode == DISABLED_ITEM);
}

bool ListItem::isEnabled()
{
    return (mode == ENABLED_ITEM);
}

QString ListItem::value() const
{
    return mValue;
}

bool ListItem::operator==( const ListItem &other )
{
    if (mBlocklist && other.blocklist())
        return (mBlocklist->location() == other.blocklist()->location());
    return (value() == other.value());
}

bool ListItem::isBlocklist()
{
    if (mBlocklist)
        return true;
    return false;
}

Blocklist* ListItem::blocklist() const
{
    return mBlocklist;
}
