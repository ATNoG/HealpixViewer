#include "tesselation.h"

Tesselation::Tesselation(int _textureNside, int _tessNside, int _pVecNside, bool _mollview, FaceCache* faceCache, TextureCache* textureCache, OverlayCache* overlayCache)
{
    textureNside = _textureNside;
    vectorsNside = _pVecNside;
    tesselationNside = _tessNside;
    mollview = _mollview;

    /* by default dont show the polarization vectors neither grid */
    displayPolarizationVectors = false;
    displayGrid = false;

    this->faceCache = faceCache;
    this->textureCache = textureCache;
    this->overlayCache = overlayCache;
    //this->mapType = mapType;
    this->roi = NULL;

    createInitialTesselation();

    /* create grid */
    grid = new Grid(45, 45);
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

/* Draw opengl primitives */
void Tesselation::draw()
{
    int totalFaces = facesv.size();
    Face* face;
    Texture* texture;
    PolarizationVectors* polVectors;

    /* get faces */
    for(int i=0; i<totalFaces; i++)
    {
        face = faceCache->getFace(facesv[i], tesselationNside, mollview);

        /* display texture */
        if(DISPLAY_TEXTURE)
        {
            texture = textureCache->getFace(facesv[i], textureNside);
            texture->draw();
        }

        face->draw();

        if(DISPLAY_TEXTURE)
        {
            texture->unbind();
        }

        /* display polarization vectors */
        if(displayPolarizationVectors)
        {
            polVectors = (PolarizationVectors*)overlayCache->getFace(facesv[i], vectorsNside, mollview, MapOverlay::POLARIZATION_VECTORS);
            polVectors->draw();
        }
    }

    if(roi!=NULL)
    {
        roi->draw();
    }

    /* display grid */
    if(displayGrid)
    {
        glColor3f(1.0, 1.0, 1.0);
        grid->draw();
    }
}

/* enable polarization vectors */
void Tesselation::showPolarizationVectors(bool show)
{
    if(healpixmap->hasPolarization())
        displayPolarizationVectors = show;
}


void Tesselation::updateTextureThreshold(ColorMap* colorMap, float min, float max)
{
    textureCache->updateTextureThreshold(colorMap, min, max);
}


void Tesselation::changeMapField(HealpixMap::MapType field)
{
    qDebug() << "Updating Map Field to " << HealpixMap::mapTypeToString(field);
    textureCache->changeMapField(field);
}

void Tesselation::showGrid(bool show)
{
    if(!mollview)
        displayGrid = show;
    else
        displayGrid = false;

    // TODO: grid should be shown in mollweide ?
}

void Tesselation::changeTo3D()
{
    if(mollview)
        mollview = false;
}

void Tesselation::changeToMollweide()
{
    if(!mollview)
        mollview = true;
}

void Tesselation::selectPixels(std::set<int> pixelIndexes)
{    
    //selectedPixels.insert(pixelIndexes.begin(), pixelIndexes.end());
    clearROI();

    roi = new ROI(pixelIndexes, 1024);
}

void Tesselation::clearROI()
{
    if(roi!=NULL)
    {
        delete roi;
        roi = NULL;
    }
}
