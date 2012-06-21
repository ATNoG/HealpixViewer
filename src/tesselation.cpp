#include "tesselation.h"
#include "coordinatesystem.h"

Tesselation::Tesselation(int _textureNside, int _tessNside, int _pVecNside, bool _mollview, FaceCache* faceCache, TextureCache* textureCache, OverlayCache* overlayCache, Grid* _grid, int maxNside)
{
    textureNside = _textureNside;
    vectorsNside = _pVecNside;
    tesselationNside = _tessNside;
    mollview = _mollview;
    grid = _grid;

    /* by default dont show the polarization vectors neither grid */
    displayPolarizationVectors = false;
    displayGrid = false;
    needToUpdateRotation = false;

    this->faceCache = faceCache;
    this->textureCache = textureCache;
    this->overlayCache = overlayCache;
    //this->mapType = mapType;
    this->roi = NULL;
    this->coordSysFrame = NULL;

    this->manager = new ROIManager(maxNside);

    xRot = 0;
    yRot = 0;

    createInitialTesselation();
}

Tesselation::~Tesselation()
{
    #if DEBUG > 0
        qDebug() << "Calling Tesselation destructor";
    #endif

    delete grid;
}

void Tesselation::createInitialTesselation()
{
    /* initial faces */
    QVector<int> initialFaces;
    for(int i=0; i<12; i++)
        initialFaces.append(i);
    updateVisibleFaces(initialFaces);
}


/* Update nside used in tesselation */
void Tesselation::updateTesselationNside(int newNside)
{
    if(tesselationNside!=newNside)
    {
        this->tesselationNside = newNside;
        qDebug() << "tess nside updated to " << newNside;
    }
}


/* Update nside used in textures */
void Tesselation::updateTextureNside(int newNside)
{
    if(textureNside!=newNside)
    {
        this->textureNside = newNside;
        //qDebug() << "texture nside updated to " << newNside;
    }
}

/* update vectors nside */
void Tesselation::updateVectorsNside(int nside)
{
    if(nside!=vectorsNside)
    {
        vectorsNside = nside;
        //qDebug() << "nside of vectors updated to " << vectorsNside;
    }
}

/* get visible faces */
void Tesselation::updateVisibleFaces(QVector<int> faces)
{
    this->facesv = faces;

    /* tell cache which faces are visible */
    faceCache->updateStatus(faces, tesselationNside, mollview);
    if(DISPLAY_TEXTURE)
        textureCache->updateStatus(faces, textureNside);
    if(displayPolarizationVectors)
        overlayCache->updateStatus(faces, vectorsNside, mollview, MapOverlay::POLARIZATION_VECTORS);
}


/* preload faces */
void Tesselation::preloadFaces(QVector<int> faces, int nside)
{
    for(int i=0; i<faces.size(); i++)
    {
        faceCache->preloadFace(faces[i], tesselationNside, mollview);
        if(DISPLAY_TEXTURE)
            textureCache->preloadFace(faces[i], textureNside);
        if(displayPolarizationVectors)
            overlayCache->preloadFace(faces[i], vectorsNside, mollview, MapOverlay::POLARIZATION_VECTORS);
    }
}


void Tesselation::setMap(HealpixMap* map)
{
    healpixmap = map;
}

void Tesselation::setCoordSysFrame(Frame* frame)
{
    this->coordSysFrame = frame;
}

/* Draw opengl primitives */
void Tesselation::draw()
{
    int totalFaces = facesv.size();
    Face* face;
    Texture* texture;
    PolarizationVectors* polVectors;
    ROI* roi;


    glPushMatrix();

    if(coordSysFrame!=NULL)
        glMultMatrixd(coordSysFrame->matrix());

    /* get faces */
    for(int i=0; i<totalFaces; i++)
    {
        face = faceCache->getFace(facesv[i], tesselationNside, mollview);
        bool hasROI = manager->hasROI(facesv[i]);

        // display texture
        if(DISPLAY_TEXTURE)
        {
            texture = textureCache->getFace(facesv[i], textureNside);
            texture->draw();

            if(hasROI)
            {
                roi = manager->getFaceROI(facesv[i]);
                roi->draw();
            }
        }

        if(needToUpdateRotation)
            face->setMollweideRotation(xRot, yRot);

        face->draw();

        if(DISPLAY_TEXTURE)
        {
            texture->unbind();

            if(hasROI)
                roi->unbind();
        }

        // display polarization vectors
        if(displayPolarizationVectors)
        {
            polVectors = (PolarizationVectors*)overlayCache->getFace(facesv[i], vectorsNside, mollview, MapOverlay::POLARIZATION_VECTORS);
            polVectors->draw();
        }
    }

    /* display grid */
    if(displayGrid)
        grid->draw();

    glPopMatrix();

    needToUpdateRotation = false;

    /* display grid */
    if(displayGrid)
        grid->draw();
}

/* enable polarization vectors */
void Tesselation::showPolarizationVectors(bool show)
{
    if(healpixmap->hasPolarization())
        displayPolarizationVectors = show;
}


void Tesselation::updateTextureThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset)
{
    textureCache->updateTextureThreshold(colorMap, min, max, sentinelColor, scale, factor, offset);
}


void Tesselation::changeMapField(HealpixMap::MapType field)
{
    qDebug() << "Updating Map Field to " << HealpixMap::mapTypeToString(field);
    textureCache->changeMapField(field);
}

void Tesselation::showGrid(bool show)
{
    displayGrid = show;
}

void Tesselation::changeTo3D()
{
    if(mollview)
    {
        mollview = false;
        grid->changeToMollweide(false);
    }
}

void Tesselation::changeToMollweide()
{
    if(!mollview)
    {
        mollview = true;
        grid->changeToMollweide(true);
    }
}

void Tesselation::selectPixels(std::set<int> pixelIndexes, int nside)
{    
    /* split pixels indexes into faces */
    manager->addPoints(pixelIndexes, nside);
}

void Tesselation::unselectPixels(std::set<int> pixelIndexes)
{
    manager->removePoints(pixelIndexes, this->textureNside);
}

void Tesselation::clearROI()
{
    manager->clear();
}

void Tesselation::setMollweideRotation(int xRot, int yRot)
{
    this->xRot = xRot;
    this->yRot = yRot;

    needToUpdateRotation = true;
}
