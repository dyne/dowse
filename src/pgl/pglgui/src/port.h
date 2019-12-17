/******************************************************************************
 *   Copyright (C) 2011-2014 by Carlos Pais <fr33mind@users.sourceforge.net>  *
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


#ifndef PORT_H
#define PORT_H

#include <QString>
#include <QStringList>

class Port
{
    int mNumber;
    QStringList mProtocols;
    QStringList mNames;

    public:
        Port();
        Port(const Port& other);
        Port(const QString& desig, const QString& prot, int n=0);
        virtual ~Port(){};
        void addProtocols(const QStringList&);
        void addName(const QString&);
        void addNames(const QStringList&);
        bool containAlias(const QString&);
        int number() const;
        QStringList names() const;
        QString name() const;
        bool containsName(const QString&) const;
        QStringList protocols() const;
        //Port& operator=(const Port& other);
        bool hasProtocol(const QString&) const;
        //bool operator==(WhitelistItem& item);
        bool operator==(const Port& );
};

#endif
