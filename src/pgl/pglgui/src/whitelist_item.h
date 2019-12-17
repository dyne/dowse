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


#ifndef WHITELIST_ITEM_H
#define WHITELIST_ITEM_H

#include <QMap>
#include <QString>
#include <QStringList>

#include "option.h"
#include "whitelist_item_p.h"

class WhitelistItemPrivate;

enum {
    TYPE_INCOMING,
    TYPE_OUTGOING,
    TYPE_FORWARD
};

enum {
    ENABLED,
    DISABLED,
    INVALID
};

class WhitelistItem : public Option
{
    WhitelistItemPrivate* d_ptr;
    WhitelistItemPrivate* d_active_ptr;

    public:
        WhitelistItem();
        WhitelistItem(const QString&, const QString&, const QString&, bool active=false, bool enabled=true);
        WhitelistItem(const WhitelistItem&);
        virtual ~WhitelistItem();
        WhitelistItem& operator=(const WhitelistItem&);

        QString value() const;
        QStringList values() const;
        QString connection() const;
        int type();
        QString protocol() const;
        QString group() const;
        void addAlias(const QString &);
        void addAliases(const QStringList& );
        //QString getTypeAsString();
        //QStringList getAsStringList(){ return QStringList() << m_Value << getTypeAsString(); }
        bool operator==(const WhitelistItem &);
        bool containsPort(const QString&);
};

#endif
