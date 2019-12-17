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


#include "command.h"

#include <QMetaType>

Command::Command(const QString& command, const QString& output, bool error)
{
    mCommand = command;
    mOutput = output;
    mError = error;
}

Command::Command(const Command& other)
{
    *this = other;
}

Command::~Command()
{
}

QString Command::command() const
{
    return mCommand;
}

QString Command::output() const
{
    return mOutput;
}

bool Command::error() const
{
    return mError;
}


void Command::setCommand(const QString& command)
{
    mCommand = command;
}

void Command::setOutput(const QString& output)
{
    mOutput = output;
}

void Command::setError(bool error)
{
    mError = error;
}

bool Command::contains(const QString& str)
{
    return mCommand.contains(str);
}
