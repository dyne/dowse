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


#include "whitelist_manager.h"
#include "utils.h"
#include "pgl_settings.h"
#include "file_transactions.h"

WhitelistManager::WhitelistManager(QSettings* settings, QObject* parent) :
    QObject(parent)
{
    m_WhitelistFile = PglSettings::value("CMD_CONF");
    m_Settings = settings;

    //if ( m_WhitelistFile.isEmpty() )
    //    return;

    m_Group[WHITE_IP_IN] = TYPE_INCOMING;
    m_Group[WHITE_IP_OUT] = TYPE_OUTGOING;
    m_Group[WHITE_IP_FWD] = TYPE_FORWARD;
    m_Group[WHITE_TCP_IN] = TYPE_INCOMING;
    m_Group[WHITE_UDP_IN] = TYPE_INCOMING;
    m_Group[WHITE_TCP_OUT] = TYPE_OUTGOING;
    m_Group[WHITE_UDP_OUT] = TYPE_OUTGOING;
    m_Group[WHITE_TCP_FWD] = TYPE_FORWARD;
    m_Group[WHITE_UDP_FWD] = TYPE_FORWARD;

    loadSystemPorts();
}

WhitelistManager::~WhitelistManager()
{
    foreach(WhitelistItem* item, mWhitelistItems)
        if (item)
            delete item;
    mWhitelistItems.clear();
}

void WhitelistManager::load()
{
    QStringList fileData = getFileData(m_WhitelistFile);
    QStringList values;
    QString key, skey, value;

     this->blockSignals(true);

    if (! mWhitelistItems.isEmpty()) {
        foreach(WhitelistItem* item, mWhitelistItems)
            if (item)
                delete item;
        mWhitelistItems.clear();
    }

    foreach(const QString& line, fileData)
    {
        if( line.startsWith("#") )
            continue;

        key = getVariable(line);
        if (m_Group.contains(key)) {
            values = getValue(line).split(" ", QString::SkipEmptyParts);
            foreach(const QString& value, values) {
                addItem(value, parseConnectionType(key), parseProtocol(key), true);
            }
        }
    }

    //Since disabled whitelisted items (IPs or Ports) can't be easily stored
    //in /etc/plg/pglcmd.conf, the best option is to store them on the GUI settings file

    foreach (key, m_Group.keys() ) {
        skey = QString("whitelist/%1").arg(key);
        if (m_Settings->contains(skey)) {
            value = m_Settings->value(skey).toString();
            values = value.split(" ", QString::SkipEmptyParts);
            foreach(const QString& value, values) {
                addItem(value, parseConnectionType(key), parseProtocol(key), true, false);
            }
        }
    }

    this->blockSignals(false);
}

QString WhitelistManager::parseProtocol(const QString& key)
{
    QRegExp keyPattern("^[a-zA-Z]+_[a-zA-Z]+_[a-zA-Z]+$");
    if (keyPattern.exactMatch(key))
        return key.split("_")[1];
    return key;
}

QString WhitelistManager::parseConnectionType(const QString& key)
{
    QRegExp keyPattern("^[a-zA-Z]+_[a-zA-Z]+_[a-zA-Z]+$");
    if (keyPattern.exactMatch(key))
        return getTypeAsString(key);
    return key;
}


QString WhitelistManager::getTypeAsString(const QString& key)
{
    switch(m_Group[key])
    {
        case TYPE_INCOMING: return QString("Incoming");
        case TYPE_OUTGOING: return QString("Outgoing");
        case TYPE_FORWARD: return QString("Forward");
        default: return QString("");
    }
}

QString WhitelistManager::getGroup(QStringList& info)
{
    //info should contain the value (a port or an ip address) and the connection type (in, out or fwd)
    if ( info.size() != 3 )
        return "";

    QMap<QString, QString> connection;
    connection["Incoming"] = "IN";
    connection["Outgoing"] = "OUT";
    connection["Forward"] = "FWD";

    QString value = info[0];
    QString conn_type = info[1];
    QString protocol = info[2];
    QString key = "WHITE_";

    if ( isValidIp(value) )
        key += "IP_";
    else
        key += protocol + "_";

    key += connection[conn_type];

    return key;

}

QStringList WhitelistManager::updateWhitelistFile()
{

    QStringList fileData = getFileData(m_WhitelistFile);
    QString value;

    foreach(QString key, m_WhitelistEnabled.keys())
    {
        value = m_WhitelistEnabled[key].join(" ");

        if ( hasValueInData(key, fileData) || value != PglSettings::value(key) )
            setValueInData(fileData, key, m_WhitelistEnabled[key].join(" "));
    }

    return fileData;
}

bool WhitelistManager::isChanged()
{
    foreach(WhitelistItem* item, mWhitelistItems)
        if (item && item->isChanged())
            return true;
    return false;
}

void WhitelistManager::updatePglSettings()
{
    QStringList info;
    QString group;

    foreach(WhitelistItem* item, mWhitelistItems) {
        if (item->isChanged()) {
            info << item->value() << item->connection() << item->protocol();
            group = getGroup(info);

            if (item->isDisabled() || item->isRemoved()) {
                PglSettings::remove(group, item->value());
            }
            else if (item->isEnabled()) {
                PglSettings::add(group, item->value());
            }

            info.clear();
        }
    }
}

void WhitelistManager::updateGuiSettings()
{
    QString key, group;
    QStringList info, values;

    foreach(WhitelistItem* item, mWhitelistItems) {
        if (item->isChanged()) {
            info << item->value() << item->connection() << item->protocol();
            group = getGroup(info);
            key = QString("whitelist/%1").arg(group);
            if (m_Settings->contains(key))
                values = m_Settings->value(key).toString().split(" ", QString::SkipEmptyParts);

            if (item->isEnabled() || item->isRemoved()) {
                if (values.contains(item->value())) {
                    qDebug() << "Removing" << item->value() << "from pglgui settings";
                    values.removeAll(item->value());
                }
            }
            else if (item->isDisabled() && ! values.contains(item->value())) {
                qDebug() << "Adding" << item->value() << "to pglgui settings";
                values += item->value();
            }

            if (values.isEmpty())
                m_Settings->remove(key);
            else
                m_Settings->setValue(key, values.join(" "));
            values.clear();
            info.clear();
        }
    }
}

QString WhitelistManager::translateConnection(const QString& conn_type)
{

    QString conn(conn_type.toUpper());

    if ( conn == "INCOMING")
        return "IN";
    else if ( conn == "OUTGOING" )
        return "OUT";
    else if ( conn == "FORWARD" )
        return "FWD";

    return conn;

}

QStringList WhitelistManager::getDirections(const QString& chain)
{
    QStringList directions;

    if ( chain == "IN" )
        directions << QString("--source");
    else if ( chain == "OUT")
        directions << QString("--destination");
    else if ( chain == "FWD" )
        directions << QString("--source") << QString("--destination");

    return directions;
}

QStringList WhitelistManager::generateIptablesCommands()
{
    QStringList values, connections, protocols;
    QList<bool> allows;

    foreach(WhitelistItem* item, mWhitelistItems) {

        if (item->isChanged()) {
            values << item->value();
            connections << item->connection();
            protocols << item->protocol();
            if (item->isRemoved())
                allows << false;
            else
                allows << item->isEnabled();
        }
    }

    return getCommands(values, connections, protocols, allows);
}

QStringList WhitelistManager::getCommands( QStringList items, QStringList connections, QStringList protocols, QList<bool> allows)
{
    QStringList commands;
    QString option;
    QString command_operator;
    QString ip_source_check, ip_destination_check, port_check;
    QString command_type("");
    QString command, iptables_list_type("iptables -L $IPTABLES_CHAIN -n | ");
    QString chain, item, connection, protocol, conn, iptables_list, checkCmd, notOp;
    QStringList directions;
    QString portNum("0");
    bool ip;
    QString iptables_target_whitelisting = PglSettings::value("IPTABLES_TARGET_WHITELISTING");
    QString ip_source_check_type = QString("grep -x \'%1 *all *-- *$IP *0.0.0.0/0 *\'").arg(iptables_target_whitelisting);
    QString ip_destination_check_type = QString("grep -x \'%1 *all *-- *0.0.0.0/0 *$IP *\'").arg(iptables_target_whitelisting);
    QString port_check_type = QString("grep -x \'%1 *$PROTO *-- *0.0.0.0/0 *0.0.0.0/0 *$PROTO dpt:$PORT *\'").arg(iptables_target_whitelisting);

    for (int i=0; i < items.size(); i++ )
    {
        item = items[i];
        connection = connections[i];
        protocol = protocols[i];
        iptables_list = iptables_list_type;
        int _port = 0;

        ip = isValidIp(item);

        if (! ip ) {
          _port = portNumber(item);
          if (_port != -1)
            portNum = QString::number(_port);
          else
            portNum = item;
        }

        if ( ip )
        {
            command_type = " $COMMAND_OPERATOR iptables $OPTION $IPTABLES_CHAIN $FROM $IP -j " + iptables_target_whitelisting;
            // NOTE jre: IN uses source, OUT uses destination, and FWD uses both.
            // So for IN and OUT we have excess functions set here:
            ip_source_check = ip_source_check_type;
            ip_destination_check = ip_destination_check_type;
            ip_source_check.replace("$IP", item);
            ip_destination_check.replace("$IP", item);
        }
        else
        {
            command_type = " $COMMAND_OPERATOR iptables $OPTION $IPTABLES_CHAIN -p $PROT --dport $PORT -j " + iptables_target_whitelisting;
            port_check = port_check_type;
            port_check.replace("$PROTO", protocol.toLower());
            port_check.replace("$PORT", portNum);
        }

        if ( allows[i] )
        {
            option = "-I";
            command_operator = "||";
            notOp = "";
        }
        else
        {
            option = "-D";
            command_operator = "||";
            notOp = " !";
        }

        //convert incoming to in, outgoing to out, etc
        conn = translateConnection(connection);
        //get iptables chain name from pglcmd.defaults
        chain = PglSettings::value("IPTABLES_" + conn);

        iptables_list.replace("$IPTABLES_CHAIN", chain);

        //FWD needs --source and --destination
        if ( ip )
            directions = getDirections(conn);
        else
        {
            protocol = protocol.toLower();
            if ( protocol != "tcp" && protocol != "udp")
                continue;
        }

        if ( ip )
        {
            foreach(const QString& direction, directions )
            {
                command = command_type;
                command.replace("$OPTION", option);
                command.replace("$COMMAND_OPERATOR", command_operator);
                command.replace("$IPTABLES_CHAIN", chain);
                command.replace("$FROM", direction);
                command.replace("$IP ", item + " ");

                if ( direction == "--source" )
                    commands << iptables_list + ip_source_check + command;
                else if ( direction == "--destination")
                    commands << notOp + '(' + iptables_list + ip_destination_check + ')' + command;

            }
        }
        else
        {
            command = command_type;
            command.replace("$OPTION", option);
            command.replace("$COMMAND_OPERATOR", command_operator);
            command.replace("$IPTABLES_CHAIN", chain);
            command.replace("$PROT", protocol);
            command.replace("$PORT", portNum);
            commands <<  notOp + '(' + iptables_list + port_check + ')' + command;
        }
    }

    return commands;
}

bool WhitelistManager::isPortAdded(const QString& value, const QString & portRange)
{
    bool ok = false;

    if ( portRange.contains(":") )
    {
        int part1 = portRange.split(":")[0].toInt(&ok);
        if ( ! ok ) return false;
        int part2 = portRange.split(":")[1].toInt(&ok);
        if ( ! ok ) return false;
        int val = value.toInt(&ok);
        if ( ! ok ) return false;

        if ( val >= part1 && val <= part2 )
            return true;
    }

    return false;
}

QString WhitelistManager::getIptablesTestCommand(const QString& value, const QString& connectType, const QString& prot)
{
    QString cmd("");
    QString target = PglSettings::value("IPTABLES_TARGET_WHITELISTING");
    QString chain;
    QString portNum = value;
    int _port = 0;

    chain = translateConnection(connectType);
    chain = PglSettings::value("IPTABLES_" + chain);

    if ( isValidIp(value) )
    {
        cmd = QString("iptables -L \"$CHAIN\" -n | grep \"$TARGET\" | grep -F \"$VALUE\"");
    }
    else
    {
        cmd = QString("iptables -L \"$CHAIN\" -n | grep \"$TARGET\" | grep \"$PROT dpt:$VALUE\"");
        cmd.replace("$PROT", prot);
        _port = portNumber(value);
        if (_port != -1)
          portNum = QString::number(_port);
    }

    cmd.replace("$CHAIN", chain);
    cmd.replace("$TARGET", target);
    cmd.replace("$VALUE", portNum);

    return cmd;
}

WhitelistItem* WhitelistManager::item(const WhitelistItem& other)
{
    foreach(WhitelistItem* item, mWhitelistItems)
        if (*item == other)
            return item;
    return 0;
}

bool WhitelistManager::contains(const WhitelistItem & other)
{
    if (item(other))
        return  true;
    return false;
}

bool WhitelistManager::contains(const QString& value, const QString& connectType, const QString& prot)
{
    return contains(WhitelistItem(value, connectType, prot));
}

bool WhitelistManager::isValid(const WhitelistItem & other, QString & reason)
{
    WhitelistItem* item = this->item(other);
    if (item && item->isEnabled() && ! item->isRemoved()) {
        reason = QObject::tr("It's already added");
        return false;
    }

    QHashIterator<int, Port> it(mSystemPorts);

    while (it.hasNext()) {
        it.next();
        Port port = (Port) it.value();
        if ( port.containsName(other.value()) && ! port.hasProtocol(other.protocol()) ) {
            reason += other.value();
            reason += QObject::tr(" doesn't work over ") + other.protocol();
            return false;
        }
    }

    return true;
}

bool WhitelistManager::isValid(const QString& value, const QString& connectType, const QString& prot, QString& reason)
{
    return isValid(WhitelistItem(value, connectType, prot), reason);
}

QList<WhitelistItem*> WhitelistManager::whitelistItems()
{
    return mWhitelistItems;
}

WhitelistItem* WhitelistManager::itemAt(int index)
{
    if (index >= 0 && index < mWhitelistItems.size())
        return mWhitelistItems[index];
    return 0;
}

void WhitelistManager::addItem(WhitelistItem * item)
{
    //fetch aliases
    QHashIterator<int, Port> it(mSystemPorts);
    while(it.hasNext()) {
        it.next();
        Port port = (Port) it.value();
        if (port.containsName(item->value())) {
            item->addAliases(port.names());
            break;
        }
    }

    mWhitelistItems.append(item);
    emit itemAdded(item);
}

void WhitelistManager::addItem(const QString& value, const QString& conntype, const QString& prot, bool active, bool enabled)
{
    addItem(new WhitelistItem(value, conntype, prot, active, enabled));
}

void WhitelistManager::removeItemAt(int index)
{
    if (index >= 0 && index < mWhitelistItems.size())
        mWhitelistItems[index]->setRemoved(true);
}

void WhitelistManager::removeItem(WhitelistItem* item)
{
    if (! item)
        return;

    if (item->isAdded()) {
        mWhitelistItems.removeAll(item);
        delete item;
    }
    else {
        item->setRemoved(true);
    }
}

void WhitelistManager::undo()
{
    WhitelistItem* item;

    for(int i=mWhitelistItems.size()-1; i >= 0; i--) {
        item = mWhitelistItems[i];
        if (item->isChanged()) {
            if (item->isAdded()) {
                mWhitelistItems.takeAt(i);
                delete item;
            }
            else
                item->undo();
        }
    }
}

QHash<int, Port> WhitelistManager::systemPorts()
{
    return mSystemPorts;
}

QHash<QString, int> WhitelistManager::systemPortsNameToNumber()
{
  QHash<QString, int> ports;
  QHashIterator<int, Port> it(mSystemPorts);
  Port port;

  while(it.hasNext()) {
      it.next();
      port = (Port) it.value();
      foreach(const QString& name, port.names())
        if (! isNumber(name))
          ports[name] = port.number();
  }

  return ports;
}

int WhitelistManager::portNumber(const QString& name)
{
  if (isNumber(name))
    return name.toInt();

  QHashIterator<int, Port> it(mSystemPorts);
  while(it.hasNext()) {
      it.next();
      Port p = (Port) it.value();
      if (p.containsName(name))
          return p.number();
  }

  return -1;
}

void WhitelistManager::loadSystemPorts()
{
    if (! mSystemPorts.isEmpty())
        mSystemPorts.clear();

    QStringList fileData = getFileData("/etc/services");
    Port port, _port;

    mSystemPorts.contains(0);

    for ( int i=0; i < fileData.size(); i++ )
    {
        port = parsePort(fileData[i]);
        if (port.number() == 0)
            continue;

        if (mSystemPorts.contains(port.number())) {
            _port = (Port) mSystemPorts.value(port.number());
            _port.addProtocols(port.protocols());
            _port.addNames(port.names());
            mSystemPorts.insert(_port.number(), _port);
        }
        else {
            mSystemPorts.insert(port.number(), port);
        }
    }
}

Port WhitelistManager::parsePort(QString line)
{
    QStringList elements;
    int portNum;
    QString protocol;
    Port port;

    line = line.simplified();

    if ( line.isEmpty() || line.startsWith("#") )
        return Port();

    elements = line.split(" ");

    portNum = elements[1].split("/")[0].toInt();
    protocol = elements[1].split("/")[1];

    port = Port(elements[0], protocol, portNum);

    if ( elements.size() >= 3 && ( ! elements[2].startsWith("#")) )
        port.addName(elements[2]);

    return port;
}

bool WhitelistManager::isPort(const QString & p)
{
    if ( p.contains(":") ) //port range
    {
        QStringList ports = p.split(":");

        if ( ports.size() > 2 )
            return false;

        foreach(QString port, ports)
            if (! isNumber(port))
                return false;

        return true;
    }

    if (isNumber(p))
      return true;

    if (portNumber(p.toLower()) != -1)
      return true;

    return false;
}
