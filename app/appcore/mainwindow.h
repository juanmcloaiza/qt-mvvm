// ************************************************************************** //
//
//  Prototype of mini MVVM framework for bornagainproject.org
//
//! @homepage  http://www.bornagainproject.org
//! @license   GNU General Public License v3 or higher
//
// ************************************************************************** //

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "global.h"
#include <QMainWindow>

class TestWidget1;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();

private:
    TestWidget1* m_widget1;
};

#endif //  MAINWINDOW_H
