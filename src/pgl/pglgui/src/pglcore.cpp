/******************************************************************************
 *   Copyright (C) 2013 by Carlos Pais <fr33mind@users.sourceforge.net>       *
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


#include "pglcore.h"

#include <QDebug>

PglCore::PglCore(QSettings* settings, QObject* parent):
    QObject(parent)
{
    //whitelisted Ips and ports - /etc/pgl/pglcmd.conf and /etc/pgl/allow.p2p and
    //$HOME/.config/pgl/pglgui.conf for disabled items
    mWhitelistManager = new WhitelistManager(settings);
    mBlocklistManager = new BlocklistManager();
}

PglCore::~PglCore()
{
    if (mWhitelistManager)
        delete mWhitelistManager;

    if (mBlocklistManager)
        delete mBlocklistManager;

    for(int i=0; i < mOptions.size(); i++)
        delete mOptions[i];
    mOptions.clear();
}

void PglCore::load()
{
    bool startAtBoot = false;
    bool updateAutomatically = false;
    bool updateDaily = false;
    bool updateWeekly = false;
    bool updateMonthly = false;

    PglSettings::loadSettings();

    if ( PglSettings::value("INIT") == "1" )
        startAtBoot = true;

    if ( PglSettings::value("CRON") == "0" )
        updateAutomatically = false;
    else
    {
        QString frequency = getUpdateFrequencyCurrentPath();

        if ( ! frequency.isEmpty() )
        {
            updateAutomatically = true;
            if (frequency.contains("daily/", Qt::CaseInsensitive))
                updateDaily = true;
            else if ( frequency.contains("weekly/", Qt::CaseInsensitive))
                updateWeekly = true;
            else if ( frequency.contains("monthly/", Qt::CaseInsensitive))
                updateMonthly = true;
        }
    }

    for(int i=0; i < mOptions.size(); i++)
        if (mOptions[i])
            delete mOptions[i];
    mOptions.clear();

    setOption("startAtBoot", startAtBoot, true);
    setOption("updateAutomatically", updateAutomatically, true);
    setOption("updateDailyRadio", updateDaily, true);
    setOption("updateWeeklyRadio", updateWeekly, true);
    setOption("updateMonthlyRadio", updateMonthly, true);

    mBlocklistManager->loadBlocklists();
    mWhitelistManager->load();
}

Option* PglCore::option(const QString& name)
{
    foreach(Option * option, mOptions) {
        if (option->name() == name)
            return option;
    }

    return 0;
}

void PglCore::setOption(const QString& name, const QVariant& value, bool active)
{
    Option* option = PglCore::option(name);
    if (option)
        option->setValue(value);
    else
        mOptions.append(new Option(name, value, active));
}

void PglCore::addOption(const QString& name, const QVariant& value, bool active)
{
    setOption(name, value, active);
}

QString PglCore::getUpdateFrequencyCurrentPath()
{
    QString path("/etc/cron.");
    QString script ("pglcmd");
    QStringList times;
    times << "daily/" << "weekly/" << "monthly/";;

    foreach(QString time, times)
        if ( QFile::exists(path + time + script ) )
            return path + time + script;

    return QString("");
}

bool PglCore::isChanged()
{
    foreach(Option* option, mOptions) {
        if (option->isChanged())
            return true;
    }

    foreach(Blocklist* blocklist, mBlocklistManager->blocklists()) {
        if (blocklist->isChanged())
            return true;
    }

    foreach(WhitelistItem* item, mWhitelistManager->whitelistItems()) {
        if (item->isChanged())
           return true;
    }

    return false;
}


BlocklistManager* PglCore::blocklistManager()
{
    return mBlocklistManager;
}

WhitelistManager* PglCore::whitelistManager()
{
    return mWhitelistManager;
}

bool PglCore::hasToUpdatePglcmdConf()
{
    Option* startAtBoot = option("startAtBoot");
    Option* updateAutomatically = option("updateAutomatically");

    if (startAtBoot && startAtBoot->isChanged())
        return true;

    if (updateAutomatically && updateAutomatically->isChanged())
        return true;

    return mWhitelistManager->isChanged();
}

bool PglCore::hasToUpdateBlocklistsFile()
{
    foreach(Blocklist* blocklist, mBlocklistManager->blocklists()) {
        if (!blocklist->isLocal() && blocklist->isChanged())
            return true;
    }

    return false;
}

bool PglCore::hasToReloadBlocklists()
{
    foreach(Blocklist* blocklist, mBlocklistManager->blocklists())
        if (blocklist->isChanged())
            return true;

    return false;
}

QStringList PglCore::generatePglcmdConf()
{
    if (mWhitelistManager->isChanged())
        mWhitelistManager->updatePglSettings();

    Option* startAtBootOption = option("startAtBoot");
    Option* updateAutomaticallyOption = option("updateAutomatically");
    if (startAtBootOption && startAtBootOption->isChanged()) {
        PglSettings::store("INIT", startAtBootOption->value().toInt());
    }

    if (updateAutomaticallyOption && updateAutomaticallyOption->isChanged()) {
        PglSettings::store("CRON", updateAutomaticallyOption->value().toInt());
    }

    return PglSettings::generatePglcmdConf();
}

void PglCore::undo()
{
    foreach(Option* option, mOptions)
        if (option->isChanged())
            option->undo();

    foreach(Blocklist* blocklist, mBlocklistManager->blocklists()) {
        if (blocklist->isChanged())
            blocklist->undo();
    }

    mWhitelistManager->undo();
}
