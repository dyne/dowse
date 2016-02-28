/******************************************************************************
 *   Copyright (C) 2012 by Carlos Pais <fr33mind@users.sourceforge.net>       *
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


#ifndef ERROR_DIALOG_H
#define ERROR_DIALOG_H

#include <QDialog>
#include <QTreeWidget>

#include "command.h"

class ErrorDialog : public QDialog
{
    Q_OBJECT

    QTreeWidget* mErrorWidget;

    public:
        explicit ErrorDialog(const CommandList& commands, QWidget* parent = 0);
        virtual ~ErrorDialog();
    private slots:

};

#endif

