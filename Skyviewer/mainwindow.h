#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "workspace.h"

// TODO: default number of viewports
#define DEFAULT_VIEWPORTS 1
#define SIDEPANE_WIDTH 300

#define DEBUG 1

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openFiles();

private slots:
    void showHistogram(bool show=true);

private:
    Ui::MainWindow *ui;
    WorkSpace* workspace;
};

#endif // MAINWINDOW_H
