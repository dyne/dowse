/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
 *   Copyright (C) 2007 by Dimitris Palyvos-Giannas <jimaras@gmail.com>       *
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


#include "proc_thread.h"

#include "utils.h"


ProcessT::ProcessT( QObject *parent ) :
        QThread( parent )
{
    m_ChanMode = QProcess::SeparateChannels;
    connect(this, SIGNAL(newCommand()), this, SLOT(executeCommand()));
}

ProcessT::ProcessT(const ProcessT& other):
    QThread(other.parent())
{
    *this = other;
}

ProcessT::~ProcessT()
{
        wait();
        //qDebug() << Q_FUNC_INFO << " destroyed";
}

void ProcessT::run()
{
        if ( m_Command.isEmpty() ) {
                qWarning() << Q_FUNC_INFO << "No command set, doing nothing";
                return;
        }

        QProcess proc;

        proc.setProcessChannelMode( m_ChanMode );
    proc.start( m_Command );
        proc.waitForStarted();
        proc.waitForFinished();
        proc.closeWriteChannel();

        m_Output = proc.readAll().trimmed();

    Command cmd(m_Command, m_Output);

    if ( proc.exitCode() != 0 )
        cmd.setError(true);

    mCommands.append(cmd);
    m_ExecutedCommands << m_Command;

    if ( mCommandsToExecute.isEmpty() ) {
        emit finished(mCommands);
    }
    else {
        m_Command = mCommandsToExecute.takeFirst();
        emit newCommand();
    }

}

void ProcessT::setCommand( const QString &name, const QStringList &args, const QProcess::ProcessChannelMode &mode )
{

    //***** This function is for backwards compatibility with the old Mobloquer code. *****//
    //***** It should be removed at some point.*****//

        if ( name.isEmpty() ) {
            qWarning() << Q_FUNC_INFO << "Empty command given, doing nothing";
            return;
        }

        m_Command = name;
        m_Args = args;
        m_ChanMode = mode;

}

void ProcessT::executeCommand(const QString& command, const QProcess::ProcessChannelMode &mode, bool startNow)
{
    if ( command.isEmpty() && m_Command.isEmpty() )
        return;

    m_ChanMode = mode;
    if (! command.isEmpty())
        m_Command = command;
    mCommands.clear();

    if (! isRunning() && startNow )
        start();
}


void ProcessT::execute( const QString &name, const QStringList &args, const QProcess::ProcessChannelMode &mode )
{

    //***** This function is for backwards compatibility with the old Mobloquer code. *****//
    //***** It should be removed at some point.*****//

        //setCommand( name, args, mode );
    QString cmd = QString("%1 %2").arg(name).arg(args.join(" "));

    executeCommand(cmd, mode);

}


void ProcessT::execute(const QStringList& command, const QProcess::ProcessChannelMode &mode )
{
    //***** This function is for backwards compatibility with the old Mobloquer code. *****//
    //***** It should be removed at some point.*****//

    //QString name = command.first();
    //QStringList args = command.mid(1);

        //setCommand( name, args, mode );
    mCommandsToExecute.clear();
    executeCommand(command.join(" "), mode);

}



void ProcessT::executeCommands(const QStringList& commands , const QProcess::ProcessChannelMode &mode, bool startNow)
{

    if ( commands.isEmpty() )
        return;

    mCommandsToExecute = commands;

    executeCommand(mCommandsToExecute.takeFirst(), mode, startNow);

}


