/******************************************************************************
 *   Copyright (C) 2013-2014 by Carlos Pais <fr33mind@users.sourceforge.net>  *
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


#include "port.h"

#include <QHash>

static QHash<QString, int> mPortsPair;

// **** PORT CLASS **** //
Port::Port(const QString& name, const QString& prot, int n)
{
    mNames << name;

    //the port number can be consider an alias too
    if ( n > 0 )
        mNames << QString::number(n);

    mProtocols << prot;
    mNumber = n;
}

Port::Port()
{
    mNumber = 0;
}

Port::Port(const Port& other)
{
    *this = other;
}

int Port::number() const
{
  return mNumber;
}

QStringList Port::protocols() const
{
  return mProtocols;
}

QStringList Port::names() const
{
  return mNames;
}

QString Port::name() const
{
    if ( ! mNames.isEmpty() )
        return mNames[0];

    return QString("");
}

bool Port::hasProtocol(const QString& protocol) const
{
  return mProtocols.contains(protocol, Qt::CaseInsensitive);
}

void Port::addProtocols(const QStringList& protocols)
{
    foreach(const QString &prot, protocols)
        if (! mProtocols.contains(prot))
            mProtocols.append(prot);
}

void Port::addName(const QString& name)
{
    if ( ! mNames.contains(name) )
        mNames << name;
}

void Port::addNames(const QStringList& names)
{
    foreach(const QString &name, names)
        if (! mNames.contains(name))
            mNames.append(name);
}

/*Port& Port::operator=(const Port& other)
{
    m_number = other.number();
    m_protocols = other.protocols();
    m_values = other.values();

    return *this;
}*/

/*bool Port::operator==(WhitelistItem& item)
{
    foreach(const QString& name, mNames)
        if (item.value() == name)
            return true;

    return false;
}*/

bool Port::containsName(const QString& name) const
{
  foreach(const QString& _name, mNames)
    if (name == _name)
      return true;

  return false;
}
