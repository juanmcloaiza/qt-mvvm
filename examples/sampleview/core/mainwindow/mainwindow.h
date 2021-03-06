// ************************************************************************** //
//
//  Model-view-view-model framework for large GUI applications
//
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @authors   see AUTHORS
//
// ************************************************************************** //

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "SampleViewController.h"
#include <QMainWindow>
#include <memory>

class ApplicationModels;

//! Application main window.

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);

private:
    void init_application();
    void init_material_editor();
    void init_sample_editor();
    void write_settings();

    std::unique_ptr<ApplicationModels> m_models;
    SampleViewController m_scene_controller;
    QDockWidget* m_material_editor;
    QDockWidget* m_sample_editor;
};

#endif // MAINWINDOW_H
