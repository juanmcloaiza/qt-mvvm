// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#include "mainwindow.h"
#include "modeleditorwidget.h"
#include "samplemodel.h"
#include "item_constants.h"
#include <QTabWidget>
#include <QCoreApplication>
#include <QSettings>

namespace {
    const QString main_window_group = "MainWindow";
    const QString size_key = "size";
    const QString pos_key = "pos";
}

MainWindow::MainWindow()
    : m_tabWidget(new QTabWidget), m_model1(std::make_unique<SampleModel>()),
      m_model2(std::make_unique<SampleModel>())
{    
    m_tabWidget->addTab(new ModelEditorWidget(m_model1.get()), "Available properties");

    m_tabWidget->setCurrentIndex(m_tabWidget->count()-1);
    setCentralWidget(m_tabWidget);

    init_application();
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent* event)
{
    write_settings();
    QMainWindow::closeEvent(event);
}

void MainWindow::write_settings()
{
    QSettings settings;
    settings.beginGroup(main_window_group);
    settings.setValue(size_key, size());
    settings.setValue(pos_key, pos());
    settings.endGroup();
}

void MainWindow::init_application()
{
    QCoreApplication::setApplicationName("qt-mvvm-example");
    QCoreApplication::setApplicationVersion("0.1");
    QCoreApplication::setOrganizationName("qt-mvvm");

    QSettings settings;
    if (settings.childGroups().contains(main_window_group)) {
        settings.beginGroup(main_window_group);
        resize(settings.value(size_key, QSize(400, 400)).toSize());
        move(settings.value(pos_key, QPoint(200, 200)).toPoint());
        settings.endGroup();
    }

    init_models();
}

void MainWindow::init_models()
{
    // populating first model with content
    m_model1->insertNewItem(Constants::DemoPropertiesType);
    m_model1->insertNewItem(Constants::DemoPropertiesType);
    m_model1->insertNewItem(Constants::DemoPropertiesType);

    // populating second model with content
    m_model2->insertNewItem(Constants::DemoPropertiesType);
    m_model2->insertNewItem(Constants::DemoPropertiesType);
    m_model2->insertNewItem(Constants::DemoPropertiesType);
}