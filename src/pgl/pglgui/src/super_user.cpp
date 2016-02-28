/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
 *   Copyright (C) 2007-2008 by Dimitris Palyvos-Giannas <jimaras@gmail.com>  *
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


#include "super_user.h"

#include <QDebug>

#include "utils.h"
#include "file_transactions.h"
#include "pgl_settings.h"

QString mSudoCmd = "";
bool mGetSudoCommand = false;

SuperUser::SuperUser( QObject *parent, const QString& rootpath ):
    QObject(parent)
{
    m_ProcT = new ProcessT(this);
    connect(m_ProcT, SIGNAL(finished(const CommandList&)), this, SLOT(processFinished(const CommandList&)));
    mGetSudoCommand = false;

    if (mSudoCmd.isEmpty() && ! mGetSudoCommand) {
        mSudoCmd = rootpath;
        if ( rootpath.isEmpty() )
            findGraphicalSudo();
    }
}

SuperUser::~SuperUser()
{
    QFile tmpfile(TMP_SCRIPT);

    if ( tmpfile.exists() )
        tmpfile.remove();

    m_ProcT->wait();
}

void SuperUser::findGraphicalSudo()
{
    QStringList gSudos;
    QStringList prefixes;

    gSudos << KDESUDO << KDESU << GKSUDO << GKSU;
    prefixes << PREFIX1 << PREFIX2;

    QDir prefix3 = QDir::home();
    if (prefix3.cd(".local") && prefix3.cd("bin"))
        prefixes << prefix3.absolutePath();

    for(int i=0; i < prefixes.size(); i++)
        for(int j=0; j < gSudos.size(); j++)
            if (QFile::exists(prefixes[i] + gSudos[j])) {
                mSudoCmd = prefixes[i] + gSudos[j];
                return;
            }


    //if the graphical sudo hasn't been found yet, try the 'which' command as last resource.
    for(int i=0; i < gSudos.size(); i++)
        gSudos[i].insert(0, "which ");

    mGetSudoCommand = true;
    m_ProcT->executeCommands(gSudos);
}

QString SuperUser::getRootPath()
{
    return mSudoCmd;
}

void SuperUser::exec(QString cmd)
{
    if ( mSudoCmd.isEmpty() || !QFile::exists(mSudoCmd) ){
        QString errorMsg = tr("Could not use either kdesu(do) or gksu(do) to execute the command requested.\
        You can set the path of the one you prefer in <b>\"Options - Settings - Sudo front-end\"</b>");
        qCritical() << errorMsg;
        emit error(errorMsg);
        return;
    }

    if ( m_ProcT->isRunning() ) {
        qWarning() << "Another process is still running...";
        emit error("Another process is still running...");
        return;
    }

    //If the program is not run by root, use kdesu or gksu as first argument
    if ( ! hasPermissions("/etc") ) {
        cmd = QString("%1 %2").arg(mSudoCmd).arg(cmd);
    }

    qDebug() << "Executing command: \n" << cmd << "\n";

    m_ProcT->executeCommand(cmd);
}

void SuperUser::addCommand(const QString& command, const QStringList& args)
{
    m_Commands << QString("%1 %2").arg(command).arg(args.join(" "));
}

void SuperUser::addCommand(const QStringList& command)
{
    m_Commands << command.join(" ");
}

void SuperUser::addCommands(const QStringList& commands)
{
    m_Commands << commands;
}

void SuperUser::executeCommand(const QString& command, const QStringList& args)
{
    QString cmd = QString("%1 %2").arg(command).arg(args.join(" "));
    exec(cmd);
}

void SuperUser::executeCommand(const QStringList& command)
{
    exec(command.join(" "));
}

void SuperUser::executeAll()
{
    if ( m_Commands.isEmpty() )
        return;

    QString cmd = QString("sh %1").arg(TMP_SCRIPT);
    QStringList lines;
    //create file with the commands to be executed
    lines << "#!/bin/sh";
    lines << "set -e";
    lines << m_Commands;

    bool ok = saveFileData(lines, TMP_SCRIPT);

    if ( ok )
        exec(cmd);
}

void SuperUser::processFinished(const CommandList& commands)
{
    if ( ! m_Commands.isEmpty() )
        m_Commands.clear();

    if (mGetSudoCommand && ! commands.isEmpty()) {
        Command command = commands.first();
        if ( command.contains("which") && mSudoCmd.isEmpty() && QFile::exists(command.output())) {
            mSudoCmd = command.output();
            mGetSudoCommand = false;
        }
    }

    emit finished(commands);
}


void SuperUser::moveFile( const QString &source, const QString &dest, bool now)
{
    QStringList cmd; cmd <<  "mv" << source << dest;
    if (now)
        executeCommand(cmd);
    else
        addCommand(cmd);
}

void SuperUser::copyFile( const QString &source, const QString &dest, bool now)
{
    QStringList cmd; cmd <<  "cp" << source << dest;
    if (now)
        executeCommand(cmd);
    else
        addCommand(cmd);
}

void SuperUser::removeFile( const QString &source, bool now) {
    QStringList cmd; cmd << "rm" << "-f" << source;
    if (now)
        executeCommand(cmd);
    else
        addCommand(cmd);
}

QString SuperUser::sudoParameters()
{
    if (mSudoCmd.endsWith("kdesudo")) {
        return " -c ";
    }
    else if (mSudoCmd.endsWith("gksudo") || mSudoCmd.endsWith("gksu")) {
        return " -- sh -c ";
    }

    return " ";
}

/*** Static methods ***/

QString SuperUser::sudoCommand()
{
    return mSudoCmd;
}

void SuperUser::setSudoCommand(const QString& cmd)
{
    mSudoCmd = cmd;
}

