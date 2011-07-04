#include "mapviewer.h"
#include <math.h>

using namespace qglviewer;

MapViewer::MapViewer(QWidget *parent) :
    QGLViewer(parent)
{
    skymap = NULL;
}

/* create the gl model for the map */
void MapViewer::draw()
{
    // Save the current model view matrix (not needed here in fact)
    glPushMatrix();

    // Multiply matrix to get in the frame coordinate system.
    glMultMatrixd(manipulatedFrame()->matrix());

    // Draw an axis using the QGLViewer static function
    //drawAxis(2.0);

    if(skymap!=NULL)
    {
        /* show texture */
        //skymap->drawMap();
    }

    //  glColor3f(1.0, 1.0, 1.0);

    // TODO: select what to draw
    tesselation->draw();

    // Restore the original (world) coordinate system
    glPopMatrix();
}


void MapViewer::loadMap(HealpixMap* map)
{
    skymap = map;
}


void MapViewer::init()
{
    /*
        General configuration.
    */
    setBackgroundColor(QColor(0,0,0));	// Black background.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*
        Define the lighting.
    */
    GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_zero[] = { 0.0, 0.0, 0.0, 1.0 };
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_zero);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_zero);

    /*
        Enable GL textures.
    */
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glEnable( GL_TEXTURE_2D );
                    // Nice texture coordinate interpolation
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


    setHandlerKeyboardModifiers(QGLViewer::FRAME,  Qt::NoModifier);


    /* initial X position of the camera */
    cameraPosition = 2.8;

    /*
        Configure the camera.
    */
    camera()->setPosition( Vec(cameraPosition,0,0) );
    camera()->lookAt( Vec(0,0,0) );
    camera()->setUpVector(Vec(0,0,1));

    //camera()->setOrientation(0, 0);


    /*qglviewer::CameraConstraint *constraint;
    constraint = new CameraConstraint(camera());
    constraint->setRotationConstraintType(AxisPlaneConstraint::FREE);
    camera()->frame()->setConstraint(constraint);*/

    setManipulatedFrame(new ManipulatedFrame());
    setAxisIsDrawn();

/*
    AxisPlaneConstraint* constraint = new LocalConstraint();
    manipulatedFrame()->setConstraint(constraint);
    Vec dir(0.0,0.0,1.0);
    constraint->setRotationConstraintType(AxisPlaneConstraint::AXIS);
    constraint->setRotationConstraintDirection(dir);*/


    /*
        Recover the state from a prior run.
    */
    //restoreStateFromFile();


    /* update initial state */
    currentNside = BASE_NSIDE;
    currentZoomLevel = MIN_ZOOM;

    /* create the sphere tesselation */
    tesselation = new Tesselation(currentNside, false);

    /* get face vertices for nside=1 to know the boundary vertices of face */
    // TODO: do this in tesselation ?
    for(int face=0; face<12; face++)
    {
        QVector<Strip> strips = FaceVertices::instance()->getFaceVertices(face, 1);

        QVector<Strip>::iterator stripIT;
        QVector<Vertice>::iterator verticeIT;

        //qDebug() << "size of strips = " << strips.size();

        int i=0;
        for(stripIT=strips.begin(); stripIT!=strips.end(); stripIT++)
        {
            //qDebug() << "  size of strip " << i << " = " << (*stripIT).size();
            for(verticeIT=(*stripIT).begin(); verticeIT!=(*stripIT).end(); verticeIT++)
            {
                //qDebug() << "   Vertice in position: " << (*verticeIT).x << ", " << (*verticeIT).y << ", " << (*verticeIT).z;
                faceBoundaries[face].append(Vec((*verticeIT).x, (*verticeIT).y, (*verticeIT).z));
            }
        }
        i++;
    }
}

void MapViewer::changeToMollview()
{

}

void MapViewer::changeTo3D()
{

}


void MapViewer::mousePressEvent(QMouseEvent* e)
{
    QGLViewer::mousePressEvent(e);
    emit(cameraChanged(e, MOUSEPRESS, this));
}


void MapViewer::mouseReleaseEvent(QMouseEvent *e)
{
    // TODO: just a hack to disable spinning... may have problems when selecting a pixel for example
    //QGLViewer::mouseReleaseEvent(e);
    //emit(cameraChanged(e, MOUSERELEASE, this));
    checkVisibility();
}

void MapViewer::mouseMoveEvent(QMouseEvent* e)
{
    QGLViewer::mouseMoveEvent(e);
    emit(cameraChanged(e, MOUSEMOVE, this));

    checkVisibility();
}


void MapViewer::checkVisibility()
{
    Vec v1, v2, v3, v4, v1b, v2b, v3b, v4b;
    bool v1back_hidden, v2back_hidden, v3back_hidden, v4back_hidden;

    visibleFaces.clear();

    qDebug("==================");

    for(int face=0; face<12; face++)
    {
        bool hidden = false;

        v1 = camera()->projectedCoordinatesOf(Vec(faceBoundaries[face][0].x, faceBoundaries[face][0].y, faceBoundaries[face][0].z));
        v2 = camera()->projectedCoordinatesOf(Vec(faceBoundaries[face][1].x, faceBoundaries[face][1].y, faceBoundaries[face][1].z));
        v3 = camera()->projectedCoordinatesOf(Vec(faceBoundaries[face][2].x, faceBoundaries[face][2].y, faceBoundaries[face][2].z));
        v4 = camera()->projectedCoordinatesOf(Vec(faceBoundaries[face][3].x, faceBoundaries[face][3].y, faceBoundaries[face][3].z));

        v1b = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][0].x, faceBoundaries[face][0].y, faceBoundaries[face][0].z));
        v2b = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][1].x, faceBoundaries[face][1].y, faceBoundaries[face][1].z));
        v3b = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][2].x, faceBoundaries[face][2].y, faceBoundaries[face][2].z));
        v4b = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][3].x, faceBoundaries[face][3].y, faceBoundaries[face][3].z));

        v1back_hidden =  v1b.x < INVISIBLE_X;
        v2back_hidden =  v2b.x < INVISIBLE_X;
        v3back_hidden =  v3b.x < INVISIBLE_X;
        v4back_hidden =  v4b.x < INVISIBLE_X;

        /* check if opposite vertice are hidden on left */
        if((v1.x < 0 && v4.x < 0) || (v2.x < 0 && v3.x < 0))
        {
            hidden = true;
            //qDebug() << "Face " << face << " hidden on left (opposite vertices)";
        }
        /* check if opposite vertice are hidden on right */
        else if((v1.x > width() && v4.x > width()) || (v2.x > width() && v3.x > width()))
        {
            hidden = true;
            //qDebug() << "Face " << face << " hidden on right (opposite vertices)";
        }
        /* check if opposite vertice are hidden on top */
        else if((v1.y < 0 && v4.y < 0) || (v2.y < 0 && v3.y < 0))
        {
            hidden = true;
            //qDebug() << "Face " << face << " hidden on top (opposite vertices)";
        }
        /* check if opposite vertice are hidden on bottom */
        else if((v1.y > height() && v4.y > height()) || (v2.y > height() && v3.y > height()))
        {
            hidden = true;
            //qDebug() << "Face " << face << " hidden on bottom (opposite vertices)";
        }
        /* check if all vertices are on backface */
        else if(v1back_hidden && v2back_hidden && v3back_hidden && v4back_hidden)
        {
            hidden = true;
            //qDebug() << "Face " << face << " hidden on backface";
        }

        if(!hidden)
            visibleFaces.append(face);
    }

    /* update tesselation with visible faces */
    tesselation->updateVisibleFaces(visibleFaces);
}


void MapViewer::wheelEvent(QWheelEvent *e)
{
    bool zoomChanged;

    if(e->delta()<0)
        zoomChanged = zoomIn();
    else
        zoomChanged = zoomOut();

    if(zoomChanged)
    {
        checkVisibility();

        //QGLViewer::wheelEvent(e);
        emit(cameraChanged(e, MOUSEWHEEL, this));
        updateGL();
    }
}


void MapViewer::resetView()
{
    // TODO: reset object position
    /* reset zoom to inital position */
    //resetZoom();

    tesselation->updateNside(1);

    /* redraw scene */
    updateGL();
}

void MapViewer::synchronize(QEvent *e, int type)
{
    switch(type)
    {
        case MOUSEMOVE:
            QGLViewer::mouseMoveEvent((QMouseEvent*) e);
            break;

        case MOUSEPRESS:
            QGLViewer::mousePressEvent((QMouseEvent*) e);
            break;

        case MOUSERELEASE:
            QGLViewer::mouseReleaseEvent((QMouseEvent*) e);
            break;

        case MOUSEWHEEL:
            QGLViewer::wheelEvent((QWheelEvent*) e);
            break;
    }
}


void MapViewer::resetZoom()
{
    currentZoomLevel = MIN_ZOOM;
    currentNside = BASE_NSIDE;
}


bool MapViewer::zoomIn()
{
    if(currentZoomLevel<MAX_ZOOM)
    {
        /* update camera position */
        cameraPosition -= CAMERA_ZOOM_INC;
        camera()->setPosition(Vec(cameraPosition, 0.0, 0.0));
        qDebug() << cameraPosition;

        currentZoomLevel++;
        int nextNside = zoomToNside(currentZoomLevel);
        //qDebug() << "current zoom level = " << currentZoomLevel;
        //qDebug() << "current nside = " << currentNside;
        qDebug() << "next nside = " << nextNside;
        if(nextNside!=currentNside)
        {
            currentNside = nextNside;
            qDebug() << "Nside changed to " << currentNside;
            tesselation->updateNside(currentNside);
        }
        return true;
    }
    return false;

}

bool MapViewer::zoomOut()
{
    if(currentZoomLevel>MIN_ZOOM)
    {
        /* update camera position */
        cameraPosition += CAMERA_ZOOM_INC;
        camera()->setPosition(Vec(cameraPosition, 0.0, 0.0));

        currentZoomLevel--;
        int nextNside = zoomToNside(currentZoomLevel);
        //qDebug() << "current zoom level = " << currentZoomLevel;
        //qDebug() << "current nside = " << currentNside;
        qDebug() << "next nside = " << nextNside;
        if(nextNside!=currentNside)
        {
            currentNside = nextNside;
            qDebug() << "Nside changed to " << currentNside;
            tesselation->updateNside(currentNside);
        }
        return true;
    }
    return false;
}


/* calculate the nside based on zoom level */
int MapViewer::zoomToNside(int zoomLevel)
{
    /* calculate the needed zoom (when changing zoomLevel, baseZoomLevel may not change */
    int baseZoomLevel = floor(zoomLevel / ZOOM_LEVEL);

    //qDebug() << "  zoomToNside: basezoomlevel=" << baseZoomLevel;

    // TODO: calculate exp based on BASE_NSIDE
    int nside = pow(2, EXP_NSIDE+baseZoomLevel);

    return nside;
}
