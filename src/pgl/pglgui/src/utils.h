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


#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfoList>
#include <QMessageBox>
#include <QRegExp>

QString getValidPath(const QString &path, const QString &defaultPath );
QStringList selectFiles(QWidget * parent=0, QString filter = "", QString title="Select one or more Blocklists", QString startPath=QDir::homePath());
QString getValue(const QString&);
QStringList getValues(const QString&);
QString getVariable(const QString&);
QString getValue(const QString&, const QString&);
QString getLineWith(const QString&, const QString&);
bool isValidIp(const QString &text );
QFileInfoList getFilesInfo(const QString &, QDir::Filters filters=QDir::QDir::NoDotAndDotDot|QDir::Files);
bool isPointingTo(QString &, QString &);
QString getPointer(const QString &, const QString &);
bool hasPermissions(const QString&);
QString getNewFileName(QString dir, const QString name);
QString joinPath(const QString& dir, const QString& file);
void replaceValueInFile(const QString& path, const QString & variable, const QString & value);
void setValueInData(QStringList& data, const QString & variable, const QString & value);
QStringList cleanData(QStringList& data);
QString getFileName(const QString& path);
bool hasValueInData(const QString&, const QStringList&);
int confirm(QString title, QString msg, QWidget *parent=NULL);
bool isNumber(const QString&);

#endif
