#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSignalMapper>
#include "healpixmap.h"

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
    ui->actionSave->setIcon(saveicon);
    ui->actionReset->setIcon(reseticon);

    /* create viewport context menu */
    QMenu* menu = new QMenu;
    QAction* setviewport1 = new QAction("1", menu);
    QAction* setviewport4 = new QAction("4", menu);
    QAction* setviewport9 = new QAction("9", menu);

    /* group viewport actions in a group, to have exclusive checked option */
    QActionGroup* viewportsActionGroup = new QActionGroup(menu);
    viewportsActionGroup->addAction(setviewport1);
    viewportsActionGroup->addAction(setviewport4);
    viewportsActionGroup->addAction(setviewport9);
    viewportsActionGroup->setExclusive(true);

    /* set options as checkable */
    setviewport1->setCheckable(true);
    setviewport4->setCheckable(true);
    setviewport9->setCheckable(true);

    /* select the default viewports */
    switch(DEFAULT_VIEWPORTS)
    {
        case 1:
            setviewport1->setChecked(true);
            break;
        case 4:
            setviewport4->setChecked(true);
            break;
        case 9:
            setviewport9->setChecked(true);
            break;
    }

    /* add actions to menu */
    menu->addAction(setviewport1);
    menu->addAction(setviewport4);
    menu->addAction(setviewport9);
    ui->actionViewports->setMenu(menu);

    /* Configure workspace */
    workspace = new WorkSpace(this, DEFAULT_VIEWPORTS);
    setCentralWidget(workspace);


    HealpixMap* map1 = new HealpixMap("/home/zeux/Tese/3dviewer/code/Skyviewer-build-desktop/temponly.fits");
    HealpixMap* map2 = new HealpixMap("/home/zeux/Tese/3dviewer/code/Skyviewer-build-desktop/iqu.fits");
    HealpixMap* map3 = new HealpixMap("/home/zeux/Tese/3dviewer/code/Skyviewer-build-desktop/nside32_synfast.fits");



    /* configure events */
    connect(ui->action3Dview, SIGNAL(triggered()), workspace, SLOT(changeTo3D()));
    connect(ui->actionMollview, SIGNAL(triggered()), workspace, SLOT(changeToMollview()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFiles()));

    /* set viewports */
    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(setviewport1, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(setviewport4, SIGNAL(triggered()), signalMapper, SLOT(map()));
    connect(setviewport9, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(setviewport1, 1);
    signalMapper->setMapping(setviewport4, 4);
    signalMapper->setMapping(setviewport9, 9);
    connect(signalMapper, SIGNAL(mapped(int)), workspace, SLOT(configureWorkspace(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::openFiles()
{
    QStringList fileNames;

    /* create the file dialog */
    fileNames = QFileDialog::getOpenFileNames(this, tr("Choose fits file to open"), QString::null, tr("Fits Files (*.fits)"));

    /* ask workspace to open that files */
    workspace->openFiles(fileNames);
}
