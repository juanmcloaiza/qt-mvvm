// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "google_test.h"
#include <QDebug>
#include <memory>
#include <mvvm/model/sessionitem.h>
#include <mvvm/viewmodel/viewitems.h>

using namespace ModelView;

class ViewLabelItemTest : public ::testing::Test
{
public:
    ~ViewLabelItemTest();
};

ViewLabelItemTest::~ViewLabelItemTest() = default;

TEST_F(ViewLabelItemTest, initialViewLabelItem)
{
    std::unique_ptr<SessionItem> item(new SessionItem);

    ViewLabelItem viewItem(item.get());
    EXPECT_EQ(viewItem.item(), item.get());
    EXPECT_EQ(viewItem.item_role(), ItemDataRole::DISPLAY);
}

//! ViewLabelItem::data method
//! Checks that the data method is correctly forwarded to underlying SessionItem.

TEST_F(ViewLabelItemTest, ViewLabelItem_data)
{
    // create SessionItem with data on board
    auto item = std::make_unique<SessionItem>();
    QVariant expected = QVariant::fromValue(std::string("Layer"));
    EXPECT_TRUE(item->setData(expected, ItemDataRole::DISPLAY));

    // initialize viewItem with sessionItem and check the data
    ViewLabelItem viewItem(item.get());
    EXPECT_EQ(Utils::toCustomVariant(viewItem.data(Qt::EditRole)), expected);
    EXPECT_EQ(Utils::toCustomVariant(viewItem.data(Qt::DisplayRole)), expected);
}

//! ViewLabelItem::setData
//! Checks that the setData method is correctly forwarded to underlying SessionItem.

TEST_F(ViewLabelItemTest, ViewLabelItem_setData)
{
    // create SessionItem with data on board
    auto item = std::make_unique<SessionItem>();
    QVariant expected = QVariant::fromValue(std::string("Layer"));
    EXPECT_TRUE(item->setData(expected, ItemDataRole::DISPLAY));

    // initialize viewItem with sessionItem and set the data
    ViewLabelItem viewItem(item.get());
    QVariant new_data("MultiLayer");
    viewItem.setData(new_data, Qt::EditRole);
    EXPECT_EQ(viewItem.data(Qt::DisplayRole), new_data); // new data
    EXPECT_EQ(viewItem.data(Qt::EditRole), new_data);    // new data

    // SessionItem itself should have new data
    EXPECT_EQ(item->data(ItemDataRole::DISPLAY), Utils::toCustomVariant(new_data)); // new data

    // it is not allowed to set another type of data to ViewLabelItem
    QVariant not_allowed_value(42);
    EXPECT_THROW(viewItem.setData(not_allowed_value, Qt::EditRole), std::runtime_error);
}
