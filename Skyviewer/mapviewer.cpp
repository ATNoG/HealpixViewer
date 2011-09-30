#include "mapviewer.h"
#include "mapviewport.h"
#include <math.h>

#include "grid.h"

using namespace qglviewer;

MapViewer::MapViewer(QWidget *parent, const QGLWidget* shareWidget) :
    QGLViewer(parent, shareWidget)
{
    skymap = NULL;
    initialized = false;
    predictCamera = NULL;

    maxNside = MAX_NSIDE;

    this->textureCache = NULL;
    this->tesselation = NULL;
    this->faceCache = NULL;
    this->overlayCache = NULL;
    this->healpixMap = NULL;
    this->displayInfo = true;
    this->constraint = NULL;
    this->mollweide = !DEFAULT_VIEW_3D;
    this->showPVectors = false;
}

MapViewer::~MapViewer()
{
    qDebug() << "Calling MapViewer destructor";
    delete currentManipulatedFrame;
    if(tesselation!=NULL)
        delete tesselation;
    if(textureCache!=NULL)
        delete textureCache;
    if(overlayCache!=NULL)
        delete overlayCache;
    if(healpixMap!=NULL)
        delete healpixMap;

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

        tesselation->draw();

        // Restore the original (world) coordinate system
        glPopMatrix();

        int zoom = floor(fabs(cameraPosition-maxCameraX)*100);

        if(displayInfo)
        {
            drawText(10, 12, QString("Nside: %1").arg(currentNside), QFont("Arial", 9));
            drawText(10, 22, QString("Zoom: %1").arg(zoom), QFont("Arial", 9));
            if(showPVectors)
                drawText(10, 32, QString("P.Vectors: %1").arg(currentVectorsNside), QFont("Arial", 9));
        }
    }
}


bool MapViewer::loadMap(QString fitsfile)
{
    currentVectorsNside = currentNside = MIN_NSIDE;
    qDebug() << "Loading on mapviewer: " << fitsfile;

    if(!initialized)
    {
        /* open fits file */
        healpixMap = new HealpixMap(fitsfile, MIN_NSIDE);

        /* get available maps */
        QList<HealpixMap::MapType> availableMaps = healpixMap->getAvailableMaps();
        mapType = HealpixMap::I;

        MapLoader* mapLoader = new MapLoader(this, fitsfile, availableMaps);
        if(mapLoader->exec())
        {
            mapType = mapLoader->getSelectedMapType();
        }

        delete mapLoader;

#if DEBUG > 0
        qDebug() << "Opening map with type: " << HealpixMap::mapTypeToString(mapType);
#endif
        healpixMap->changeCurrentMap(mapType);

        /* calculate max nside */
        maxNside = min(healpixMap->getMaxNside(), MAX_NSIDE);

        /* get face cache */
        faceCache = FaceCache::instance(MIN_NSIDE, MAX_NSIDE);          // TODO: correct ? MAX_NSIDE is used because facecache is shared
        textureCache = new TextureCache(healpixMap, MIN_NSIDE, maxNside);
        overlayCache = new OverlayCache(healpixMap, MIN_NSIDE, maxNside);

        QObject::connect(faceCache, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );
        QObject::connect(textureCache, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );
        QObject::connect(overlayCache, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );

        qDebug() << "Max nside for this map is " << healpixMap->getMaxNside();

        /* create the sphere tesselation */
        tesselation = new Tesselation(currentNside, mollweide, faceCache, textureCache, overlayCache);
        tesselation->setMap(healpixMap);

        /* preload next faces */
        preloadFaces();

        initialized = true;

        return true;
    }

    return false;
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
    camera()->setSceneRadius(1.0);

    currentManipulatedFrame = new ManipulatedFrame();
    setManipulatedFrame(currentManipulatedFrame);

    /* update initial state */
    currentNside = MIN_NSIDE;

    /* change projection constraints */
    changeProjectionConstraints();

    /* compute max camera distance for the new viewport size */
    computeMaxCameraDistance();

    /* update camera position */
    updateCameraPosition(maxCameraX);


    /* get face vertices for nside=1 to know the boundary vertices of face */
    // TODO: do this in tesselation ?
    for(int face=0; face<12; face++)
    {
        QVector<Strip> strips = FaceVertices::instance()->getFaceVertices(face, 1);

        QVector<Strip>::iterator stripIT;
        QVector<Vertice>::iterator verticeIT;

        int i=0;
        for(stripIT=strips.begin(); stripIT!=strips.end(); stripIT++)
        {
            for(verticeIT=(*stripIT).begin(); verticeIT!=(*stripIT).end(); verticeIT++)
                faceBoundaries[face].append(Vec((*verticeIT).x, (*verticeIT).y, (*verticeIT).z));
        }
        i++;
    }

    /* update scene */
    sceneUpdated(false);
}


void MapViewer::updateCameraPosition(float pos)
{
    cameraPosition = pos;
    camera()->setPosition(Vec(cameraPosition,0,0));
}


void MapViewer::changeProjectionConstraints()
{
    if(mollweide)
    {
        if(constraint!=NULL)
            delete constraint;

        /* set camera constraints: translation enabled, rotation disabled */
        constraint = new CameraConstraint(camera());
        constraint->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
        constraint->setTranslationConstraintType(AxisPlaneConstraint::FREE);

        /* set min camera X for mollweide */
        minCameraX = CAMERA_MOLL_MAX_X;
    }
    else
    {
        if(constraint!=NULL)
            delete constraint;

        /* set camera constraints: translation disabled, rotation enabled */
        constraint = new CameraConstraint(camera());
        constraint->setRotationConstraintType(AxisPlaneConstraint::FREE);
        constraint->setTranslationConstraintType(AxisPlaneConstraint::FORBIDDEN);

        /* set min camera X for 3D */
        minCameraX = CAMERA_3D_MAX_X;
    }

    /* calcula max zoom out */
    computeMaxCameraDistance();
}


void MapViewer::changeToMollview()
{    
    if(!mollweide)
    {
        mollweide = true;

        /* set constraints for mollview */
        changeProjectionConstraints();

        resetView();

        /* update tesselation */
        tesselation->changeToMollweide();

        sceneUpdated();
    }
}

void MapViewer::changeTo3D()
{
    if(mollweide)
    {
        mollweide = false;

        /* set constraints for 3D */
        changeProjectionConstraints();

        resetView();

        /* update tesselation */
        tesselation->changeTo3D();

        sceneUpdated();
    }
}


void MapViewer::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_I:
            displayInfo = !displayInfo;
            updateGL();
            break;
        case Qt::Key_Plus:
            updateNside(min(currentNside*2, maxNside));
            break;
        case Qt::Key_Minus:
            updateNside(max(currentNside/2, MIN_NSIDE));
            break;
        case Qt::Key_Z:
            updateVectorsNside(min(currentVectorsNside*2, maxNside));
            break;
        case Qt::Key_A:
            updateVectorsNside(max(currentVectorsNside/2, MIN_NSIDE));
            break;
        case Qt::Key_D:
            qDebug() << "Show entire scene";

            computeMaxCameraDistance();
            //camera()->showEntireScene();
            //camera()->fitSphere(Vec(0,0,0), 1.0);
            //updateGL();
            //camera()->showEntireScene();
            //qDebug() << "Scene width: " << camera()->screenWidth() << "," << camera()->screenHeight();
            //qDebug() << width() << "," << height();
            //computeMaxCameraDistance();
            break;
    }
}


void MapViewer::mousePressEvent(QMouseEvent* e)
{
    mousePressEvent(e, true);
}

void MapViewer::mousePressEvent(QMouseEvent* e, bool propagate)
{
    QGLViewer::mousePressEvent(e);
    if(propagate)
        emit(cameraChanged(e, MOUSEPRESS, this));
}


void MapViewer::mouseReleaseEvent(QMouseEvent *e)
{
    mouseReleaseEvent(e, true);
}

void MapViewer::mouseReleaseEvent(QMouseEvent *e, bool propagate)
{
    // TODO: just a hack to disable spinning... may have problems when selecting a pixel for example
    //QGLViewer::mouseReleaseEvent(e);
    //emit(cameraChanged(e, MOUSERELEASE, this));
    //qDebug("mouse release!!!!!!!!!!!!!!!");
    //checkVisibility();
    preloadFaces();
}


void MapViewer::postSelection (const QPoint &point)
{
    qDebug() << "Pixel selected: " << point.x() << "," << point.y();
    Vec origin, direction;
    camera()->convertClickToLine(point, origin, direction);

    qDebug() << "Origin: " << origin.x << "," << origin.y << "," << origin.z;
    qDebug() << "Direction: " << direction.x << "," << direction.y << "," << direction.z;

    Vec v = currentManipulatedFrame->coordinatesOf(origin);
    qDebug() << "ManFrame: " << v.x << "," << v.y << "," << v.z;
}


void MapViewer::mouseMoveEvent(QMouseEvent* e)
{
    mouseMoveEvent(e, true);
}

void MapViewer::mouseMoveEvent(QMouseEvent* e, bool propagate)
{
    QGLViewer::mouseMoveEvent(e);

    if(propagate)
        emit(cameraChanged(e, MOUSEMOVE, this));

    checkVisibility();
}


void MapViewer::wheelEvent(QWheelEvent *e)
{
    wheelEvent(e, true);
}

void MapViewer::wheelEvent(QWheelEvent *e, bool propagate)
{
    //qDebug("==================================");
    bool zoomChanged;

    if(e->delta()>0)
        zoomChanged = zoomIn();
    else
        zoomChanged = zoomOut();

    if(zoomChanged)
    {
        if(propagate)
            emit(cameraChanged(e, MOUSEWHEEL, this));
    }
}


void MapViewer::resizeGL(int width, int height)
{
    QGLViewer::resizeGL(width, height);

    qDebug() << "Resized!";

    /* compute max camera distance for the new viewport size */
    computeMaxCameraDistance();

    qDebug() << "New max cam distance = " << maxCameraX;

    if(FIT_CONTENT_RESIZE)
    {
        /* change camera to fit content */
        updateCameraPosition(maxCameraX);
    }

    updateGL();

    /* update scene */
    sceneUpdated();
}


void MapViewer::checkVisibility()
{
    if(DISPLAY_ONLY_VISIBLE_FACES)
    {
        visibleFaces.clear();

        //qDebug("\n\n-------------------------");

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

            // TODO: UNCOMMENT !!!!!!!
            if(!hidden)
            {
                //qDebug() << "Face " << face << " visible";
                visibleFaces.append(face);
            }
        }

        /* update tesselation with visible faces */
        tesselation->updateVisibleFaces(visibleFaces);
    }
}


void MapViewer::resetView()
{
    currentNside = MIN_NSIDE;

    /* change to initial position */
    ManipulatedFrame* newManipulatedFrame = new ManipulatedFrame();
    setManipulatedFrame(newManipulatedFrame);
    delete currentManipulatedFrame;
    currentManipulatedFrame = newManipulatedFrame;
    currentManipulatedFrame->setConstraint(constraint);

    /* update camera position */
    updateCameraPosition(maxCameraX);
    camera()->lookAt( Vec(0,0,0) );

    sceneUpdated();
}


void MapViewer::synchronize(QEvent *e, int type)
{
    switch(type)
    {
        case MOUSEMOVE:
            mouseMoveEvent((QMouseEvent*) e, false);
            break;

        case MOUSEPRESS:
            mousePressEvent((QMouseEvent*) e, false);
            break;

        case MOUSERELEASE:
            mouseReleaseEvent((QMouseEvent*) e, false);
            break;

        case MOUSEWHEEL:
            wheelEvent((QWheelEvent*) e, false);
            break;
    }
}


void MapViewer::sceneUpdated(bool update)
{
    /* check wich faces are now visible or not */
    //checkVisibility();

    /* refresh the view */
    if(update)
    {
        /* check if nside needs to be updated */
        if(AUTO_NSIDE)
            checkNside();

        updateGL();
    }

    /* calculate faces to preload */
    //preloadFaces();
}


bool MapViewer::zoomIn()
{
    float cx = cameraPosition-CAMERA_ZOOM_INC;

    if(cx >= minCameraX)
    {
        /* update camera position */
        cameraPosition = cx;
        camera()->setPosition(Vec(cameraPosition, 0.0, 0.0));

        //qDebug() << "Zooming in to " << cameraPosition;

        sceneUpdated();
        return true;
    }
    return false;
}

bool MapViewer::zoomOut()
{
    float cx = cameraPosition+CAMERA_ZOOM_INC;

    if(cx <= maxCameraX)
    {
        /* update camera position */
        cameraPosition = cx;
        camera()->setPosition(Vec(cameraPosition, 0.0, 0.0));

        //qDebug() << "Zooming out to " << cameraPosition;

        sceneUpdated();
        return true;
    }
    return false;
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
        if(currentNside!=maxNside)
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


void MapViewer::showPolarizationVectors(bool show)
{
    showPVectors = show;
    tesselation->showPolarizationVectors(show);
    /* force redraw */
    updateGL();
}


void MapViewer::updateThreshold(ColorMap* colorMap, float min, float max)
{
    tesselation->updateTextureThreshold(colorMap, min, max);

    /* force redraw */
    updateGL();
}


void MapViewer::changeMapField(HealpixMap::MapType field)
{
    tesselation->changeMapField(field);

    mapType = field;

    /* force draw */
    updateGL();

    /*
    MapViewport* mapviewport = (MapViewport*)parent()->parent();
    mapviewport->viewportUpdated(healpixMap->getFullMap(MIN_NSIDE), nside2npix(MIN_NSIDE));
    */

    mapInfo *info = getMapInfo();
    emit(mapFieldChanged(info));
}


mapInfo* MapViewer::getMapInfo()
{
    mapInfo *info = new mapInfo;
    info->values = healpixMap->getFullMap(MIN_NSIDE);
    info->nvalues = nside2npix(MIN_NSIDE);
    info->currentField = mapType;
    info->availableFields = healpixMap->getAvailableMaps();
    info->hasPolarization = healpixMap->hasPolarization();
    info->colorMap = ColorMapManager::instance()->getDefaultColorMap();

    float minTex, maxTex;
    textureCache->getTextureMinMax(minTex, maxTex);

    info->min = minTex;
    info->max = maxTex;

    return info;
}


void MapViewer::showGrid(bool show)
{
    tesselation->showGrid(show);
    /* force draw */
    updateGL();
}


/* check which is the best nside to use */
void MapViewer::checkNside()
{
    qDebug() << "Checking best nside!";

    long pixelsToDisplay;
    long pixelsDisplayed;

    float percentageVisible;
    float sphereWidth, sphereHeight;

    //if(!mollweide)
    //{
        camera()->computeModelViewMatrix();
        camera()->computeProjectionMatrix();
        Vec left, right, top, bottom;
        left = camera()->projectedCoordinatesOf(Vec(0, -1, 0), manipulatedFrame());
        right = camera()->projectedCoordinatesOf(Vec(0, 1, 0), manipulatedFrame());
        top = camera()->projectedCoordinatesOf(Vec(0, 0, -1), manipulatedFrame());
        bottom = camera()->projectedCoordinatesOf(Vec(0, 0, 1), manipulatedFrame());

        sphereWidth = right.x - left.x;
        sphereHeight = top.y - bottom.y;

        percentageVisible = min(sphereHeight, (float)height()) * min(sphereWidth, (float)width()) / (sphereWidth*sphereHeight);

        /* calculate pixels that will be displayed */
        pixelsDisplayed = min(sphereHeight, (float)height()) * min(sphereWidth, (float)width());
    //}
    //else
        //percentageVisible = 1;

    int nextNside = maxNside;

    float radius = min(width(), height())/2;
    //pixelsDisplayed = width()*height();//M_PI*pow(radius, 2);

    /*
    if(percentageVisible>=0.8)
    {
        qDebug() << "Pixs displayed using circ area";
        pixelsDisplayed = M_PI*pow(radius, 2);
    }
    else
    {
    */
        qDebug() << "Pixs displayed using rect area";
        //pixelsDisplayed = width()*height();
        //pixelsDisplayed = sphereWidth*sphereHeight;
    //}

    qDebug() << "Viewport pixels = " << pixelsDisplayed;

    long oldPixelsToDisplay = 0;

    for(int auxNside=MIN_NSIDE; auxNside<=maxNside; auxNside*=2)
    {
        pixelsToDisplay = nside2npix(auxNside)*SPHERE_VISIBLE;

        qDebug() << "Pixels to display (" << auxNside << "): " << pixelsToDisplay;

        if(pixelsToDisplay>=pixelsDisplayed)
        {
            nextNside = max(MIN_NSIDE, auxNside);
            break;
        }
        else
        {
            oldPixelsToDisplay = pixelsToDisplay;
        }
    }

    float zoom = fabs(cameraPosition-maxCameraX);
    float zoomFactor = 1+zoom/(maxCameraX-minCameraX);

    qDebug() << "Zoom factor: " << zoomFactor;

    if(abs(oldPixelsToDisplay-pixelsDisplayed)*zoomFactor < abs(pixelsToDisplay-pixelsDisplayed))
    {
        /* previous nside closer */
        nextNside = max(MIN_NSIDE, nextNside/2);
    }

    qDebug() << "Best nside: " << nextNside;

    updateNside(nextNside);
}


void MapViewer::updateNside(int nside)
{
    if(nside>=MIN_NSIDE && nside<=maxNside && nside!=currentNside)
    {
        currentNside = nside;
        tesselation->updateNside(nside);

        if(CHANGE_VECTORS_WITH_NSIDE)
            updateVectorsNside(nside);

        updateGL();
    }
}


void MapViewer::updateVectorsNside(int nside)
{
    if(showPVectors)
    {
        if(nside>=MIN_NSIDE && nside<=maxNside && nside!=currentVectorsNside)
        {
            currentVectorsNside = nside;
            tesselation->updateVectorsNside(nside);
            updateGL();
        }
    }
}


/* calculate the max camera distance need to show all the object inside scene */
void MapViewer::computeMaxCameraDistance()
{
    Camera* auxCamera = new Camera(*camera());

    if(!mollweide)
        auxCamera->fitSphere(Vec(0,0,0), 1.05);
    else
        auxCamera->fitSphere(Vec(0,0,0), 1.12);
    float auxPosition = auxCamera->position().x;

    //qDebug() << "computeMaxCameraDistance = " << auxPosition;

    maxCameraX = auxPosition;
}
