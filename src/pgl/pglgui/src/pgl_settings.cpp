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


#include "pgl_settings.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QObject>
#include <QStringList>

#include "file_transactions.h"
#include "utils.h"

QHash<QString, QString> mDefaultVariables;
QHash<QString, QString> mConfVariables;
QString mPglcmdDefaultsPath = "";
QString mLastError = "";
QStringList mPglcmdConfData;

QString PglSettings::getVariableInValue(const QString & var)
{
    QString variable(var);

    if ( var.contains("$") ) {
        QString strippedVar(var);
        int n = 0;

        if ( var.contains("{") && var.contains("}") ) {
            int pos1 = var.indexOf('{') + 1;
            n = var.indexOf('}') - pos1;
            strippedVar = var.mid(pos1, n);
            n = 3;
        }
        else {
            int pos1 = var.indexOf('$') + 1;
            strippedVar = var.mid(pos1);
            n = 1;
        }

        if ( PglSettings::contains(strippedVar) )
            return variable.replace(var.indexOf("$"), strippedVar.size()+n, PglSettings::value(strippedVar, ""));
    }

    return var;
}

QString PglSettings::getValueInLine(const QString& line)
{

    QString value = getValue(line);

    if ( ! value.contains("$") )
        return value;

    QString newValue("");

    if ( value.contains("/") )
    {

        foreach(QString val, value.split("/", QString::SkipEmptyParts))
        {
            newValue += getVariableInValue(val);
            newValue += "/";
        }
    }
    else
        newValue += getVariableInValue(value);

    if ( newValue.endsWith("/") )
        newValue.remove(newValue.size()-1, 1);

    return newValue;
}

// PGLCMD_DEFAULTS_PATH is now set automatically by the Makefile/configure.ac
// TODO: just test if it exists, don't try automatic detection. jre, 2012-06-15
QString PglSettings::findPglcmdDefaultsPath()
{
    if (QFile::exists(PGLCMD_DEFAULTS_PATH))
        return PGLCMD_DEFAULTS_PATH;

    //deprecated code below, should be removed at some point
    QDir currentDir = QDir::current();
    currentDir.cdUp();

    if (! currentDir.cd("lib") )
        return "";

    if (! currentDir.exists("pgl"))
        return "";

    currentDir.cd("pgl");
    return currentDir.absolutePath();
}

bool PglSettings::loadSettings()
{
    mPglcmdDefaultsPath = findPglcmdDefaultsPath();
    // TODO: Readd pglcmd.defaults path setting in preferences, jre, 2012-06-15
    if (mPglcmdDefaultsPath.isEmpty()) {
        mLastError = QObject::tr("Couldn't find pglcmd's defaults path.");
        return false;
    }

    QStringList data = getFileData(mPglcmdDefaultsPath);
    QString variable, value;

    foreach (QString line, data)
    {
        line = line.trimmed();
        if ( line.startsWith('#') ) //ignore comments
            continue;

        variable = getVariable(line);

        if( ! variable.isEmpty() )
            mDefaultVariables[variable] = PglSettings::getValueInLine(line);
    }

    //Overwrite the variables' values with the values from pglcmd.conf
    QString pglcmdConfPath(PglSettings::value("CMD_CONF"));

    if ( pglcmdConfPath.isEmpty() ) {
        mLastError = QObject::tr("Couldn't find plgcmd's configuration path. Did you install pgld and pglcmd?");
        return false;
    }

    mPglcmdConfData = getFileData(pglcmdConfPath);
    foreach(QString line, mPglcmdConfData)
    {
        line = line.trimmed();

        if ( line.startsWith('#') ) //ignore comments
            continue;

        variable = getVariable(line);
        value = getValue(line);

        if ( ! mDefaultVariables.contains(variable)  || mDefaultVariables.value(variable) != value)
            mConfVariables[variable] = value;
    }

    mLastError = "";
    return true;
}

QHash<QString, QString> PglSettings::confVariables()
{
    return mConfVariables;
}

QString PglSettings::pglcmdDefaultsPath()
{
    return mPglcmdDefaultsPath;
}

QString PglSettings::lastError()
{
    return mLastError;
}

void PglSettings::store(const QString & var, const QVariant& value)
{
    mConfVariables[var] = value.toString().trimmed();
}

void PglSettings::add(const QString& var, const QVariant& value)
{
    QString val = value.toString().trimmed();
    qDebug() << "Adding to PglSettings" << var << val;
    QStringList values = PglSettings::values(var);
    if (! values.contains(val))
        values += val;
    mConfVariables[var] = values.join(" ");
}

void PglSettings::remove(const QString& var, const QVariant& value)
{
    QString val = value.toString().trimmed();
    qDebug() << "Removing from PglSettings" << var << val;
    if (mConfVariables.contains(var)) {
        QStringList values = PglSettings::values(var);
        if (values.contains(val))
            values.removeAll(val);
        mConfVariables[var] = values.join(" ");
    }
}

QString PglSettings::value(const QString& var, const QString& def)
{
    if (mConfVariables.contains(var))
        return mConfVariables.value(var);
    else if (mDefaultVariables.contains(var))
        return mDefaultVariables.value(var);
    return def;
}

QStringList PglSettings::values(const QString& var)
{
    QString value = PglSettings::value(var);
    return value.split(" ", QString::SkipEmptyParts);
}

bool PglSettings::contains(const QString& var, const QString& value)
{
    QString val = PglSettings::value(var, "");
    if (! value.isEmpty())
        return value == val;

    return ! val.isEmpty();
}

QStringList PglSettings::pglcmdConfData()
{
    return mPglcmdConfData;
}

QStringList PglSettings::generatePglcmdConf()
{
    QStringList data = mPglcmdConfData;

    foreach(const QString& var, mConfVariables.keys()) {
        setValueInData(data, var, mConfVariables[var]);
    }

    return data;
}
