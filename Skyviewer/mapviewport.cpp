#include "mapviewport.h"
#include <QIcon>
#include <QAction>
#include "workspace.h"

MapViewport::MapViewport(QWidget *parent, QString title, WorkSpace* _workspace, int viewportId) :
    QWidget(parent)
{
    this->title = title;
    this->selected = false;  // by default viewport is not selected
    this->mollview = !DEFAULT_VIEW_3D;
    this->workspace = _workspace;
    this->loaded = false;
    this->viewportId = viewportId;

    /* configure the user interface */
    configureUI();

    /* connect events */
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(selectionChanged(bool)));
    connect(mapviewer, SIGNAL(mapFieldChanged(float*,int)), this, SLOT(viewportUpdated(float*,int)));
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

    /* create the widgets: title, and viewer area */
    QLabel* lbltitle = new QLabel;
    lbltitle->setText(title);
    mapviewer = new MapViewer(this);
    mapviewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);   // allow viewer area to expand

    /* create toolbar */
    /*
    QToolBar *toolbar = new QToolBar();
    QAction *open = new QAction(QIcon::fromTheme("document-open"), "Open Map", toolbar);
    QAction *close = new QAction(QIcon::fromTheme("window-close"), "Close Map", toolbar);
    QAction *reset = new QAction(QIcon::fromTheme("edit-undo"), "Reset view", toolbar);
    QAction *maximize = new QAction(QIcon::fromTheme("view-fullscreen"), "Fullscreen", toolbar);
    QAction *restore = new QAction(QIcon::fromTheme("view-restore"), "Restore window", toolbar);
    toolbar->addAction(open);
    toolbar->addAction(close);
    toolbar->addAction(reset);
    toolbar->addAction(maximize);
    toolbar->addAction(restore);
    toolbar->addSeparator();
    */
    checkbox = new QCheckBox;
    checkbox->setEnabled(false);
    //toolbar->addWidget(checkbox);

    /* add widgets */
    titlebarlayout->addWidget(lbltitle);
    //titlebarlayout->addWidget(progressBar);
    //titlebarlayout->addWidget(toolbar, 0, Qt::AlignRight);
    titlebarlayout->addWidget(checkbox, 0, Qt::AlignRight);
    vboxlayout->addWidget(titlewidget);
    vboxlayout->addWidget(mapviewer);

    /* configure the size policy */
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* disable mapviewer */
    mapviewer->setDisabled(true);
}

bool MapViewport::isSelected()
{
    return selected;
}

void MapViewport::selectViewport(bool changeCheckbox)
{
    if(inUse() && !isSelected())
    {
        selected = true;

        /* if function called by workspace, change the checkbox status */
        if(changeCheckbox)
            checkbox->setChecked(true);

        /* change titlebar color */
        // TODO: dont rewrite all properties again
        titlewidget->setStyleSheet(QString("background-color: %1; border-top-right-radius: 9px; border-top-left-radius: 9px; ").arg(COLOR_SELECTED));

        /* connect signals */
        /* listen for object moved in a viewport */
        connect(mapviewer, SIGNAL(cameraChanged(QEvent*, int, MapViewer*)), workspace, SLOT(syncViewports(QEvent*, int, MapViewer*)));
        /* listen for sync needed, to force viewports to update */
        connect(workspace, SIGNAL(syncNeeded(QEvent*,int, MapViewer*)), this, SLOT(synchronizeView(QEvent*, int, MapViewer*)));

        //mapviewer->resetView();
    }
}

void MapViewport::deselectViewport(bool changeCheckbox)
{
    selected = false;

    /* if function called by workspace, change the checkbox status */
    if(changeCheckbox)
        checkbox->setChecked(false);

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

void MapViewport::resetViewport()
{
    mapviewer->resetView();
}


bool MapViewport::openMap(QString fitsfile)
{
    qDebug() << "Opening " << fitsfile << " on " << title;

    /* close the current map */
    //closeMap();    

    /* load the map into viewport */
    bool _loaded = mapviewer->loadMap(fitsfile);

    if(_loaded)
    {
        mapviewer->setDisabled(false);

        /* viewport is now in use */
        loaded = true;

        checkbox->setEnabled(true);

        /* set viewport as selected */
        selectViewport(true);

        showHistogram();
    }

    return _loaded;
}


void MapViewport::closeMap()
{
    // TODO: ask if really wanna close, etc...
    // TODO: mapviewer should be deleted, or just load the file into the current mapview ?
    //if(mapviewer!=NULL)
        //delete mapviewer;
}


/* Slots */
void MapViewport::selectionChanged(bool selected)
{
    if(selected)
        selectViewport(false);
    else
        deselectViewport(false);
}



void MapViewport::synchronizeView(QEvent *event, int type, MapViewer* source)
{
    if(isSelected() && mapviewer!=source)
    {
        mapviewer->synchronize(event, type);
    }
}


bool MapViewport::inUse()
{
    return loaded;
}


void MapViewport::showHistogram()
{
    // TODO
}


void MapViewport::showPolarizationVectors(bool show)
{
    if(inUse())
    {
        mapviewer->showPolarizationVectors(show);
    }
}


void MapViewport::updateThreshold(float min, float max)
{
    //qDebug() << "Updating threshold from " << title << " to " << min << "," << max;
    mapviewer->updateThreshold(min, max);
}


mapInfo* MapViewport::getMapInfo()
{
    return mapviewer->getMapInfo();
}


void MapViewport::changeMapField(HealpixMap::MapType field)
{
    mapviewer->changeMapField(field);
}


void MapViewport::viewportUpdated(float* values, int nValues)
{
    emit(mapFieldChanged(viewportId, values, nValues));
}

void MapViewport::showGrid(bool show)
{
    mapviewer->showGrid(show);
}
