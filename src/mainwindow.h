#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "workspace.h"
#include "viewportmanager.h"
#include "histogramwidget.h"
#include "mapoptions.h"
#include "configs.h"

#define SIDEPANE_WIDTH 300

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateInterface();

private slots:
    void showHistogram(bool show=true);
    void viewportSelectionChanged(QList<int> selectedViewports);
    void syncAllViewports();

private:
    void enableButtons();
    void disableButtons();
    int sidePanelSize;

private:
    Ui::MainWindow *ui;
    WorkSpace* workspace;
};

#endif // MAINWINDOW_H
