// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include <QTreeView>
#include <QVBoxLayout>
#include <mvvm/viewmodel/abstractviewmodel.h>
#include <mvvm/viewmodel/viewmodeldelegate.h>
#include <mvvm/widgets/itemstreeview.h>

using namespace ModelView;

ItemsTreeView::ItemsTreeView(QWidget* parent)
    : QWidget(parent), m_treeView(new QTreeView), m_delegate(std::make_unique<ViewModelDelegate>()),
      m_block_selection(false)
{
    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_treeView);
    setLayout(layout);
}

ItemsTreeView::~ItemsTreeView() = default;

void ItemsTreeView::setViewModel(std::unique_ptr<AbstractViewModel> viewModel)
{
    m_viewModel = std::move(viewModel);
    m_treeView->setItemDelegate(m_delegate.get());
    m_treeView->setModel(m_viewModel.get());
    m_treeView->expandAll();
    m_treeView->resizeColumnToContents(0);
    set_connected(true);
}

//! Make given item selected in QTreeView.

void ItemsTreeView::setSelected(SessionItem* item)
{
    if (!m_viewModel)
        return;

    auto indexes = m_viewModel->indexOfSessionItem(item);
    if (!indexes.empty())
        selectionModel()->select(indexes.at(0), QItemSelectionModel::SelectCurrent);
}

void ItemsTreeView::setRootSessionItem(SessionItem* item)
{
    m_viewModel->setRootSessionItem(item);
    m_treeView->expandAll();
}

AbstractViewModel* ItemsTreeView::viewModel() const
{
    return m_viewModel.get();
}

//! Processes selections in QTreeView. Finds SessionItem corresponding to selected indexes
//! and emit itemSelected signal.

void ItemsTreeView::onSelectionChanged(const QItemSelection&, const QItemSelection&)
{
    if (m_block_selection)
        return;

    auto indexes = m_treeView->selectionModel()->selectedIndexes();
    if (!indexes.empty()) {
        auto item = m_viewModel->sessionItemFromIndex(indexes.at(0));
        m_block_selection = true;
        itemSelected(item);
        m_block_selection = false;
    }
}

void ItemsTreeView::set_connected(bool flag)
{
    Q_ASSERT(selectionModel());

    if (flag)
        connect(selectionModel(), &QItemSelectionModel::selectionChanged, this,
                &ItemsTreeView::onSelectionChanged);
    else
        disconnect(selectionModel(), &QItemSelectionModel::selectionChanged, this,
                   &ItemsTreeView::onSelectionChanged);
}

QTreeView* ItemsTreeView::treeView()
{
    return m_treeView;
}

QItemSelectionModel* ItemsTreeView::selectionModel()
{
    return m_treeView->selectionModel();
}
