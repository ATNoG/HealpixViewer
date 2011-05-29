#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QSignalMapper>
#include "workspace.h"
#include "healpixmap.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* configure toolbar icons */
    QIcon openicon          = QIcon::fromTheme("document-open");
    QIcon saveicon          = QIcon::fromTheme("document-save");
    QIcon reseticon         = QIcon::fromTheme("edit-undo");
    QIcon view3dicon        = QIcon(":/images/icons/3dview.gif");
    QIcon mollviewicon      = QIcon(":/images/icons/mollview.gif");
    QIcon viewportsicon     = QIcon(":/images/icons/viewports.gif");
    QIcon synchronizeicon   = QIcon(":/images/icons/sync.gif");
    QIcon colormapicon      = QIcon(":/images/icons/colormap.gif");
    QIcon extracticon       = QIcon(":/images/icons/extract.gif");

    /* create actions */
    QAction* openAction         = new QAction(openicon, tr("&Open"), ui->toolBar);
    QAction* saveAction         = new QAction(saveicon, tr("&Save"), ui->toolBar);
    QAction* resetAction        = new QAction(reseticon, tr("&Reset"), ui->toolBar);
    QAction* view3DAction       = new QAction(view3dicon, tr("3D View"), ui->toolBar);
    QAction* mollviewAction     = new QAction(mollviewicon, tr("Mollview"), ui->toolBar);
    QAction* viewportsAction    = new QAction(viewportsicon, tr("Viewports"), ui->toolBar);
    QAction* synchronizeAction  = new QAction(synchronizeicon, tr("Synchronize Views"), ui->toolBar);
    QAction* colormapAction     = new QAction(colormapicon, tr("Change Colors"), ui->toolBar);
    QAction* extractAction      = new QAction(extracticon, tr("Extract"), ui->toolBar);

    /* add actions to toolbar */
    ui->toolBar->addAction(openAction);
    ui->toolBar->addAction(saveAction);
    ui->toolBar->addAction(resetAction);
    ui->toolBar->addAction(view3DAction);
    ui->toolBar->addAction(mollviewAction);
    ui->toolBar->addAction(viewportsAction);
    ui->toolBar->addAction(synchronizeAction);
    ui->toolBar->addAction(colormapAction);
    ui->toolBar->addAction(extractAction);

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
    viewportsAction->setMenu(menu);

    /* Configure workspace */

    /* with mdisubwindow */
    /*
    QMdiArea* workspace = new QMdiArea();
    setCentralWidget(workspace);

    QMdiSubWindow* test1 = new QMdiSubWindow(this, Qt::WindowCloseButtonHint);
    QMdiSubWindow* test2 = new QMdiSubWindow;
    QMdiSubWindow* test3 = new QMdiSubWindow;
    QMdiSubWindow* test4 = new QMdiSubWindow;
    QMdiSubWindow* test5 = new QMdiSubWindow;
    QMdiSubWindow* test6 = new QMdiSubWindow;
    QMdiSubWindow* test7 = new QMdiSubWindow;
    QMdiSubWindow* test8 = new QMdiSubWindow;
    QMdiSubWindow* test9 = new QMdiSubWindow;

    int width = workspace->width()/3;
    int height = workspace->height()/3;

    test1->setGeometry(0,0,width,height);
    test2->setGeometry(width,0,width,height);
    test3->setGeometry(2*width,0,width,height);
    test4->setGeometry(0,height,width,height);
    test5->setGeometry(width,height,width,height);
    test6->setGeometry(2*width,height,width,height);
    test7->setGeometry(0,2*height,width,height);
    test8->setGeometry(width,2*height,width,height);
    test9->setGeometry(2*width,2*height,width,height);
    workspace->addSubWindow(test1);
    workspace->addSubWindow(test2);
    workspace->addSubWindow(test3);
    workspace->addSubWindow(test4);
    workspace->addSubWindow(test5);
    workspace->addSubWindow(test6);
    workspace->addSubWindow(test7);
    workspace->addSubWindow(test8);
    workspace->addSubWindow(test9);

    test1->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test2->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test3->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test4->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test5->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test6->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test7->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test8->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    test9->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    */

    /* with workspace class */
    WorkSpace* workspace = new WorkSpace(this, DEFAULT_VIEWPORTS);
    setCentralWidget(workspace);


    //QHBoxLayout* layout = new QHBoxLayout;
    //setLayout(layout);

    //QDockWidget* dock1 = new QDockWidget;
    //layout->addWidget(dock1);

    //setCentralWidget(dock1);


    //HealpixMap map("map512.fits");


    /* configure events */

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
