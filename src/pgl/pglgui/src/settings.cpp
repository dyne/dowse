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


#include "settings.h"
#include "super_user.h"

SettingsDialog::SettingsDialog(QSettings *settings, QWidget *parent) :
        QDialog( parent )
{
        setupUi( this );


    file_SetRootPath( SuperUser::sudoCommand() );
    int val = settings->value("maximum_log_entries").toInt();
    m_MaxLogEntries->setValue(val);

    connect( m_RootPathButton, SIGNAL( clicked() ), this, SLOT( file_BrowseRootPath() ) );

}


void SettingsDialog::file_BrowseRootPath() {

        QString path = QFileDialog::getOpenFileName( this, tr( "Choose the appropriate file" ), "/usr/bin" );

        if ( path.isNull() )
                return;

        m_RootPathEdit->setText( path );

}


void SettingsDialog::file_SetDefaults() {

        file_SetRootPath( SuperUser::sudoCommand() );
}
