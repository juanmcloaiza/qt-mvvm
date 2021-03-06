// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef LAYERSELECTIONMODEL_H
#define LAYERSELECTIONMODEL_H

#include <QItemSelectionModel>
#include <vector>

namespace ModelView
{
class AbstractViewModel;
class SessionItem;
} // namespace ModelView

class LayerEditorActions;

//! Custom selection model for layer view model (AbstractViewModel).

class LayerSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    LayerSelectionModel(ModelView::AbstractViewModel* view_model, QObject* parent = nullptr);
    ~LayerSelectionModel() = default;

    void selectItems(std::vector<ModelView::SessionItem*> items);
    void selectItem(ModelView::SessionItem* item);

    std::vector<ModelView::SessionItem*> selectedItems() const;

private:
    const ModelView::AbstractViewModel* viewModel() const;
};

#endif // LAYERSELECTIONMODEL_H
