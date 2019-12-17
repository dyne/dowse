/******************************************************************************
 *   Copyright (C) 2011-2012 by Carlos Pais <fr33mind@users.sourceforge.net>  *
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


#ifndef pglcmd_H
#define pglcmd_H

#include <QObject>
#include <QProcess>
#include <QtDebug>
#include <QFile>
#include <QString>
#include <QStringList>

#include "super_user.h"

#define PGLCMD_PATH "/usr/bin/pglcmd"

#define MESSAGE_TIMEOUT 20000

/**
*
* @short Simple class using SuperUser to handle the pgl daemon
*
*/


class PglCmd : public SuperUser {

        Q_OBJECT


        public:
            /**
                * Default constructor, creates a pglcmd object using the script specified in path.
                * @param path The path of the pglcmd script.
                * @param parent The parent of this QObject.
                */
            explicit PglCmd(  QObject *parent = 0, const QString &path="", const QString& gSudoPath="");
            /**
                * Constructor, creates a pglcmd object without setting any path for the pglcmd script.
                * @param parent The parent of this QObject.
                */
            explicit PglCmd( QObject *parent = 0 );
            /**
                * Destructor
                */
            virtual ~PglCmd() { };
            /**
                * Set the path to the pglcmd script.
                * If the path specified is empty, then the default path, PGLCMD_PATH is used instead.
                * @param path The path to the pglcmd script file.
                */
            void setFilePath( const QString &path, bool verified=false );

            QString getPath();
            static QString getFilePath();
            static QString getFilePath(const QString &path);

        public slots:
                /**
                 * Start the pgld daemon using pglcmd
                 */
                void start();
                /**
                 * Restart the pgld daemon using pglcmd
                 */
                void restart();
                /**
                 * Stop the pgld daemon using pglcmd
                 */
                void stop();
                /**
                 * Reload pgld using pglcmd
                 */
                void reload();
                /**
                 * Update pgld using pglcmd
                 */
                void update();
                /**
                 * Get the pglcmd status.
                 */
                void status();


        signals:
                /**
                 * Message describing the currently running action
                 * @param  message The message describing the action
                 * @param timeout The time the message will be displayed
                 */
                void actionMessage( const QString &message, const int &timeout );

        private:
                QString m_FileName;

};

#endif
