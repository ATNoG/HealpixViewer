#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSignalMapper>

#include "histogramwidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* configure toolbar icons */
    QIcon openicon  = QIcon::fromTheme("document-open");
    QIcon saveicon  = QIcon::fromTheme("document-save");
    QIcon reseticon = QIcon::fromTheme("edit-undo");
    ui->actionOpen->setIcon(openicon);
    //ui->actionSave->setIcon(saveicon);
    ui->actionReset->setIcon(reseticon);


    /* Configure workspace */
    workspace = new WorkSpace();

    /* add workspace to splitter */
    ui->splitter->insertWidget(0, workspace);


    /* configure events */
    connect(ui->action3Dview, SIGNAL(triggered()), workspace, SLOT(changeTo3D()));
    connect(ui->actionMollview, SIGNAL(triggered()), workspace, SLOT(changeToMollview()));
    connect(ui->actionSynchronize, SIGNAL(triggered(bool)), workspace, SLOT(changeSynchronization(bool)));
    connect(ui->actionSelectAll, SIGNAL(triggered()), workspace, SLOT(selectAllViewports()));
    connect(ui->actionDeselectAll, SIGNAL(triggered()), workspace, SLOT(deselectAllViewports()));
    connect(ui->actionReset, SIGNAL(triggered()), workspace, SLOT(resetViewports()));
    connect(ui->actionPolarizationVectors, SIGNAL(triggered(bool)), workspace, SLOT(showPolarizationVectors(bool)));
    connect(ui->actionColors, SIGNAL(triggered(bool)), this, SLOT(showHistogram(bool)));
    connect(ui->actionShowGrid, SIGNAL(triggered(bool)), workspace, SLOT(showGrid(bool)));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));


    ui->actionColors->setChecked(true);


    ViewportManager *viewportManager  = ui->viewportManager;
    HistogramWidget *histogramControl = ui->histogramControl;

    /* connect viewportManager to histogramWidget */
    connect(viewportManager, SIGNAL(mapUpdated(int,mapInfo*)), histogramControl, SLOT(updateMapInfo(int,mapInfo*)));
    connect(viewportManager, SIGNAL(mapClosed(int)), histogramControl, SLOT(unloadMapInfo(int)));
    connect(viewportManager, SIGNAL(histogramViewportsSelectedUpdated(QList<int>)), histogramControl, SLOT(updateHistogram(QList<int>)));

    /* connect viewportManager to workspace */
    connect(viewportManager, SIGNAL(viewportShowOn(int,MapViewport*)), workspace, SLOT(addViewport(int,MapViewport*)));
    connect(viewportManager, SIGNAL(viewportShowOff(int)), workspace, SLOT(removeViewport(int)));

    /* connect histogramWidget to viewportManager */
    connect(histogramControl, SIGNAL(thresholdUpdated(float,float)), viewportManager, SLOT(updateThresholds(float,float)));

    /* connect mainwindow to viewportManager */
    connect(ui->actionOpen, SIGNAL(triggered()), viewportManager, SLOT(openFiles()));


    /* open application in fullscreen */
    QWidget::showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete workspace;
}


void MainWindow::showHistogram(bool show)
{
    if(show)
    {
        ui->tabWidget->show();
        QList<int> sizes;
        sizes.append(centralWidget()->width()-SIDEPANE_WIDTH);
        sizes.append(SIDEPANE_WIDTH);
        ui->splitter->setSizes(sizes);
        ui->tabWidget->setCurrentIndex(0);
    }
    else
    {
        ui->tabWidget->hide();
    }
}
