// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#ifndef JSONUTILS_H
#define JSONUTILS_H

#include "global.h"
#include <string>

class QJsonObject;

namespace ModelView {

class SessionModel;

namespace JsonUtils {

//! Returns multiline string representing model content as json.
CORE_EXPORT std::string ModelToJsonString(const SessionModel& model);

} // JsonUtils

} // ModelView

#endif // JSONUTILS_H
