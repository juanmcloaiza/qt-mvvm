// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "google_test.h"
#include "toy_includes.h"
#include <mvvm/model/propertyitem.h>
#include <mvvm/model/sessionitem.h>
#include <mvvm/model/sessionmodel.h>
#include <mvvm/model/taginfo.h>
#include <mvvm/standarditems/vectoritem.h>
#include <mvvm/viewmodel/propertyflatviewmodel.h>

using namespace ModelView;

//! Tests for PropertyFlatModel class.

class PropertyFlatModelTest : public ::testing::Test
{
public:
    ~PropertyFlatModelTest();
};

PropertyFlatModelTest::~PropertyFlatModelTest() = default;

TEST_F(PropertyFlatModelTest, initialState)
{
    PropertyFlatViewModel viewModel;
    EXPECT_EQ(viewModel.rowCount(), 0);
    EXPECT_EQ(viewModel.columnCount(), 0);
    EXPECT_EQ(viewModel.sessionItemFromIndex(QModelIndex()), nullptr);
}

TEST_F(PropertyFlatModelTest, baseItem)
{
    SessionModel model;
    model.insertItem<SessionItem>();

    PropertyFlatViewModel viewModel;
    viewModel.setSessionModel(&model);

    // Root item has default tag and all items considered as top items.
    // PropertyViewModel shouldn't see any items.
    EXPECT_EQ(viewModel.rowCount(), 0);
    EXPECT_EQ(viewModel.columnCount(), 2);
}

TEST_F(PropertyFlatModelTest, propertyItem)
{
    SessionModel model;
    auto parent = model.insertItem<SessionItem>();

    parent->registerTag(TagInfo::universalTag("universal_tag"));
    parent->registerTag(TagInfo::propertyTag("property_tag", Constants::PropertyType));

    model.insertItem<SessionItem>(parent, "universal_tag");
    model.insertItem<PropertyItem>(parent, "property_tag");
    model.insertItem<SessionItem>(parent, "universal_tag");

    PropertyFlatViewModel viewModel;
    viewModel.setSessionModel(&model);
    viewModel.setRootSessionItem(parent);

    // View model should see only property item belonging to parent.
    EXPECT_EQ(viewModel.rowCount(), 1);
    EXPECT_EQ(viewModel.columnCount(), 2);
}

//! VectorItem in a model.

TEST_F(PropertyFlatModelTest, vectorItem)
{
    SessionModel model;
    auto parent = model.insertItem<VectorItem>();

    PropertyFlatViewModel viewModel;
    viewModel.setSessionModel(&model);

    EXPECT_EQ(viewModel.rowCount(), 0); // root item doesn't have properties
    EXPECT_EQ(viewModel.columnCount(), 2);

    // switching to vectorItem and checking that it has 3 properties
    viewModel.setRootSessionItem(parent);
    EXPECT_EQ(viewModel.rowCount(), 3);
    EXPECT_EQ(viewModel.columnCount(), 2);
}

//! ParticleItem in a model

TEST_F(PropertyFlatModelTest, particleItem)
{
    ToyItems::SampleModel model;
    auto particle = model.insertItem<ToyItems::ParticleItem>();
    auto group = dynamic_cast<GroupItem*>(particle->getItem(ToyItems::ParticleItem::P_SHAPES));
    group->setCurrentType(ToyItems::Constants::SphereItemType);

    PropertyFlatViewModel viewModel;
    viewModel.setSessionModel(&model);
    viewModel.setRootSessionItem(particle);

    // We should see 3 rows: VectorItem, GroupItem itself, and Radius of sphere
    EXPECT_EQ(viewModel.rowCount(), 3);
    EXPECT_EQ(viewModel.columnCount(), 2);

    // switching group
    group->setCurrentType(ToyItems::Constants::CylinderItemType);
    // We should see 3 rows: VectorItem, GroupItem itself, Cylinderr length and radius
    EXPECT_EQ(viewModel.rowCount(), 4);
    EXPECT_EQ(viewModel.columnCount(), 2);
}


