#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSignalMapper>

#include "histogramwidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Configure workspace */
    workspace = ui->workspace;
    ViewportManager *viewportManager  = ui->viewportManager;
    HistogramWidget *histogramControl = ui->histogramControl;
    MapOptions *mapOptions = ui->mapOptions;

    /* add workspace to splitter */
    ui->splitter->insertWidget(0, workspace);

    ui->mapOptions->setDisabled(true);

    /* disable geral buttons */
    disableButtons();

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

    /* connect viewportManager to histogramWidget */
    connect(viewportManager, SIGNAL(mapUpdated(int,mapInfo*)), histogramControl, SLOT(updateMapInfo(int,mapInfo*)));
    connect(viewportManager, SIGNAL(mapClosed(int)), histogramControl, SLOT(unloadMapInfo(int)));
    connect(viewportManager, SIGNAL(histogramViewportsSelectedUpdated(QList<int>)), histogramControl, SLOT(updateHistogram(QList<int>)));

    /* connect viewportManager to mapOptions */
    connect(viewportManager, SIGNAL(mapClosed(int)), mapOptions, SLOT(unloadMapInfo(int)));
    connect(viewportManager, SIGNAL(mapUpdated(int,mapInfo*)), mapOptions, SLOT(updateMapInfo(int,mapInfo*)));
    connect(viewportManager, SIGNAL(viewportsSelectionUpdated(QList<int>)), mapOptions, SLOT(updateSelectedMaps(QList<int>)));
    connect(mapOptions, SIGNAL(optionsChanged(mapOptions*)), viewportManager, SLOT(applyMapOptions(mapOptions*)));

    /* connect viewportManager to workspace */
    connect(viewportManager, SIGNAL(viewportShowOn(int,MapViewport*)), workspace, SLOT(addViewport(int,MapViewport*)));
    connect(viewportManager, SIGNAL(viewportShowOff(int)), workspace, SLOT(removeViewport(int)));

    /* connect histogramWidget to viewportManager */
    connect(histogramControl, SIGNAL(histogramUpdated(ColorMap*, float,float)), viewportManager, SLOT(updateThresholds(ColorMap*, float,float)));

    /* connect mainwindow to viewportManager */
    connect(ui->actionOpen, SIGNAL(triggered()), viewportManager, SLOT(openFiles()));

    /* connect viewportManager to mainWindow */
    connect(viewportManager, SIGNAL(viewportsSelectionUpdated(QList<int>)), this, SLOT(viewportSelectionChanged(QList<int>)));

    /* connect workspace to mapOptions */
    connect(workspace, SIGNAL(textureNsideUpdated(int,int)), mapOptions, SLOT(updateTextureNside(int,int)));
    connect(workspace, SIGNAL(vectorsNsideUpdated(int,int)), mapOptions, SLOT(updateVectorsNside(int,int)));


    /* open application in fullscreen */
    QWidget::showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete workspace;

    /* delete faceCache */
    FaceCache* faceCache = FaceCache::instance(MIN_NSIDE, MAX_NSIDE);
    delete faceCache;
}


void MainWindow::showHistogram(bool show)
{
    ui->tabWidget->setCurrentIndex(0);
    /*
    if(show)
    {
        //ui->sidepane->show();
        QList<int> sizes;
        sizes.append(centralWidget()->width()-SIDEPANE_WIDTH);
        sizes.append(SIDEPANE_WIDTH);
        ui->splitter->setSizes(sizes);
        ui->tabWidget->setCurrentIndex(0);
    }
    else
    {
        ui->sidepane->hide();
    }
    */
}


void MainWindow::viewportSelectionChanged(QList<int> selectedViewports)
{
    if(selectedViewports.size()==0)
        disableButtons();
    else
        enableButtons();
}

void MainWindow::enableButtons()
{
    ui->action3Dview->setEnabled(true);
    ui->actionMollview->setEnabled(true);
    ui->actionReset->setEnabled(true);
    ui->actionSynchronize->setEnabled(true);
    ui->actionShowGrid->setEnabled(true);
    ui->actionPolarizationVectors->setEnabled(true);
}

void MainWindow::disableButtons()
{
    ui->action3Dview->setDisabled(true);
    ui->actionMollview->setDisabled(true);
    ui->actionReset->setDisabled(true);
    ui->actionSynchronize->setDisabled(true);
    ui->actionShowGrid->setDisabled(true);
    ui->actionPolarizationVectors->setDisabled(true);
}
