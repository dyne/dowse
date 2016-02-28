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


#include "blocklist.h"
#include "blocklist_p.h"

#include <QFile>
#include <QDebug>

Blocklist::Blocklist(const QString& text, bool active, bool enabled) :
    Option(),
    d_active_ptr(new BlocklistPrivate),
    d_ptr(new BlocklistPrivate)
{
    setActiveData(d_active_ptr);
    setData(d_ptr);

    d_ptr->local = false;
    d_ptr->isSymLink = false;
    d_ptr->targetLocation = "";

    if (QFile::exists(text)) {
        d_ptr->local = true;
        QFileInfo info = QFileInfo(text);
        if (info.isSymLink()) {
            d_ptr->isSymLink = true;
            d_ptr->targetLocation = info.symLinkTarget();
        }
    }

    setEnabled(enabled);
    setName(parseName(text));
    setValue(parseUrl(text));
    d_ptr->valid = isValid(text);
    d_ptr->removed = false;

    if (active)
        *d_active_ptr = *d_ptr;
}

Blocklist::Blocklist(const Blocklist& blocklist)
{
    d_ptr = 0;
    d_active_ptr = 0;
    *this = blocklist;
}

Blocklist::~Blocklist()
{
    //will be deleted by Option destructor
    d_ptr = 0;
    d_active_ptr = 0;
}

bool Blocklist::isSymLink() const
{
    if (d_ptr)
        return d_ptr->isSymLink;
    return false;
}

bool Blocklist::isLocal() const
{
    if (d_ptr)
        return d_ptr->local;
    return false;
}

bool Blocklist::isValid() const
{
    if (d_ptr)
        return d_ptr->valid;
    return false;
}

QString Blocklist::location() const
{
    if (d_ptr)
        return d_ptr->value.toString();
    return "";
}

QString Blocklist::targetLocation() const
{
    if (d_ptr) {
        if (d_ptr->isSymLink)
            return d_ptr->targetLocation;
        return location();
    }
    return "";
}

bool Blocklist::exists() const
{
    if (isLocal())
        return QFile::exists(location());
    return true; //if it's a remote blocklist, assume it exists
}

bool Blocklist::isValid(const QString & url)
{
    if ( url.contains("list.iblocklist.com") )
        return true;

    if ( url.contains("http://") || url.contains("ftp://") || url.contains("https://") )
        return true;

    if ( QFile::exists(url) )
        return true;

    return false;
}

QString Blocklist::parseName(const QString& text)
{
    if ( text.contains("list.iblocklist.com/lists/") )
        return text.split("list.iblocklist.com/lists/").last();

    if ( text.contains("http://") || text.contains("ftp://") || text.contains("https://") )
        return text.split("/").last();

    QFileInfo info(text);

    if ( info.exists() ) {
        QString filename  = info.fileName();
        if (filename.startsWith("."))
            return info.fileName().mid(1);
        else
            return info.fileName();
    }

    return text;
}

QString Blocklist::parseUrl(const QString& text)
{
    if (text.contains('#')) {
        QStringList parts = text.split('#', QString::SkipEmptyParts);
        if(parts.size() >= 1)
            return parts[0].trimmed();
        else
            return "";
    }

    return text;
}

