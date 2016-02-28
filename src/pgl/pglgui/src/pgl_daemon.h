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


#ifndef PGL_DAEMON_H
#define PGL_DAEMON_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QTimer>
#include <QProcess>
#include <QtDebug>
#include <QTimer>

#include "file_transactions.h"

#define DAEMON "pgld"
#define EMIT_SIGNAL_DELAY 20000
#define TIMER_INTERVAL 2000

/**
*
* @short Simple class which loads data from the pglcmd log and pgld's main log to display information about pgld. This class is also used to check the state of the pgld daemon.
*
*/

class PglDaemon : public QObject {

        Q_OBJECT

        public:
                /**
         * Constructor. Creates a PglDaemon object and loads data from the pgld log in the path specified
                 * @param filename The path to the pglcmd log
                 * @param parent The QObject parent.
                 */
        PglDaemon( const QString & logPath, QObject *parent = 0 );
                /**
                 * Destructor.
                 */
        ~PglDaemon() { }
                /**
         * @return The number of the ranges pgld has loaded.
                 */
                inline QString loadedRanges() const { return m_LoadedRanges; }
                /**
         * @return The number of the ranges pgld has skipped.
                 */
                inline QString skippedRanges() const { return m_SkippedRanges; }
                /**
         * @return The number of the ranges pgld has merged.
                 */
                inline QString mergedRanges() const { return m_MergedRanges; }
                /**
                 * Get the date/time of the last update of the blocklists.
                 * @return
                 */
                inline QString lastUpdateTime() const { return m_LastUpdateTime; }
                /**
         * The state of the pgld daemon.
         * @return True if pgld is running, otherwise false.
                 */
                inline bool daemonState() const { return m_DaemonState; }
                /**
         * The process PID of the pgld daemon.
         * @return The process PID, or a null string if pgld is not running.
                 */
        inline QString processPID() const { return m_ProcessID; }
                /**
                 * The pglcmd log.
                 * @return The contents of the pglcmd log with slightly changed format.
                 */
                inline QVector< QString > controlLog() const { return m_LastUpdateLog; }

        void getLoadedIps();
        void setLoadedIps(const QString& loadedIps) { m_LoadedRanges = loadedIps; }

        public slots:
                /**
                 * Update the daemon state.
                 */
                void updateDaemonState();

        signals:
                /**
         * Emited when the state of the pgld daemon changes.
         * @param state True if pgld is running, otherwise false.
                 */
        void stateChanged( const bool &state );
                /**
         * Emited when the PID of the pgld daemon changes.
         * @param PID The process PID of the pgld daemon.
                 */
        void processIDChanged( const QString &PID );
                /**
         * Emited when pgld starts running.
                 */
                void pgldStarted();
                /**
         * Emited when pgld is terminated.
                 */
                void pgldStopped();
                /**
                 * Emited when there is a change in the pglcmd log.
                 */
                void logChanged();



        private:
        //Check if pgld is running
        QString checkProcessID();
        void processDate( QString &date );
                QString m_LoadedRanges;
                QString m_SkippedRanges;
                QString m_MergedRanges;
                QString m_LastUpdateTime;
        QString m_ProcessID;
        QString m_LogPath;
                bool m_DaemonState;
                bool m_infoLoaded;
        QTimer* mTimer;
                QVector< QString > m_FileNames;
                QVector< QString > m_LastUpdateLog;
                QVector< QString > m_PreviousUpdateLog;
                QVector< QString > m_ControlLogNewL;
};

#endif

