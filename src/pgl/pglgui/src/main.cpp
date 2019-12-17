/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
 *   Copyright (C) 2007-2009 by Dimitris Palyvos-Giannas <jimaras@gmail.com>  *
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


#include <QApplication>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QMetaType>
#include <QList>

#include <cstdio>
#include <cstdlib>

#include "pglgui.h"
#include "command.h"

void customOutput( QtMsgType type, const char *msg );

int main(int argc, char *argv[])
{

        qInstallMsgHandler( customOutput );

        //Start the real application
        QApplication app(argc, argv);
        //Set the application information here so QSettings objects can be easily used later.
        QApplication::setOrganizationName( "pgl" );
        QApplication::setOrganizationDomain( "https://sourceforge.net/projects/peerguardian" );
        QApplication::setApplicationName( "pglgui" );
    app.setQuitOnLastWindowClosed(false);

        int i = qRegisterMetaType<Command> ("Command");
        i = qRegisterMetaType<CommandList> ("CommandList");
    PglGui pgWindow;
        //new Adaptor(&pgWindow);

        QStringList args = QApplication::arguments();
        //If tray argument was not given show the window normally
        //Otherwise show minimized in tray
        pgWindow.setVisible( !args.contains( "--tray" ) );

        return app.exec();

}

void customOutput( QtMsgType type, const char *msg ) {

        switch( type ) {
                case QtDebugMsg:
                        fprintf( stderr, "** Debug: %s\n", msg );
                        break;
                case QtWarningMsg:
                        fprintf( stderr, "** Warning: %s\n", msg );
                        break;
                case QtCriticalMsg:
                        fprintf( stderr, "** Critical: %s\n", msg );
                        break;
                case QtFatalMsg:
                        fprintf( stderr, "** Fatal: %s\n", msg );
                        break;
        }

}
