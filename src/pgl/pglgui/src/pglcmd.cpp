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


#include "pglcmd.h"
#include "super_user.h"
#include "utils.h"

PglCmd::PglCmd( QObject *parent, const QString &path, const QString& gSudoPath) :
        SuperUser( parent , gSudoPath)
{
    m_FileName = path;
}

QString PglCmd::getPath()
{
    return m_FileName;
}

void PglCmd::setFilePath( const QString &path, bool verified)
{

    if ( verified )
        m_FileName = path;
    else
        m_FileName = getFilePath(path);

    if ( m_FileName.isEmpty() ){
        qCritical() << Q_FUNC_INFO << "File " << m_FileName << " could not be found.";
                qCritical() << Q_FUNC_INFO << "pglcmd will probably not work";
    }
}



void PglCmd::start()
{
    if (  m_FileName.isEmpty() )
    {
        qCritical() << Q_FUNC_INFO << "File " << m_FileName << " could not be found.";
        return;
    }

    SuperUser::executeCommand( QStringList() << m_FileName << "start" );
        emit actionMessage( tr( "Starting Peerguardian..." ), MESSAGE_TIMEOUT );

}

void PglCmd::restart()
{

    if (  m_FileName.isEmpty() )
    {
        qCritical() << Q_FUNC_INFO << "File " << m_FileName << " could not be found.";
        return;
    }

    SuperUser::executeCommand( QStringList() << m_FileName << "restart" );
        emit actionMessage( tr( "Restarting Peerguardian..." ), MESSAGE_TIMEOUT );
}

void PglCmd::stop()
{

    if (  m_FileName.isEmpty() )
    {
        qCritical() << Q_FUNC_INFO << "File " << m_FileName << " could not be found.";
        return;
    }

    SuperUser::executeCommand( QStringList() << m_FileName << "stop" );
        emit actionMessage( tr( "Stopping Peerguardian..." ), MESSAGE_TIMEOUT );

}

void PglCmd::reload()
{

    if (  m_FileName.isEmpty() )
    {
        qCritical() << Q_FUNC_INFO << "File " << m_FileName << " could not be found.";
        return;
    }

    SuperUser::executeCommand( QStringList() << m_FileName << "reload" );
        emit actionMessage( tr( "Reloading Peerguardian..." ), MESSAGE_TIMEOUT );

}

void PglCmd::update()
{

    if (  m_FileName.isEmpty() )
    {
        qCritical() << Q_FUNC_INFO << "File " << m_FileName << " could not be found.";
        return;
    }

    SuperUser::executeCommand( QStringList() << m_FileName << "update" );
        emit actionMessage( tr( "Updating Peerguardian..." ), MESSAGE_TIMEOUT );


}

void PglCmd::status()
{

    if (  m_FileName.isEmpty() )
    {
        qCritical() << Q_FUNC_INFO << "File " << m_FileName << " could not be found.";
        return;
    }

    SuperUser::executeCommand( QStringList() << m_FileName << "status" );
    emit actionMessage( tr( "Getting status for Peerguardian..." ), MESSAGE_TIMEOUT );

}

/*** Static methods ***/

QString PglCmd::getFilePath()
{
    QString path("");
    return getValidPath(path, PGLCMD_PATH);
}

QString PglCmd::getFilePath(const QString &path)
{
    return getValidPath(path, PGLCMD_PATH);
}

