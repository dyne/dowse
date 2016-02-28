/******************************************************************************
 *   Copyright (C) 2011-2014 by Carlos Pais <fr33mind@users.sourceforge.net>  *
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


#ifndef WHITELIST_MANAGER_H
#define WHITELIST_MANAGER_H

#include <QMultiMap>
#include <QMap>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QSettings>
#include <QList>
#include <QObject>

#include "whitelist_item.h"
#include "utils.h"
#include "port.h"

#define WHITE_IP_IN "WHITE_IP_IN"
#define WHITE_IP_OUT "WHITE_IP_OUT"
#define WHITE_IP_FWD "WHITE_IP_FWD"
#define WHITE_TCP_IN "WHITE_TCP_IN"
#define WHITE_UDP_IN "WHITE_UDP_IN"
#define WHITE_TCP_OUT "WHITE_TCP_OUT"
#define WHITE_UDP_OUT "WHITE_UDP_OUT"
#define WHITE_TCP_FWD "WHITE_TCP_FWD"
#define WHITE_UDP_FWD "WHITE_UDP_FWD"

class WhitelistManager : public QObject
{
    Q_OBJECT

    QList<WhitelistItem*> mWhitelistItems;
    QList<WhitelistItem*> mRemovedWhitelistItems;
    QString m_WhitelistFile;
    QStringList mWhitelistFileData;
    QMap<QString, int> m_Group;
    QSettings * m_Settings;
    QMap<QString, QStringList> m_WhitelistEnabled;
    QMap<QString, QStringList> m_WhitelistDisabled;
    QHash<int, Port> mSystemPorts;

        public:
                /**
         * Constructor. Creates an emtpy WhitelistManager object with no data loaded.
                 */
        WhitelistManager(QSettings *, QObject* parent=0);
                /**
                 * Destructor.
                 */
        ~WhitelistManager();

        QList<WhitelistItem*> whitelistItems();
        void loadDisabledItems(QSettings*);
        //QStringList updatePglcmdConf(QList<QTreeWidgetItem*>);
        QMap<QString, QStringList> getEnabledWhitelistedItems() { return m_WhitelistEnabled; }
        QMap<QString, QStringList> getDisabledWhitelistedItems(){ return m_WhitelistDisabled; }
        QString getTypeAsString(const QString&);
        QString getGroup(QStringList&);
        QStringList updateWhitelistFile();
        QString translateConnection(const QString&);
        QStringList getDirections(const QString& chain);
        QStringList getCommands(QStringList items, QStringList connections, QStringList protocols, QList<bool> allows);
        void load();
        QStringList generateIptablesCommands();
        bool isPortAdded(const QString& value, const QString & portRange);
        bool contains(const QString&, const QString&, const QString&);
        bool contains(const WhitelistItem&);
        bool isValid(const QString&, const QString&, const QString&, QString&);
        bool isValid(const WhitelistItem&, QString&);
        WhitelistItem* item(const WhitelistItem&);
        QString getIptablesTestCommand(const QString& value, const QString& connectType, const QString& prot);
        QString parseProtocol(const QString&);
        QString parseConnectionType(const QString&);
        WhitelistItem* itemAt(int);
        void addItem(WhitelistItem*);
        void addItem(const QString&, const QString&, const QString&, bool active=false, bool enabled=true);
        void removeItemAt(int);
        void removeItem(WhitelistItem*);
        void updatePglSettings();
        void updateGuiSettings();
        bool isChanged();
        void undo();
        void loadSystemPorts();
        QHash<int, Port> systemPorts();
        QHash<QString, int> systemPortsNameToNumber();
        Port parsePort(QString);
        int portNumber(const QString&);
        bool isPort(const QString&);

signals:
        void itemAdded(WhitelistItem*);
};

#endif
