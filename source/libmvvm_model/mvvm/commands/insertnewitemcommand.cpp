// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#include <mvvm/commands/insertnewitemcommand.h>
#include <mvvm/model/path.h>
#include <mvvm/model/sessionitem.h>
#include <sstream>

using namespace ModelView;

namespace
{
std::string generate_description(const std::string& modelType, const TagRow& tagrow);
} // namespace

struct InsertNewItemCommand::InsertNewItemCommandImpl {
    item_factory_func_t factory_func;
    TagRow tagrow;
    result_t result;
    Path item_path;
    InsertNewItemCommandImpl(item_factory_func_t func, TagRow tagrow)
        : factory_func(func), tagrow(std::move(tagrow)), result(nullptr)
    {
    }
};

InsertNewItemCommand::InsertNewItemCommand(item_factory_func_t func, SessionItem* parent,
                                           TagRow tagrow)
    : AbstractItemCommand(parent), p_impl(std::make_unique<InsertNewItemCommandImpl>(func, tagrow))
{
    p_impl->item_path = pathFromItem(parent);
}

InsertNewItemCommand::~InsertNewItemCommand() = default;

void InsertNewItemCommand::undo_command()
{
    auto parent = itemFromPath(p_impl->item_path);
    delete parent->takeItem(p_impl->tagrow);
    p_impl->result = nullptr;
}

void InsertNewItemCommand::execute_command()
{
    auto parent = itemFromPath(p_impl->item_path);
    auto child = p_impl->factory_func().release();
    setDescription(generate_description(child->modelType(), p_impl->tagrow));
    if (parent->insertItem(child, p_impl->tagrow)) {
        p_impl->result = child;
    } else {
        delete child;
        setObsolete(true);
    }
}

InsertNewItemCommand::result_t InsertNewItemCommand::result() const
{
    return p_impl->result;
}

namespace
{
std::string generate_description(const std::string& modelType, const TagRow& tagrow)
{
    std::ostringstream ostr;
    ostr << "New item type '" << modelType << "' tag:'" << tagrow.tag << "', row:" << tagrow.row;
    return ostr.str();
}
} // namespace
