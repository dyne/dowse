/******************************************************************************
 *   Copyright (C) 2011-2013 by Carlos Pais <fr33mind@users.sourceforge.net>  *
 *   Copyright (C) 2007-2008 by Dimitris Palyvos-Giannas <jimaras@gmail.com>  *
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


#include <QDebug>
#include <QMultiMap>
#include <QHash>
#include <QRegExp>
#include <QDBusConnection>
#include <QApplication>
#include <QDesktopWidget>
#include <QScrollBar>
//#include <Action>
//#include <ActionButton>

#include "pglgui.h"
#include "file_transactions.h"
#include "utils.h"
#include "pgl_settings.h"
#include "viewer_widget.h"
#include "error_dialog.h"

//using namespace PolkitQt1;
//using namespace PolkitQt1::Gui;

PglGui::PglGui( QWidget *parent) :
        QMainWindow( parent )
{
    mUi.setupUi( this );

    mUi.logTreeWidget->setContextMenuPolicy ( Qt::CustomContextMenu );

    if ( ! PglSettings::loadSettings() )
        QMessageBox::warning(this, tr("Error"), PglSettings::lastError(), QMessageBox::Ok);

    m_StopLogging = false;
    mAutomaticScroll = true;
    mIgnoreScroll = false;
    mTrayIconDisabled = QIcon(TRAY_DISABLED_ICON);
    mTrayIconEnabled = QIcon(TRAY_ICON);
    setWindowIcon(mTrayIconDisabled);

    initCore();
    g_MakeTray();
    g_MakeMenus();
    g_MakeConnections();
    loadGUI();
    checkDaemonStatus();

    //resize columns in log view
    QHeaderView * header = mUi.logTreeWidget->header();
    header->resizeSection(0, header->sectionSize(0) / 1.5 );
    header->resizeSection(1, header->sectionSize(0) * 3 );
    header->resizeSection(3, header->sectionSize(0) / 1.4 );
    header->resizeSection(5, header->sectionSize(0) / 1.4 );
    header->resizeSection(6, header->sectionSize(6) / 2);

    //resize column in whitelist view
    header = mUi.whitelistTreeWidget->header();
    header->resizeSection(0, header->sectionSize(0) * 2);
    header->resizeSection(2, header->sectionSize(2) / 2);

    a_whitelistIpTemp = new QAction(tr("Allow temporarily"), this);
    a_whitelistIpTemp->setToolTip(tr("Allows until pgld is restarted."));
    a_whitelistIpPerm = new QAction(tr("Allow permanently"), this);
    a_whitelistPortTemp = new QAction(tr("Allow temporarily"), this);
    a_whitelistPortTemp->setToolTip(tr("Allows until pgld is restarted."));
    a_whitelistPortPerm = new QAction(tr("Allow permanently"), this);
    aWhoisIp = new QAction(tr("Whois "), this);

    m_ConnectType["OUT"] = tr("Outgoing");
    m_ConnectType["IN"] = tr("Incoming");
    m_ConnectType["FWD"] = tr("Forward");

    m_ConnectIconType[tr("Outgoing")] = QIcon(LOG_LIST_OUTGOING_ICON);
    m_ConnectIconType[tr("Incoming")] = QIcon(LOG_LIST_INCOMING_ICON);
    m_ConnectIconType[tr("Forward")] = QIcon();


    QDBusConnection connection (QDBusConnection::systemBus());
    QString service("");
    QString name("pgld_message");
    QString path("/org/netfilter/pgl");
    QString interface("org.netfilter.pgl");

    bool ok = connection.connect(service, path, interface, name, qobject_cast<QObject*>(this), SLOT(addLogItem(QString)));

    if ( ! ok )
        qDebug() << "Connection to DBus failed.";
    else
        qDebug() << "Connection to DBus was successful.";

    //center window
    QDesktopWidget *desktop = QApplication::desktop();
    int yy = desktop->height()/2-height()/2;
    int xx = desktop->width() /2-width()/2;
    move(xx, yy);

    mUi.logTreeWidget->verticalScrollBar()->installEventFilter(this);

    connect(aWhoisIp, SIGNAL(triggered()), this, SLOT(onWhoisTriggered()));
    connect(a_whitelistIpTemp, SIGNAL(triggered()), this, SLOT(whitelistItem()));
    connect(a_whitelistIpPerm, SIGNAL(triggered()), this, SLOT(whitelistItem()));
    connect(a_whitelistPortTemp, SIGNAL(triggered()), this, SLOT(whitelistItem()));
    connect(a_whitelistPortPerm, SIGNAL(triggered()), this, SLOT(whitelistItem()));

    //ActionButton *bt;
    //bt = new ActionButton(kickPB, "org.qt.policykit.examples.kick", this);
    //bt->setText("Kick... (long)");

    restoreSettings();
}

PglGui::~PglGui()
{
    qWarning() << "~PglGui()";

    saveSettings();

    //Free memory
    if(mPglCore)
        delete mPglCore;
}

void PglGui::addLogItem(QString itemString)
{
    if ( m_StopLogging )
        return;

    if ( itemString.contains("INFO:") && itemString.contains("Blocking") )
    {

        QStringList parts = itemString.split("INFO:", QString::SkipEmptyParts);
        m_Info->setLoadedIps(parts[0]);
        return;
    }

    if ( itemString.contains("||") )
    {
        QStringList parts = itemString.split("||", QString::SkipEmptyParts);
        QStringList firstPart = parts.first().split(" ", QString::SkipEmptyParts);
        QString connectType, srcip, destip, srcport, destport;

        if ( firstPart.first().contains(":") )
            connectType = m_ConnectType[firstPart.first().split(":")[0]];
        else
            connectType = m_ConnectType[firstPart.first()];

        if ( firstPart[3] == "TCP" || firstPart[3] == "UDP" )
        {
            srcip = firstPart[1].split(":", QString::SkipEmptyParts)[0];
            srcport = firstPart[1].split(":", QString::SkipEmptyParts)[1];
            destip = firstPart[2].split(":", QString::SkipEmptyParts)[0];
            destport = firstPart[2].split(":", QString::SkipEmptyParts)[1];
        }
        else
        {
            srcip = firstPart[1];
            srcport = "";
            destip = firstPart[2];
            destport = "";
        }

        QStringList info;

        if ( mUi.logTreeWidget->topLevelItemCount() > m_MaxLogSize ) {
            //mIgnoreScroll = true;
            mUi.logTreeWidget->takeTopLevelItem(0);
            //mIgnoreScroll = false;
        }

        info << QTime::currentTime().toString("hh:mm:ss") << parts.last() << srcip << srcport << destip << destport << firstPart[3] << connectType;
        QTreeWidgetItem * item = new QTreeWidgetItem(mUi.logTreeWidget, info);
        item->setIcon(7, m_ConnectIconType[connectType]);
        mUi.logTreeWidget->addTopLevelItem(item);


        if (mAutomaticScroll)
            mUi.logTreeWidget->scrollToBottom();
    }
}


void PglGui::saveSettings()
{
    QString name;
    //Save column sizes
    for (int i = 0; i < mUi.logTreeWidget->columnCount() ; i++ ) {
        name = QString("logTreeView/column_%1").arg(i);
        m_ProgramSettings->setValue( name, mUi.logTreeWidget->columnWidth(i) );
    }

    //Save window settings
    m_ProgramSettings->setValue( "window/state", saveState() );
    m_ProgramSettings->setValue( "window/geometry", saveGeometry() );
}

void PglGui::restoreSettings()
{
    bool ok;
    //Restore to the window's previous state
    if (m_ProgramSettings->contains("window/geometry"))
        restoreGeometry( m_ProgramSettings->value("window/geometry").toByteArray() );

    if (m_ProgramSettings->contains("window/state"))
        restoreState( m_ProgramSettings->value( "window/state" ).toByteArray() );

    for (int i = 0; i < mUi.logTreeWidget->columnCount(); i++ ) {
        QString settingName = "logTreeView/column_" + QString::number(i);
        if (m_ProgramSettings->contains(settingName)) {
            int value = m_ProgramSettings->value(settingName).toInt(&ok);
            if (ok)
                mUi.logTreeWidget->setColumnWidth(i, value);
        }
    }

    QString max = m_ProgramSettings->value("maximum_log_entries").toString();
    if ( max.isEmpty() ) {
        m_ProgramSettings->setValue("maximum_log_entries", MAX_LOG_SIZE);
        m_MaxLogSize = MAX_LOG_SIZE;
    }
    else {
        bool ok;
        m_MaxLogSize = max.toInt(&ok);
        if ( ! ok )
            m_MaxLogSize = MAX_LOG_SIZE;
    }
}

void PglGui::loadGUI()
{
    mUi.startAtBootCheckBox->setChecked(mPglCore->option("startAtBoot")->isEnabled());
    mUi.updateAutomaticallyCheckBox->setChecked(mPglCore->option("updateAutomatically")->isEnabled());
    mUi.updateDailyRadio->setChecked(mPglCore->option("updateDailyRadio")->isEnabled());
    mUi.updateWeeklyRadio->setChecked(mPglCore->option("updateWeeklyRadio")->isEnabled());
    mUi.updateMonthlyRadio->setChecked(mPglCore->option("updateMonthlyRadio")->isEnabled());

    setButtonChanged(mUi.startAtBootCheckBox, mPglCore->option("startAtBoot")->isChanged());
    setButtonChanged(mUi.updateAutomaticallyCheckBox, mPglCore->option("updateAutomatically")->isChanged());
    setButtonChanged(mUi.updateDailyRadio, mPglCore->option("updateDailyRadio")->isChanged());
    setButtonChanged(mUi.updateWeeklyRadio, mPglCore->option("updateWeeklyRadio")->isChanged());
    setButtonChanged(mUi.updateMonthlyRadio, mPglCore->option("updateMonthlyRadio")->isChanged());

    loadBlocklistWidget();
    loadWhitelistWidget();
}

void PglGui::g_MakeConnections()
{
        //Log tab connections
    connect( mUi.logTreeWidget, SIGNAL(customContextMenuRequested ( const QPoint &)), this, SLOT(showLogRightClickMenu(const QPoint &)));
    connect( mUi.logTreeWidget->verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(onLogViewVerticalScrollbarMoved(int)));
    connect( mUi.logTreeWidget->verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(onLogViewVerticalScrollbarActionTriggered(int)));
    connect( mUi.clearLogButton, SIGNAL( clicked() ), mUi.logTreeWidget, SLOT( clear() ) );
    connect(mUi.stopLoggingButton, SIGNAL(clicked()), this, SLOT(startStopLogging()));


    connect( mUi.addExceptionButton, SIGNAL(clicked()), this, SLOT(showAddExceptionDialog()) );
    connect( mUi.addBlockListButton, SIGNAL(clicked()), this, SLOT(showAddBlocklistDialog()) );

    //Menu related
    connect( mUi.a_Exit, SIGNAL( triggered() ), this, SLOT( quit() ) );
    connect( mUi.a_AboutDialog, SIGNAL( triggered() ), this, SLOT( g_ShowAboutDialog() ) );
    connect(mUi.viewPglcmdLogAction, SIGNAL(triggered()), this, SLOT(onViewerWidgetRequested()));
    connect(mUi.viewPgldLogAction, SIGNAL(triggered()), this, SLOT(onViewerWidgetRequested()));

    //Control related
    if ( m_Control ) {
        connect( mUi.startPglButton, SIGNAL( clicked() ), m_Control, SLOT( start() ) );
        connect( mUi.stopPglButton, SIGNAL( clicked() ), m_Control, SLOT( stop() ) );
        connect( mUi.restartPglButton, SIGNAL( clicked() ), m_Control, SLOT( restart() ) );
        connect( mUi.reloadPglButton, SIGNAL( clicked() ), m_Control, SLOT( reload() ) );
        connect( mUi.a_Start, SIGNAL( triggered() ), m_Control, SLOT( start() ) );
        connect( mUi.a_Stop, SIGNAL( triggered() ), m_Control, SLOT( stop() ) );
        connect( mUi.a_Restart, SIGNAL( triggered() ), m_Control, SLOT( restart() ) );
        connect( mUi.a_Reload, SIGNAL( triggered() ), m_Control, SLOT( reload() ) );
        connect( mUi.updatePglButton, SIGNAL( clicked() ), m_Control, SLOT( update() ) );
        connect( m_Control, SIGNAL(error(const QString&)), this, SLOT(rootError(const QString&)));
        connect( m_Control, SIGNAL(finished(const CommandList&)), this, SLOT(controlFinished(const CommandList&)));
        connect( m_Control, SIGNAL( actionMessage(const QString&, int ) ), mUi.statusBar, SLOT( showMessage( const QString&, int ) ) );
    }

        //Blocklist and Whitelist Tree Widgets
    connect(mUi.whitelistTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(whitelistItemChanged(QTreeWidgetItem*, int)));
    connect(mUi.blocklistTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(blocklistItemChanged(QTreeWidgetItem*,int)));
    connect(mUi.whitelistTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(treeItemPressed(QTreeWidgetItem*, int)));
    connect(mUi.blocklistTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(treeItemPressed(QTreeWidgetItem*, int)));

    /********************************Configure tab****************************/
    connect(mUi.undoButton, SIGNAL(clicked()), this, SLOT(undoAll()));
    connect(mUi.applyButton, SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(mUi.startAtBootCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkboxChanged(bool)));
    connect(mUi.updateAutomaticallyCheckBox, SIGNAL(clicked(bool)), this, SLOT(checkboxChanged(bool)));

    //connect update frequency radio buttons
    connect(mUi.updateDailyRadio, SIGNAL(clicked(bool)), this, SLOT(updateRadioButtonToggled(bool)));
    connect(mUi.updateWeeklyRadio, SIGNAL(clicked(bool)), this, SLOT(updateRadioButtonToggled(bool)));
    connect(mUi.updateMonthlyRadio, SIGNAL(clicked(bool)), this, SLOT(updateRadioButtonToggled(bool)));

    if ( m_Root ) {
        connect(m_Root, SIGNAL(finished(const CommandList&)), this, SLOT(rootFinished(const CommandList&)));
        connect(m_Root, SIGNAL(error(const QString&)), this, SLOT(rootError(const QString&)));
    }

    if (m_Info)
        connect(m_Info, SIGNAL(stateChanged(bool)), this, SLOT(onDaemonChanged(bool)));

    //connect whitelist and blocklists managers
    connect(mPglCore->whitelistManager(), SIGNAL(itemAdded(WhitelistItem*)), this, SLOT(addWhitelistItem(WhitelistItem*)));
    connect(mPglCore->blocklistManager(), SIGNAL(blocklistAdded(Blocklist*)), this, SLOT(addBlocklistItem(Blocklist*)));

    //connect the remove buttons
    connect(mUi.removeBlocklistButton, SIGNAL(clicked()), this, SLOT(removeListItems()));
    connect(mUi.removeExceptionButton, SIGNAL(clicked()), this, SLOT(removeListItems()));

    //tray iconPath
    connect(m_Tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIconClicked(QSystemTrayIcon::ActivationReason)));

    connect(mUi.a_SettingsDialog, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));
}

void PglGui::quit()
{
    int answer;

    if (mUi.applyButton->isEnabled()) {
        answer = confirm(tr("Really quit?"), tr("You have <b>unapplied</b> changes, do you really want to quit?"), this);
        if ( answer == QMessageBox::No )
            return;
    }

    qApp->quit();
}

void PglGui::closeEvent ( QCloseEvent * event )
{
    this->hide();
}

void PglGui::onTrayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    if ( reason == QSystemTrayIcon::Trigger )
        this->setVisible ( ! this->isVisible() );
}


void PglGui::removeListItems()
{
    QTreeWidget * tree;
    bool isWhitelist;

    if ( sender()->objectName().contains("block", Qt::CaseInsensitive) ) {
        tree  = mUi.blocklistTreeWidget;
        isWhitelist = false;
    }
    else {
        tree  = mUi.whitelistTreeWidget;
        isWhitelist = true;
    }

    foreach(QTreeWidgetItem *item, tree->selectedItems()) {
        QVariant _item = item->data(0, Qt::UserRole);

        if ( isWhitelist ) {
            WhitelistItem* whitelistItem = (WhitelistItem*) _item.value<void*>();
            mPglCore->whitelistManager()->removeItem(whitelistItem);
        }
        else {
            Blocklist* blocklist = (Blocklist*) _item.value<void*>();
            mPglCore->blocklistManager()->removeBlocklist(blocklist);
        }

        item->setData(0, Qt::UserRole, 0);
    }

    foreach(QTreeWidgetItem* item, tree->selectedItems())
        tree->takeTopLevelItem(tree->indexOfTopLevelItem(item));

    setApplyButtonEnabled(mPglCore->isChanged());
}

void PglGui::controlFinished(const CommandList & commands)
{
    CommandList failedCommands;
    foreach(const Command& cmd, commands) {
        if (cmd.error())
            failedCommands << cmd;
    }

    if (! failedCommands.isEmpty()) {
        setApplyButtonEnabled(true);
        ErrorDialog dialog(failedCommands);
        dialog.exec();
    }

    mUi.statusBar->clearMessage();
}

void PglGui::rootFinished(const CommandList& commands)
{
    CommandList failedCommands;
    foreach(const Command& cmd, commands) {
        if (cmd.error())
            failedCommands << cmd;
    }

    if (! failedCommands.isEmpty()) {
        setApplyButtonEnabled(true);
        ErrorDialog dialog(failedCommands);
        dialog.exec();
    }
    else {
        WhitelistManager* whitelist = mPglCore->whitelistManager();
        whitelist->updateGuiSettings();
        mPglCore->load();
        loadGUI();
        setApplyButtonEnabled(false);
    }
}

void PglGui::rootError(const QString& errorMsg)
{
    QMessageBox::warning( this, tr("Error"), errorMsg,
        QMessageBox::Ok
    );

    setApplyButtonEnabled(mPglCore->isChanged());
}


void PglGui::updateRadioButtonToggled(bool toggled)
{
   QRadioButton * radioButton = qobject_cast<QRadioButton*>(sender());
   if (! radioButton)
       return;

    QList<QRadioButton*> radioButtons;
    radioButtons.append(mUi.updateDailyRadio);
    radioButtons.append(mUi.updateWeeklyRadio);
    radioButtons.append(mUi.updateMonthlyRadio);

    foreach(QRadioButton* button, radioButtons) {
        if (radioButton != button) {
            mPglCore->option(button->objectName())->setValue(false);
            setButtonChanged(button, false);
        }
    }

    Option* option = mPglCore->option(radioButton->objectName());
    if (option) {
        option->setValue(toggled);
        setButtonChanged(radioButton, option->isChanged());
        setApplyButtonEnabled(mPglCore->isChanged());
    }
}


void PglGui::checkboxChanged(bool state)
{
    QCheckBox *button = qobject_cast<QCheckBox*>(sender());
    if (! button)
        return;

    QString senderName = sender()->objectName();
    bool changed = false;

    if (senderName.contains("startAtBoot")) {
        mPglCore->setOption("startAtBoot", state);
        changed = mPglCore->option("startAtBoot")->isChanged();
    }
    else if (senderName.contains("updateAutomatically")){
        mPglCore->setOption("updateAutomatically", state);
        changed = mPglCore->option("updateAutomatically")->isChanged();
    }
    else
        return;

    setButtonChanged(button, changed);
    setApplyButtonEnabled(mPglCore->isChanged());
}

QString PglGui::getUpdateFrequencyPath()
{
    QString path("/etc/cron.");
    QString script ("pglcmd");

    if ( mUi.updateDailyRadio->isChecked() )
        return path += "daily/" + script;
    else if ( mUi.updateWeeklyRadio->isChecked() )
        return path += "weekly/" + script;
    else if ( mUi.updateMonthlyRadio->isChecked() )
        return path += "monthly/" + script;

    return QString("");
}

QString PglGui::getUpdateFrequencyCurrentPath()
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

void PglGui::applyChanges()
{
    WhitelistManager* whitelist = mPglCore->whitelistManager();
    BlocklistManager* blocklistManager = mPglCore->blocklistManager();
    QStringList pglcmdConf;
    QString pglcmdConfPath = PglSettings::value("CMD_CONF");
    bool updatePglcmdConf = mPglCore->hasToUpdatePglcmdConf();
    bool updateBlocklistsFile = mPglCore->hasToUpdateBlocklistsFile();
    QString filepath;
    bool reload = mPglCore->hasToReloadBlocklists();

    if ( pglcmdConfPath.isEmpty() ) {
        QString errorMsg = tr("Could not determine pglcmd.conf path! Did you install pgld and pglcmd?");
        QMessageBox::warning( this, tr("Error"), errorMsg, QMessageBox::Ok);
        qWarning() << errorMsg;
        return;
    }

    //only apply IPtables commands if the daemon is running
    if ( m_Info->daemonState() )
    {
        //apply new changes directly in iptables
        QStringList iptablesCommands = whitelist->generateIptablesCommands();
        if ( ! iptablesCommands.isEmpty() )
            m_Root->addCommands(iptablesCommands);
    }

    //================ update /etc/pgl/pglcmd.conf ================/
    if ( updatePglcmdConf ) {
        pglcmdConf = mPglCore->generatePglcmdConf();

        //replace pglcmd.conf
        QString  pglcmdTempPath = QDir::temp().absoluteFilePath(getFileName(pglcmdConfPath));
        m_Root->moveFile(pglcmdTempPath, pglcmdConfPath, false);
        saveFileData(pglcmdConf, pglcmdTempPath);
    }

    //================ update /etc/pgl/blocklists.list ================/
    if ( updateBlocklistsFile ) {
        QStringList data = blocklistManager->generateBlocklistsFile();
        QString outputFilePath = QDir::temp().absoluteFilePath(getFileName(blocklistManager->blocklistsFilePath()));
        saveFileData(data, outputFilePath);

        //update the blocklists.list file
        if (QFile::exists(outputFilePath))
            m_Root->moveFile(outputFilePath, blocklistManager->blocklistsFilePath(), false);
    }

    //================ manage the local blocklists ====================/
    QDir localBlocklistDir(blocklistManager->localBlocklistsDir());
    QList<Blocklist*> localBlocklists = blocklistManager->localBlocklists();
    QDir tempDir = QDir::temp();

    foreach(Blocklist* blocklist, localBlocklists) {
        if (! blocklist->isChanged() || ! blocklist->exists())
            continue;

        filepath = blocklist->location();
        if (blocklist->isAdded()) {
            QFile::link(filepath, tempDir.absoluteFilePath(blocklist->name()));
            m_Root->moveFile(tempDir.absoluteFilePath(blocklist->name()), localBlocklistDir.absolutePath(), false);
        }
        else if (blocklist->isRemoved()) {
            m_Root->removeFile(blocklist->location(), false);
        }
        else if (blocklist->isEnabled()) {
           m_Root->moveFile(blocklist->location(), localBlocklistDir.absoluteFilePath(blocklist->name()), false);
        }
        else if (blocklist->isDisabled()) {
            m_Root->moveFile(blocklist->location(), localBlocklistDir.absoluteFilePath("."+blocklist->name()), false);
        }
    }

    //====== update  frequency radio buttons ==========/
    filepath = getUpdateFrequencyPath();
    if ( ! QFile::exists(filepath) )
        m_Root->moveFile(getUpdateFrequencyCurrentPath(), filepath, false);

    //assume changes will be applied, if not this button will be enabled afterwards
    setApplyButtonEnabled(false);

    if (reload)
        m_Root->addCommand(m_Control->getPath(), QStringList() << "reload");
    m_Root->executeAll(); //execute previously gathered commands
}

QList<QTreeWidgetItem*> PglGui::getTreeItems(QTreeWidget *tree, int checkState)
{
        QList<QTreeWidgetItem*> items;

        for (int i=0; i < tree->topLevelItemCount(); i++ )
                if ( tree->topLevelItem(i)->checkState(0) == checkState || checkState == -1)
                        items << tree->topLevelItem(i);

        return items;
}

void PglGui::blocklistItemChanged(QTreeWidgetItem* item, int column)
{
    QTreeWidget* treeWidget = item->treeWidget();
    if (! treeWidget)
        return;

    BlocklistManager* manager = mPglCore->blocklistManager();
    //Blocklist* blocklist = manager->blocklistAt(treeWidget->indexOfTopLevelItem(item));
    QVariant bl = item->data(0, Qt::UserRole);
    Blocklist* blocklist = (Blocklist*) bl.value<void*>();
    if (blocklist) {
        if (item->checkState(0) == Qt::Checked)
            blocklist->setEnabled(true);
        else
            blocklist->setEnabled(false);

        setTreeWidgetItemChanged(item, blocklist->isChanged());
        setApplyButtonEnabled(mPglCore->isChanged());
    }
}

void PglGui::whitelistItemChanged(QTreeWidgetItem* item, int column)
{
    QTreeWidget* treeWidget = item->treeWidget();
    if (! treeWidget)
        return;

    WhitelistManager* manager = mPglCore->whitelistManager();
    //WhitelistItem* whitelistItem = manager->whitelistItemAt(treeWidget->indexOfTopLevelItem(item));
    QVariant wlitem = item->data(0, Qt::UserRole);
    WhitelistItem* whitelistItem = (WhitelistItem*) wlitem.value<void*>();

    if (whitelistItem) {
        if (item->checkState(0) == Qt::Checked)
            whitelistItem->setEnabled(true);
        else
            whitelistItem->setEnabled(false);

        setTreeWidgetItemChanged(item, whitelistItem->isChanged());
        setApplyButtonEnabled(mPglCore->isChanged());
    }
}

void PglGui::treeItemPressed(QTreeWidgetItem* item, int column)
{
    if ( item->treeWidget()->objectName().contains("block", Qt::CaseInsensitive) )
    {
        mUi.removeExceptionButton->setEnabled(false);
        mUi.removeBlocklistButton->setEnabled(true);
    }
    else
    {
        mUi.removeExceptionButton->setEnabled(true);
        mUi.removeBlocklistButton->setEnabled(false);
    }
}

void PglGui::loadBlocklistWidget()
{
    BlocklistManager* blocklistManager = mPglCore->blocklistManager();

    mUi.blocklistTreeWidget->blockSignals(true);
    if (mUi.blocklistTreeWidget->topLevelItemCount())
        mUi.blocklistTreeWidget->clear();

    //get information about the blocklists being used
    foreach(Blocklist* blocklist, blocklistManager->blocklists()) {
        if (blocklist->isRemoved())
            continue;
        addBlocklistItem(blocklist, false);
    }

    mUi.blocklistTreeWidget->blockSignals(false);
}

void PglGui::addBlocklistItem(Blocklist* blocklist, bool blockSignals)
{
    if (blockSignals)
        mUi.blocklistTreeWidget->blockSignals(true);

    QStringList info;
    info << blocklist->name();
    QTreeWidgetItem * item = new QTreeWidgetItem(mUi.blocklistTreeWidget, info);
    item->setToolTip(0, blocklist->targetLocation());
    item->setData(0, Qt::UserRole, qVariantFromValue((void *) blocklist));

    if ( blocklist->isEnabled() )
        item->setCheckState(0, Qt::Checked);
    else
        item->setCheckState(0, Qt::Unchecked);
    setTreeWidgetItemChanged(item, blocklist->isChanged(), false);

    if (blockSignals)
        mUi.blocklistTreeWidget->blockSignals(false);
}

void PglGui::loadWhitelistWidget()
{
    WhitelistManager *whitelist = mPglCore->whitelistManager();

    mUi.whitelistTreeWidget->blockSignals(true);

    if ( mUi.whitelistTreeWidget->topLevelItemCount() > 0 )
        mUi.whitelistTreeWidget->clear();

    foreach(WhitelistItem * item, whitelist->whitelistItems()) {
        if (item->isRemoved())
            continue;
        addWhitelistItem(item, false);
    }

    mUi.whitelistTreeWidget->setSortingEnabled(true);
    mUi.whitelistTreeWidget->sortByColumn(0);
    mUi.whitelistTreeWidget->blockSignals(false);
}

void PglGui::addWhitelistItem(WhitelistItem* wlItem, bool blockSignals)
{
    if (blockSignals)
        mUi.whitelistTreeWidget->blockSignals(true);

    QStringList info;
    info << wlItem->value() << wlItem->connection() << wlItem->protocol();
    QTreeWidgetItem * treeItem = new QTreeWidgetItem(mUi.whitelistTreeWidget, info);
    treeItem->setData(0, Qt::UserRole, qVariantFromValue((void *) wlItem));

    if (wlItem->isEnabled())
        treeItem->setCheckState(0, Qt::Checked );
    else
        treeItem->setCheckState(0, Qt::Unchecked );

    setTreeWidgetItemChanged(treeItem, wlItem->isChanged(), false);

    if (blockSignals)
        mUi.whitelistTreeWidget->blockSignals(false);
}


void PglGui::initCore()
{
    //Intiallize all pointers to zero before creating the objects
    m_Info = 0;
    m_Root = 0;
    m_Control = 0;
    quitApp = false;

    m_ProgramSettings = new QSettings(QSettings::UserScope, "pgl", "pglgui", this);

    mPglCore = new PglCore(m_ProgramSettings);
    mPglCore->load();

    g_SetRoot();
    g_SetInfoPath();
    g_SetControlPath();
}

void PglGui::g_SetRoot( ) {

    if ( m_Root )
        delete m_Root;

    m_Root = new SuperUser(this, m_ProgramSettings->value("paths/super_user", "").toString());
}

void PglGui::g_SetInfoPath() {
    if (! m_Info)
        m_Info = new PglDaemon(PglSettings::value("DAEMON_LOG"), this);
}

void PglGui::g_SetControlPath()
{
    QString  gSudo = m_ProgramSettings->value("paths/super_user").toString();
    m_Control = new PglCmd(this, PglSettings::value("CMD_PATHNAME"), gSudo);

}

void PglGui::g_ShowAddDialog(int openmode) {
    AddExceptionDialog *dialog = 0;
    bool newItems = false;

    if ( openmode == (ADD_MODE | EXCEPTION_MODE) )
    {
        dialog = new AddExceptionDialog( this, openmode, mPglCore);
        dialog->exec();

        WhitelistManager* whitelist = mPglCore->whitelistManager();

        foreach(WhitelistItem whiteItem, dialog->getItems()) {
            QStringList info; info << whiteItem.value() << whiteItem.connection() << whiteItem.protocol();
            if (whitelist->contains(whiteItem)) {
                WhitelistItem *wlitem = whitelist->item(whiteItem);
                wlitem->setEnabled(true);
                if (wlitem->isRemoved()) {
                    wlitem->setRemoved(false);
                    addWhitelistItem(wlitem);
                }
            }
            else
                whitelist->addItem(whiteItem.value(), whiteItem.connection(), whiteItem.protocol());

            newItems = true;
        }

        mUi.whitelistTreeWidget->scrollToBottom();
        }
    else if ( openmode == (ADD_MODE | BLOCKLIST_MODE) )
    {
        dialog = new AddExceptionDialog( this, openmode, mPglCore);
        dialog->exec();
        BlocklistManager* blocklistManager = mPglCore->blocklistManager();

        foreach(const QString& blocklist, dialog->getBlocklists()) {
            if (blocklistManager->contains(blocklist)) {
                Blocklist *bl = blocklistManager->blocklist(blocklist);
                bl->setEnabled(true);
                if (bl->isRemoved()) {
                    bl->setRemoved(false);
                    addBlocklistItem(bl);
                }
            }
            else
                blocklistManager->addBlocklist(blocklist);
            newItems = true;
        }

        mUi.blocklistTreeWidget->scrollToBottom();
    }

    if ( newItems ) {
        setApplyButtonEnabled(mPglCore->isChanged());
    }

    if ( dialog )
        delete dialog;
}

void PglGui::g_MakeTray()
{
        m_Tray = new QSystemTrayIcon( mTrayIconDisabled );
        m_Tray->setVisible( true );
    m_Tray->setToolTip(tr("Pgld is not running"));
}

void PglGui::onDaemonChanged(bool running)
{
    if ( running ) {
        mUi.controlPglButtons->setCurrentIndex(1);
        m_Tray->setIcon(mTrayIconEnabled);
        setWindowIcon(mTrayIconEnabled);
        setWindowTitle(QString("%1 - %2").arg(DEFAULT_WINDOW_TITLE).arg(m_Info->loadedRanges()));
        m_Tray->setToolTip(tr("Pgld is up and running"));
    }
    else {
        mUi.controlPglButtons->setCurrentIndex(0);
        m_Tray->setIcon(mTrayIconDisabled);
        setWindowIcon(mTrayIconDisabled);
        setWindowTitle(DEFAULT_WINDOW_TITLE);
        m_Tray->setToolTip(tr("Pgld is not running"));
    }
}

void PglGui::checkDaemonStatus()
{
    this->onDaemonChanged(m_Info->daemonState());
}

void PglGui::g_MakeMenus()
{
    //tray icon menu
        m_TrayMenu = new QMenu(this);
    m_TrayMenu->addAction( mUi.a_Start );
    m_TrayMenu->addAction( mUi.a_Stop );
    m_TrayMenu->addAction( mUi.a_Restart );
    m_TrayMenu->addAction( mUi.a_Reload );
        m_TrayMenu->addSeparator();
    m_TrayMenu->addAction( mUi.a_Exit );
        m_Tray->setContextMenu(m_TrayMenu);
}

void PglGui::g_ShowAboutDialog()
{

    QString message;
    message += QString("<b><i>Peerguardian Linux version %1</b><br>A Graphical User Interface for Peerguardian Linux<br><br>").arg( VERSION_NUMBER );
    message += "Copyright (C) 2007-2008 Dimitris Palyvos-Giannas<br>";
    message += "Copyright (C) 2011-2013 Carlos Pais <br><br>";
    message += "pgl is licensed under the GNU General Public License v3, or (at\
                your option) any later version. This program comes with\
                ABSOLUTELY NO WARRANTY. This is free software, and you are\
                welcome to modify and/or redistribute it.<br><br><font size=2>";
    message +="Using modified version of the crystal icon theme:<br>http://www.everaldo.com/<br>http://www.yellowicon.com/<br><br>";
    message += "Credits go to Morpheus, jre, TheBlackSun, Pepsi_One and siofwolves from phoenixlabs.org for their help and suggestions. <br>";
    message += "I would also like to thank Art_Fowl from e-pcmag.gr for providing valuable help with Qt4 and for helping me with the project's development. <br>";
    message += "Special credit goes to Evangelos Foutras for developing the old project's website, <a href=http://mobloquer.sourceforge.net>mobloquer.foutrelis.com</a></font></i>";

    QMessageBox::about( this, tr( "About Peerguardian Linux GUI" ), tr( message.toUtf8() ));

}

void PglGui::undoAll()
{
    int answer = 0;

    answer = confirm(tr("Really Undo?"), tr("Are you sure you want to ignore the unsaved changes?"), this);

    if ( answer == QMessageBox::Yes) {
        mPglCore->undo();
        loadGUI();
        setApplyButtonEnabled(false);
    }
}

void PglGui::startStopLogging()
{
    m_StopLogging = ! m_StopLogging;

    QPushButton *button = qobject_cast<QPushButton*> (sender());

    if ( m_StopLogging )
    {
        button->setIcon(QIcon(ENABLED_ICON));
        button->setText(tr("Start Logging"));
    }
    else
    {
        button->setIcon(QIcon(DISABLED_ICON));
        button->setText(tr("Stop Logging"));
    }
}

void PglGui::openSettingsDialog()
{
    SettingsDialog * dialog = new SettingsDialog(m_ProgramSettings, this);

    int exitCode = dialog->exec();

    if ( exitCode )
    {
        m_ProgramSettings->setValue("paths/super_user", dialog->file_GetRootPath());
        SuperUser::setSudoCommand(m_ProgramSettings->value("paths/super_user", SuperUser::sudoCommand()).toString());
        m_MaxLogSize = dialog->getMaxLogEntries();
        m_ProgramSettings->setValue("maximum_log_entries", QString::number(m_MaxLogSize));
    }

    if ( dialog )
        delete dialog;
}

void PglGui::showLogRightClickMenu(const QPoint& p)
{
    QTreeWidgetItem * item = mUi.logTreeWidget->itemAt(p);

    if ( ! item )
        return;

    QMenu menu(this);
    QMenu *menuIp;
    QMenu *menuPort;
    int index = 4;

    if ( item->text(7) == "Incoming" )
        index = 2;

    QVariantMap data;
    data.insert("ip", item->text(index));
    data.insert("port", item->text(5));
    data.insert("prot", item->text(6));
    data.insert("type", item->text(7));

    a_whitelistIpTemp->setData(data);
    a_whitelistIpPerm->setData(data);
    a_whitelistPortTemp->setData(data);
    a_whitelistPortPerm->setData(data);

    menuIp =  menu.addMenu("Allow IP " + item->text(index));
    menuPort = menu.addMenu("Allow Port " + item->text(5));

    menu.addSeparator();
    aWhoisIp->setData(item->text(index));
    aWhoisIp->setText(tr("Whois ") + item->text(index));
    menu.addAction(aWhoisIp);

    menuIp->addAction(a_whitelistIpTemp);
    menuIp->addAction(a_whitelistIpPerm);
    menuPort->addAction(a_whitelistPortTemp);
    menuPort->addAction(a_whitelistPortPerm);

    menu.exec(mUi.logTreeWidget->mapToGlobal(p));
}

void PglGui::whitelistItem()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (! action)
        return;

    if (! m_Info->daemonState()) {
        QMessageBox::information(this, tr("Peerguardian is not running"), tr("It's not possible to whitelist while Peerguardian is not running."));
        return;
    }

    WhitelistManager* whitelist = mPglCore->whitelistManager();
    QVariantMap data = action->data().toMap();
    QString ip = data.value("ip").toString();
    QString port = data.value("port").toString();
    QString type = data.value("type").toString();
    QString prot = data.value("prot").toString();
    QString value = "";
    if (action == a_whitelistIpPerm || action == a_whitelistIpTemp)
        value = ip;
    else
        value = port;

    if ( action == a_whitelistIpTemp || action ==  a_whitelistPortTemp )
    {
        QStringList iptablesCommands = whitelist->getCommands(QStringList() << value, QStringList() << type, QStringList() << prot, QList<bool>() << true);
        //QString testCommand = whitelist->getIptablesTestCommand(ip, type, prot);
        m_Root->addCommands(iptablesCommands);
        m_Root->executeAll();
    }
    else if (  action == a_whitelistIpPerm || action == a_whitelistPortPerm )
    {
        if ( ! whitelist->contains(value, type, prot) )
        {
            whitelist->addItem(new WhitelistItem(value, type, prot));
            applyChanges();
        }
    }
}

void PglGui::onViewerWidgetRequested()
{
    QString path("");

    if ( mUi.viewPglcmdLogAction == sender() ) {
        path = PglSettings::value("CMD_LOG");
    }
    else if (mUi.viewPgldLogAction == sender()) {
        path = PglSettings::value("DAEMON_LOG");
    }

    ViewerWidget viewer(path);
    viewer.exec();
}

bool PglGui::eventFilter(QObject* obj, QEvent* event)
{
    //if (obj == mUi.logTreeWidget->verticalScrollBar() && mIgnoreScroll)
    //    return true;

    if (obj == mUi.logTreeWidget->verticalScrollBar() && event->type() == QEvent::Wheel) {

        if (mUi.logTreeWidget->verticalScrollBar()->value() == mUi.logTreeWidget->verticalScrollBar()->maximum())
            mAutomaticScroll = true;
        else
            mAutomaticScroll = false;
    }

    return false;
}

void PglGui::onLogViewVerticalScrollbarMoved(int value)
{
    QScrollBar *bar = static_cast<QScrollBar*>(sender());

    if (bar->maximum() == value)
        mAutomaticScroll = true;
    else
        mAutomaticScroll = false;
}

void PglGui::onLogViewVerticalScrollbarActionTriggered(int action)
{
    QScrollBar *scrollBar = static_cast<QScrollBar*>(sender());

    if (mAutomaticScroll && scrollBar->value() < scrollBar->maximum())
        scrollBar->setSliderPosition(scrollBar->maximum());

}

void PglGui::showCommandsOutput(const CommandList& commands)
{

    QString output("");
    QString title("");
    foreach(const Command& command, commands) {
        output += command.output();
        output += "\n";

        if (! title.isEmpty())
            title += tr(" and ");
        title += command.command();
    }

    ViewerWidget viewer(output);
    viewer.setWindowTitle(title);
    viewer.exec();
}

void PglGui::onWhoisTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (! action)
        return;
    ProcessT *process = new ProcessT(this);
    connect(process, SIGNAL(finished(const CommandList&)), this, SLOT(showCommandsOutput(const CommandList&)));
    if (action->text().contains(" "))
        process->execute("whois", QStringList() << action->data().toString());
}

void PglGui::showAddBlocklistDialog()
{
    g_ShowAddDialog(ADD_MODE | BLOCKLIST_MODE);
}

void PglGui::showAddExceptionDialog()
{
    g_ShowAddDialog(ADD_MODE | EXCEPTION_MODE);
}

void PglGui::setApplyButtonEnabled(bool enable)
{
    mUi.applyButton->setEnabled(enable);
    mUi.undoButton->setEnabled(enable);
}

void PglGui::setButtonChanged(QAbstractButton* button, bool changed)
{
    if (changed) {
        button->setIcon(QIcon(WARNING_ICON));
        button->setStatusTip(tr("You need to click the Apply button so the changes take effect"));
    }
    else {
        button->setIcon(QIcon());
        button->setStatusTip("");
    }
}

void PglGui::setTreeWidgetItemChanged(QTreeWidgetItem* item, bool changed, bool blockSignals)
{
    QTreeWidget* treeWidget = item->treeWidget();
    if (! treeWidget)
        return;

    if (blockSignals)
        treeWidget->blockSignals(true);

    if (changed) {
        item->setIcon(0, QIcon(WARNING_ICON));
        item->setStatusTip(0, tr("You need to click the Apply button so the changes take effect"));
    }
    else {
        item->setIcon(0, QIcon());
        item->setStatusTip(0, "");
    }

    if (blockSignals)
        treeWidget->blockSignals(false);
}
