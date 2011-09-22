#include "workspace.h"

WorkSpace::WorkSpace(int numberViewports, QWidget *parent) :
    QWidget(parent)
{
    /* create the layout */
    gridlayout = new QGridLayout(this);
    gridlayout->setSpacing(0);
    gridlayout->setMargin(0);
    setLayout(gridlayout);

    /* configure the viewports wanted */
    this->numberViewports = 0;
    this->usedViewports = 0;
    configureWorkspace(numberViewports);

    this->setMinimumSize(350, 300);

    /* do not sync at begin */
    this->synchronize = false;
}


void WorkSpace::drawViewports(int oldnviewports, int newnviewports)
{
    int nhorizontal, nvertical;

    if(newnviewports==1)
    {
        nhorizontal=1; nvertical=1;
    }
    else if(newnviewports==4)
    {
        nhorizontal=2; nvertical=2;
    }
    else if(newnviewports==9)
    {
        nhorizontal=3; nvertical=3;
    }

    //qDebug() << " Redraw workspace for " << newnviewports << " viewports";
    //qDebug() << "  horizontal = " << nhorizontal;
    //qDebug() << "  vertical = " << nvertical;

    int pos = 0;

    /* remove all current viewports */
    for(int i=0; i<oldnviewports; i++)
    {
        gridlayout->removeWidget(viewports[i]);
        viewports[i]->setParent(NULL);
    }

    /* add new viewports */
    for (int i=0; i<nhorizontal; i++)
    {
        for (int j=0; j<nvertical; j++)
        {
            gridlayout->addWidget(viewports[pos], i, j);
            pos++;
        }
    }
}



void WorkSpace::openFiles(QStringList filenames)
{
    qDebug() << "Available viewports = " << viewports.count();

    // TODO: allow open multiple files at a time
    //foreach(QString fitsfile, filenames)
    //{
        // TODO: select in which viewport map will be opened
    //    viewports[i]->openMap(fitsfile);
    //    i++;
    //}

    if(numberViewports==usedViewports)
    {
        /* no viewports free to open new map. Verify if more viewports can be added */
        if(numberViewports!=MAX_VIEWPORTS)
        {
            if(numberViewports<4)
                configureWorkspace(4);
            else
                configureWorkspace(MAX_VIEWPORTS);
            qDebug("Configuring more viewport");
        }
        else
        {
            qDebug("All viewports in use");

            /* show warning dialog */
            QMessageBox::warning (this, "HealpixViewer", "No more viewports available to open a new map. Please close some map first");

            return;
        }
    }

    int i = 0;
    bool found = false;
    do
    {
        if(!viewports[i]->inUse())
        {
            /* open map in viewport */
            found = true;

            qDebug() << "Trying to load on viewport " << i;
            bool opened = viewports[i]->openMap(filenames[0]);

            if(opened)
            {
                usedViewports++;

                connect(viewports[i], SIGNAL(mapFieldChanged(int,float*,int)), this, SIGNAL(mapFieldChanged(int,float*,int)));

                /* emit signal */
                QString title = QString("Healpixmap %1").arg(i);
                emit(mapOpened(i, title, viewports[i]->getMapInfo()));
            }
            else
                qDebug() << "ERROR OPENING!!!!";
        }
        i++;
    }while(i<numberViewports && !found);
}


/* Slots */

void WorkSpace::configureWorkspace(int viewportsWanted)
{
    qDebug() << "Change to " << viewportsWanted << " viewports";

    /* save the current number of viewports */
    int oldviewports = this->numberViewports;

    // TODO: how many viewports allow ? 1, 2, 4, 9


    /* if more viewports are needed create them */
    int availableViewports = viewports.count();
    if(viewportsWanted > availableViewports)
    {
        int i, needed = (viewportsWanted-availableViewports);
        qDebug() << " need more " << needed << " viewports";
        for(i=0; i<needed; i++)
        {
            int viewportId = availableViewports + i;
            QString qs = QString("HealpixMap %1").arg(viewportId);

            /* create the viewport without parent */
            MapViewport* viewport = new MapViewport(NULL, qs, this, viewportId);
            viewports.append(viewport);
        }
    }

    /* update current number of viewports */
    this->numberViewports = viewportsWanted;

    drawViewports(oldviewports, viewportsWanted);
}

void WorkSpace::changeToMollview()
{
    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->isSelected())
            viewport->changeToMollview();
}

void WorkSpace::changeTo3D()
{
    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->isSelected())
            viewport->changeTo3D();
}


void WorkSpace::syncViewports(QEvent* e, int type, MapViewer* source)
{
    if(synchronize)
    {
        /* synchronize is enable, so emit the signal to be catched by the viewports */
        emit(syncNeeded(e, type, source));
    }
}


void WorkSpace::changeSynchronization(bool on)
{
    synchronize = on;
}


void WorkSpace::selectAllViewports()
{
    MapViewport* viewport;
    foreach(viewport, viewports)
    {
        if(viewport->inUse())
            viewport->selectViewport(true);
    }
}


void WorkSpace::deselectAllViewports()
{
    MapViewport* viewport;
    foreach(viewport, viewports)
        viewport->deselectViewport(true);
}


void WorkSpace::resetViewports()
{
    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->inUse() && viewport->isSelected())
            viewport->resetViewport();
}

void WorkSpace::showPolarizationVectors(bool show)
{
    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->inUse() && viewport->isSelected())
            viewport->showPolarizationVectors(show);
}

void WorkSpace::updateMapField(int viewportId, HealpixMap::MapType field)
{
    viewports[viewportId]->changeMapField(field);
}

void WorkSpace::updateThreshold(QList<int> viewportIds, float min, float max)
{
    for(int i=0; i<viewportIds.size(); i++)
    {
        int viewportId = viewportIds[i];

        /* check if it exists */
        if(viewportId<numberViewports)
        {
            viewports[viewportId]->updateThreshold(min, max);
        }
    }
}


void WorkSpace::showGrid(bool show)
{
    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->inUse() && viewport->isSelected())
            viewport->showGrid(show);
}
