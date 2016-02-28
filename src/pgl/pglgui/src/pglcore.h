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


#ifndef PGLCORE_H
#define PGLCORE_H

#include <QObject>
#include <QSettings>
#include <QList>
#include <QString>

#include "settings.h"
#include "add_exception_dialog.h"
#include "whitelist_manager.h"
#include "blocklist_manager.h"
#include "pgl_settings.h"
#include "option.h"


/**
*
* @short Class representing the core of the program. Contains an internal representation of the options in GUI.
*
*/

class PglCore : QObject {

    Q_OBJECT

    QList<Option*> mOptions;
    WhitelistManager* mWhitelistManager;
    BlocklistManager* mBlocklistManager;
    QString mBlocklistsFilePath;
    QString mLocalBlocklistsDir;

        public:
        PglCore(QSettings* settings, QObject* parent=0);
        virtual ~PglCore();
        Option* option(const QString&);
        void setOption(const QString&, const QVariant&, bool active=false);
        void addOption(const QString&, const QVariant&, bool active=false);
        QString getUpdateFrequencyCurrentPath();
        bool isChanged();
        BlocklistManager* blocklistManager();
        WhitelistManager* whitelistManager();
        bool hasToUpdatePglcmdConf();
        bool hasToUpdateBlocklistsFile();
        bool hasToReloadBlocklists();
        QStringList generatePglcmdConf();
        void undo();
        void load();
};

#endif //PGL_CORE_H

