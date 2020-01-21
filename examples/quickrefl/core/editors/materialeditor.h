// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef MATERIALEDITOR_H
#define MATERIALEDITOR_H

#include <QWidget>

class MaterialEditorToolBar;
class MaterialEditorActions;

//! Material editor.

class MaterialEditor : public QWidget
{
    Q_OBJECT
public:
    MaterialEditor(QWidget* parent = nullptr);
    ~MaterialEditor();

private:
    MaterialEditorActions* actions{nullptr};
    MaterialEditorToolBar* toolbar{nullptr};
};

#endif // MATERIALEDITOR_H

