#include "mapviewport.h"
#include <QIcon>
#include <QAction>
#include "workspace.h"

MapViewport::MapViewport(QWidget *parent, QString title, int viewportId, const QGLWidget* shareWidget) :
    QWidget(parent)
{
    this->title = title;
    this->selected = false;  // by default viewport is not selected
    this->mollview = !DEFAULT_VIEW_3D;
    this->loaded = false;
    this->viewportId = viewportId;
    this->fieldsActionGroup = NULL;
    this->info = NULL;
    this->shareWidget = shareWidget;
    this->hasPolarization = false;
    this->synchronized = false;

    actAsSource = false;
    actAsDestination = false;

    /* configure the user interface */
    configureUI();

    /* connect events */
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(updateSelection(bool)));
    connect(mapviewer, SIGNAL(mapFieldChanged(mapInfo*)), this, SLOT(viewportUpdated(mapInfo*)));

    signalMapper = new QSignalMapper(this);
}


MapViewport::~MapViewport()
{
    #if DEBUG > 0
        qDebug() << "Calling MapVieweport destructor";
    #endif

    if(fieldsActionGroup!=NULL)
        delete fieldsActionGroup;
    if(info!=NULL)
    {
        delete[] info->values;
        delete info;
    }
    delete fieldMenu;
}


void MapViewport::setWorkspace(WorkSpace *workspace)
{
    this->workspace = workspace;

    connect(this, SIGNAL(restored()), workspace, SLOT(restore()));
    connect(this, SIGNAL(maximized(int)), workspace, SLOT(maximize(int)));
}


void MapViewport::configureUI()
{
    /* create the layouts */
    titlebarlayout = new QHBoxLayout;
    titlebarlayout->setContentsMargins(6, 0, 6, 0);
    titlebarlayout->setSpacing(0);
    vboxlayout = new QVBoxLayout;
    vboxlayout->setSpacing(0);
    vboxlayout->setMargin(5);   // set spacing between viewports
    setLayout(vboxlayout);

    /* create titlebar widget */
    titlewidget = new QWidget;
    titlewidget->setObjectName("TitleWidget");
    titlewidget->setStyleSheet(QString("#TitleWidget {background-color: #c3c3c3; border-top-right-radius: 9px; border-top-left-radius: 9px; }"));
    titlewidget->setMinimumHeight(24);
    titlewidget->setMaximumHeight(30);
    titlewidget->setLayout(titlebarlayout);

    /* create the widgets: title, and viewer area */
    QLabel* lbltitle = new QLabel;
    lbltitle->setText(title);

    mapviewer = new MapViewer(this, shareWidget);
    mapviewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);   // allow viewer area to expand

    /* create toolbar */
    QToolBar *toolbar = new QToolBar();
    //toolbar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    toolbar->layout()->setMargin(0);
    toolbar->layout()->setSpacing(0);

    /* create toolbar actions */
    QAction *actionClose = new QAction(QIcon(":/close.gif"), "Close Map", toolbar);
    QAction *actionReset = new QAction(QIcon(":/reset.gif"), "Reset view", toolbar);
    actionMaximize = new QAction(QIcon(":/fullscreen.gif"), "Fullscreen", toolbar);
    actionRestore = new QAction(QIcon(":/restore.gif"), "Restore window", toolbar);
    QAction *actionField = new QAction(QIcon(":/mollview.gif"), "Map Field", toolbar);
    actionGrid = new QAction(QIcon(":/grid.gif"), "Show Grid", toolbar);
    actionPvectors = new QAction(QIcon(":/pol-vectors.gif"), "Show Polarization Vectors", toolbar);
    actionSync = new QAction(QIcon(":/sync.gif"), "Sync", toolbar);
    action3D = new QAction(QIcon(":/3dview.gif"), "3D", toolbar);
    actionMollweide = new QAction(QIcon(":/mollview.gif"), "Mollweide", toolbar);
    QAction *actionActAs = new QAction("Act as", toolbar);

    actionGrid->setCheckable(true);
    actionPvectors->setCheckable(true);
    actionSync->setCheckable(true);
    actionPvectors->setEnabled(false);

    /* connect actions to buttons */
    connect(actionGrid, SIGNAL(triggered(bool)), this, SLOT(showGrid(bool)));
    connect(actionPvectors, SIGNAL(triggered(bool)), this, SLOT(showPolarizationVectors(bool)));
    connect(actionReset, SIGNAL(triggered()), this, SLOT(resetViewport()));
    connect(actionMaximize, SIGNAL(triggered()), this, SLOT(maximize()));
    connect(actionRestore, SIGNAL(triggered()), this, SLOT(restore()));
    connect(actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionSync, SIGNAL(triggered(bool)), this, SLOT(enableSynchronization(bool)));
    connect(action3D, SIGNAL(triggered()), this, SLOT(changeTo3D()));
    connect(actionMollweide, SIGNAL(triggered()), this, SLOT(changeToMollview()));

    connect(mapviewer, SIGNAL(textureNsideUpdated(int)), this, SLOT(updateOptionTextureNside(int)));
    connect(mapviewer, SIGNAL(vectorsNsideUpdated(int)), this, SLOT(updateOptionVectorsNside(int)));

    /* create field selector menu */
    fieldMenu = new QMenu;

    /* create "act as" menu */
    actAsMenu = new QMenu;

    /* add actions to toolbars */
    toolbar->addAction(action3D);
    toolbar->addAction(actionMollweide);
    toolbar->addAction(actionGrid);
    toolbar->addAction(actionPvectors);
    toolbar->addAction(actionActAs);
    toolbar->addAction(actionField);
    toolbar->addAction(actionReset);
    toolbar->addAction(actionSync);
    toolbar->addAction(actionMaximize);
    toolbar->addAction(actionRestore);
    toolbar->addAction(actionClose);

    /* hide restore from toolbar */
    actionRestore->setVisible(false);

    if(mollview)
        actionMollweide->setVisible(false);
    else
        action3D->setVisible(false);

    /* add actions to menu */
    actionField->setMenu(fieldMenu);
    actionActAs->setMenu(actAsMenu);

    fillActAsMenu();

    checkbox = new QCheckBox;
    //checkbox->setEnabled(true);
    toolbar->addWidget(checkbox);


    //toolbar->setMaximumHeight(18);
    toolbar->setIconSize(QSize(12, 12));

    /* add widgets */
    titlebarlayout->addWidget(lbltitle);
    titlebarlayout->addWidget(toolbar, 0, Qt::AlignRight);
    //titlebarlayout->addWidget(checkbox, 0, Qt::AlignRight);
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

void MapViewport::syncPixelSelection(std::set<int> pixels, int nside, bool add)
{
    mapviewer->updateSelection(pixels, nside, add);
}

void MapViewport::selectViewport(bool changeCheckbox)
{
    if(!isSelected())
    {
        selected = true;

        /* if function called by workspace, change the checkbox status */
        if(changeCheckbox)
            checkbox->setChecked(true);

        /* change titlebar color */
        // TODO: dont rewrite all properties again
        titlewidget->setStyleSheet(QString("#TitleWidget {background-color: %1; border-top-right-radius: 9px; border-top-left-radius: 9px; }").arg(COLOR_SELECTED));

        emit(selectionChanged(viewportId, true));
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
    titlewidget->setStyleSheet(QString("#TitleWidget {background-color: %1; border-top-right-radius: 9px; border-top-left-radius: 9px; }").arg(COLOR_INACTIVE));

    emit(selectionChanged(viewportId, false));
}

void MapViewport::changeToMollview()
{
    //if(!mollview)
    //{
        mapviewer->changeToMollview();
        mollview = true;
        actionMollweide->setVisible(false);
        action3D->setVisible(true);
    //}
}

void MapViewport::changeTo3D()
{
    //if(mollview)
    //{
        #if DEBUG > 0
            qDebug() << "Changing " << title << " to 3D";
        #endif
        mapviewer->changeTo3D();
        mollview = false;
        actionMollweide->setVisible(true);
        action3D->setVisible(false);
    //}
}

void MapViewport::resetViewport()
{
    mapviewer->resetView();
}


bool MapViewport::openMap(QString fitsfile, HealpixMap::MapType type)
{
    qDebug() << "Opening " << fitsfile << " on " << title;

    /* load the map into viewport */
    bool _loaded = mapviewer->loadMap(fitsfile, type);

    if(_loaded)
    {
        mapviewer->setDisabled(false);

        /* viewport is now in use */
        loaded = true;

        /* fill map field with available fields */
        fillMapField();

        mapInfo *info = getMapInfo();
        if(info->hasPolarization)
        {
            hasPolarization = true;
            actionPvectors->setEnabled(true);
        }
    }

    return _loaded;
}


/* Slots */
void MapViewport::updateSelection(bool selected)
{
    if(selected)
        selectViewport(false);
    else
        deselectViewport(false);
}


void MapViewport::updateCameraPosition(float position, MapViewer *viewer)
{
    if(mapviewer!=viewer)
    {
        mapviewer->updateCameraPosition(position);
    }
}


void MapViewport::updatePosition(Vec position, MapViewer *viewer)
{
    if(mapviewer!=viewer)
    {
        mapviewer->updatePosition(position);
    }
}


void MapViewport::updateRotation(Quaternion rotation, MapViewer *viewer)
{
    if(mapviewer!=viewer)
    {
        mapviewer->updateRotation(rotation);
    }
}


void MapViewport::updateKeyPress(QKeyEvent *e, MapViewer *viewer)
{
    if(mapviewer!=viewer)
    {
        mapviewer->updateKeyPress(e);
    }
}


void MapViewport::showPolarizationVectors(bool show)
{
    if(hasPolarization)
    {
        actionPvectors->setChecked(show);
        mapviewer->showPolarizationVectors(show);
    }
}


void MapViewport::updateThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset)
{
    //qDebug() << "Updating threshold from " << title << " to " << min << "," << max;
    mapviewer->updateThreshold(colorMap, min, max, sentinelColor, scale, factor, offset);
}


mapInfo* MapViewport::getMapInfo()
{
    if(info==NULL)
        info = mapviewer->getMapInfo();
    return info;
}


void MapViewport::changeMapField(HealpixMap::MapType field)
{
    updateMapField(field);
    mapviewer->changeMapField(field);
}


void MapViewport::changeMapField(int field)
{
    HealpixMap::MapType type = (HealpixMap::MapType)field;
    //updateMapField(type);
    mapviewer->changeMapField(type);
}


void MapViewport::viewportUpdated(mapInfo *info)
{
    emit(mapFieldInfoChanged(viewportId, info));
}

void MapViewport::showGrid(bool show)
{
    actionGrid->setChecked(show);
    mapviewer->showGrid(show);
}

void MapViewport::fillActAsMenu()
{
    actAsActionGroup = new QActionGroup(actAsMenu);

    QAction* actionSelSource = new QAction("Source", actAsMenu);
    actionSelSource->setCheckable(true);

    QAction* actionSelDestination = new QAction("Destination", actAsMenu);
    actionSelDestination->setCheckable(true);

    actAsActionGroup->addAction(actionSelSource);
    actAsActionGroup->addAction(actionSelDestination);

    actAsMenu->addAction(actionSelSource);
    actAsMenu->addAction(actionSelDestination);

    actAsActionGroup->setExclusive(true);

    connect(actionSelSource, SIGNAL(triggered()), this, SLOT(activeSelectionAsSource()));
    connect(actionSelDestination, SIGNAL(triggered()), this, SLOT(activeSelectionAsDestination()));
}


void MapViewport::activeSelectionAsSource()
{
    actAsSource = true;
    actAsDestination = false;
}

void MapViewport::activeSelectionAsDestination()
{
    actAsSource = false;
    actAsDestination = true;
}

void MapViewport::fillMapField()
{
    fieldsActionGroup = new QActionGroup(fieldMenu);

    mapInfo *info = getMapInfo();
    QList<HealpixMap::MapType> availableFields = info->availableFields;

    for(int i=0; i<availableFields.size(); i++)
    {
        QAction* action = new QAction(HealpixMap::mapTypeToString(availableFields[i]), fieldMenu);
        action->setCheckable(true);

        /* group viewport actions in a group, to have exclusive checked option */
        fieldsActionGroup->addAction(action);
        fieldMenu->addAction(action);

        /* connect signals */
        connect(action, SIGNAL(triggered()), signalMapper, SLOT(map()));
        signalMapper->setMapping(action, availableFields[i]);

        mapFieldsActions[availableFields[i]] = action;
    }

    fieldsActionGroup->setExclusive(true);

    updateMapField(info->currentField);

    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(changeMapField(int)));
}


void MapViewport::updateMapField(HealpixMap::MapType field)
{
    QAction *action = mapFieldsActions[field];
    action->setChecked(true);
}


void MapViewport::maximize()
{
    actionMaximize->setVisible(false);
    actionRestore->setVisible(true);
    emit(maximized(viewportId));
}

void MapViewport::restore()
{
    actionMaximize->setVisible(true);
    actionRestore->setVisible(false);
    emit(restored());
}

void MapViewport::close()
{
    if(QMessageBox::question(this, "HealpixViewer", "Are you sure?", (QMessageBox::No | QMessageBox::Yes)) == QMessageBox::Yes)
        emit(closed(viewportId));
}

void MapViewport::enableSynchronization(bool on)
{
    int currentProjection = workspace->getProjectionToSyncTo();

    if(currentProjection==1 && mollview)
        changeTo3D();
    else if(currentProjection==2 && !mollview)
        changeToMollview();

    synchronized = on;
    actionSync->setChecked(on);

    if(on)
    {
        resetViewport();

        /* connect signals */
        connect(mapviewer, SIGNAL(signalZoomChanged(float,MapViewer*)), workspace, SLOT(syncZoom(float,MapViewer*)));
        connect(mapviewer, SIGNAL(signalPositionChanged(Vec,MapViewer*)), workspace, SLOT(syncPosition(Vec,MapViewer*)));
        connect(mapviewer, SIGNAL(signalRotationChanged(Quaternion,MapViewer*)), workspace, SLOT(syncRotation(Quaternion,MapViewer*)));
        connect(mapviewer, SIGNAL(signalKeyPressed(QKeyEvent*,MapViewer*)), workspace, SLOT(syncKeyPress(QKeyEvent*,MapViewer*)));
        connect(mapviewer, SIGNAL(signalSelectionChanged(std::set<int>,int,bool)), this, SLOT(pixelSelectionChanged(std::set<int>,int,bool)));

        connect(workspace, SIGNAL(signalSyncZoom(float,MapViewer*)), this, SLOT(updateCameraPosition(float,MapViewer*)));
        connect(workspace, SIGNAL(signalSyncPosition(Vec,MapViewer*)), this, SLOT(updatePosition(Vec,MapViewer*)));
        connect(workspace, SIGNAL(signalSyncRotation(Quaternion,MapViewer*)), this, SLOT(updateRotation(Quaternion,MapViewer*)));
        connect(workspace, SIGNAL(signalSyncKeyPress(QKeyEvent*,MapViewer*)), this, SLOT(updateKeyPress(QKeyEvent*,MapViewer*)));
    }
    else
    {
        /* disconnect signals */
        disconnect(mapviewer, SIGNAL(signalZoomChanged(float,MapViewer*)), workspace, SLOT(syncZoom(float,MapViewer*)));
        disconnect(mapviewer, SIGNAL(signalPositionChanged(Vec,MapViewer*)), workspace, SLOT(syncPosition(Vec,MapViewer*)));
        disconnect(mapviewer, SIGNAL(signalRotationChanged(Quaternion,MapViewer*)), workspace, SLOT(syncRotation(Quaternion,MapViewer*)));
        disconnect(mapviewer, SIGNAL(signalKeyPressed(QKeyEvent*,MapViewer*)), workspace, SLOT(syncKeyPress(QKeyEvent*,MapViewer*)));
        disconnect(mapviewer, SIGNAL(signalSelectionChanged(std::set<int>,int,bool,MapViewer*)), workspace, SLOT(syncPixelSelection(std::set<int>,int,bool,MapViewer*)));

        disconnect(workspace, SIGNAL(signalSyncZoom(float,MapViewer*)), this, SLOT(updateCameraPosition(float,MapViewer*)));
        disconnect(workspace, SIGNAL(signalSyncPosition(Vec,MapViewer*)), this, SLOT(updatePosition(Vec,MapViewer*)));
        disconnect(workspace, SIGNAL(signalSyncRotation(Quaternion,MapViewer*)), this, SLOT(updateRotation(Quaternion,MapViewer*)));
        disconnect(workspace, SIGNAL(signalSyncKeyPress(QKeyEvent*,MapViewer*)), this, SLOT(updateKeyPress(QKeyEvent*,MapViewer*)));
    }
}

void MapViewport::pixelSelectionChanged(std::set<int> pixels, int nside, bool add)
{
    if(actAsSource)
        workspace->syncPixelSelection(pixels, nside, add);
}

bool MapViewport::isSelectionSource()
{
    return actAsSource;
}

bool MapViewport::isSelectionDestination()
{
    return actAsDestination;
}

void MapViewport::disconnectFromWorkspace()
{
    enableSynchronization(false);
}


void MapViewport::applyOptions(mapOptions *options)
{
    mapviewer->applyOptions(options);
}


void MapViewport::applyGridOptions(gridOptions *options)
{
    mapviewer->applyGridOptions(options);
}

void MapViewport::applyCoordSys(HealpixMap::Coordsys coordIn, HealpixMap::Coordsys coordOut)
{
    mapviewer->applyCoordSys(coordIn, coordOut);
}


void MapViewport::updateOptionTextureNside(int nside)
{
    emit(textureNsideUpdated(nside, viewportId));
}

void MapViewport::updateOptionVectorsNside(int nside)
{
    emit(vectorsNsideUpdated(nside, viewportId));
}

bool MapViewport::isSynchronized()
{
    return synchronized;
}

bool MapViewport::isMollweide()
{
    return mollview;
}
