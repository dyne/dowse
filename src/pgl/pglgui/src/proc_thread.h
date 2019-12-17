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


#ifndef PROC_THREAD_H
#define PROC_THREAD_H

#include <QObject>
#include <QThread>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QtDebug>
#include <QList>
#include <QTimer>

#include "command.h"


/**
*
*
* @short Class which can execute a command as a seperate thread.
*
*/

class ProcessT : public QThread
{

        Q_OBJECT

        public:
                /**
                 * Constructor. Creats a new ProcessT object.
                 * @param parent The QObject parent of this object.
                 */
                ProcessT( QObject *parent = 0 );
        ProcessT(ProcessT const& other);
                /**
                 * Destructor.
                 */
                virtual ~ProcessT();
                /**
                 * Reimplementation of QThread::run().
                 * Executes the command which was set using setCommand().
                 * If no command was set, this function does nothing.
                 */
                void run();
                /**
                 * Set the given command to be executed when run() is called.
                 * @param name The name of the program.
                 * @param args The command line arguments.
                 * @param mode The process channel modes of the command which will be executed.
                 */
                void setCommand( const QString &name, const QStringList &args, const QProcess::ProcessChannelMode &mode = QProcess::SeparateChannels );


        void operator=(const ProcessT& p){ *this = p;}


        void executeCommands(const QStringList& commands , const QProcess::ProcessChannelMode &mode=QProcess::SeparateChannels, bool startNow=true);
        void execute(const QStringList& command, const QProcess::ProcessChannelMode &mode );

        //for backwards compatibility with the old Mobloquer code
        void execute( const QString &name, const QStringList &args, const QProcess::ProcessChannelMode &mode = QProcess::SeparateChannels );
        bool allFinished() { return mCommandsToExecute.isEmpty() && (! this->isRunning()); }

        signals:
                /**
                 * Emitted when a command has finished running.
                 * @param output The output of the command which was executed.
                 */
                void commandOutput(const QString&);
        void finished(const CommandList&);
        void newCommand();

        private:
        QHash<QString, QString> commandToOutput;
        QList<Command> mCommands;
                QString m_Command;
        QStringList mCommandsToExecute;
        QStringList m_ExecutedCommands;
        QStringList m_Args;
        QProcess::ProcessChannelMode m_ChanMode;
        QString m_Output;

    public slots:
        void executeCommand(const QString& command="", const QProcess::ProcessChannelMode &mode = QProcess::SeparateChannels, bool startNow = true);

};

#endif //PROC_THREAD_H
