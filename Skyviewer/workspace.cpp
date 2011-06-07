#include "workspace.h"

WorkSpace::WorkSpace(QWidget *parent, int numberViewports) :
    QWidget(parent)
{
    /* create the layout */
    gridlayout = new QGridLayout(this);
    gridlayout->setSpacing(0);
    setLayout(gridlayout);

    /* configure the viewports wanted */
    this->numberViewports = 0;
    configureWorkspace(numberViewports);

    this->setMinimumSize(350, 300);
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
    QString fitsfile;
    int i = 0;
    qDebug() << "Available viewports = " << viewports.count();
    foreach(fitsfile, filenames)
    {
        // TODO: select in which viewport map will be opened
        viewports[i]->openMap(fitsfile);
        i++;
    }
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
            QString qs1;
            qs1.setNum(availableViewports + i);
            QString qs = "HealpixMap " + qs1;

            /* create the viewport without parent */
            MapViewport* viewport = new MapViewport(NULL, qs, this);
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


void WorkSpace::syncViewportsMouseMove(QMouseEvent* e)
{
    qDebug() << "MouseMove!";

    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->isSelected())
            viewport->synchronize(e, 1);
}


void WorkSpace::syncViewportsMousePress(QMouseEvent* e)
{
    qDebug() << "MousePress!";

    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->isSelected())
            viewport->synchronize(e, 0);
}
