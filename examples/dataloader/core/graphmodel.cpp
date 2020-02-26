// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "graphmodel.h"
#include "iolib.h"
#include <QColor>
#include <cmath>
#include <mvvm/model/modelutils.h>
#include <mvvm/model/mvvm_types.h>
#include <mvvm/standarditems/axisitems.h>
#include <mvvm/standarditems/containeritem.h>
#include <mvvm/standarditems/data1ditem.h>
#include <mvvm/standarditems/graphitem.h>
#include <mvvm/standarditems/graphviewportitem.h>
#include <mvvm/utils/numericutils.h>

namespace
{

constexpr double pi = 3.14159265358979323846;
constexpr int npoints = 400;
constexpr double xmin = 0.0;
constexpr double xmax = 5.0;
constexpr double dx = (xmax - xmin) / npoints;

std::vector<double> bin_values(double amp_factor = 1.0)
{
    std::vector<double> result;
    for (int i = 0; i < npoints; ++i) {
        double x = xmin + i * dx;
        double value = amp_factor * 10.0 * std::sin(2.0 * pi * 2 * x)
                       + amp_factor * 5.0 * std::sin(2 * pi * 2.25 * x);
        result.push_back(value);
    }
    return result;
}

} // namespace

using namespace ModelView;

GraphModel::GraphModel() : SessionModel("GraphModel")
{
    init_model();
}

void GraphModel::load_data()
{
    auto filePath = "../testfiles/singlefiles/nfringes_1.out";
    auto data = insertItem<Data1DItem>(data_container());
    data->setAxis(FixedBinAxisItem::create(npoints, xmin, xmax));
    data->setContent(getOutputData(filePath));

    auto graph = insertItem<GraphItem>(viewport());
    graph->setDataItem(data);
    auto rndm = []() -> int { return ModelView::Utils::RandInt(0, 255); };
    graph->setProperty(GraphItem::P_COLOR, QVariant::fromValue(QColor(rndm(), rndm(), rndm())));
}

//! Adds Graph1DItem with some random points.

void GraphModel::add_graph()
{
    auto data = insertItem<Data1DItem>(data_container());
    data->setAxis(FixedBinAxisItem::create(npoints, xmin, xmax));
    data->setContent(bin_values(ModelView::Utils::RandDouble(0.5, 1.0)));

    auto graph = insertItem<GraphItem>(viewport());
    graph->setDataItem(data);
    auto rndm = []() -> int { return ModelView::Utils::RandInt(0, 255); };
    graph->setProperty(GraphItem::P_COLOR, QVariant::fromValue(QColor(rndm(), rndm(), rndm())));
}

//! Remove last graph and data item.

void GraphModel::remove_graph()
{
    const int graph_count = viewport()->itemCount(ViewportItem::T_ITEMS);
    const int data_count = data_container()->itemCount(ContainerItem::T_ITEMS);

    if (graph_count != data_count)
        throw std::runtime_error("Number of graphs do not much number of data items.");

    if (graph_count)
        removeItem(viewport(), {"", graph_count - 1});

    if (data_count)
        removeItem(data_container(), {"", data_count - 1});
}

//! Put random noise to graph.

void GraphModel::randomize_graphs()
{
    for (auto item : data_container()->items<Data1DItem>(ContainerItem::T_ITEMS)) {
        auto values = item->binValues();
        std::transform(std::begin(values), std::end(values), std::begin(values),
                       [](auto x) { return x * ModelView::Utils::RandDouble(0.8, 1.2); });
        item->setContent(values);
    }
}

//! Returns viewport item containig graph items.

GraphViewportItem* GraphModel::viewport()
{
    return Utils::TopItem<GraphViewportItem>(this);
}

//! Returns container with data items.

ContainerItem* GraphModel::data_container()
{
    return Utils::TopItem<ContainerItem>(this);
}

void GraphModel::init_model()
{
    auto container = insertItem<ContainerItem>();
    container->setDisplayName("Data container");

    auto viewport = insertItem<GraphViewportItem>();
    viewport->setDisplayName("Graph container");
    add_graph();
}
