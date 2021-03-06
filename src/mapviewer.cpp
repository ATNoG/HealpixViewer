 #include "mapviewer.h"
#include "mapviewport.h"
#include <math.h>

#include "grid.h"
#include "exceptions.h"

using namespace qglviewer;

MapViewer::MapViewer(QWidget *parent, const QGLWidget* shareWidget) :
    QGLViewer(parent, shareWidget)
{
    skymap = NULL;
    initialized = false;

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
    this->automaticTextureNside = AUTO_TEXTURE_NSIDE;
    this->automaticPVectorsNside = AUTO_PVECTORS_NSIDE;
    this->pvectorsNsideFactor = PVECTORS_NSIDE_FACTOR;
    this->tesselationNside = TESSELATION_DEFAULT_NSIDE;
    this->automaticGraticule = true;
    this->overlayAlpha = 0.5;
    this->colorMap = ColorMapManager::instance()->getDefaultColorMap();

    selectionType = SINGLE_POINT;

    unselectSelection = false;

    xRot = 0;
    yRot = 0;
}

MapViewer::~MapViewer()
{
    #if DEBUG > 0
        qDebug() << "Calling MapViewer destructor";
    #endif

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
        // Multiply matrix to get in the frame coordinate system.
        glMultMatrixd(manipulatedFrame()->matrix());

        /* draw map */
        tesselation->draw();
        //drawColorBar();

        if(displayInfo)
        {
            int zoom = floor(fabs(cameraPosition-maxCameraX)*100);
            glDisable(GL_TEXTURE_2D);

            glClientActiveTexture(GL_TEXTURE0);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_VERTEX_ARRAY);

            glColor3f(1.0, 1.0, 1.0);
            drawText(10, 12, QString("Nside: %1").arg(currentNside), QFont("Arial", 9));
            drawText(10, 22, QString("Zoom: %1").arg(zoom), QFont("Arial", 9));
            if(showPVectors)
                drawText(10, 32, QString("P.Vectors: %1").arg(currentVectorsNside), QFont("Arial", 9));
            glEnable(GL_TEXTURE_2D);
        }
    }
}

bool MapViewer::loadMap(QString fitsfile, HealpixMap::MapType mapType)
{
    currentVectorsNside = currentNside = MIN_NSIDE;

    HealpixMap* healpixMapOverlay;

    if(!initialized)
    {
        bool mapCreated;

        /* open fits file */
        try
        {
            filename = fitsfile;
            this->mapType = mapType;
            healpixMap = new HealpixMap(fitsfile, MIN_NSIDE);
            mapCreated = true;
        }
        catch(HealpixMapException e)
        {
            qDebug() << e.what();
            mapCreated = false;
        }
        catch(std::bad_alloc e)
        {
            qDebug() << "Not enough memory";
            mapCreated = false;
        }

        if(mapCreated)
        {
            #if DEBUG > 0
                qDebug() << "Opening map with type: " << mapType;
            #endif

            healpixMap->changeCurrentMap(mapType);

            qDebug() << "Coordsys is " << healpixMap->coordsysToString(healpixMap->getCoordSystem());

            /* calculate max nside */
            maxNside = min(healpixMap->getMaxNside(), MAX_NSIDE);

            /* get face cache */
            faceCache = FaceCache::instance(MIN_NSIDE, MAX_NSIDE);          // TODO: correct ? MAX_NSIDE is used because facecache is shared
            textureCache = new TextureCache(healpixMap, MIN_NSIDE, maxNside, false);
            overlayCache = new OverlayCache(healpixMap, MIN_NSIDE, maxNside);

            QObject::connect(faceCache, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );
            QObject::connect(textureCache, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );
            QObject::connect(overlayCache, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );

            /* create grid */
            grid = new Grid(this, GRID_MERIDIANS, GRID_PARALLELS);

            /* create the sphere tesselation */

            /* OVERLAY */
            if(false)
            {
                /*
                healpixMapOverlay = new HealpixMap(fitsfile, MIN_NSIDE);
                healpixMapOverlay->changeCurrentMap(HealpixMap::NObs);
                textureCacheOverlay = new TextureCache(healpixMapOverlay, MIN_NSIDE, maxNside, true);
                QObject::connect(textureCacheOverlay, SIGNAL(newFaceAvailable(bool)), this, SLOT(checkForUpdates(bool)) );
                tesselation = new Tesselation(currentNside, tesselationNside, currentVectorsNside, mollweide, faceCache, textureCache, overlayCache, grid, maxNside, textureCacheOverlay);
                */
            }
            else
            {
                tesselation = new Tesselation(currentNside, tesselationNside, currentVectorsNside, mollweide, faceCache, textureCache, overlayCache, grid, maxNside, NULL);
            }

            tesselation->setMap(healpixMap);

            /* preload next faces */
            preloadFaces();

            initialized = true;

            return true;
        }
    }

    return false;
}


void MapViewer::init()
{
    /*
        General configuration.
    */
    setBackgroundColor(QColor(0,0,0));	// Black background.

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
    */
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
    camera()->setSceneRadius(1.0);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    /* change projection constraints */
    changeProjectionConstraints();

    /* more constraints */
//    WorldConstraint* constraintWorld = new WorldConstraint();
//    constraintWorld->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
//    constraintWorld->setTranslationConstraintType(AxisPlaneConstraint::FORBIDDEN);

    /* create manipulated frame */
    currentManipulatedFrame = new ManipulatedFrame();
    currentManipulatedFrame->setConstraint(constraint);
    setManipulatedFrame(currentManipulatedFrame);

    coordSysFrame = new Frame();
    coordSysFrame->setReferenceFrame(manipulatedFrame());
    tesselation->setCoordSysFrame(coordSysFrame);
    originalRotation = coordSysFrame->rotation();

    /* update initial state */
    currentNside = MIN_NSIDE;

    /* compute max camera distance for the new viewport size */
    computeMaxCameraDistance();

    /* update camera position */
    updateCameraPosition(maxCameraX, false, false);


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

	/*Setup Mouse Events */
	setWheelBinding(Qt::AltModifier, CAMERA, MOVE_FORWARD);
	setMouseBinding(Qt::LeftButton, FRAME, ROTATE);

    /* update scene */
    sceneUpdated(false);




    /* create data */
    QColor color;
    uint texk = 0;
    unsigned char cmtexture[255*3];
    for(int i=0; i<255; i++)
    {
        texk = i*3;
        cmtexture[texk] = 0;
        cmtexture[texk+1] = 0;
        cmtexture[texk+2] = 255;
    }

    /* create texture */
    glEnable(GL_TEXTURE_1D);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_1D, textureId);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, 3, 255, 0, GL_RGB, GL_UNSIGNED_BYTE, cmtexture);
}


void MapViewer::updateCameraPosition(float pos, bool signal, bool update)
{
    cameraPosition = pos;

    if(mollweide && !FLIP)
        camera()->setPosition(Vec(-fabs(cameraPosition),0,0));
    else
        camera()->setPosition(Vec(fabs(cameraPosition),0,0));

    if(signal)
        emit(signalZoomChanged(cameraPosition, this));

    updateMouseSensitivity();

    if(update)
        sceneUpdated();
}

void MapViewer::updatePosition(Vec position)
{
    manipulatedFrame()->setPosition(position);
    updateGL();
}

void MapViewer::updateRotation(Quaternion rotation)
{
    manipulatedFrame()->setRotation(rotation);
    updateGL();
}

bool MapViewer::updateKeyPress(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_I:
            displayInfo = !displayInfo;
            updateGL();
            return true;
            break;

        case Qt::Key_Plus:
            automaticTextureNside = false;
            updateNside(min(currentNside*2, maxNside), true);
            return true;
            break;

        case Qt::Key_Minus:
            automaticTextureNside = false;
            updateNside(max(currentNside/2, MIN_NSIDE), true);
            return true;
            break;

        case Qt::Key_Z:
            automaticPVectorsNside = false;
            updateVectorsNside(min(currentVectorsNside*2, maxNside), true);
            return true;
            break;

        case Qt::Key_A:
            automaticPVectorsNside = false;
            updateVectorsNside(max(currentVectorsNside/2, MIN_NSIDE), true);
            return true;
            break;

        case Qt::Key_S:
            changeSelectionType(SINGLE_POINT);
            break;
        case Qt::Key_D:
            changeSelectionType(DISC);
            break;
        case Qt::Key_T:
            changeSelectionType(TRIANGLE);
            break;
        case Qt::Key_P:
            changeSelectionType(POLYGON);
            break;
        case Qt::Key_C:
            tesselation->clearROI();
            auxiliarPixels.clear();
            selectedPixels.clear();
            updateGL();
            break;
        case Qt::Key_Enter:
            if(selectionType == POLYGON)
            {
                if(auxiliarPixels.size() >= 3)
                {
                    displayMessage("Calculating polygon", 10000);
                    int nsideToUse = transformSameNside(auxiliarPixels, auxiliarNsides);
                    std::set<int> pixels = healpixMap->query_polygon(auxiliarPixels, nsideToUse);
                    auxiliarPixels.clear();

                    if(!unselectSelection)
                        addPixelsToSelection(pixels, nsideToUse);
                    else
                        removePixelsFromSelection(pixels);

                    update();
                    displayMessage("", 0);
                }
                else
                {
                    QMessageBox msgBox;
                    msgBox.setText("At least 3 vertices should be picked before close the polygon");
                    msgBox.exec();
                }
            }
            break;
        case Qt::Key_Q:
            qDebug("Save snapshot");
            setSnapshotFormat("PNG");
            setSnapshotQuality(100);
            grabFrameBuffer();
            saveSnapshot();
            break;
        case Qt::Key_E:
            if(unselectSelection)
                displayMessage("Selection enabled");
            else
                displayMessage("Unselection enabled");
            unselectSelection = !unselectSelection;
            break;
        case Qt::Key_O:
            exportSelectedArea();
            break;
        case Qt::Key_M:
            exportSelectedAreaAsMask();
            break;

        case Qt::Key_U:
            yRot++;
            updateMollweideRotation();
            break;

        case Qt::Key_J:
            yRot--;
            updateMollweideRotation();
            break;

        case Qt::Key_H:
            xRot++;
            updateMollweideRotation();
            break;

        case Qt::Key_K:
            xRot--;
            updateMollweideRotation();
            break;

        case Qt::Key_1:
            overlayAlpha -= 0.05;
            if(overlayAlpha<0)
                overlayAlpha = 0;
            tesselation->updateAlpha(overlayAlpha);
            updateGL();
            break;

        case Qt::Key_2:
            overlayAlpha += 0.05;
            if(overlayAlpha>1)
                overlayAlpha = 1;
            tesselation->updateAlpha(overlayAlpha);
            updateGL();
            break;
    }

    return false;
}


void MapViewer::updateMollweideRotation()
{
    //qDebug() << "xRot: " << xRot << " , yRot: " << yRot;
    tesselation->setMollweideRotation(xRot, yRot);
    updateGL();
}


void MapViewer::changeProjectionConstraints()
{
    if(constraint!=NULL)
        delete constraint;

    if(mollweide)
    {
        //qDebug() << "Changing projection constraints to mollweide";

        /* set camera constraints: translation enabled, rotation disabled */
        constraint = new CameraConstraint(camera());
        constraint->setRotationConstraintType(AxisPlaneConstraint::FORBIDDEN);
        constraint->setTranslationConstraintType(AxisPlaneConstraint::FREE);

        /* set min camera X for mollweide */
        minCameraX = CAMERA_MOLL_MAX_X;
    }
    else
    {
        //qDebug() << "Changing projection constraints to 3D";

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

        glCullFace(GL_FRONT);

        /* update tesselation */
        tesselation->changeToMollweide();

        /* reset rotation */
        coordSysFrame->setRotation(originalRotation);

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

        glCullFace(GL_BACK);

        /* update tesselation */
        tesselation->changeTo3D();

        sceneUpdated();
    }
}


void MapViewer::keyPressEvent(QKeyEvent *e)
{
    if(updateKeyPress(e))
        emit(signalKeyPressed(e, this));
}

void MapViewer::mousePressEvent(QMouseEvent *e)
{
    QGLViewer::mousePressEvent(e);
    firstPos = e->pos();
}

void MapViewer::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button()==Qt::LeftButton && mollweide)
    {
        int xMovement = e->x() - firstPos.x();
        int yMovement = -(e->y() - firstPos.y());

        camera()->computeModelViewMatrix();
        camera()->computeProjectionMatrix();
        Vec v1 = camera()->projectedCoordinatesOf(Vec(0, -2, 1));
        Vec v2 = camera()->projectedCoordinatesOf(Vec(0, 2, -1));

        int objWidth = v2.x - v1.x;
        int objHeight = v2.y - v1.y;

        //qDebug() << "Movement:" << xMovement << "/" << objWidth << " , " << yMovement << "/" << objHeight;

        // transform displacement into angles
        xRot += xMovement*360 / objWidth;
        yRot -= yMovement*180 / objHeight;

        //updateMollweideRotation();
    }

    // TODO: just a hack to disable spinning... may have problems when selecting a pixel for example
    //QGLViewer::mouseReleaseEvent(e);
    //emit(cameraChanged(e, MOUSERELEASE, this));
    //qDebug("mouse release!!!!!!!!!!!!!!!");
    //checkVisibility();
    preloadFaces();
}

int MapViewer::calculatePixelIndex(const QPoint &point)
{
    Vec d, v, o;
    camera()->convertClickToLine(point, o, d);

    int pix = -1;
    long p;

    if(!mollweide)
    {
        const float srad2 = 1.;						// Radius^2 of the 3D sphere.

        float sqrtterm, t1, t2, oo, od, dd, t;
        oo = o * o;
        od = o * d;
        dd = d * d;
        sqrtterm = (od * od) - (dd * (oo - srad2));
        if (sqrtterm < 0)
            qDebug("error calculating selected pixel");
        else
        {
            t1 = (-od - sqrt(sqrtterm))/(dd);
            t2 = (-od + sqrt(sqrtterm))/(dd);
            t = (t1 < t2) ? t1 : t2;
            v = o + t*d;

            Vec r = coordSysFrame->coordinatesOf(v);

            double lambda, phi;
            phi    = acos(r.z);
            lambda = atan2(r.y, r.x);

            healpixMap->angle2pix(phi, lambda, currentNside, p);

            pix = int(p);
        }
    }
    else
    {
        bool found;
        Vec k = camera()->pointUnderPixel(point, found);
        Vec r = coordSysFrame->coordinatesOf(k);

        double v1, u1, s1, a1, z, phi, sz;
        v1 = r.z;
        u1 = r.y;
        s1 = sqrt((1.-v1)*(1.+v1));
        a1 = asin(v1);

        z = 2. / M_PI * (a1+v1*s1);
        phi = (M_PI/2.) * u1/s1;
        sz = sqrt((1.-z)*(1.+z));

        double *v = new double[3];
        v[0] = sz*cos(phi);
        v[1] = sz*sin(phi);
        v[2] = z;
        vec2pix_nest(currentNside, v, &p);

        pix = int(p);
    }

    return pix;
}


void MapViewer::postSelection (const QPoint &point)
{
    int pix = calculatePixelIndex(point);
    int nsideToUse = currentNside;

    std::set<int> pixels;

    if(pix>=0)
    {
        // TODO: verify if is a good selection
        switch(selectionType)
        {
            case SINGLE_POINT:
                //qDebug() << "Selected pixel: " << pix;
                pixels.insert(pix);
                break;

            case DISC:
                auxiliarPixels.push_back(pix);
                auxiliarNsides.push_back(currentNside);
                if(auxiliarPixels.size()==1)
                {
                    pixels.insert(pix);
                    displayMessage("Select second point");
                }
                else
                {
                    displayMessage("Calculating disc", 10000);
                    nsideToUse = transformSameNside(auxiliarPixels, auxiliarNsides);
                    pixels = healpixMap->query_disc(auxiliarPixels[0], auxiliarPixels[1], nsideToUse);
                    auxiliarPixels.clear();
                    auxiliarNsides.clear();
                    displayMessage("", 0);
                }
                break;

            case TRIANGLE:
                auxiliarPixels.push_back(pix);
                auxiliarNsides.push_back(currentNside);
                if(auxiliarPixels.size()==1)
                {
                    pixels.insert(pix);
                    displayMessage("Select second point");
                }
                else if(auxiliarPixels.size()==2)
                {
                    pixels.insert(pix);
                    displayMessage("Select last point");
                }
                else
                {
                    displayMessage("Calculating triangle", 10000);
                    nsideToUse = transformSameNside(auxiliarPixels, auxiliarNsides);
                    //qDebug() << "Pixels: " << auxiliarPixels[0] << "," << auxiliarPixels[1] << "," << auxiliarPixels[2] << "with nside " << nsideToUse;
                    pixels = healpixMap->query_triangle(auxiliarPixels[0], auxiliarPixels[1], auxiliarPixels[2], nsideToUse);

                    /*
                    std::set<int>::iterator it;
                    for(it=pixels.begin(); it!=pixels.end(); it++)
                        qDebug() << *it;
                    qDebug() << "Pixels selected: " << pixels.size();
                    */
                    auxiliarPixels.clear();
                    auxiliarNsides.clear();
                    displayMessage("", 0);
                }
                break;

            case POLYGON:
                /* check if shape is closed */
                auxiliarPixels.push_back(pix);
                auxiliarNsides.push_back(currentNside);
                pixels.insert(pix);
                if(auxiliarPixels.size()>=3)
                    displayMessage("Select another point or <enter> to close the polygon");
                else
                    displayMessage("Select another point");
                break;
        }

        if(!unselectSelection)
            addPixelsToSelection(pixels, nsideToUse);
        else
            removePixelsFromSelection(pixels);
    }
    else
        displayMessage("Invalid point");

}

int MapViewer::transformSameNside(std::vector<int>& pixels, std::vector<int> nsides)
{
    int min_nside = nsides[0];

    std::vector<int>::iterator it;
    for(it=nsides.begin(); it!=nsides.end(); it++)
    {
        if(*it<min_nside)
            min_nside = *it;
    }

    for(int i=0; i<pixels.size(); i++)
    {
        if(nsides[i]!=min_nside)
        {
            pixels[i] = FieldMap::degradePixel(pixels[i], nsides[i], min_nside);
        }
    }

    return min_nside;
}

void MapViewer::updateSelection(std::set<int> pixels, int nside, bool add)
{
    if(add)
    {
        selectedPixels.insert(pixels.begin(), pixels.end());
        if(pixels.size()>=1)
            tesselation->selectPixels(pixels, nside);
    }
    else
    {
        std::set<int>::iterator it;
        for(it=pixels.begin(); it!=pixels.end(); it++)
            selectedPixels.erase(*it);
        tesselation->unselectPixels(pixels);
    }

    update();
}

void MapViewer::addPixelsToSelection(std::set<int> pixels, int nside)
{
    selectedPixels.insert(pixels.begin(), pixels.end());

    if(pixels.size()>=1)
        tesselation->selectPixels(pixels, nside);

    emit(signalSelectionChanged(pixels, nside, true));
    pixels.clear();
}

void MapViewer::removePixelsFromSelection(std::set<int> pixels)
{
    std::set<int>::iterator it;
    for(it=pixels.begin(); it!=pixels.end(); it++)
        selectedPixels.erase(*it);
    tesselation->unselectPixels(pixels);

    emit(signalSelectionChanged(pixels, currentNside, false));
    pixels.clear();
}

void MapViewer::exportSelectedArea()
{
    //qDebug() << "Total pixels: " << selectedPixels.size();

    QString outfile = QFileDialog::getSaveFileName(this, "Export as", "", "Fits Files (*.fits)");

    if(!outfile.endsWith(".fits"))
        outfile.append(".fits");

    /* overwrite permission was already given */
    outfile = "!" + outfile;

    Healpix_Map<float> inmap;
    read_Healpix_map_from_fits(filename.toUtf8().constData(),inmap,1,2);

    Healpix_Map<float> newMap(inmap.Nside(), inmap.Scheme(), nside_dummy());
    //newMap.Import(inmap, false);
    newMap.fill(Healpix_undef);

    std::set<int>::iterator it;
    for(it=selectedPixels.begin(); it!=selectedPixels.end(); it++)
    {
        newMap[*it] = inmap[*it];
    }

    write_Healpix_map_to_fits(outfile.toUtf8().constData(), newMap, planckType<float>());

    displayMessage("Extraction done with success");

    /*
    string infile = "wmap_imap_r10_yr1_K1_v4.fits";
    string outfile = "!teste1.fits";

    Healpix_Map<float> inmap;
    read_Healpix_map_from_fits(infile,inmap,1,2);
    Healpix_Map<float> outmap (inmap.Nside(), inmap.Scheme(), nside_dummy());

    //outmap.Import(inmap, false);
    write_Healpix_map_to_fits (outfile,outmap,planckType<float>());
    */

    /*
    Healpix_Map<float> inmap;
    read_Healpix_map_from_fits("wmap_imap_r10_yr1_K1_v4.fits",inmap,1,2);

    Healpix_Map<float> outmap;
    outmap.Set(inmap.Order(), inmap.Scheme());
    write_Healpix_map_to_fits ("!teste.fits",outmap,PLANCK_FLOAT32);
    */
}

void MapViewer::exportSelectedAreaAsMask()
{
    //qDebug() << "Total pixels: " << selectedPixels.size();

    QString outfile = QFileDialog::getSaveFileName(this, "Export mask as", "", "Fits Files (*.fits)");

    if(!outfile.endsWith(".fits"))
        outfile.append(".fits");

    /* overwrite permission was already given */
    outfile = "!" + outfile;

    Healpix_Map<float> inmap;
    read_Healpix_map_from_fits(filename.toUtf8().constData(),inmap,1,2);

    Healpix_Map<float> newMap(inmap.Nside(), inmap.Scheme(), nside_dummy());
    //newMap.Import(inmap, false);
    newMap.fill(0);

    std::set<int>::iterator it;
    for(it=selectedPixels.begin(); it!=selectedPixels.end(); it++)
    {
        newMap[*it] = 1;
    }

    write_Healpix_map_to_fits(outfile.toUtf8().constData(), newMap, planckType<float>());

    displayMessage("Mask created with success");
}

void MapViewer::changeSelectionType(SelectionType stype)
{
    selectionType = stype;

    /* clear status */
    switch(selectionType)
    {
        case SINGLE_POINT:
            displayMessage("Selection changed to single point");
            break;
        case DISC:
            displayMessage("Selection changed to disc");
            break;
        case TRIANGLE:
            displayMessage("Selection changed to triangle");
            break;
        case POLYGON:
            displayMessage("Selection changed to polygon");
            break;
    }

    auxiliarPixels.clear();
    auxiliarNsides.clear();
}

void MapViewer::mouseMoveEvent(QMouseEvent* e)
{
    QGLViewer::mouseMoveEvent(e);

    if(mollweide)
        emit(signalPositionChanged(manipulatedFrame()->position(), this));
    else
        emit(signalRotationChanged(manipulatedFrame()->rotation(), this));

    checkVisibility();
}


void MapViewer::wheelEvent(QWheelEvent *e)
{
    bool zoomChanged;

    if(e->delta()>0)
        zoomChanged = zoomIn();
    else
        zoomChanged = zoomOut();
}


void MapViewer::resizeGL(int width, int height)
{
    QGLViewer::resizeGL(width, height);

    //qDebug() << "Resize!";

    /* compute max camera distance for the new viewport size */
    computeMaxCameraDistance();

    //qDebug() << "New max cam distance = " << maxCameraX;

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
    setManipulatedFrame(currentManipulatedFrame);
    coordSysFrame->setReferenceFrame(manipulatedFrame());

    /* update camera position */
    updateCameraPosition(maxCameraX);

    camera()->lookAt( Vec(0,0,0) );

    /* reset mollweide rotation */
    xRot = 0;
    yRot = 0;
    updateMollweideRotation();

    sceneUpdated();
}



void MapViewer::sceneUpdated(bool update)
{
    /* check wich faces are now visible or not */
    //checkVisibility();

    /* refresh the view */
    if(update)
    {
        /* check if nside needs to be updated */
        if(automaticTextureNside)
            checkNside();

        updateGL();
    }

    /* calculate faces to preload */
    //preloadFaces();
}


void MapViewer::updateGraticule()
{
    int totLevels = floor((maxCameraX-minCameraX) / CAMERA_ZOOM_INC);
    float currentLevel = floor((cameraPosition-minCameraX) / CAMERA_ZOOM_INC) + 1;

    int graticules[11] = {1,2,2,3,5,6,9,10,15,18,30};

    float logMax = log10(totLevels+1);
    float v = log10(currentLevel);
    int aux = round(v/logMax * 10);

    grid->setConfiguration(graticules[aux], graticules[aux]);

    // TODO: need to update gl to draw the graticule!
    //updateGL();
}


bool MapViewer::zoomIn()
{
    float cx = cameraPosition-CAMERA_ZOOM_INC;

    if(cx >= minCameraX)
    {
        /* update camera position */
        updateCameraPosition(cx, true);

        if(automaticGraticule)
            updateGraticule();

        //sceneUpdated();
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
        updateCameraPosition(cx, true);

        if(automaticGraticule)
            updateGraticule();

        //sceneUpdated();
        return true;
    }
    return false;
}


void MapViewer::checkForUpdates(bool cleanCache)
{
    //QGLContext* newContext = new QGLContext(format(),this);

    updateGL();

    // TODO: try to clean cache from thread...
    if(cleanCache)
    {
        textureCache->cleanCache();
        //faceCache->cleanCache();
    }
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

                predictCamera.reset(new Camera(*camera()));
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


void MapViewer::updateThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset)
{
    this->colorMap = colorMap;
    tesselation->updateTextureThreshold(colorMap, min, max, sentinelColor, scale, factor, offset);

    /* force redraw */
    updateGL();
}


void MapViewer::drawColorBar()
{
    glEnable (GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, textureId);

    /* draw rectangle */
    glBegin(GL_QUADS);
        glTexCoord1f(1.);
        glVertex3f(0., -0.4, -0.1);
        glTexCoord1f(0.6);
        glVertex3f(0., 0.4, -0.1);
        glTexCoord1f(0.3);
        glVertex3f(0., 0.4, 0.1);
        glTexCoord1f(0.);
        glVertex3f(0., -0.4, 0.1);
    glEnd();

    /* unbind texture */
    //glDisable(GL_TEXTURE_1D);
    glBindTexture(GL_TEXTURE_1D, NULL);
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
    info->sentinelColor = QColor(DEFAULT_SENTINEL_COLOR);
    info->minNside = MIN_NSIDE;
    info->maxNside = maxNside;
    info->scale = LINEAR;
    info->factor = 1.0;
    info->offset = 0.0;

    double minMag, maxMag;
    healpixMap->getMagMinMax(minMag, maxMag);
    info->minMag = minMag;
    info->maxMag = maxMag;

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
    //qDebug() << "Checking best nside!";

    long pixelsToDisplay=0;
    long pixelsDisplayed;

    float percentageVisible;
    float sphereWidth, sphereHeight;

    long viewportArea, sphereArea;

    // TODO: adapt to mollweide

        camera()->computeModelViewMatrix();
        camera()->computeProjectionMatrix();
        Vec left, right, top, bottom;
        left = camera()->projectedCoordinatesOf(Vec(0, -1, 0));
        right = camera()->projectedCoordinatesOf(Vec(0, 1, 0));
        top = camera()->projectedCoordinatesOf(Vec(0, 0, -1));
        bottom = camera()->projectedCoordinatesOf(Vec(0, 0, 1));

        sphereWidth = right.x - left.x;
        sphereHeight = top.y - bottom.y;

        viewportArea = height() * width();
        sphereArea = 4 * M_PI * pow(sphereWidth/2, 2) * SPHERE_PROPORTION;

        /* calculate pixels that will be displayed */
        pixelsDisplayed = min(viewportArea, sphereArea);


    int nextNside = maxNside;

    long oldPixelsToDisplay = 0;

    float zoom = fabs(cameraPosition-maxCameraX);
    float zoomFactor = 1- zoom/(maxCameraX-minCameraX);

    for(int auxNside=MIN_NSIDE; auxNside<=maxNside; auxNside*=2)
    {
        if(sphereWidth>width() && sphereHeight>height())
        {
            percentageVisible = (float)viewportArea/(float)sphereArea;
            //qDebug() << "Using percentage: " << percentageVisible;
        }
        else
        {
            percentageVisible = 1.0;//SPHERE_VISIBLE;
            //qDebug() << "Using sphere visible";
        }

        pixelsToDisplay = (long)(nside2npix(auxNside)*percentageVisible*0.14*zoomFactor);

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

    updateNside(nextNside);
}


void MapViewer::updateNside(int nside, bool signal)
{
    if(nside>=MIN_NSIDE && nside<=maxNside && nside!=currentNside)
    {
        currentNside = nside;
        tesselation->updateTextureNside(nside);

        if(automaticPVectorsNside)
            updateVectorsNside(nside);

        updateGL();

        if(signal)
            emit(textureNsideUpdated(currentNside));
    }
}


void MapViewer::updateVectorsNside(int nside, bool signal)
{
    if(healpixMap->hasPolarization())
    {
        if(nside>=MIN_NSIDE && nside<=maxNside && nside!=currentVectorsNside)
        {
            currentVectorsNside = nside;
            tesselation->updateVectorsNside(nside);
            updateGL();

            if(signal)
                emit(vectorsNsideUpdated(currentVectorsNside));
        }
    }
}


/* calculate the max camera distance need to show all the object inside scene */
void MapViewer::computeMaxCameraDistance()
{
    hv::unique_ptr<Camera> auxCamera(new Camera(*camera()));

    if(!mollweide)
        auxCamera->fitSphere(Vec(0,0,0), 1.05);
    else
        auxCamera->fitSphere(Vec(0,0,0), 1.98);
    float auxPosition = auxCamera->position().x;

    //qDebug() << "computeMaxCameraDistance = " << auxPosition;

    maxCameraX = auxPosition;
}



void MapViewer::applyOptions(mapOptions *options)
{
    /* update tesselation nside */
    if(options->tesselationNside!=0 && tesselationNside!=options->tesselationNside)
    {
        //qDebug() << "==> Update tesselation Nside";
        tesselationNside = options->tesselationNside;
        tesselation->updateTesselationNside(tesselationNside);
    }

    /* update texture nside */
    if(options->textureNside==1)
    {
        //qDebug() << "==> Enable automatic texture nside";
        automaticTextureNside = true;
    }
    else if(options->textureNside!=0)
    {
        automaticTextureNside = false;
        if(currentNside!=options->textureNside)
        {
            //qDebug() << "==> Update texture nside";
            updateNside(options->textureNside);
        }
    }

    /* update pvectors nside */
    if(options->pVectorsNside==1)
    {
        //qDebug() << "==> Enable automatic pVectors Nside";
        automaticPVectorsNside = true;
    }
    else if(options->pVectorsNside!=0)
    {
        automaticPVectorsNside = false;
        if(currentVectorsNside!=options->pVectorsNside)
        {
            //qDebug() << "==> Update pVectors Nside";
            updateVectorsNside(options->pVectorsNside);
        }
    }

    /* update pvectors magnification value */
    overlayCache->setPolarizationMagnification(options->magnification);

    /* update pvectors spacing */
    if(options->vectorsSpacing!=-1)
        overlayCache->setVectorsSpacing(options->vectorsSpacing);

    /* update pvectors magnitude threshold */
    if(options->minPolThreshold!=-1 && options->maxPolThreshold!=-1)
        overlayCache->setPolarizationMagThreshold(options->minPolThreshold, options->maxPolThreshold);

    updateGL();
}


void MapViewer::applyGridOptions(gridOptions* options)
{
    if(options->automatic)
    {
        automaticGraticule = true;
    }
    else
    {
        grid->setConfiguration(options->meridians, options->parallels);
        grid->setLabeling(options->labeling, options->labelSize);
        automaticGraticule = false;
    }
    grid->setColor(options->color);
    updateGL();
}


void MapViewer::applyCoordSys(HealpixMap::Coordsys coordIn, HealpixMap::Coordsys coordOut)
{
    if(!mollweide)
    {
        CoordinateSystemMatrices* coordsys = CoordinateSystemMatrices::instance();
        Quaternion rotQuaternion, rotQuaternionOrigin;

        if(coordIn!=coordOut)
        {
            switch(coordIn)
            {
                case HealpixMap::CELESTIAL:

                    switch(coordOut)
                    {
                        case HealpixMap::GALACTIC:
                            rotQuaternion.setFromRotationMatrix(coordsys->q2g);
                            //rotQuaternionOrigin.setFromRotationMatrix(coordsys->g2q);
                            break;
                        case HealpixMap::ECLIPTIC:
                            rotQuaternion.setFromRotationMatrix(coordsys->q2e);
                            //rotQuaternionOrigin.setFromRotationMatrix(coordsys->e2q);
                            break;
                    }
                    break;

                case HealpixMap::GALACTIC:

                    switch(coordOut)
                    {
                        case HealpixMap::CELESTIAL:
                            rotQuaternion.setFromRotationMatrix(coordsys->g2q);
                            //rotQuaternionOrigin.setFromRotationMatrix(coordsys->q2g);
                            break;
                        case HealpixMap::ECLIPTIC:
                            rotQuaternion.setFromRotationMatrix(coordsys->g2e);
                            //rotQuaternionOrigin.setFromRotationMatrix(coordsys->e2g);
                            break;
                    }
                    break;

                case HealpixMap::ECLIPTIC:

                    switch(coordOut)
                    {
                        case HealpixMap::CELESTIAL:
                            rotQuaternion.setFromRotationMatrix(coordsys->e2q);
                            //rotQuaternionOrigin.setFromRotationMatrix(coordsys->q2e);
                            break;
                        case HealpixMap::GALACTIC:
                            rotQuaternion.setFromRotationMatrix(coordsys->e2g);
                            //rotQuaternionOrigin.setFromRotationMatrix(coordsys->g2e);
                            break;
                    }
                    break;

            }
        }
        else
            rotQuaternion.setFromRotationMatrix(coordsys->norot);

        coordSysFrame->setRotation(rotQuaternion);
        //manipulatedFrame()->setRotation(rotQuaternionDst);

        updateGL();
    }
}


void MapViewer::updateMouseSensitivity()
{
    float sens = (cameraPosition-minCameraX)*((1.0-MIN_MOUSE_SENSITIVITY)/(maxCameraX-minCameraX)) + MIN_MOUSE_SENSITIVITY;
    //qDebug() << "Updating mouse sensitivity to " << sens;
    manipulatedFrame()->setRotationSensitivity(sens);
}
