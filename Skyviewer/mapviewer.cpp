#include "mapviewer.h"
#include <math.h>

using namespace qglviewer;

MapViewer::MapViewer(QWidget *parent) :
    QGLViewer(parent)
{
    skymap = NULL;
    initialized = false;
    predictCamera = NULL;

    /* get face cache */
    faceCache = FaceCache::instance();

    QObject::connect(faceCache, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );
    //QObject::connect(faceCache, SIGNAL(newFaceAvailable()), this, SLOT(checkForUpdates()) );
    //QObject::connect(faceCache, SIGNAL(newFaceAvailable(Face*)), this, SLOT(preloadFace(Face*)) );


    /* launch thread to wait for new updates to faces available */
    //QtConcurrent::run(this, &MapViewer::checkForUpdates);
}

/* create the gl model for the map */
void MapViewer::draw()
{
    if(initialized)
    {
        // Save the current model view matrix (not needed here in fact)
        glPushMatrix();

        // Multiply matrix to get in the frame coordinate system.
        glMultMatrixd(manipulatedFrame()->matrix());

        // Draw an axis using the QGLViewer static function
        //drawAxis(2.0);

        // TODO: select what to draw
        tesselation->draw();

        // Restore the original (world) coordinate system
        glPopMatrix();
    }
}


/*
void MapViewer::loadMap(HealpixMap* map)
{
    skymap = map;
}
*/


void MapViewer::loadMap(QString fitsfile)
{
    qDebug() << "Loading on mapviewer: " << fitsfile;

    if(!initialized)
    {
        /* create the sphere tesselation */
        tesselation = new Tesselation(currentNside, false);

        /* preload next faces */
        preloadFaces();

        initialized = true;
    }
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
    glEnable(GL_TEXTURE_2D);
    /*
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glEnable( GL_TEXTURE_2D );
    */
                    // Nice texture coordinate interpolation
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);


    setHandlerKeyboardModifiers(QGLViewer::FRAME,  Qt::NoModifier);


    /* initial X position of the camera */
    cameraPosition = 2.8;

    /*
        Configure the camera.
    */
    camera()->setPosition( Vec(cameraPosition,0,0) );
    camera()->lookAt( Vec(0,0,0) );
    camera()->setUpVector(Vec(0,0,1));

    /*qglviewer::CameraConstraint *constraint;
    constraint = new CameraConstraint(camera());
    constraint->setRotationConstraintType(AxisPlaneConstraint::FREE);
    camera()->frame()->setConstraint(constraint);*/

    setManipulatedFrame(new ManipulatedFrame());

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

    QString test;
    loadMap(test);
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
    //qDebug("mouse release!!!!!!!!!!!!!!!");
    //checkVisibility();
    preloadFaces();
}


void MapViewer::mouseMoveEvent(QMouseEvent* e)
{
    QGLViewer::mouseMoveEvent(e);
    //emit(cameraChanged(e, MOUSEMOVE, this));

    checkVisibility();
}


void MapViewer::wheelEvent(QWheelEvent *e)
{
    //qDebug("==================================");
    bool zoomChanged;

    if(e->delta()<0)
        zoomChanged = zoomIn();
    else
        zoomChanged = zoomOut();

    /*
    if(zoomChanged)
    {
        updateGL();
        checkVisibility();
        //emit(cameraChanged(e, MOUSEWHEEL, this));
        updateGL();
    }
    */
}


void MapViewer::checkVisibility()
{
    visibleFaces.clear();

    qDebug("--------------");

    /*
    Vec a,b;
    qDebug() << "Viewport witdh: " << width();
    qDebug() << "Viewport height: " << height();
    a = manipulatedFrame()->inverseCoordinatesOf(Vec(0, 1.0, 0));
    b = camera()->projectedCoordinatesOf(Vec(a.x, a.y, a.z));
    qDebug() << "Top left corner: (" << b.x << "," << b.y << ")";
    */

    /* compute matrixs because camera could be changed */
    camera()->computeModelViewMatrix();
    camera()->computeProjectionMatrix();

    for(int face=0; face<12; face++)
    {
        bool hidden = false;

        Vec verticesCoords[4];
        verticesCoords[0] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][0].x, faceBoundaries[face][0].y, faceBoundaries[face][0].z));
        verticesCoords[1] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][1].x, faceBoundaries[face][1].y, faceBoundaries[face][1].z));
        verticesCoords[2] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][2].x, faceBoundaries[face][2].y, faceBoundaries[face][2].z));
        verticesCoords[3] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][3].x, faceBoundaries[face][3].y, faceBoundaries[face][3].z));

        Vec projectedVerticesVP[4];
        projectedVerticesVP[0] = camera()->projectedCoordinatesOf(Vec(verticesCoords[0].x, verticesCoords[0].y, verticesCoords[0].z));
        projectedVerticesVP[1] = camera()->projectedCoordinatesOf(Vec(verticesCoords[1].x, verticesCoords[1].y, verticesCoords[1].z));
        projectedVerticesVP[2] = camera()->projectedCoordinatesOf(Vec(verticesCoords[2].x, verticesCoords[2].y, verticesCoords[2].z));
        projectedVerticesVP[3] = camera()->projectedCoordinatesOf(Vec(verticesCoords[3].x, verticesCoords[3].y, verticesCoords[3].z));

        bool verticeHidden[4];
        verticeHidden[0] = verticesCoords[0].x < INVISIBLE_X;
        verticeHidden[1] = verticesCoords[1].x < INVISIBLE_X;
        verticeHidden[2] = verticesCoords[2].x < INVISIBLE_X;
        verticeHidden[3] = verticesCoords[3].x < INVISIBLE_X;

        QList<Vec> outOfViewport;
        int totalBack = 0;
        int totalOut  = 0;


        for(int v=0; v<4; v++)
        {
            //if(face==4 && v==1)
                //qDebug() << "Pos on viewport (cam) = " << projectedVerticesVP[v].x << ", " << projectedVerticesVP[v].y;
            if(!verticeHidden[v])
            {
                /* vertice is not on backface */
                /* check if vertice is inside viewport */
                if(!(projectedVerticesVP[v].x>0 && projectedVerticesVP[v].x<width() && projectedVerticesVP[v].y>0 && projectedVerticesVP[v].y<height()))
                {
                    totalOut++;
                    outOfViewport.push_front(projectedVerticesVP[v]);
                }
            }
            else
                totalBack++;
        }

        //qDebug() << "Face " << face << " - Vertice on back: " << totalBack << ", not in back but out of viewport: " << totalOut;

        /* out only count number of vertices out that are not on backface */

        if(totalBack==4)
            hidden = true;
        else if(totalBack==3 && totalOut>=1)
            hidden = true;
        else if(totalBack>=2 && totalOut>=2)
        {
            //qDebug() << "Face " << face << " - check inside";
            if(outOfViewport[0].x<outOfViewport[1].x)
            {
                hidden = !faceIsInside(outOfViewport[0].x, outOfViewport[0].y, outOfViewport[1].x, outOfViewport[1].y, width(), height());
                //qDebug() << "Face " << face << " - Check if inside (" << outOfViewport[0].x << "," << outOfViewport[0].y << ") (" << outOfViewport[1].x << "," << outOfViewport[1].y << ")";
            }
            else
            {
                hidden = !faceIsInside(outOfViewport[1].x, outOfViewport[1].y, outOfViewport[0].x, outOfViewport[0].y, width(), height());
                //qDebug() << "Face " << face << " - Check if inside (" << outOfViewport[1].x << "," << outOfViewport[1].y << ") (" << outOfViewport[0].x << "," << outOfViewport[0].y << ")";
            }
            /*
            float dx = fabs(outOfViewport[0].x-outOfViewport[1].x)/2;
            float dy = fabs(outOfViewport[0].y-outOfViewport[1].y)/2;

            //if(dx>dy)
                if(outOfViewport[0].x < -dx || outOfViewport[1].x < -dx || outOfViewport[0].x > width()+dx || outOfViewport[1].x > width()+dx)
                    if(outOfViewport[0].y < -dy || outOfViewport[1].y < -dy || outOfViewport[0].y > height()+dy || outOfViewport[1].y > height()+dy)
                        hidden = true;
                    //hidden = true;
            */

        }

        if(!hidden)
        {
            qDebug() << "Face " << face << " visible";
            visibleFaces.append(face);
        }
    }

    /* update tesselation with visible faces */
    tesselation->updateVisibleFaces(visibleFaces);
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

        currentZoomLevel++;
        int nextNside = zoomToNside(currentZoomLevel);
        //qDebug() << "current zoom level = " << currentZoomLevel;
        //qDebug() << "current nside = " << currentNside;
        //qDebug() << "next nside = " << nextNside;

        /* check visibilty (after apply the zoom, some face can be no longer visible */
        checkVisibility();

        /* check if need to update nside */
        if(nextNside!=currentNside)
        {
            currentNside = nextNside;
            tesselation->updateNside(currentNside);
        }

        /* refresh the view */
        updateGL();

        /* calculate faces to preload */
        preloadFaces();

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
        //qDebug() << "next nside = " << nextNside;

        /* check visibilty (after apply the zoom, some face can be no longer visible */
        checkVisibility();

        if(nextNside!=currentNside)
        {
            currentNside = nextNside;
            tesselation->updateNside(currentNside);
        }

        /* refresh the view */
        updateGL();

        /* calculate faces to preload */
        preloadFaces();

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


void MapViewer::checkForUpdates(bool cleanCache)
{
    //QGLContext* newContext = new QGLContext(format(),this);


    //qDebug("check for updates");
    updateGL();

    // TODO: try to clean cache from thread...
    if(cleanCache)
        faceCache->cleanCache();
    /*
    Face* f = NULL;

    while(true)
    {
        f = faceCache->waitForUpdates();

        if(f!=NULL)
        {
            qDebug() << "New face available on cache";
            // new face is available on cache
            // create display list for face
            f->createDisplayList();
            // force update
            updateGL();
        }
    }
    */
}


bool MapViewer::faceIsInside(float ax, float ay, float bx, float by, float width, float height)
{
    bool inside = true;
    float tl_x, tl_y, tr_x, tr_y, bl_x, bl_y, br_x, br_y;

    tl_x = tl_y = 0;
    tr_x = width; tr_y = 0;
    bl_x = 0; bl_y = height;
    br_x = width; br_y = height;

    /*
    qDebug() << "Face points: (" << ax << "," << ay << "), (" << bx << "," << by << ")";
    qDebug() << "Top Left Corner: " << tl_x << "," << tl_y;
    qDebug() << "Top Right Corner: " << tr_x << "," << tr_y;
    qDebug() << "Bottom Left Corner: " << bl_x << "," << bl_y;
    qDebug() << "Bottom Right Corner: " << br_x << "," << br_y;
    */

    /* check top left corner */
    if(ax<0 && by<0)
    //if(ax<0 && ay>0 && bx>0 && by<0)
    //if(ax<0 && ay>0 && ay<height && bx>0 && by<0)
    {
        //qDebug("Check top left corner");
        if(((bx - ax)*(tl_y - ay) - (by - ay)*(tl_x - ax)) > 0)
        {
            inside = false;
            //qDebug("out on top left corner");
        }
    }


    /* check top right corner */
    else if(ay<0 && bx>width)
    //else if(ax<width && ay<0 && bx>width && by>0)
    //else if(ax>0 && ax<width && ay<0 && bx>width && bx>0)
    {
        //qDebug("Check top right corner");
        if(((bx - ax)*(tr_y - ay) - (by - ay)*(tr_x - ax)) >= 0)
        {
            inside = false;
            //qDebug("out on top right corner");
        }
    }

    /* check bottom left corner */
    else if(ax<0 && by>height)
    //else if(ax<0 && ay<height && bx>0 && by>height)
    //else if(ax<0 && ay>0 && ay<height && bx>0 && by>height)
    {
        //qDebug("Check bottom left corner");
        if(((bx - ax)*(bl_y - ay) - (by - ay)*(bl_x - ax)) < 0)
        {
            inside = false;
            //qDebug("out on bottom left corner");
        }
    }


    /* check bottom right corner */
    else if(ay>height && bx>width)
    //else if(ax<width && ay>height && bx>width && by<height)
    //else if(ax>0 && ax<width && ay>height && bx>width && by>0 && by<height)
    {
        //qDebug("Check bottom right corner");
        if(((bx - ax)*(br_y - ay) - (by - ay)*(br_x - ax)) < 0)
        {
            inside = false;
            //qDebug("out on bottom right corner");
        }
    }

    return inside;
}



void MapViewer::preloadFaces()
{
    if(PRELOAD_FACES)
    {
        /* zoomlevel that changed nside */
        if(currentZoomLevel % 2 == 0 && currentNside!=MAX_NSIDE)
        {
            float camPosition = cameraPosition-2*CAMERA_ZOOM_INC;
            int nextNside = currentNside*2;

            QVector<int> facesToPreload;

            for(int face=0; face<12; face++)
            {
                bool hidden = false;

                if(predictCamera!=NULL)
                    delete predictCamera;
                predictCamera = new Camera(*camera());
                predictCamera->setPosition(Vec(camPosition, 0.0, 0.0));
                predictCamera->setScreenWidthAndHeight(width(), height());
                predictCamera->computeProjectionMatrix();
                predictCamera->computeModelViewMatrix();

                Vec verticesCoords[4];
                verticesCoords[0] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][0].x, faceBoundaries[face][0].y, faceBoundaries[face][0].z));
                verticesCoords[1] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][1].x, faceBoundaries[face][1].y, faceBoundaries[face][1].z));
                verticesCoords[2] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][2].x, faceBoundaries[face][2].y, faceBoundaries[face][2].z));
                verticesCoords[3] = manipulatedFrame()->inverseCoordinatesOf(Vec(faceBoundaries[face][3].x, faceBoundaries[face][3].y, faceBoundaries[face][3].z));

                Vec projectedVerticesVP[4];
                projectedVerticesVP[0] = predictCamera->projectedCoordinatesOf(Vec(verticesCoords[0].x, verticesCoords[0].y, verticesCoords[0].z));
                projectedVerticesVP[1] = predictCamera->projectedCoordinatesOf(Vec(verticesCoords[1].x, verticesCoords[1].y, verticesCoords[1].z));
                projectedVerticesVP[2] = predictCamera->projectedCoordinatesOf(Vec(verticesCoords[2].x, verticesCoords[2].y, verticesCoords[2].z));
                projectedVerticesVP[3] = predictCamera->projectedCoordinatesOf(Vec(verticesCoords[3].x, verticesCoords[3].y, verticesCoords[3].z));

                bool verticeHidden[4];
                verticeHidden[0] = verticesCoords[0].x < INVISIBLE_X;
                verticeHidden[1] = verticesCoords[1].x < INVISIBLE_X;
                verticeHidden[2] = verticesCoords[2].x < INVISIBLE_X;
                verticeHidden[3] = verticesCoords[3].x < INVISIBLE_X;

                QList<Vec> outOfViewport;
                int totalBack = 0;
                int totalOut  = 0;


                for(int v=0; v<4; v++)
                {
                    //if(face==4 && v==1)
                        //qDebug() << "Pos on viewport (predict) = " << projectedVerticesVP[v].x << ", " << projectedVerticesVP[v].y;
                    if(!verticeHidden[v])
                    {
                        /* vertice is not on backface */
                        /* check if vertice is inside viewport */
                        if(!(projectedVerticesVP[v].x>0 && projectedVerticesVP[v].x<width() && projectedVerticesVP[v].y>0 && projectedVerticesVP[v].y<height()))
                        {
                            totalOut++;
                            outOfViewport.push_front(projectedVerticesVP[v]);
                        }
                    }
                    else
                        totalBack++;
                }

                /* out only count number of vertices out that are not on backface */
                if(totalBack==4)
                    hidden = true;
                else if(totalBack==3 && totalOut>=1)
                    hidden = true;
                else if(totalBack>=2 && totalOut>=2)
                {
                    if(outOfViewport[0].x<outOfViewport[1].x)
                        hidden = !faceIsInside(outOfViewport[0].x, outOfViewport[0].y, outOfViewport[1].x, outOfViewport[1].y, width(), height());
                    else
                        hidden = !faceIsInside(outOfViewport[1].x, outOfViewport[1].y, outOfViewport[0].x, outOfViewport[0].y, width(), height());
                }

                if(!hidden)
                {
                    //qDebug() << "Preload face " << face << " with nside " << nextNside;
                    facesToPreload.append(face);
                }

            }

            tesselation->preloadFaces(facesToPreload, nextNside);
        }
    }
}
