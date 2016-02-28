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


#ifndef SUPER_USER_H
#define SUPER_USER_H

#include <QObject>
#include <QProcess>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QDebug>
#include <QSettings>
#include <QTimer>

#include "proc_thread.h"

#define PREFIX1 "/usr/bin/"
#define PREFIX2 "/usr/local/bin/"
#define PREFIX3 "$HOME/.local/bin/"
#define KDESU "kdesu"
#define KDESUDO "kdesudo"
#define GKSU "gksu"
#define GKSUDO "gksudo"
#define TMP_SCRIPT "/tmp/execute-all-pgl-commands.sh"

/**
*
* @short A simple class to execute commands with sudo rights.
*
*/


class SuperUser : public QObject
{

    Q_OBJECT

    public:

        /**
        * Constructor.
        */
        SuperUser(QObject *parent = 0, const QString& rootpath ="");
        /**
            * Destructor.
            */
        virtual ~SuperUser();
        /**
            * Set the path to the sudo front end the class is going to use.
            * @param path The new path.
            */
        void setFilePath( const QString &path );
        /**
            * Execute a command with sudo rights.
            * @param command QStringList containing the command to be executed.
            * @param detached Execute the command either detached or start it normally and wait for it to be finished. This is ingored if pglgui was started with sudo rights.
            */
        void executeCommands( QStringList commands);
        /**
            * Move a file from one place of the filesystem to another, using the mv command.
            * @param source The current path of the file.
            * @param dest The file's new location.
            */
        void moveFile( const QString &source, const QString &dest, bool now=false);
        /**
            * Copy a file from one place of the filesystem to another, using the cp command.
            * @param source The current path of the file.
            * @param dest The file's new location.
            */
        void copyFile( const QString &source, const QString &dest, bool now=false);
        /**
            * Remove a file using the mv command.
            * @param source The current path of the file.
            */
        void removeFile( const QString &source, bool now=false );

        QString getRootPath();
        static QString sudoCommand();
        static void setSudoCommand(const QString&);
        void startThread(const QString &name, const QStringList &args, const QProcess::ProcessChannelMode &mode );
        void moveFiles( const QMap<QString, QString>, bool start = true);
        void executeScript();
        void operator=(const SuperUser& su);
        void executeCommand(const QString&, const QStringList& args=QStringList());
        void executeCommand(const QStringList&);
        void addCommand(const QString&, const QStringList& args=QStringList());
        void addCommand(const QStringList&);
        void addCommands(const QStringList&);
        void executeAll();

    public slots:
        void processFinished(const CommandList&);

    signals:
        void finished(const CommandList&);
        void error(const CommandList&);
        void error(const QString&);

protected:
        void exec(QString cmd);

    private:
        SuperUser(const SuperUser& other);
        void findGraphicalSudo();
        QString sudoParameters();
        QMap<QString, QString> m_filesToMove;
        ProcessT * m_ProcT;
        QStringList m_Commands;
        bool emitFinished;
        bool mGetSudoCommand;

};

#endif
