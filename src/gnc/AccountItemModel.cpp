/*
 * AccountItemModel.hpp
 * Copyright (C) 2010 Christian Stimming
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, contact:
 *
 * Free Software Foundation           Voice:  +1-617-542-5942
 * 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652
 * Boston, MA  02110-1301,  USA       gnu@gnu.org
 */

#include "AccountItemModel.hpp"

#include "engine/gnc-event.h" // for GNC_EVENT_ITEM_ADDED
#include "gncmm/Numeric.hpp"
#include <QDebug>

namespace gnc
{

AccountTreeModel::AccountTreeModel(Glib::RefPtr<Account> rootaccount, QObject *parent)
        : QAbstractItemModel(parent)
        , m_root(rootaccount)
{
}

QModelIndex AccountTreeModel::index(int row, int column,
                                    const QModelIndex &parent) const
{
    //qDebug() << "index(), " << row << column << parent;
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    Glib::RefPtr<Account> parentItem;

    if (!parent.isValid())
        parentItem = m_root;
    else
        parentItem = Glib::wrap(static_cast< ::Account*>(parent.internalPointer()));

    Glib::RefPtr<Account> childItem = parentItem->get_nth_child(row);
    if (childItem)
    {
        //qDebug() << "returning" << childItem.getName();
        return createIndex(row, column, childItem->gobj());
    }
    else
        return QModelIndex();
}

QModelIndex AccountTreeModel::parent(const QModelIndex &index) const
{
    //qDebug() << "parent()" << index;
    if (!index.isValid())
        return QModelIndex();

    Glib::RefPtr<Account> childItem = Glib::wrap(static_cast< ::Account*>(index.internalPointer()));
    Glib::RefPtr<Account> parentItem(childItem->get_parent());

    if (parentItem->gobj() == m_root->gobj())
        return QModelIndex();

    return createIndex(parentItem->child_index(), 0, parentItem->gobj());
}

int AccountTreeModel::rowCount(const QModelIndex& parent) const
{
    //qDebug() << "rowCount()" << parent;
    if (parent.column() > 0)
        return 0;
    // FIXME: Doesn't this just mean the nonzero columns don't have a
    // tree? In that case it would be correct.

    Glib::RefPtr<Account> parentItem;
    if (!parent.isValid())
        parentItem = m_root;
    else
        parentItem = Glib::wrap(static_cast< ::Account*>(parent.internalPointer()));

    //qDebug() << "Returning " << parentItem.n_children();
    return parentItem->get_num_children();
}

int AccountTreeModel::columnCount(const QModelIndex& parent) const
{
    //qDebug() << "columnCount()" << parent;
//     if (!parent.isValid())
//         return 0;
//     else
    return 4; // Fixed number for now
}

QVariant AccountTreeModel::data(const QModelIndex& index, int role) const
{
    //qDebug() << "data(), " << index;
    if (!index.isValid())
        return QVariant();

    Glib::RefPtr<Account> account = Glib::wrap(static_cast< ::Account*>(index.internalPointer()));
    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return g2q(account->get_name());
        case 1:
            return g2q(account->get_code());
        case 2:
            return g2q(account->get_description());
        case 3:
        {
            Numeric balance = gnc_ui_account_get_balance(account->gobj(), false);
            PrintAmountInfo printInfo(account, true);
            return g2q(balance.printAmount(printInfo));
        }
        default:
            return QVariant();
        }
    }
    else
        return QVariant();
}

Qt::ItemFlags AccountTreeModel::flags(const QModelIndex &index) const
{
    //qDebug() << "flags()" << index;
    if (!index.isValid())
        return 0;

    // Ensure read-only access only
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant AccountTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug() << "headerData()" << section;
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return QString("Name");
        case 1:
            return QString("Code");
        case 2:
            return QString("Description");
        case 3:
            return QString("Balance");
        default:
            return QVariant();
        }
    }
    else
        return QString("%1").arg(1 + section);
}

// ////////////////////////////////////////////////////////////


AccountListModel::AccountListModel(Glib::RefPtr<Account> rootaccount, QObject *parent)
        : base_class(rootaccount, parent)
        , m_list()
        , m_eventWrapperAccount(*this, &AccountListModel::accountEvent)
{
    recreateCache();
}

void AccountListModel::recreateCache()
{
    m_list = accountFromGList(m_root->get_descendants());
    reset();
}

void AccountListModel::accountEvent( ::Account* acc, QofEventId event_type)
{
    //qDebug() << "AccountListModel::accountEvent, id=" << qofEventToString(event_type);

    switch (event_type)
    {
    case QOF_EVENT_CREATE:
    case QOF_EVENT_DESTROY:
        recreateCache();
        break;
    default:
        break;
    }
}

QModelIndex AccountListModel::index(int row, int column,
                                    const QModelIndex &parent) const
{
    //qDebug() << "index(), " << row << column << parent;
    if (!hasIndex(row, column, parent) || row >= m_list.size())
        return QModelIndex();

    Glib::RefPtr<Account> childItem = Glib::wrap(m_list.at(row));
    if (childItem)
    {
        //qDebug() << "returning" << childItem.getName();
        return createIndex(row, column, childItem->gobj());
    }
    else
        return QModelIndex();
}


// ////////////////////////////////////////////////////////////

int AccountListNamesModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant AccountListNamesModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Glib::RefPtr<Account> account = Glib::wrap(static_cast< ::Account*>(index.internalPointer()));
    switch (index.column())
    {
    case 0:
        switch (role)
        {
        case Qt::DisplayRole:
            return g2q(account->get_full_name());
        default:
            return QVariant();
        }
    default:
        return QVariant();
    }
}

} // END namespace gnc
