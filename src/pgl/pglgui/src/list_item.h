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


#ifndef LIST_ITEM_H
#define LIST_ITEM_H

#include <QString>

#include "blocklist.h"
#include "enums.h"

//extern typedef enum ItemMode { ENABLED_ITEM, DISABLED_ITEM, COMMENT_ITEM } ItemMode;

class Blocklist;

/**
*
* @short Class representing both blocklist entries and comments in the pglcmd blocklists file.
*
*/

class ListItem {

    public:
        /**
         * Constructor. Creates a ListItem and analyzes the raw line from the configuration file.
         * @param itemRawLine The line from the pglcmd blocklists file.
         */
        ListItem( const QString &itemRawLine );
        /**
         * Constructor, Creates an empty ListItem.
         */
        ListItem() { mode = ENABLED_ITEM; }
        /**
         * Destructor.
         */
        ~ListItem() { }

        /**
         * The mode of the ListItem.
         * ENABLED_ITEM: Enabled blocklist entry.
         * DISABLED_ITEM: Disabled blocklist entry, line starts with #.
         * COMMENT_ITEM: Not a blocklist. Line starts with # but it's not valid.
         */
        ItemMode mode;
        QString value() const;

        /**
         * Check if this ListItem matches another.
         * This function compares only the locations of the ListItems.
         * @param other The second ListItem.
         * @return True if the ListItems are the same, otherwise false.
         */
        bool operator==( const ListItem &);

        bool isEnabled();
        bool isDisabled();
        bool isBlocklist();
        Blocklist* blocklist() const;

        static bool isValidBlockList(const QString&);
        QString getListName(const QString& );

    private:
        QString mValue;
        Blocklist* mBlocklist;

};

#endif
