#include "mapviewport.h"
#include <QIcon>
#include <QAction>
#include "workspace.h"

MapViewport::MapViewport(QWidget *parent, QString title, WorkSpace* _workspace) :
    QWidget(parent)
{
    this->title = title;
    this->selected = false;  // by default viewport is not selected
    this->mollview = !DEFAULT_VIEW_3D;
    this->workspace = _workspace;

    /* configure the user interface */
    configureUI();

    /* connect events */
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(selectionChanged(bool)));
}


void MapViewport::configureUI()
{
    /* create the layouts */
    titlebarlayout = new QHBoxLayout;
    titlebarlayout->setMargin(6);
    vboxlayout = new QVBoxLayout;
    vboxlayout->setSpacing(0);
    vboxlayout->setMargin(5);   // set spacing between viewports
    setLayout(vboxlayout);

    /* create titlebar widget */
    titlewidget = new QWidget;
    titlewidget->setStyleSheet(QString("background-color: #c3c3c3; border-top-right-radius: 9px; border-top-left-radius: 9px; "));
    titlewidget->setMinimumHeight(24);
    titlewidget->setMaximumHeight(30);
    titlewidget->setLayout(titlebarlayout);

    /* create the widgets: title, checkbox and viewer area */
    QLabel* lbltitle = new QLabel;
    lbltitle->setText(title);
    checkbox = new QCheckBox;
    mapviewer = new MapViewer(this);
    mapviewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);   // allow viewer area to expand

    /* add widgets */
    titlebarlayout->addWidget(lbltitle);
    titlebarlayout->addWidget(checkbox, 0, Qt::AlignRight);
    vboxlayout->addWidget(titlewidget);
    vboxlayout->addWidget(mapviewer);

    /* configure the size policy */
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


void MapViewport::loadHealpixMap(QString path)
{
    /* create new healpixmap */
    //healpixMap(path);

    // TODO: verify if no map is currently loaded
}


bool MapViewport::isSelected()
{
    return selected;
}

void MapViewport::selectViewport()
{
    selected = true;

    /* change titlebar color */
    // TODO: dont rewrite all properties again
    titlewidget->setStyleSheet(QString("background-color: %1; border-top-right-radius: 9px; border-top-left-radius: 9px; ").arg(COLOR_SELECTED));

    /* connect signals */
    /* listen for object moved in a viewport */
    connect(mapviewer, SIGNAL(cameraChanged(QEvent*, int, MapViewer*)), workspace, SLOT(syncViewports(QEvent*, int, MapViewer*)));
    /* listen for sync needed, to force viewports to update */
    connect(workspace, SIGNAL(syncNeeded(QEvent*,int, MapViewer*)), this, SLOT(synchronizeView(QEvent*, int, MapViewer*)));
}

void MapViewport::deselectViewport()
{
    selected = false;

    /* change titlebar color */
    // TODO: dont rewrite all properties again
    titlewidget->setStyleSheet(QString("background-color: %1; border-top-right-radius: 9px; border-top-left-radius: 9px; ").arg(COLOR_INACTIVE));

    /* disconnect signals */
    disconnect(mapviewer, SIGNAL(cameraChanged(QEvent*, int, MapViewer*)), workspace, SLOT(syncViewports(QEvent*, int, MapViewer*)));
    disconnect(workspace, SIGNAL(syncNeeded(QEvent*, int, MapViewer*)), this, SLOT(synchronizeView(QEvent*, int, MapViewer*)));
}

void MapViewport::changeToMollview()
{
    if(!mollview)
    {
        qDebug() << "Changing " << title << " to Mollview";
        mapviewer->changeToMollview();
        mollview = true;
    }
}

void MapViewport::changeTo3D()
{
    if(mollview)
    {
        qDebug() << "Changing " << title << " to 3D";
        mapviewer->changeTo3D();
        mollview = false;
    }
}


void MapViewport::openMap(QString fitsfile)
{
    qDebug() << "Opening " << fitsfile << " on " << title;

    /* close the current map */
    //closeMap();

    /* create new map */
    HealpixMap* map = new HealpixMap(fitsfile);
}


void MapViewport::closeMap()
{
    // TODO: ask if really wanna close, etc...
    // TODO: mapviewer should be deleted, or just load the file into the current mapview ?
    if(mapviewer!=NULL)
        delete mapviewer;
}


/* Slots */
void MapViewport::selectionChanged(bool selected)
{
    if(selected)
        selectViewport();
    else
        deselectViewport();
}



void MapViewport::synchronizeView(QEvent *event, int type, MapViewer* source)
{
    if(isSelected() && mapviewer!=source)
        mapviewer->synchronize(event, type);
}
