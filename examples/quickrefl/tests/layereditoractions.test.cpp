// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "google_test.h"
#include "layereditoractions.h"
#include "item_constants.h"
#include "layeritems.h"
#include "layerselectionmodel.h"
#include "layerviewmodel.h"
#include "samplemodel.h"
#include "test_utils.h"
#include <mvvm/viewmodel/viewmodelutils.h>

using namespace ModelView;

//! Tests of LayerEditorActions.

class LayerEditorActionsTest : public ::testing::Test
{
public:
    ~LayerEditorActionsTest();

    //! Test data representing MultiLayer with two layers and all machinery around:
    //! ViewModel, SelectionModel and actions.

    struct TestData {
        SampleModel sample_model;
        LayerViewModel view_model{&sample_model};
        LayerSelectionModel selection_model{&view_model};
        LayerEditorActions actions{&sample_model};
        SessionItem* multilayer{nullptr};
        SessionItem* top{nullptr};
        SessionItem* bottom{nullptr};
        TestData()
        {
            actions.setSelectionModel(&selection_model);
            multilayer = sample_model.insertItem<MultiLayerItem>();
            top = sample_model.insertItem<LayerItem>(multilayer);
            bottom = sample_model.insertItem<LayerItem>(multilayer);
            view_model.setRootSessionItem(multilayer);
        }
    };
};

LayerEditorActionsTest::~LayerEditorActionsTest() = default;

//! Checking initial data for testing.

TEST_F(LayerEditorActionsTest, initialState)
{
    TestData test_data;

    // checking layout of multilayer
    auto layers = test_data.multilayer->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(layers.size(), 2);
    EXPECT_EQ(layers.at(0), test_data.top);
    EXPECT_EQ(layers.at(1), test_data.bottom);

    // checking, that there are no layers selected
    EXPECT_EQ(test_data.selection_model.selectedItems(), std::vector<SessionItem*>());
}

//! Adds new layer after selected layer.

TEST_F(LayerEditorActionsTest, addNewLayerAfterSelection)
{
    TestData test_data;

    // selecting top layer
    test_data.selection_model.selectItem(test_data.top);

    // adding new layer after selection
    test_data.actions.onAddLayer();

    // checking layout of multilayer
    auto layers = test_data.multilayer->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(layers.size(), 3);
    EXPECT_EQ(layers.at(0), test_data.top);
    EXPECT_EQ(layers.at(2), test_data.bottom);

    // checking, that new layer is selected
    std::vector<SessionItem*> expected = {layers.at(1)};
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);
}

//! Adds new multi-layer after selected layer.

TEST_F(LayerEditorActionsTest, addNewMultiLayerAfterSelection)
{
    TestData test_data;

    // selecting top layer
    test_data.selection_model.selectItem(test_data.top);

    // adding new layer after selection
    test_data.actions.onAddMultiLayer();


    // checking layout of multilayer
    auto layers = test_data.multilayer->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(layers.size(), 3);
    EXPECT_EQ(layers.at(0), test_data.top);
    EXPECT_EQ(layers.at(2), test_data.bottom);

    // checking that layer was added
    EXPECT_EQ(layers.at(1)->modelType(), ::Constants::MultiLayerItemType);
    auto sublayers = layers.at(1)->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(sublayers.size(), 2);

    // checking, that new layer is selected
    std::vector<SessionItem*> expected = {layers.at(1)};
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);
}

//! Adds new layer when no selection exists.

TEST_F(LayerEditorActionsTest, addNewLayerNoSelection)
{
    TestData test_data;

    // adding new layer when no selection exist
    test_data.actions.onAddLayer();

    // checking layout of multilayer
    auto layers = test_data.multilayer->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(layers.size(), 3);
    EXPECT_EQ(layers.at(0), test_data.top);
    EXPECT_EQ(layers.at(1), test_data.bottom);

    // checking, that new layer is selected
    std::vector<SessionItem*> expected = {layers.at(2)};
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);
}

//! Adds new layer after selected layer.

TEST_F(LayerEditorActionsTest, onRemoveLayer)
{
    TestData test_data;

    // selecting top layer
    test_data.selection_model.selectItem(test_data.top);

    // removing selected top layer
    test_data.actions.onRemove();

    // checking layout of multilayer
    auto layers = test_data.multilayer->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(layers.size(), 1);
    EXPECT_EQ(layers.at(0), test_data.bottom);

    // checking, that next layer beneath is selected
    std::vector<SessionItem*> expected = {test_data.bottom};
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);
}

//! Move one layer up.

TEST_F(LayerEditorActionsTest, onMoveUp)
{
    TestData test_data;

    // selecting bottom layer
    test_data.selection_model.selectItem(test_data.bottom);

    // moving selected bottom layer up
    test_data.actions.onMoveUp();

    // checking layout of multilayer
    auto layers = test_data.multilayer->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(layers.size(), 2);
    EXPECT_EQ(layers.at(0), test_data.bottom);
    EXPECT_EQ(layers.at(1), test_data.top);

    // checking, that former bottom layer is stil selected
    std::vector<SessionItem*> expected = {test_data.bottom};
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);

    // moving up once again, nothing should change
    test_data.actions.onMoveUp();
    EXPECT_EQ(layers.at(0), test_data.bottom);
    EXPECT_EQ(layers.at(1), test_data.top);
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);
}

//! Move one layer up.

TEST_F(LayerEditorActionsTest, onMoveDown)
{
    TestData test_data;

    // selecting top layer
    test_data.selection_model.selectItem(test_data.top);

    // moving selected top layer down
    test_data.actions.onMoveDown();

    // checking layout of multilayer
    auto layers = test_data.multilayer->getItems(MultiLayerItem::T_LAYERS);
    EXPECT_EQ(layers.size(), 2);
    EXPECT_EQ(layers.at(0), test_data.bottom);
    EXPECT_EQ(layers.at(1), test_data.top);

    // checking, that former top layer is stil selected
    std::vector<SessionItem*> expected = {test_data.top};
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);

    // moving down once again, nothing should change
    test_data.actions.onMoveDown();
    EXPECT_EQ(layers.at(0), test_data.bottom);
    EXPECT_EQ(layers.at(1), test_data.top);
    EXPECT_EQ(test_data.selection_model.selectedItems(), expected);
}
