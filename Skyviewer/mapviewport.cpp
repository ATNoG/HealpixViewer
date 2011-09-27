#include "mapviewport.h"
#include <QIcon>
#include <QAction>
#include "workspace.h"

MapViewport::MapViewport(QWidget *parent, QString title, int viewportId) :
    QWidget(parent)
{
    this->title = title;
    this->selected = false;  // by default viewport is not selected
    this->mollview = !DEFAULT_VIEW_3D;
    this->loaded = false;
    this->viewportId = viewportId;

    /* configure the user interface */
    configureUI();

    /* connect events */
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(updateSelection(bool)));
    connect(mapviewer, SIGNAL(mapFieldChanged(mapInfo*)), this, SLOT(viewportUpdated(mapInfo*)));

    signalMapper = new QSignalMapper(this);
}


MapViewport::~MapViewport()
{
    closeMap();
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

    mapviewer = new MapViewer(this);
    mapviewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);   // allow viewer area to expand

    /* create toolbar */
    QToolBar *toolbar = new QToolBar();
    //toolbar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    toolbar->layout()->setMargin(0);
    toolbar->layout()->setSpacing(0);

    /* create toolbar actions */
    QAction *actionClose = new QAction(QIcon::fromTheme("window-close"), "Close Map", toolbar);
    QAction *actionReset = new QAction(QIcon::fromTheme("edit-undo"), "Reset view", toolbar);
    actionMaximize = new QAction(QIcon::fromTheme("view-fullscreen"), "Fullscreen", toolbar);
    actionRestore = new QAction(QIcon::fromTheme("view-restore"), "Restore window", toolbar);
    QAction *actionField = new QAction(QIcon(":/field.gif"), "Map Field", toolbar);
    QAction *actionGrid = new QAction(QIcon(":/grid.gif"), "Show Grid", toolbar);
    QAction *actionPvectors = new QAction(QIcon(":/pol-vectors.gif"), "Show Polarization Vectors", toolbar);
    actionSync = new QAction(QIcon(":/sync.gif"), "Sync", toolbar);
    actionGrid->setCheckable(true);
    actionPvectors->setCheckable(true);
    actionSync->setCheckable(true);

    /* connect actions to buttons */
    connect(actionGrid, SIGNAL(triggered(bool)), this, SLOT(showGrid(bool)));
    connect(actionPvectors, SIGNAL(triggered(bool)), this, SLOT(showPolarizationVectors(bool)));
    connect(actionReset, SIGNAL(triggered()), this, SLOT(resetViewport()));
    connect(actionMaximize, SIGNAL(triggered()), this, SLOT(maximize()));
    connect(actionRestore, SIGNAL(triggered()), this, SLOT(restore()));
    connect(actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionSync, SIGNAL(triggered(bool)), this, SLOT(enableSynchronization(bool)));

    /* create field selector menu */
    fieldMenu = new QMenu;

    /* add actions to toolbars */
    toolbar->addAction(actionGrid);
    toolbar->addAction(actionPvectors);
    toolbar->addAction(actionField);
    toolbar->addAction(actionReset);
    toolbar->addAction(actionSync);
    toolbar->addAction(actionMaximize);
    toolbar->addAction(actionRestore);
    toolbar->addAction(actionClose);

    /* hide restore from toolbar */
    actionRestore->setVisible(false);

    /* add actions to menu */
    actionField->setMenu(fieldMenu);

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

    /* load the map into viewport */
    bool _loaded = mapviewer->loadMap(fitsfile);

    if(_loaded)
    {
        mapviewer->setDisabled(false);

        /* viewport is now in use */
        loaded = true;

        /* fill map field with available fields */
        fillMapField();
    }

    return _loaded;
}


void MapViewport::closeMap()
{
    // TODO: ask if really wanna close, etc...
    // TODO: mapviewer should be deleted, or just load the file into the current mapview ?
    if(mapviewer!=NULL)
        delete mapviewer;
}


/* Slots */
void MapViewport::updateSelection(bool selected)
{
    if(selected)
        selectViewport(false);
    else
        deselectViewport(false);
}



void MapViewport::synchronizeView(QEvent *event, int type, MapViewer* source)
{
    if(mapviewer!=source)
    {
        mapviewer->synchronize(event, type);
    }
}


void MapViewport::showPolarizationVectors(bool show)
{
    mapviewer->showPolarizationVectors(show);
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
    mapviewer->showGrid(show);
}




void MapViewport::fillMapField()
{
    QActionGroup* fieldsActionGroup = new QActionGroup(fieldMenu);

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
    actionSync->setChecked(on);
    if(on)
    {
        /* connect signals */
        /* listen for object moved in a viewport */
        connect(mapviewer, SIGNAL(cameraChanged(QEvent*, int, MapViewer*)), workspace, SLOT(syncViewports(QEvent*, int, MapViewer*)));
        /* listen for sync needed, to force viewports to update */
        connect(workspace, SIGNAL(syncNeeded(QEvent*,int, MapViewer*)), this, SLOT(synchronizeView(QEvent*, int, MapViewer*)));
    }
    else
    {
        /* disconnect signals */
        disconnect(mapviewer, SIGNAL(cameraChanged(QEvent*, int, MapViewer*)), workspace, SLOT(syncViewports(QEvent*, int, MapViewer*)));
        disconnect(workspace, SIGNAL(syncNeeded(QEvent*, int, MapViewer*)), this, SLOT(synchronizeView(QEvent*, int, MapViewer*)));
    }
}

void MapViewport::disconnectFromWorkspace()
{
    enableSynchronization(false);
}
