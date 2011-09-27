#include "workspace.h"
#include <QHBoxLayout>

WorkSpace::WorkSpace(QWidget *parent) :
    QWidget(parent)
{
    /* create the layout */
    layout = new QGridLayout();
    setLayout(layout);

    numberViewports = 0;

    layout->setSpacing(0);
    layout->setMargin(0);

    //this->setMinimumSize(350, 300);

    /* do not sync at begin */
    this->synchronize = false;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


/* Slots */

/* add viewport to workspace */
void WorkSpace::addViewport(int viewportId, MapViewport *viewport)
{
    if(numberViewports!=MAX_WORKSPACE_VIEWPORTS)
    {
        viewport->setWorkspace(this);
        viewports.insert(viewportId, viewport);
        numberViewports++;

        reorganizeLayout();
    }
    else
    {
        /* show warning dialog */
        QMessageBox::warning (this, "HealpixViewer", "No more viewports can be displayed in workspace, hide some viewport first");
    }
}


/* remove viewport from workspace */
void WorkSpace::removeViewport(int viewportId)
{
    if(viewports.contains(viewportId))
    {
        qDebug() << "Remove viewport from workspace " << viewportId;

        viewports[viewportId]->disconnectFromWorkspace();
        viewports[viewportId]->setParent(0);
        viewports.remove(viewportId);
        numberViewports--;

        reorganizeLayout();
    }
}


/* Change all selected maps in workspace to mollview  */
void WorkSpace::changeToMollview()
{
    MapViewport* viewport;
    foreach(viewport, viewports)
        if(viewport->isSelected())
            viewport->changeToMollview();
}


/* Change all selected maps in workspace to 3Dview */
void WorkSpace::changeTo3D()
{
    MapViewport* viewport;
    foreach(viewport, viewports.values())
        if(viewport->isSelected())
            viewport->changeTo3D();
}


/* need to sync viewports */
void WorkSpace::syncViewports(QEvent* e, int type, MapViewer* source)
{
    //if(synchronize)
    //{
        /* synchronize is enable, so emit the signal to be catched by the viewports */
        emit(syncNeeded(e, type, source));
    //}
}


/* enable/disable synchronization */
void WorkSpace::changeSynchronization(bool on)
{
    //synchronize = on;
    MapViewport* viewport;
    foreach(viewport, viewports.values())
        if(viewport->isSelected())
            viewport->enableSynchronization(on);
}


/* select all viewports in workspace */
void WorkSpace::selectAllViewports()
{
    MapViewport* viewport;
    foreach(viewport, viewports.values())
    {
        viewport->selectViewport(true);
    }
}


/* deselect all viewports in workspace */
void WorkSpace::deselectAllViewports()
{
    MapViewport* viewport;
    foreach(viewport, viewports.values())
        viewport->deselectViewport(true);
}


/* reset position in all viewports */
void WorkSpace::resetViewports()
{
    MapViewport* viewport;
    foreach(viewport, viewports.values())
        if(viewport->isSelected())
            viewport->resetViewport();
}


/* show/hide polarization vectors in selected viewports */
void WorkSpace::showPolarizationVectors(bool show)
{
    MapViewport* viewport;
    foreach(viewport, viewports.values())
        if(viewport->isSelected())
            viewport->showPolarizationVectors(show);
}


/* show/hide grid in selected viewports */
void WorkSpace::showGrid(bool show)
{
    MapViewport* viewport;
    foreach(viewport, viewports.values())
        if(viewport->isSelected())
            viewport->showGrid(show);
}



/* PRIVATE */

/*
void WorkSpace::addWidgetToLayout(QWidget* widget)
{

    layout->addWidget(widget, row, col, rspan, cspan);
}

void WorkSpace::removeWidgetFromLayout(QWidget* widget)
{
    layout->removeWidget(widget);
}
*/












void WorkSpace::calculateViewportPositions(int totalViewports)
{
    int leftViewports = totalViewports;

    double viewportsPerCol;
    int viewportsAllocated, side, row, position;

    QList<int> columnsPerRow;
    QSet<int> nCols;

    row = 0;
    position = 0;

    while(leftViewports>0 && row<5)
    {
        viewportsPerCol = sqrt(leftViewports);
        side = (int)ceil(viewportsPerCol);


        if(leftViewports%side==0)
        {
            viewportsAllocated = leftViewports;
            int rows = leftViewports/side;

            for(int i=0; i<rows; i++)
            {
                for(int col=0; col<side; col++)
                {
                    viewportPosition *pos = new viewportPosition;
                    pos->row = row;
                    pos->col = col;
                    pos->cspan = 1;
                    viewportPositions[position] = pos;
                    position++;
                }

                columnsPerRow.append(side);
                row++;
            }
        }
        else
        {
            viewportsAllocated = side;

            for(int col=0; col<side; col++)
            {
                viewportPosition *pos = new viewportPosition;
                pos->row = row;
                pos->col = col;
                pos->cspan = 1;
                viewportPositions[position] = pos;
                position++;
            }

            columnsPerRow.append(side);
            row++;
        }

        // calculate number of viewports needed to be allocated
        leftViewports = leftViewports-viewportsAllocated;
    }

    /* calculate mmc */
    nCols = columnsPerRow.toSet();
    int previousMMC, actualMMC;


    if(nCols.size()>1)
    {
        QList<int> nColsList = nCols.toList();

        /* need to calculate cols spans */
        actualMMC = previousMMC = mmc(nColsList[0], nColsList[1]);

        for(int i=2; i<nColsList.size(); i++)
        {
            actualMMC = mmc(nColsList[i], previousMMC);
        }

        int span = 1;

        /* udpate cols and span */
        for(int i=0; i<totalViewports; i++)
        {
            viewportPosition *pos = viewportPositions[i];
            span = actualMMC/columnsPerRow[pos->row];
            pos->cspan = span;
            pos->col = pos->col * span;
        }
    }
}





void WorkSpace::reorganizeLayout()
{
    /* remove all widgets from layout */
    MapViewport *viewport;
    foreach(viewport, viewports.values())
    {
        layout->removeWidget(viewport);
    }

    /* calculate positions of viewports */
    calculateViewportPositions(numberViewports);

    /* put widgets again in layout */
    int i=0;
    foreach(viewport, viewports.values())
    {
        viewportPosition *pos = viewportPositions[i];
        layout->addWidget(viewport, pos->row, pos->col, 1, pos->cspan);

        /* remove position info */
        delete pos;
        viewportPositions.remove(i);

        i++;
    }
}


int WorkSpace::mmc(int a, int b)
{
    int aa, bb, remain;
    aa = a;
    bb = b;

    while(bb!=0)
    {
        remain = aa%bb;
        aa = bb;
        bb = remain;
    }

    return (a*b/aa);
}



void WorkSpace::maximize(int viewportId)
{
    /* remove actual viewports */
    MapViewport *viewport;
    foreach(viewport, viewports.values())
    {
        viewport->setParent(0);
        layout->removeWidget(viewport);
    }

    /* add only selected widget */
    layout->addWidget(viewports[viewportId], 0, 0);
}

void WorkSpace::restore()
{
    reorganizeLayout();
}
