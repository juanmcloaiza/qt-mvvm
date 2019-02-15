// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#include "defaultviewmodel.h"
#include "itemutils.h"
#include "modelmapper.h"
#include "sessionitem.h"
#include "sessionmodel.h"
#include "viewitems.h"
#include "rowconstructor.h"
#include "viewmodelutils.h"
#include <QDebug>
#include <algorithm>

using namespace ModelView;

DefaultViewModel::DefaultViewModel(QObject* parent) : ViewModel(parent),
    m_row_constructor(new DefaultRowConstructor)
{
    setItemPrototype(new ViewEmptyItem);
}

DefaultViewModel::~DefaultViewModel() = default;

//! Returns list of ViewItems representing given item.

std::vector<ViewItem*> DefaultViewModel::findViews(SessionItem* item)
{
    return Utils::findViews(this, QModelIndex(), item);
}

void DefaultViewModel::init_view_model()
{
    clear();
    setColumnCount(2);
    update_model();
}

void DefaultViewModel::onDataChange(SessionItem* item, int role)
{
    qDebug() << "DefaultViewModel::onDataChange";

    for (auto view : findViews(item)) {
        if (view->item_role() == role) {
            auto index = indexFromItem(view);
            dataChanged(index, index, Utils::item_role_to_qt(role));
        }
    }
}

void DefaultViewModel::onRowInserted(SessionItem* parent, int row)
{
    qDebug() << "DefaultViewModel::onRowInserted";
    Q_UNUSED(parent);
    Q_UNUSED(row);

}

//! Removes all children views of given parent.

void DefaultViewModel::onRowRemoved(SessionItem* parent, int row, std::string id)
{
    Q_UNUSED(parent);
    Q_UNUSED(row);
    Q_UNUSED(id);

    qDebug() << "DefaultViewModel::onRowRemoved" << parent;

    // FIXME make more elegant without if
    if (parent == rootItem()) {
        invisibleRootItem()->removeRows(0, rowCount());
        iterate(rootItem(), invisibleRootItem());
    } else {
        auto views = findViews(parent);
        for (auto view : views) {
            view->removeRows(0, view->rowCount());
        }
        if (views.size())
            iterate(parent, views.at(0));
    }
}

void DefaultViewModel::update_model()
{
    qDebug() << "DefaultViewModel::update_model";
    iterate(rootItem(), invisibleRootItem());
}

void DefaultViewModel::iterate(SessionItem* item, QStandardItem* parent)
{
    qDebug() << "DefaultViewModel::iterate";

    QStandardItem* origParent(parent);
    for (auto child : item->children()) {

        auto row = m_row_constructor->constructRow(child);
        qDebug() << "aaa" << row.size();
        parent->appendRow(row);

        if (row.size())
            parent = row.at(0); // labelItem

        iterate(child, parent);
        parent = origParent;
    }

}