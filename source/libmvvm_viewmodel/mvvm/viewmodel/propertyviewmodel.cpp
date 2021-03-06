// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include <mvvm/viewmodel/propertyviewmodel.h>
#include <mvvm/viewmodel/standardviewmodelcontrollers.h>

using namespace ModelView;

PropertyViewModel::PropertyViewModel(SessionModel* model, QObject* parent)
    : AbstractViewModel(std::make_unique<PropertyViewModelController>(this), parent)
{
    setSessionModel(model);
}
