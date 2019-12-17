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


#include "option.h"
#include "option_p.h"

OptionPrivate::OptionPrivate()
{
}

OptionPrivate::~OptionPrivate()
{
}

bool OptionPrivate::operator ==(const OptionPrivate& other)
{
    if (this->name == other.name && this->value == other.value && this->enabled == other.enabled && this->removed == other.removed)
        return true;
    return false;
}

Option::Option(const QString& name, const QVariant& value, bool active) :
    d_active_ptr(new OptionPrivate),
    d_ptr(new OptionPrivate)
{
    setEnabled(false);
    setRemoved(false);
    setName(name);
    setValue(value);
    if (active)
        *d_active_ptr = *d_ptr;
}

Option::Option()
{
    d_active_ptr = 0;
    d_ptr = 0;
}

Option::Option(const Option& other)
{
    d_active_ptr = 0;
    d_ptr = 0;
    *(this) = other;
}

Option::~Option()
{
    if (d_active_ptr)
        delete d_active_ptr;
    if(d_ptr)
        delete d_ptr;

    d_active_ptr = 0;
    d_ptr = 0;
}

bool Option::isActive() const
{
    if (d_ptr && d_active_ptr)
        return (*d_active_ptr == *d_ptr);
    return false;
}

bool Option::isChanged() const
{
    if (d_ptr)
        return ! isActive();
    return false;
}

QString Option::name() const
{
    if (d_ptr)
        return d_ptr->name;
    return "";
}

void Option::setName(const QString& name)
{
    if (d_ptr)
        d_ptr->name = name;
}

QVariant Option::value() const
{
    if (d_ptr)
        return d_ptr->value;
    return "";
}

void Option::setValue(const QVariant& value)
{
    d_ptr->value = value;
    if (value.type() == QVariant::Bool)
        setEnabled(value.toBool());
}

bool Option::isEnabled() const
{
    if (d_ptr)
        return d_ptr->enabled;
    return false;
}

bool Option::isDisabled() const
{
    if (d_ptr)
        return ! d_ptr->enabled;
    return false;
}

void Option::setEnabled(bool enabled)
{
    d_ptr->enabled = enabled;
    if (value().type() == QVariant::Bool)
        d_ptr->value = enabled;
}

void Option::applyChanges()
{
    if (d_ptr && d_active_ptr)
        *d_active_ptr = *d_ptr;
}

void Option::undo()
{
    if (d_ptr && d_active_ptr)
        *d_ptr = *d_active_ptr;
}

bool Option::isAdded() const
{
    if (! d_active_ptr)
        return true;
    return (d_active_ptr->name.isEmpty() && d_active_ptr->value.isNull());
}

bool Option::isRemoved() const
{
    return d_ptr->removed;
}

void Option::setRemoved(bool removed)
{
    if (d_ptr)
        d_ptr->removed = removed;
}

void Option::setActiveData(OptionPrivate * other_ptr)
{
    if (d_active_ptr && d_active_ptr != other_ptr)
        delete d_active_ptr;
    d_active_ptr = other_ptr;
}

void Option::setData(OptionPrivate * other_ptr)
{
    if (d_ptr && d_ptr != other_ptr)
        delete d_ptr;
    d_ptr = other_ptr;
}

bool Option::operator==(const Option& other)
{
    if (d_ptr == other.d_ptr)
        return true;
    if (d_ptr && other.d_ptr)
        return (*d_ptr == *(other.d_ptr));
    return false;
}

Option& Option::operator=(const Option& other)
{
    if (other.d_ptr) {
        if (! d_ptr)
            d_ptr = new OptionPrivate();
        *d_ptr = *(other.d_ptr);
    }

    if (other.d_active_ptr) {
        if (! d_active_ptr)
            d_active_ptr = new OptionPrivate();
        *d_active_ptr = *(other.d_active_ptr);
    }

    return *this;
}
