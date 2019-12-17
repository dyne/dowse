/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
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


#include "utils.h"
#include "file_transactions.h"
#include <QFile>
#include <QFileDialog>
#include <QMap>
#include <QVariant>
#include <QDir>
#include <QDebug>

QString getValidPath(const QString &path, const QString &defaultPath )
{
    QString new_path;

    if ( ! path.isEmpty() && QFile::exists(path) )
                new_path = path;
        else if ( QFile::exists(defaultPath) )
                new_path = defaultPath;
        else
        new_path = "";

    return new_path;
}

QStringList selectFiles(QWidget * parent, QString filter, QString title, QString startPath)
{
     QStringList files = QFileDialog::getOpenFileNames(
                         parent,
                         title,
                         startPath,
                         filter);

    return files;
}

QString getValue(const QString& line)
{
    QString value("");

    if ( line.contains("=") )
    {
        value = line.split("=", QString::SkipEmptyParts)[1];

        if ( value == "\"\"" )
            return QString("");

        if ( value.size() > 2 && value.contains('"') )
            return value.split('"', QString::SkipEmptyParts)[0];

    }

    return value;
}


QStringList getValues(const QString& line)
{
    return getValue(line).split(" ", QString::SkipEmptyParts);
}

QString getVariable(const QString& line)
{
    if ( line.contains("=") )
         return line.split("=", QString::SkipEmptyParts)[0];

    return QString("");
}

QString getValue(const QString& path, const QString& search)
{
    QString line = getLineWith(path, search);
    if ( line.contains("=") )
        line = line.split("=", QString::SkipEmptyParts)[1];
    if ( line.contains('"') )
        line = line.split('"', QString::SkipEmptyParts)[0];

    return line;
}

QString getLineWith(const QString& path, const QString& search)
{
    QFile file( path );
    QString line ("");
    QString searchLine("");
        if ( path.isEmpty() ) {
                qWarning() << Q_FUNC_INFO << "Empty file path given, doing nothing";
                return line;
        }
    else if ( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
                qWarning() << Q_FUNC_INFO << "Could not read from file" << path;
                return line;
        }

    QTextStream in( &file );

        while ( ! in.atEnd() )
    {
        line = in.readLine().trimmed();
        if ( line.contains(search) )
        {
            searchLine = line;
            break;
        }
        }

    file.close();
    return searchLine;
}

bool isValidIp( const QString &text ){

        QString ip = text.trimmed();

        if ( ip.isEmpty() ) {
                return false;
        }
        else {
                //Split the string into two sections
                //For example the string 127.0.0.1/24 will be split into two strings:
                //mainIP = "127.0.0.1"
                //range = "24"
                QVector< QString > ipSections = QVector< QString >::fromList( ip.split( "/" ) );
                if ( ipSections.size() < 1 || ipSections.size() > 2 ) {
                        return false;
                }
                QString mainIp = ipSections[0];
                QString range = ( ipSections.size() == 2 ) ? ipSections[1] : QString();
                //Split the IP address
                //E.g. split 127.0.0.1 to "127", "0", "0", "1"
                QVector< QString > ipParts = QVector<QString>::fromList( mainIp.split( "." ) );
                //If size != 4 then it's not an IP
                if ( ipParts.size() != 4 ) {
                        return false;
                }

                for ( int i = 0; i < ipParts.size(); i++ ) {
                        if ( ipParts[i].isEmpty() ) {
                                return false;
                        }
                        //Check that every part of the IP is a positive  integers less or equal to 255
                        if ( QVariant( ipParts[i] ).toInt() > 255 || QVariant( ipParts[i] ).toInt() < 0 ) {
                                return false;
                        }
                        for ( int j = 0; j < ipParts[i].length(); j++ ) {
                                if ( !ipParts[i][j].isNumber() ) {
                                        return false;
                                }
                        }
                }
                //Check if the range is a valid subnet mask
                if ( !isValidIp( range ) ) {
                        //Check that the range is a positive integer less or equal to 24
                        if ( QVariant( range ).toInt() <= 24 && QVariant( range ).toInt() >= 0 ) {
                                for ( int i = 0; i < range.length(); i++ ) {
                                        if ( !range[i].isNumber() ) {
                                                return false;
                                        }
                        }
                }
                        else {
                                return false;
                        }
                }
        }

        return true;

}

QFileInfoList getFilesInfo(const QString & dir, QDir::Filters filters)
{
    QDir directory(dir);

    return directory.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
}

QString getPointer(const QString & dir, const QString & filepathPointed)
{
    foreach(const QFileInfo& fileInfo, getFilesInfo(dir) )
        if ( fileInfo.isSymLink() && fileInfo.symLinkTarget() == filepathPointed)
            return fileInfo.absoluteFilePath();

    return "";
}

bool isPointingTo(QString & dir, QString & filepathPointed)
{
        if ( ! getPointer(dir, filepathPointed).isEmpty() )
            return true;

        return false;
}

QString getNewFileName(QString dir, const QString name)
{
    if ( dir.isEmpty() )
        return dir;

    QDir directory(dir);
    if ( ! directory.exists() )
        return QString();


    int counter = 0;
    QString temp_name = name;

    while(1)
    {
        if ( ! directory.exists(temp_name) )
            return dir + "/" + temp_name;

        counter++;
        temp_name = name + "_" + QString::number(counter);
    }

    return QString();

}

bool hasPermissions(const QString & filepath)
{
    if ( filepath.isEmpty() ) {
                qWarning() << Q_FUNC_INFO << "Empty file path given, doing nothing";
                return false;
        }

    QFileInfo fileInfo (filepath);
    QFile file;
    if ( ! fileInfo.isDir() )
        file.setFileName(fileInfo.absolutePath() + "/test_file");
    else
        file.setFileName(fileInfo.filePath() + "/test_file");

    if ( ! file.open( QIODevice::ReadWrite | QIODevice::Text ) ) {
                return false;
        }

    file.close();

    return true;
}

QString joinPath(const QString & dir, const QString & file)
{
    if ( dir.isEmpty() )
        return file;

    QString directory = dir.trimmed();

    if (directory[directory.size()-1] != '/')
        directory += "/";

    return directory + file;

}

//remove lines with empty values
QStringList cleanData(QStringList& data)
{
    QRegExp emptyValuePattern("^( )*[a-zA-Z0-9_]+( )*=( )*\"\"$");
    QString line;

    for(int i=data.size()-1; i >= 0; --i) {
        line = data[i].trimmed();
        if (line.startsWith("#"))
            continue;

        if (emptyValuePattern.exactMatch(line))
            data.removeAt(i);
    }

    return data;
}

void setValueInData(QStringList& data, const QString & variable, const QString & value)
{
    QString var, comment;
    QString line;
    bool found = false;

    for(int i=0; i < data.size(); i++) {
        line = data[i].trimmed();
        if (line.startsWith("#"))
            continue;

        comment = "";
        if (line.contains("#")) { //if there are any comments
            comment = " #" + line.split("#")[1];
            line = line.split("#")[0];
        }

        if (line.contains(variable)) {
            found = true;
            var = getVariable(line);

            if (var == variable) {
                if (value != getValue(line))
                    data[i] = var + "=\"" + value + '"' + comment;
            }
        }
    }

    if (! found) {
        data.append(variable + "=\"" + value + '"');
    }
}

void replaceValueInFile(const QString& path, const QString & variable, const QString & value)
{

    QFile file( path );
    QStringList newData;
    QString line("");

    if ( path.isEmpty() ) {
                qWarning() << Q_FUNC_INFO << "Empty file path given, doing nothing";
                return;
        }
    else if ( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
                qWarning() << Q_FUNC_INFO << "Could not read from file" << path;
                return;
        }

    QTextStream in( &file );
    QFileInfo fileInfo( path );
    bool found = false;

    while ( ! in.atEnd() )
    {
        line = in.readLine().trimmed();
        if ( line.contains(variable) )
        {
            found = true;
            newData << variable + QString("=\"") + value + QString('"');
        }
        else
            newData << line;
        }

    if ( ! found )
        newData << variable + QString("=\"") + value + QString('"');

    saveFileData(newData, "/tmp/" + fileInfo.fileName());

    file.close();
}

QString getFileName(const QString& path)
{
    QFileInfo fileInfo(path);
    return fileInfo.fileName();
}

bool hasValueInData(const QString& value, const QStringList& data)
{
    foreach(const QString& line, data)
        if ( line.contains(value) )
            return true;

    return false;
}

int confirm(QString title, QString msg, QWidget *parent)
{
       int confirm = QMessageBox::warning(
        parent,
        title,
        msg,
        QMessageBox::Yes, QMessageBox::No
        );

       return confirm;
}

bool isNumber(const QString& str)
{
    for (int i=0; i < str.size(); i++)
      if (! str[i].isDigit())
          return false;

    return true;
}
