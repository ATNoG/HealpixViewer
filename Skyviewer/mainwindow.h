#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "workspace.h"
#include "viewportmanager.h"
#include "histogramwidget.h"

#define SIDEPANE_WIDTH 300

#define DEBUG 1

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void showHistogram(bool show=true);

private:
    Ui::MainWindow *ui;
    WorkSpace* workspace;
};

#endif // MAINWINDOW_H
