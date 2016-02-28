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


#include <QEvent>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QCompleter>
#include <QFileSystemModel>

#include "add_exception_dialog.h"
#include "utils.h"
#include "file_transactions.h"
#include "blocklist_manager.h"

class WhitelistItem;

AddExceptionDialog::AddExceptionDialog(QWidget *p, int mode, PglCore* pglCore) :
        QDialog( p )
{
        setupUi( this );

    mPglCore = pglCore;
    m_validExtensions << ".p2p" << ".zip" << ".7z" << ".gzip" << ".dat";
    QString help;

    groupBox_2->hide();

    if ( mode == (ADD_MODE | EXCEPTION_MODE) )
    {
        //setPortsFromFile();
        //completer for the ports' names
        QStringList portNames = mPglCore->whitelistManager()->systemPortsNameToNumber().keys();

        QCompleter * completer = new QCompleter(portNames, m_addEdit);
        m_addEdit->setCompleter(completer);
        m_browseButton->hide();

        help  = QObject::tr("Valid Inputs:") + "\n";
        help += QObject::tr("You can enter an IP Address with or without mask,");
        help += QObject::tr("a range of ports (eg. 1:50) or a single port number/name (e.g. 80, http, ftp, etc).") + "\n\n";
        help += QObject::tr("You can enter multiple items separated by spaces.") + "\n\n";
        help += QObject::tr("Please note that allowing an outgoing port is a certain security risk:");
        help += QObject::tr("a malicious host may listen on just this port. It is safer to allow traffic to certain IPs instead.");

        /*foreach(QTreeWidgetItem *treeItem, treeItems)
        {
            value = treeItem->text(0);
            WhitelistItem item = WhitelistItem(value, treeItem->text(1), treeItem->text(2));

            foreach(const Port& port, mPorts)
                if ( port.containsName(item.value()) )
                    item.addAliases(port.names());

            m_Items.push_back(item);
        }*/

        connect(m_addEdit, SIGNAL(returnPressed()), this, SLOT(addEntry()));
        connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(addEntry()));

        resize(width(), minimumSize().height());
    }
    else if ( mode == (ADD_MODE | BLOCKLIST_MODE) )
    {

            QFileSystemModel * model = new QFileSystemModel(m_addEdit);
            model->setRootPath(QDir::homePath());

            QCompleter * completer = new QCompleter(model, m_addEdit);
            m_addEdit->setCompleter(completer);

            help = QObject::tr("Valid Inputs: You can enter a local path or an address to a valid blocklist.") + "\n";
            help += QObject::tr("You can enter multiple items separated by spaces!") + "\n\n";
            help += QObject::tr("If you are adding a local blocklist, please remember that the file you're adding won't be copied! ");
            help += QObject::tr("In other words, if you delete the file you're adding, it will be no longer available or used in pgl after a restart or reload") + "\n\n";
            help += QObject::tr("If you are adding an URL please prepend \"http://\", \"https://\" or \"ftp://\" in case your address doesn't have it!");

            setWindowTitle(QObject::tr("Add Blocklists"));
            groupBox->setTitle(QObject::tr("Add one or more Blocklists"));
            m_ConnectionGroup->hide();
            m_ProtocolGroup->hide();
            m_ipRadio->hide();
            m_portRadio->hide();
            connect(m_browseButton, SIGNAL(clicked()), this, SLOT(selectLocalBlocklist()));


            m_notValidTreeWidget->header()->hideSection(1);
            m_notValidTreeWidget->header()->hideSection(2);


        connect(m_addEdit, SIGNAL(returnPressed()), this, SLOT(addBlocklist()));
        connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(addBlocklist()));

       setMinimumSize(0, 200);
       resize(width(), height()/1.8);
    }

    m_helpLabel->setText(QObject::tr(help.toUtf8 ()));

    /*else if ( mode == (EDIT_MODE | EXCEPTION_MODE) )
    {
            m_exceptionTreeWidget->hide();
            m_helpLabel->hide();
            resize(width(), 300);
    }
    else if ( mode == (EDIT_MODE | BLOCKLIST_MODE))
    {
    }*/

    this->mode = mode;

}

//************************** Whitelist dialog ***************************//

AddExceptionDialog::~AddExceptionDialog()
{
}

bool AddExceptionDialog::isValidException(QString& text)
{
    WhitelistManager* whitelist = mPglCore->whitelistManager();
    if (isValidIp(text) || whitelist->isPort( text ) )
        return true;

    return false;
}

QStringList AddExceptionDialog::getConnections()
{
    QStringList connections;

    if ( m_OutCheck->checkState() == Qt::Checked )
        connections << m_OutCheck->text();

    if ( m_InCheck->checkState() == Qt::Checked )
        connections << m_InCheck->text();

    if ( m_FwdCheck->checkState() == Qt::Checked )
        connections << m_FwdCheck->text();

    return connections;
}

QStringList AddExceptionDialog::getProtocols(bool isIp)
{
    QStringList protocols;

    if ( isIp )
        protocols << "IP";
    else
    {
        if ( m_TcpCheck->checkState() ==  Qt::Checked )
            protocols << m_TcpCheck->text();
        if ( m_UdpCheck->checkState() ==  Qt::Checked )
            protocols << m_UdpCheck->text();
    }

    return protocols;
}

QStringList AddExceptionDialog::getParams(const QString& text)
{
    if ( mode == (ADD_MODE | BLOCKLIST_MODE) )
    {
        QStringList params, paths("");
        QString param;
        QFileInfo file("");
        bool append = false;
        params = text.split(' ');


        for(int i=0; i < params.size(); i++)
        {

            if ( append )
                param += params[i];
            else
                param = params[i];

            //if it's a filepath
            if ( param.trimmed()[0] == '/' || append)
            {
                file.setFile(param.trimmed());

                //if the file exists and it's not a directory
                //it means it's the full path
                if ( file.exists() && ( ! file.isDir()) )
                {
                    paths << param.trimmed();
                    param.clear();
                    append = false;
                }
                else  //or else we assume it's a part of the path
                {
                    param += " "; //since the string was splitted by spaces, we have to re add them to reconstruct the string properly
                    append = true;
                }
            }
            else if ( ! param.simplified().isEmpty() )//if it's not an empty string it might be an URL
            {
                paths << param;
                param.clear();
            }
        }

        if ( ! param.isEmpty() )
            paths << param;

        return paths;
    }
    else
        return text.split(' ', QString::SkipEmptyParts);
}

bool AddExceptionDialog::isValidWhitelistItem(WhitelistItem& whiteItem, QString& reason)
{
    reason = "";

    //checks if the new item doesn't already exist
    foreach(WhitelistItem tempItem, m_Items) {
        if ( tempItem == whiteItem ) {
            reason = QObject::tr("It's already added");
            return false;
        }
    }

    foreach(const Port& port, mPorts)
        if ( port.containsName(whiteItem.value()) )
            if ( ! port.hasProtocol(whiteItem.protocol()) ) {
                reason += whiteItem.value();
                reason += QObject::tr(" doesn't work over ") + whiteItem.protocol();
                return false;
            }

    return true;
}

void AddExceptionDialog::setWhitelistItems(QString& value, bool isIp)
{
    QList<WhitelistItem> items;
    QStringList protocols, connections;
    WhitelistManager* whitelist = mPglCore->whitelistManager();

    protocols = getProtocols(isIp);
    connections = getConnections();

    QString reason("");

    foreach(QString protocol, protocols)
    {
        foreach(QString connection, connections)
        {
            WhitelistItem item = WhitelistItem(value, connection, protocol);

            if ( whitelist->isValid(item, reason) )
                m_validItems << item;
            else
            {
                m_invalidItems << item;
                reasons << reason;
            }
        }

    }
}

void AddExceptionDialog::addEntry()
//Used by exception (whitelist) window
{
    if ( m_addEdit->text().isEmpty() )
        return;

    m_invalidItems.clear();
    m_validItems.clear();
    reasons.clear();

    QStringList values, info;
    bool ip = false;
    QStringList unrecognizedValues;
    WhitelistManager* whitelist = mPglCore->whitelistManager();

    values = getParams(m_addEdit->text());

    foreach(QString param, values)
    {
        param = param.trimmed();

        if ( param.isEmpty() )
            continue;

        if ( whitelist->isPort(param) )
            ip = false;
        else if ( isValidIp(param ) )
            ip = true;
        else
        {
            unrecognizedValues << param;
            continue;
        }

        if (ip && m_portRadio->isChecked()) {
            m_invalidItems << WhitelistItem(param, "", "");
            reasons << "Not a valid port.";
        }
        else if (!ip && m_ipRadio->isChecked()) {
            m_invalidItems << WhitelistItem(param, "", "");
            reasons << "Not a valid ip.";
        }
        else {
            setWhitelistItems(param, ip);
        }
    }

    m_notValidTreeWidget->clear();

    if ( ! unrecognizedValues.isEmpty() || ! m_invalidItems.isEmpty() )
    {
        if ( ! groupBox_2->isVisible() )
            groupBox_2->setVisible(true);

        foreach(QString value, unrecognizedValues)
        {
            QStringList info; info << value << "ANY" << "ANY" << "Not a valid IP nor a Port";
            QTreeWidgetItem *item = new QTreeWidgetItem(m_notValidTreeWidget, info);
            m_notValidTreeWidget->addTopLevelItem(item);
        }

        for(int i=0; i < m_invalidItems.size(); i++)
        {
            WhitelistItem whiteItem = m_invalidItems[i];
            QStringList info; info << whiteItem.value() << whiteItem.connection() << whiteItem.protocol() << reasons[i];
            QTreeWidgetItem *item = new QTreeWidgetItem(m_notValidTreeWidget, info);
            m_notValidTreeWidget->addTopLevelItem(item);
        }

        resize(width(), minimumSize().height()*1.3);

    }
    else
        emit( accept() );

}

//************************** Blocklist dialog ***************************//

void AddExceptionDialog::selectLocalBlocklist()
{
    QString filter;
    filter += "All Supported files  (*.p2p *.gz *.7z *.zip *.dat );;";
    filter += "P2P (*.p2p);;Zip (*.zip);; 7z (*.7z);;Gzip (*.gz);;Dat (*.dat)";

    QStringList files = selectFiles(this, filter);

    QString text = m_addEdit->text();
    text += files.join(QString(" "));
    text += " ";
    m_addEdit->setText(text);
}

void AddExceptionDialog::addBlocklist()
{
    QStringList values = getParams(m_addEdit->text());
    BlocklistManager* blocklistManager = mPglCore->blocklistManager();
    m_notValidTreeWidget->clear();
    QStringList invalidValues;
    m_blocklists.clear();
    QString reason("");

    foreach(const QString& value, values)
    {
        if ( value.simplified().isEmpty() )
            continue;

        Blocklist* blocklist = blocklistManager->blocklist(value);

        if ( ! Blocklist::isValid(value)) {
            reason = tr("Not recognized as a valid local path nor it seems a valid URL (did you forget to prepend http, https or ftp?).");
            invalidValues << value;
        }
        else if (blocklist && blocklist->isEnabled() && ! blocklist->isRemoved()) {
            reason = QObject::tr("It's already added");
            invalidValues << value;
        }
        else
            m_blocklists.push_back(value);
    }

    if ( invalidValues.isEmpty() )
        emit(accept());
    else
    {
        if ( ! groupBox_2->isVisible() )
        {
            resize(width(), height()*2);
            groupBox_2->setVisible(true);
        }

        for(int i=0; i < invalidValues.size(); i++)
        {
            QStringList info;
            info << invalidValues[i] << "N/A" << "N/A" << reason;
            QTreeWidgetItem *item = new QTreeWidgetItem(m_notValidTreeWidget, info);
            item->setToolTip(3, reason);
        }
    }
}

//Dialog specific

void AddExceptionDialog::keyPressEvent ( QKeyEvent * e )
{
    if (e->key() == Qt::Key_Escape)
        QDialog::keyPressEvent (e);
}
