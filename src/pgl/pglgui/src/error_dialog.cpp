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


#include "error_dialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QFile>
#include <QList>
#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QStyle>
#include <QIcon>
#include <QSize>
#include <QHeaderView>
#include <QDialogButtonBox>


ErrorDialog::ErrorDialog(const CommandList& failedCommands, QWidget* parent) :
    QDialog(parent)
{
    QIcon warningIcon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
    QString errorMsg = tr("One or more command(s) failed to execute.") +
                        "<br/>" + tr("You can check below the commands that failed and their output.") +
                        "<br/>" + tr("You can also check pgld's and/or pglcmd's log, through the <i>View menu</i>.");

    QLabel * textLabel = new QLabel(errorMsg, this);
    QLabel * iconLabel = new QLabel(this);
    iconLabel->setPixmap(warningIcon.pixmap(QSize(32, 32)));

    mErrorWidget = new QTreeWidget(this);
    mErrorWidget->setColumnCount(2);
    mErrorWidget->setHeaderItem(new QTreeWidgetItem(QStringList() << "Command" << "Output"));
    mErrorWidget->setRootIsDecorated(false);
    mErrorWidget->header()->resizeSection(0, mErrorWidget->header()->sectionSize(0)*3);

    QDialogButtonBox *okButton = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    okButton->setCenterButtons(true);
    connect(okButton, SIGNAL(accepted()), this, SLOT(accept()));

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(iconLabel);
    hlayout->addWidget(textLabel);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(mErrorWidget);
    vlayout->addWidget(okButton);


    QTreeWidgetItem* item;
    QTextEdit *textEdit;
    QString output("");

    foreach(const Command& cmd, failedCommands) {
        if (cmd.output().isEmpty())
            output = tr("(None)");
        else
            output = cmd.output();

        item = new QTreeWidgetItem(mErrorWidget, QStringList() << cmd.command() << output);
        //textEdit = new QTextEdit(output, mErrorWidget);
        //mErrorWidget->setItemWidget(item, 1, textEdit);
        //textEdit->setFixedHeight(textEdit->height()/3);
    }

    setWindowTitle(tr("Error"));
    setWindowIcon(warningIcon);

}

ErrorDialog::~ErrorDialog()
{
}
