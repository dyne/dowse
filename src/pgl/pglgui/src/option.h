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


#ifndef OPTION_H
#define OPTION_H

#include <QVariant>
#include <QString>

#include "option_p.h"

/**
*
* @short Class representing an option in the program.
*
*/

class OptionPrivate;

class Option {

        public:
        Option(const QString&, const QVariant&, bool active=false);
        Option(const Option&);
        explicit Option();
        virtual ~Option();
        bool operator==(const Option&);
        Option& operator=(const Option&);

        QString name() const;
        void setName(const QString&);
        QVariant value() const;
        void setValue(const QVariant&);
        bool isActive() const;
        bool isChanged() const;
        bool isEnabled() const;
        bool isDisabled() const;
        bool isRemoved() const;
        bool isAdded() const;
        void setRemoved(bool);
        void setEnabled(bool);
        void applyChanges();
        void undo();

protected:
        void setActiveData(OptionPrivate*);
        void setData(OptionPrivate*);

private:
    OptionPrivate* d_ptr;
    OptionPrivate* d_active_ptr;
};

#endif //OPTION_H

