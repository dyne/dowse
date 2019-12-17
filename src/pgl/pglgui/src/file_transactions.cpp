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


#include "file_transactions.h"

QStringList getFileData( const QString &path ) {

        QStringList fileContents;
        QFile file( path );
        if ( path.isEmpty() ) {
                qWarning() << Q_FUNC_INFO << "Empty file path given, doing nothing";
                return fileContents;
        }
        else if ( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
                qWarning() << Q_FUNC_INFO << "Could not read from file" << path;
                return fileContents;
        }

        QTextStream in( &file );
        while ( ! in.atEnd() ) {
                QString line = in.readLine();
                line = line.trimmed();
                fileContents.push_back(line);
        }

        return fileContents;

}

bool saveFileData( const QStringList &data, const QString &path ) {

        QFile file( path );
        if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
                qWarning() << Q_FUNC_INFO << "Could not write to file" << path;
                return false;
        }
        QTextStream out(&file);
        for ( QStringList::const_iterator s = data.begin(); s != data.end(); s++ ) {
                out << *s << "\n";
        }
        return true;
}

bool compareFileData( const QString &pathA, const QString &pathB ) {

        QStringList fileA = getFileData( pathA );
        QStringList fileB = getFileData( pathB );

        if ( fileA.isEmpty() || fileB.isEmpty() ) {
                return false;
        }

        if ( fileA == fileB ) {
                return true;
        }

        return false;

}


