// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "customplot_test_utils.h"
#include "google_test.h"
#include "qcustomplot.h"
#include <QSignalSpy>
#include <mvvm/model/sessionmodel.h>
#include <mvvm/plotting/graphplotcontroller.h>
#include <mvvm/standarditems/axisitems.h>
#include <mvvm/standarditems/data1ditem.h>
#include <mvvm/standarditems/graphitem.h>

using namespace ModelView;

//! Testing GraphPlotController.

class GraphPlotControllerTest : public ::testing::Test
{
public:
    ~GraphPlotControllerTest();
};

GraphPlotControllerTest::~GraphPlotControllerTest() = default;

//! Initial state.

TEST_F(GraphPlotControllerTest, initialState)
{
    auto custom_plot = std::make_unique<QCustomPlot>();
    GraphPlotController controller(custom_plot.get());
    EXPECT_EQ(controller.currentItem(), nullptr);
    EXPECT_EQ(custom_plot->graphCount(), 1);
    auto graph = custom_plot->graph();
    EXPECT_EQ(TestUtils::binCenters(graph), std::vector<double>());
    EXPECT_EQ(TestUtils::binValues(graph), std::vector<double>());
}

//! Setting GraphItem with data and checking that plottable contains correct data.

TEST_F(GraphPlotControllerTest, setItem)
{
    auto custom_plot = std::make_unique<QCustomPlot>();
    GraphPlotController controller(custom_plot.get());

    // setup model and single data item in it
    SessionModel model;
    auto data_item = model.insertItem<Data1DItem>();
    data_item->setAxis(FixedBinAxisItem::create(2, 0.0, 2.0));
    std::vector<double> expected_centers = {0.5, 1.5};
    std::vector<double> expected_values = {42.0, 43.0};
    data_item->setContent(expected_values);

    // setup graph item
    auto graph_item = model.insertItem<GraphItem>();
    graph_item->setProperty(GraphItem::P_COLOR, QColor(Qt::red));
    graph_item->setDataItem(data_item);

    // initializing controller
    controller.setItem(graph_item);

    // Checking resulting plottables
    EXPECT_EQ(custom_plot->graphCount(), 1);
    auto graph = custom_plot->graph();
    EXPECT_EQ(TestUtils::binCenters(graph), expected_centers);
    EXPECT_EQ(TestUtils::binValues(graph), expected_values);
    EXPECT_EQ(graph->pen().color(), QColor(Qt::red));
}

//! Setting data to graph after.

TEST_F(GraphPlotControllerTest, setDataAfter)
{
    auto custom_plot = std::make_unique<QCustomPlot>();
    GraphPlotController controller(custom_plot.get());

    SessionModel model;
    auto graph_item = model.insertItem<GraphItem>();

    controller.setItem(graph_item);

    // without data QCustomPlot has a graph without points
    EXPECT_EQ(custom_plot->graphCount(), 1);
    auto graph = custom_plot->graph();
    EXPECT_EQ(TestUtils::binCenters(graph), std::vector<double>());
    EXPECT_EQ(TestUtils::binValues(graph), std::vector<double>());

    // setup Data1DItem and assign to GraphItem
    auto data_item = model.insertItem<Data1DItem>();
    data_item->setAxis(FixedBinAxisItem::create(2, 0.0, 2.0));
    std::vector<double> expected_centers = {0.5, 1.5};
    std::vector<double> expected_values = {42.0, 43.0};
    data_item->setContent(expected_values);

    graph_item->setDataItem(data_item);

    // Checking resulting plottables
    EXPECT_EQ(custom_plot->graphCount(), 1);
    EXPECT_EQ(TestUtils::binCenters(graph), expected_centers);
    EXPECT_EQ(TestUtils::binValues(graph), expected_values);
}

//! Unlinking from Data1DItem or GraphItem.

TEST_F(GraphPlotControllerTest, unlinkFromItem)
{
    auto custom_plot = std::make_unique<QCustomPlot>();
    GraphPlotController controller(custom_plot.get());

    // setup model and single data item in it
    SessionModel model;
    auto data_item = model.insertItem<Data1DItem>();
    data_item->setAxis(FixedBinAxisItem::create(2, 0.0, 2.0));
    std::vector<double> expected_centers = {0.5, 1.5};
    std::vector<double> expected_values = {42.0, 43.0};
    data_item->setContent(expected_values);

    // setup graph item
    auto graph_item = model.insertItem<GraphItem>();
    graph_item->setProperty(GraphItem::P_COLOR, QColor(Qt::red));
    graph_item->setDataItem(data_item);

    // initializing controller
    controller.setItem(graph_item);

    // unlinking from data item
    graph_item->setDataItem(nullptr);

    // Checking resulting plottables
    // Current convention is that graph stays intact, but points disappear.
    EXPECT_EQ(custom_plot->graphCount(), 1);
    auto graph = custom_plot->graph();
    EXPECT_EQ(TestUtils::binCenters(graph), std::vector<double>());
    EXPECT_EQ(TestUtils::binValues(graph), std::vector<double>());
    EXPECT_EQ(graph->pen().color(), QColor(Qt::red));

    // unlinking from graph item should leave GraphItem intact.
    controller.setItem(nullptr);
    EXPECT_EQ(custom_plot->graphCount(), 1);
}

//! Deletion of controller should lead to graph removal.

TEST_F(GraphPlotControllerTest, controllerDelete)
{
    auto custom_plot = std::make_unique<QCustomPlot>();
    auto controller = std::make_unique<GraphPlotController>(custom_plot.get());

    // setup model and single data item in it
    SessionModel model;
    auto data_item = model.insertItem<Data1DItem>();

    // setup graph item
    auto graph_item = model.insertItem<GraphItem>();
    graph_item->setDataItem(data_item);

    // initializing controller
    controller->setItem(graph_item);
    EXPECT_EQ(custom_plot->graphCount(), 1);

    // deleting controller should lead to graph removal
    controller.reset();
    EXPECT_EQ(custom_plot->graphCount(), 0);
}
