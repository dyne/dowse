/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
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


#include "blocklist_manager.h"
#include "utils.h"
#include "pgl_settings.h"

// =========== BlocklistManager class =========== //

BlocklistManager::BlocklistManager( const QString &path, QObject* parent ) :
    QObject(parent)
{
    if (path.isEmpty())
        setFilePath(getValidPath(path, PglSettings::value("BLOCKLISTS_LIST")), true);
    else
        setFilePath(path, true);
    mLocalBlocklistsDir = PglSettings::value("LOCAL_BLOCKLIST_DIR") + "/";
}

BlocklistManager::~BlocklistManager()
{
    foreach(Blocklist* blocklist, mBlocklists) {
        if (blocklist)
            delete blocklist;
    }
}

void BlocklistManager::addBlocklist(const QString & url)
{
    Blocklist* bl = new Blocklist(url);
    mBlocklists.append(bl);
    emit blocklistAdded(bl);
}

void BlocklistManager::removeBlocklist(Blocklist* blocklist)
{
    if (! blocklist)
        return;

    if (blocklist->isAdded()) {
        mBlocklists.removeAll(blocklist);
        delete blocklist;
    }
    else {
        blocklist->setRemoved(true);
    }
}

void BlocklistManager::loadBlocklists()
{
    //clear all items, if any
    for(int i=0; i < mBlocklists.size(); i++)
        delete mBlocklists[i];
    mBlocklists.clear();
    m_ListsFile.clear();

    loadBlocklistsFile();
    loadLocalBlocklists();
}

void BlocklistManager::loadBlocklistsFile()
{
    if ( mBlocklistsFilePath.isEmpty() )
        return;

    QStringList tempFileData = getFileData( mBlocklistsFilePath );
    ListItem item;

    for( int i=0; i < tempFileData.size(); i++) {
        item = ListItem(tempFileData[i]);
        m_ListsFile.append(item);
        if (item.isBlocklist())
            mBlocklists.append(item.blocklist());
    }
}

void BlocklistManager::loadLocalBlocklists()
{
    if (! QFile::exists(mLocalBlocklistsDir))
        return;

    QDir blocklistsDir(mLocalBlocklistsDir);
    QString path;

    //enabled local blocklists
    foreach(const QFileInfo& info, blocklistsDir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files|QDir::Hidden)) {
        path = info.absoluteFilePath();
        //if (info.isSymLink())
            //path = info.symLinkTarget();

        if (info.isHidden())
            mBlocklists.append(new Blocklist(path, true, false));
        else
            mBlocklists.append(new Blocklist(path, true, true));
    }
}

QString BlocklistManager::localBlocklistsDir()
{
    return mLocalBlocklistsDir;
}

QString BlocklistManager::blocklistsFilePath()
{
    return mBlocklistsFilePath;
}

void BlocklistManager::setFilePath( const QString &path, bool verified )
{

    if ( verified )
        mBlocklistsFilePath = path;
    else
        mBlocklistsFilePath = getValidPath(path, PglSettings::value("BLOCKLISTS_LIST"));

    if ( mBlocklistsFilePath.isEmpty() )
        qWarning() << Q_FUNC_INFO << "Empty path given, doing nothing";

}

int BlocklistManager::indexOfName( const QString &name )
{
    for ( int i = 0; i < m_ListsFile.size(); i++ ) {
        if (m_ListsFile[i].blocklist() && m_ListsFile[i].blocklist()->name() == name ) {
            return i;
        }
    }

    return -1;
}

void BlocklistManager::addItem( const ListItem &newItem )
{
    m_ListsFile.append( newItem );
}

void BlocklistManager::addItem( const QString &line )
{
    ListItem newItem( line );
    addItem( newItem );
}

void BlocklistManager::setMode( const ListItem &item, const ItemMode &mode )
{
    int i = m_ListsFile.indexOf( item );
    if ( i >= 0 ) {
        m_ListsFile[i].mode = mode;
    }
}

void BlocklistManager::setModeByLocation( const QString &location, const ItemMode &mode )
{
    ListItem *item = getItemByLocation( location );
    if (item) {
        setMode( *item, mode );
    }
}

void BlocklistManager::setModeByName( const QString &name, const ItemMode &mode )
{
    ListItem *item = getItemByName( name );
    if (item) {
        setMode( *item, mode );
    }
}

void BlocklistManager::removeItem( const ListItem &item ) {

    int i = m_ListsFile.indexOf( item );
    if ( i >= 0 ) {
        m_ListsFile.remove( i );
    }

}

void BlocklistManager::removeItemByLocation( const QString &location ) {

    ListItem *item = getItemByLocation( location );
    if (item) {
        removeItem( *item );
    }
}

void BlocklistManager::removeItemByName( const QString &name)
{
    ListItem *item = getItemByName( name );
    if ( item) {
        removeItem( *item );
    }
}

ListItem *BlocklistManager::getItemByName( const QString &name  )
{
    /*for ( QVector<ListItem>::iterator s = m_ListsFile.begin(); s != m_ListsFile.end(); s++ ) {
        if ( s->name() == name )
            return s;
    }*/

    return 0;
}

ListItem *BlocklistManager::getItemByLocation( const QString &location )
{
    /*for ( QVector< ListItem >::iterator s = m_ListsFile.begin(); s != m_ListsFile.end(); s++ ) {
        if ( s->location() == location )
            return s;
    }*/

    return 0;
}

QVector< ListItem *> BlocklistManager::getItems(const ItemMode &mode ) {

    QVector< ListItem * > result;

    for ( QVector< ListItem >::iterator s = m_ListsFile.begin(); s != m_ListsFile.end(); s++ )
        if ( s->mode == mode )
            result.push_back(s);

    return result;
}

QVector< ListItem * > BlocklistManager::getValidItems()
{
    QVector< ListItem * > result;
    for ( QVector< ListItem >::iterator s = m_ListsFile.begin(); s != m_ListsFile.end(); s++ )
        if ( s->mode != COMMENT_ITEM )
            result.push_back(s);

    return result;
}

QVector< ListItem * >  BlocklistManager::getEnabledItems()
{
    return getItems(ENABLED_ITEM);
}

QVector< ListItem * >  BlocklistManager::getDisabledItems()
{
    return getItems(DISABLED_ITEM);
}

QStringList BlocklistManager::generateBlocklistsFile()
{
    QStringList newFileData;
    QList<Blocklist*> addedBlocklists;

    foreach(const ListItem& item, m_ListsFile) {
        if ( item.mode == COMMENT_ITEM )
            newFileData << item.value();
        else if (item.blocklist()) {
            addedBlocklists.append(item.blocklist());

            if (item.blocklist()->isRemoved())
                continue;

            if (item.blocklist()->isChanged()){
                if (item.blocklist()->isEnabled())
                    newFileData << item.blocklist()->location();
                else
                    newFileData << "# " + item.blocklist()->location();
            }
            else {
                newFileData << item.value();
            }
        }

    }

    //add possible new blocklists
    foreach(Blocklist* blocklist, mBlocklists) {
        if (blocklist->isLocal() || addedBlocklists.contains(blocklist) || ! blocklist->isChanged() || blocklist->isRemoved())
            continue;
        if (blocklist->isEnabled())
            newFileData << blocklist->location();
        else
            newFileData << "# " + blocklist->location();
    }

    //saveFileData(newFileData, QDir::temp().absoluteFilePath(getFileName(mBlocklistsFilePath)));
    return newFileData;
}

/*QFileInfoList BlocklistManager::localBlocklists()
{
      QFileInfoList localBlocklists;
      QDir defaultDir (mLocalBlocklistsDir);

      foreach(QFileInfo fileInfo, defaultDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files) )
          if ( fileInfo.isSymLink() )
              localBlocklists << QFileInfo(fileInfo.symLinkTarget());

      return localBlocklists;
}*/

QList<Blocklist*> BlocklistManager::localBlocklists()
{
    QList<Blocklist*> localBlocklists;

    foreach(Blocklist* blocklist, mBlocklists)
        if (blocklist->isLocal())
            localBlocklists.append(blocklist);
    return localBlocklists;
}

QStringList BlocklistManager::localBlocklistsUpdate(const QList<QTreeWidgetItem*>& treeItems)
{
    bool state;
    foreach(QTreeWidgetItem * treeItem, treeItems) {
        //if it's a filepath
        if ( QFile::exists( treeItem->text(1) ) ){
            state = true;
            if ( treeItem->checkState(0) == Qt::Unchecked )
                state = false;

            m_localLists.insert(treeItem->text(1), state);
        }
    }
}

QList<Blocklist*> BlocklistManager::blocklists()
{
    return mBlocklists;
}

bool BlocklistManager::containsLocalBlocklist(Blocklist* blocklist)
{
    return ! localBlocklistPath(blocklist).isEmpty();
}

QString BlocklistManager::localBlocklistPath(Blocklist * blocklist)
{
    QFileInfoList filesInfo = getFilesInfo(mLocalBlocklistsDir, QDir::NoDotAndDotDot|QDir::Files|QDir::Hidden);
    QString nameMatch;

    foreach(const QFileInfo& info, filesInfo) {
        if (info.isDir()) continue;
        if (info.isSymLink() && info.symLinkTarget() == blocklist->location())
            return info.absoluteFilePath();
        else if (info.fileName() == blocklist->name())
            nameMatch = info.absoluteFilePath();
    }

    return nameMatch;
}

Blocklist* BlocklistManager::blocklistAt(int index)
{
    if (index >= 0 && index < mBlocklists.size())
        return mBlocklists[index];
    return 0;
}

void BlocklistManager::removeBlocklistAt(int index)
{
    if (index >= 0 && index < mBlocklists.size())
        mBlocklists[index]->setRemoved(true);
}

Blocklist* BlocklistManager::blocklist(const QString & value)
{
    foreach(Blocklist* blocklist, mBlocklists)
        if (blocklist->targetLocation() == value)
            return blocklist;
    return 0;
}


bool BlocklistManager::contains(const QString & value)
{
    if (blocklist(value))
        return true;
    return false;
}

/*** Static methods ***/

QString BlocklistManager::getFilePath()
{
    QString path("");
    return getValidPath(path, PglSettings::value("BLOCKLISTS_LIST"));
}

QString BlocklistManager::getFilePath(const QString &path)
{
    return getValidPath(path, PglSettings::value("BLOCKLISTS_LIST"));
}


