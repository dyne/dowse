/******************************************************************************
 *   Copyright (C) 2012 by Carlos Pais <fr33mind@users.sourceforge.net>       *
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


#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QMetaType>
#include <QList>

class Command
{
    QString mCommand;
    QString mOutput;
    bool mError;

    public:
        explicit Command(const QString& command="", const QString& output="", bool error = false);
        Command(const Command&);
        virtual ~Command();
        QString command() const;
        QString output() const;
        bool error() const;
        void setCommand(const QString&);
        void setOutput(const QString&);
        void setError(bool);
        bool contains(const QString&);

};

typedef QList<Command> CommandList;

Q_DECLARE_METATYPE(Command)
Q_DECLARE_METATYPE(CommandList)

#endif
