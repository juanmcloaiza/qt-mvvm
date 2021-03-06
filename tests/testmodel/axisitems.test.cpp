// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include "google_test.h"
#include <mvvm/standarditems/axisitems.h>

using namespace ModelView;

//! Testing AxisItems.

class AxisItemsTest : public ::testing::Test
{
public:
    ~AxisItemsTest();
};

AxisItemsTest::~AxisItemsTest() = default;

//! Initial state

TEST_F(AxisItemsTest, initialState)
{
    FixedBinAxisItem axis;
    std::vector<double> expected{0.5};
    EXPECT_EQ(axis.binCenters(), expected);
    EXPECT_EQ(axis.size(), 1);
}

//! Initial state

TEST_F(AxisItemsTest, viewportAxisInitialState)
{
    ViewportAxisItem axis;
    EXPECT_EQ(axis.property(ViewportAxisItem::P_MIN), 0.0);
    EXPECT_EQ(axis.property(ViewportAxisItem::P_MAX), 1.0);
    EXPECT_FALSE(axis.property(ViewportAxisItem::P_IS_LOG).value<bool>());
}

//! ViewportAxisItem::setRange

TEST_F(AxisItemsTest, viewportAxisSetRange)
{
    ViewportAxisItem axis;

    // default range
    auto [lower, upper] = axis.range();
    EXPECT_EQ(lower, 0.0);
    EXPECT_EQ(upper, 1.0);

    axis.set_range(1.0, 2.0);
    EXPECT_EQ(axis.property(ViewportAxisItem::P_MIN), 1.0);
    EXPECT_EQ(axis.property(ViewportAxisItem::P_MAX), 2.0);
}

//! Factory method

TEST_F(AxisItemsTest, fixedBinAxisFactory)
{
    auto axis = FixedBinAxisItem::create(3, 1.0, 4.0);

    EXPECT_EQ(axis->property(FixedBinAxisItem::P_NBINS), 3);
    EXPECT_EQ(axis->property(FixedBinAxisItem::P_MIN), 1.0);
    EXPECT_EQ(axis->property(FixedBinAxisItem::P_MAX), 4.0);

    std::vector<double> expected{1.5, 2.5, 3.5};
    EXPECT_EQ(axis->binCenters(), expected);
    EXPECT_EQ(axis->size(), 3);
}

//! Range method.

TEST_F(AxisItemsTest, fixedBinAxisRange)
{
    auto axis = FixedBinAxisItem::create(3, 1.0, 4.0);

    auto [lower, upper] = axis->range();
    EXPECT_EQ(lower, 1.0);
    EXPECT_EQ(upper, 4.0);
}
