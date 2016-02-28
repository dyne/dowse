/******************************************************************************
 *   Copyright (C) 2013 by Carlos Pais <fr33mind@users.sourceforge.net>       *
 *   Copyright (C) 2007 by Dimitris Palyvos-Giannas <jimaras@gmail.com>       *
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


#ifndef BLOCKLIST_MANAGER_H
#define BLOCKLIST_MANAGER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QFileInfo>
#include <QHash>
#include <QString>

#include "file_transactions.h"
#include "list_item.h"
#include "blocklist.h"
#include "enums.h"

class Blocklist;
class ListItem;

/**
*
* @short Class containing information about blocklists in use.
*
*/

class BlocklistManager : public QObject {

    Q_OBJECT

        public:
                /**
                 * Constructor. Creates a PeerguardianList object and loads the data from the pglcmd blocklists file.
                 * @param path The path to the pglcmd blocklists file.
                 */
        BlocklistManager( const QString &path = "", QObject* parent=0);
                /**
                 * Destructor.
                 */
        ~BlocklistManager();
                /**
                 * Set the file path to the pglcmd blocklists file.
                 * If the path is invalid and no path is already set, PGL_LIST_PATH is used.
                 * @param path The path to the pglcmd blocklists file.
                 */
                void setFilePath( const QString &path, bool verified=false );
        QString blocklistsFilePath();
                /**
                 * Insert a new blocklist item into the blocklists file.
                 * @param newItem The new ListItem to be added.
                 */
                void addItem( const ListItem &newItem );
                /**
                 * Insert a new blocklist item into the blocklists file.
                 * @param line The raw line from the blocklists file.
                 */
                void addItem( const QString &line );
                /**
                 * Change the mode of a ListItem which already exists in the pglcmd blocklists file.
                 * @param item The ListItem the mode of which is to be changed.
                 * @param newMode The new mode of the ListItem.
                 */
        void setMode( const ListItem &item, const ItemMode &newMode );
                /**
                 * Change the mode of a ListItem which already exists in the pglcmd blocklists file.
                 * The ListItem will be found according to its location.
                 * @param location The ListItem's location.
                 * @param newMode The ListItem's new mode.
                 */
        void setModeByLocation( const QString &location, const ItemMode &newMode );
                /**
                 * Change the mode of a ListItem which already exists in the pglcmd blocklists file.
                 * The ListItem will be found according to its name.
                 * @param location The ListItem's name.
                 * @param newMode The ListItem's new mode.
                 */
        void setModeByName( const QString &name, const ItemMode &newMode );
                /**
                 * Remove a ListItem completely from the blocklists file.
                 * WARNING: Searches by name are NOT safe and should be avoided.
                 * @param item The item to be found and removed.
                 */
                void removeItem( const ListItem &item );
                /**
                 * Remove a ListItem completely from the blocklists file.
                 * @param location The item's location.
                 */
                void removeItemByLocation( const QString &location );
                /**
                 * Remove a ListItem completely from the blocklists file.
                 * @param name The item's name.
                 */
                void removeItemByName( const QString &name );
                /**
                 * Get a ListItem which already exists in the blocklists file by its name.
                 * If no item is found a NULL pointer is returned.
                 * WARNING: Searches by name are NOT safe and should be avoided.
                 * @param name The name of the ListItem.
                 * @return A pointer to the ListItem requested, or a NULL pointer if the item is not found.
                 */
                ListItem *getItemByName( const QString &name );
                /**
                 * Get a ListItem which already exists in the blocklists file by its location.
                 * If no item is found a NULL pointer is returned.
                 * WARNING: Searches by name are NOT safe and should be avoided.
                 * @param location The location of the ListItem.
                 * @return A pointer to the ListItem requested, or a NULL pointer if the item is not found.
                 */
                ListItem *getItemByLocation( const QString &location );
                /**
                 * Get the items with the speicified mode.
                 * @param mode The mode of the ListItems.
                 * @return A vector of pointers containing the items with the requested mode.
                 */
        QVector< ListItem * > getItems( const ItemMode &mode );
                 /**
                  * Get all the ListItems which exist in the pglcmd blocklists file.
                  * Items with mode COMMENT_ITEM are not returned by this function.
                  * @return A vector of pointers containing the ListItems requested.
                  */
                 QVector< ListItem * > getValidItems();
        QVector< ListItem * > getEnabledItems();
        QVector< ListItem * > getDisabledItems();
                /**
                 * Export the PeerguardianList object to a file as a pglcmd blocklists file.
                 * @param filename The path to the file where the object is to be exported.
                 * @return True if the file was exported, otherwise false.
                 */
                bool exportToFile( const QString &filename );

        static QString getFilePath();
        static QString getFilePath(const QString &path);
        void update(QList<QTreeWidgetItem*>);
        QStringList localBlocklistsUpdate(const QList<QTreeWidgetItem*>&);
        QHash<QString, bool> getLocalLists(){ return m_localLists; }
        QString localBlocklistsDir();
        //QFileInfoList localBlocklists();
        void updateListsFromFile();
        void loadBlocklists();
        void addBlocklist(const QString&);
        QStringList generateBlocklistsFile();
        QList<Blocklist*> blocklists();
        QList<Blocklist*> localBlocklists();
        bool containsLocalBlocklist(Blocklist*);
        QString localBlocklistPath(Blocklist*);
        Blocklist* blocklistAt(int);
        void removeBlocklistAt(int);
        void removeBlocklist(Blocklist*);
        Blocklist* blocklist(const QString&);
        bool contains(const QString&);

    private:
        void loadLocalBlocklists();
        void loadBlocklistsFile();

        private:
                int indexOfName( const QString &name );
                QVector< ListItem > m_ListsFile;
        QString mBlocklistsFilePath;
        QHash<QString, bool> m_localLists;
        QStringList mLocalBlocklists;
        QString mLocalBlocklistsDir;
        QList<Blocklist*> mBlocklists;

signals:
        void blocklistAdded(Blocklist*);

};

#endif
