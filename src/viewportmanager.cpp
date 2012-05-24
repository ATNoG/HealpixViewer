#include "viewportmanager.h"
#include "ui_viewportmanager.h"

ViewportManager::ViewportManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewportManager)
{
    ui->setupUi(this);
    defaultTitle = QString("HealpixMap %1");
    usedViewports = 0;
    viewportIdx = 0;
    shareWidget = new QGLWidget();

    /* setup ui */
    ui->treeViewports->setColumnWidth(0, 50);
    ui->treeViewports->setColumnWidth(1, 30);
    QStringList headers;
    headers << "" << "" << "Viewport";
    ui->treeViewports->setHeaderLabels(headers);
    ui->treeViewports->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeViewports->setSelectionBehavior(QAbstractItemView::SelectRows);

    /* connect signals */
    connect(ui->treeViewports, SIGNAL(itemSelectionChanged(void)), this, SLOT(selectionChanged(void)));
    connect(ui->treeViewports, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(histogramSelectionChanged(QTreeWidgetItem*,int)));
    connect(ui->btnOpen, SIGNAL(released()), this, SLOT(openFiles()));
    connect(ui->btnClose, SIGNAL(released()), this, SLOT(closeViewports()));
    connect(ui->mapFieldSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(mapFieldChanged(int)));

    /* connect signal mapper */
    signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(QWidget*)), this, SLOT(viewStateChanged(QWidget*)));
}

ViewportManager::~ViewportManager()
{
    #if DEBUG > 0
        qDebug() << "Calling ViewportManager destructor";
    #endif

    // TODO: Invalid read of size 4
    for(int i=0; i<viewports.size(); i++)
    {
        qDebug() << "Viewport " << viewports[i];
        delete viewports[i];
    }
    /*
    for(int i=0; i<mapsInformation.size(); i++)
    {
        delete[] mapsInformation[i]->values;
        delete mapsInformation[i];
    }
    */

    delete ui;
}


/* called when treeview selection changed */
void ViewportManager::selectionChanged(void)
{
    /* get selected viewports in treeview */
    QList<int> selectedViewports = getSelectedViewports();

    if(selectedViewports.size()==1)
    {
        currentSelectedViewport = selectedViewports[0];

        /* only one viewport selected, show field selection */
        // TODO: update field selector ?
        updateFieldSelector(currentSelectedViewport);
        ui->mapFieldSelector->setEnabled(true);
    }
    else
    {
        currentSelectedViewport = -1;

        /* disable field selection */
        ui->mapFieldSelector->setDisabled(true);
    }

    /* select/deselect viewports */
    for(int i=0; i<ui->treeViewports->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = ui->treeViewports->topLevelItem(i);
        int viewportId = item->data(0, Qt::UserRole).toInt();
        if(displayedViewports.contains(viewportId))
        {
            if(item->isSelected())
                viewports[viewportId]->selectViewport(true);
            else
                viewports[viewportId]->deselectViewport(true);
        }
    }

    emit(viewportsSelectionUpdated(selectedViewports));
}


/* called when treeview histogram selection changed */
void ViewportManager::histogramSelectionChanged(QTreeWidgetItem*, int)
{
    /* get selected viewports in treeview */
    QList<int> selectedViewports = getHistogramActiveViewports();

    /* emit signal to histogram update */
    qDebug() << "histogramViewportsSelectedUpdated";
    emit(histogramViewportsSelectedUpdated(selectedViewports));
}


/* called when view state changes */
void ViewportManager::viewStateChanged(QWidget *widget)
{
    QTreeWidgetItem *item = (QTreeWidgetItem*)widget;
    int viewportId = item->data(0, Qt::UserRole).toInt();
    bool view = ((QCheckBox*)ui->treeViewports->itemWidget(item, 1))->checkState();

    if(view)
        showViewport(viewportId);
    else
        hideViewport(viewportId);
}



/* PUBLIC SLOTS */


void ViewportManager::openFiles()
{
    QStringList fileNames;

    /* create the file dialog */
    //fileNames = QFileDialog::getOpenFileNames(this, tr("Choose fits file to open"), QString::null, tr("Fits Files (*.fits)"));

    QFileDialog dialog(this, tr("Choose fits file to open"), ".", tr("Fits Files (*.fits)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    if(dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        dialog.close();

        /* open that files */
        openFiles(fileNames);
    }
}


/* update threshold for the selected maps */
void ViewportManager::updateThresholds(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset)
{
    // TODO: if map is not in workspace update threshold too ?

    QList<int> viewportIds = getHistogramActiveViewports();
    for(int i=0; i<viewportIds.size(); i++)
    {
        int viewportId = viewportIds[i];
        // check if it exists
        if(viewports.contains(viewportId))
            viewports[viewportId]->updateThreshold(colorMap, min, max, sentinelColor, scale, factor, offset);
    }
}


void ViewportManager::viewportSelectionChanged(int viewportId, bool selected)
{
    /* viewport selection changed in workspace, so change the selection in treewidget */
    QList<QTreeWidgetItem *> items = ui->treeViewports->findItems(QString("HealpixMap %1").arg(viewportId), Qt::MatchExactly, 2);
    if(items.size()>0)
    {
        ui->treeViewports->blockSignals(true);
        items[0]->setSelected(selected);
        ui->treeViewports->blockSignals(false);
    }
}



/* PRIVATE SLOTS */


/* close all selected viewports */
void ViewportManager::closeViewports()
{
    QList<int> viewportIds = getSelectedViewports();

    if(viewportIds.size()==0)
    {
        QMessageBox::warning (this, "HealpixViewer", "No viewports selected");
    }
    else if(QMessageBox::question(this, "HealpixViewer", "Are you sure?", (QMessageBox::No | QMessageBox::Yes)) == QMessageBox::Yes)
    {
        for(int i=0; i<viewportIds.size(); i++)
        {
            closeViewport(viewportIds[i]);
        }
    }
}


/* handler for mapFieldSelector */
void ViewportManager::mapFieldChanged(int currentIdx)
{
    QVariant aux = ui->mapFieldSelector->itemData(currentIdx);
    HealpixMap::MapType field = getMapField(aux.toInt());

    if(currentSelectedViewport>=0)
    {
        updateMapField(currentSelectedViewport, field);
    }
}


/* called when viewport has new values */
void ViewportManager::updateMapInfo(int viewportId, mapInfo *info)
{
    if(viewports.contains(viewportId))
    {
        delete[] mapsInformation[viewportId]->values;
        delete mapsInformation[viewportId];

        mapsInformation[viewportId] = info;

        /* update mapField combobox */
        updateFieldSelector(viewportId);

        /* emit signal to update histogram */
        emit(mapUpdated(viewportId, info));
    }
}



/* PRIVATE */


/* open all selected files */
void ViewportManager::openFiles(QStringList filenames)
{
    foreach(QString fitsfile, filenames)
    {
        /* no viewports free to open new map */
        if(usedViewports!=MAXVIEWPORTS)
        {
            QString title = defaultTitle.arg(viewportIdx);


            MapViewport *viewport = new MapViewport(0, title, viewportIdx, shareWidget);
            bool opened = viewport->openMap(fitsfile);

            if(opened)
            {
                /* get map information */
                mapInfo *info = viewport->getMapInfo();
                viewports.insert(viewportIdx, viewport);
                mapsInformation.insert(viewportIdx, info);

                /* connect signal to update values after mapfield changed */
                connect(viewport, SIGNAL(mapFieldInfoChanged(int,mapInfo*)), this, SLOT(updateMapInfo(int,mapInfo*)));
                connect(viewport, SIGNAL(closed(int)), this, SLOT(closeViewport(int)));
                connect(viewport, SIGNAL(selectionChanged(int,bool)), this, SLOT(viewportSelectionChanged(int,bool)));

                /* emit signal to update info in histogram */
                emit(mapUpdated(viewportIdx, info));

                addViewportToList(viewportIdx, title, info);


                if(viewports.size()==1)
                {
                    QList<int> vports;
                    vports.append(viewportIdx);

                    // emit signal to histogram update
                    emit(viewportsSelectionUpdated(vports));
                }

                viewportIdx++;
                usedViewports++;
            }
            else
            {
                /* show warning dialog */
                QMessageBox::warning (this, "HealpixViewer", "Error opening map " + fitsfile);
            }
        }
        else
        {
            /* show warning dialog */
            QMessageBox::warning (this, "HealpixViewer", "No more viewports available to open a new map. Please close some map first");
        }
    }
}


/* return list of histogram checked viewports in treeview */
QList<int> ViewportManager::getHistogramActiveViewports()
{
    QList<int> checkedViewports;

    for(int i=0; i<ui->treeViewports->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item =  ui->treeViewports->topLevelItem(i);
        if(item->checkState(0)==Qt::Checked)
            checkedViewports.append(item->data(0, Qt::UserRole).toInt());
    }

    return checkedViewports;
}


/* return list of workspace checked viewports in treeview */
QList<int> ViewportManager::getWorkspaceActiveViewports()
{
    QList<int> selectedViewports;
    // TODO: implement
    return selectedViewports;
}


/* return list of selected viewports in treeview */
QList<int> ViewportManager::getSelectedViewports()
{
    QList<int> selectedViewports;
    QList<QTreeWidgetItem *> items = ui->treeViewports->selectedItems();

    for(int i=0; i<items.size(); i++)
    {
        selectedViewports.append(items[i]->data(0, Qt::UserRole).toInt());
    }

    return selectedViewports;
}


/* Update field selector with information about current map */
void ViewportManager::updateFieldSelector(int viewportId)
{
    ui->mapFieldSelector->blockSignals(true);

    // TODO: INFO
    mapInfo *info = mapsInformation[viewportId];
    QList<HealpixMap::MapType> availableFiels = info->availableFields;

    int selectedIndex = 0;
    ui->mapFieldSelector->clear();
    for(int i=0; i<availableFiels.size(); i++)
    {
        /* add item to combobox */
        ui->mapFieldSelector->insertItem(i, HealpixMap::mapTypeToString(availableFiels[i]), availableFiels[i]);
        if(availableFiels[i]==info->currentField)
            selectedIndex = i;
    }

    /* change current item to selected field */
    ui->mapFieldSelector->setCurrentIndex(selectedIndex);

    ui->mapFieldSelector->blockSignals(false);
}


/* update viewport to use selected field */
void ViewportManager::updateMapField(int viewportId, HealpixMap::MapType field)
{
    // TODO: check if its not the current field...
    if(viewports.contains(viewportId) && mapsInformation[viewportId]->currentField!=field)
    {
        mapsInformation[viewportId]->currentField = field;
        viewports[viewportId]->changeMapField(field);
    }
}


/* Show viewport in workspace */
void ViewportManager::showViewport(int viewportId)
{
    // TODO: signal should return a value, because if workspace doesnt have more space, the viewport still marked as in viewport !
    displayedViewports.insert(viewportId);
    emit(viewportShowOn(viewportId, viewports[viewportId]));
}


/* Hide viewport from workspace */
void ViewportManager::hideViewport(int viewporId)
{
    displayedViewports.remove(viewporId);
    emit(viewportShowOff(viewporId));
}


bool ViewportManager::isInWorkspace(int viewportId)
{
    return displayedViewports.contains(viewportId);
}


void ViewportManager::closeViewport(int viewportId)
{
    /* first remove from workspace if displayed */
    if(isInWorkspace(viewportId))
        hideViewport(viewportId);

    /* remove from treeview */
    removeViewportFromList(viewportId);

    /* emit signal to remove map from histogram */
    emit(mapClosed(viewportId));

    // TODO: check memory release

    /* delete viewport */
    delete viewports[viewportId];
    //delete[] mapsInformation[viewportId]->values;
    //delete mapsInformation[viewportId];

    /* deallocate map info */
    viewports.remove(viewportId);
    mapsInformation.remove(viewportId);

    // TODO: verify if viewportsSelectedUpdated emitted to histogram!
}



void ViewportManager::addViewportToList(int viewportId, QString title, mapInfo *info)
{
    /* add viewport to tree */
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(2, title);
    item->setData(0, Qt::UserRole, viewportId);
    item->setCheckState(0, Qt::Unchecked);
    ui->treeViewports->addTopLevelItem(item);

    QCheckBox *viewCheck = new QCheckBox;
    viewCheck->setStyleSheet("QCheckBox::indicator {width: 16px;height: 16px;} QCheckBox::indicator:checked { image: url(src/images/icons/view.gif);} QCheckBox::indicator:unchecked { image: url(src/images/icons/noview.gif);} ");
    ui->treeViewports->setItemWidget(item, 1, viewCheck);

    /* display viewport when opening */
    viewCheck->setChecked(true);
    showViewport(viewportId);

    connect(viewCheck, SIGNAL(clicked()), signalMapper, SLOT(map()));
    signalMapper->setMapping(viewCheck, (QWidget*)item);

    /* add information to map */
    mapsInformation[viewportId] = info;

    if(viewports.size()==1)
    {
        item->setCheckState(0, Qt::Checked);
    }

    // TODO: load map info of current map ?
    /*if(getCheckedViewports().size()<=1)
    {
        loadViewportInfo(viewportId);
    }
    */
}


/* remove viewport from treeview */
void ViewportManager::removeViewportFromList(int viewportId)
{
    QList<QTreeWidgetItem *> items = ui->treeViewports->findItems(QString("HealpixMap %1").arg(viewportId), Qt::MatchExactly, 2);
    if(items.size()>0)
        delete items[0];
}


/* apply map options */
void ViewportManager::applyMapOptions(mapOptions* options)
{
    QList<int> selectedViewports = getSelectedViewports();

    foreach(int viewportId, selectedViewports)
    {
        viewports[viewportId]->applyOptions(options);
    }
}
