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


#ifndef PGL_SETTINGS_H
#define PGL_SETTINGS_H

#include <QString>
#include <QHash>
#include <QStringList>
#include <QVariant>

#include "super_user.h"

#define PGLCMD_DEFAULTS_PATH PGLCMDDEFAULTSPATH

class PglSettings
{
    public:
        PglSettings();
        ~PglSettings();
        static bool loadSettings();
        static QHash<QString, QString> confVariables();
        static QString getValueInLine(const QString&);
        static QString getVariableInValue(const QString &);
        static QString findPglcmdDefaultsPath();
        static QString pglcmdDefaultsPath();
        static QString lastError();
        static QStringList pglcmdConfData();
        static void store(const QString&, const QVariant&);
        static void add(const QString&, const QVariant&);
        static void remove(const QString&, const QVariant&);
        static QString value(const QString&, const QString&def="");
        static QStringList values(const QString&);
        static bool contains(const QString&, const QString&val="");
        static QStringList generatePglcmdConf();

};


#endif

