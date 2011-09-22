#include "tesselation.h"

Tesselation::Tesselation(int _nside, bool _mollview, FaceCache* faceCache, TextureCache* textureCache, OverlayCache* overlayCache)
{
    nside = _nside;
    mollview = _mollview;

    /* by default dont show the polarization vectors neither grid */
    displayPolarizationVectors = false;
    displayGrid = false;

    this->faceCache = faceCache;
    this->textureCache = textureCache;
    this->overlayCache = overlayCache;
    //this->mapType = mapType;

    createInitialTesselation();

    /* create grid */
    grid = new Grid(GRID_LINES);
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
void Tesselation::updateNside(int newNside)
{
    if(nside!=newNside)
    {
        this->nside = newNside;
    }
    qDebug() << "nside updated to " << newNside;
}

/* get visible faces */
void Tesselation::updateVisibleFaces(QVector<int> faces)
{
    this->facesv = faces;

    /* tell cache which faces are visible */
    faceCache->updateStatus(faces, nside);
    if(DISPLAY_TEXTURE)
        textureCache->updateStatus(faces, nside);
}


/* preload faces */
void Tesselation::preloadFaces(QVector<int> faces, int nside)
{
    for(int i=0; i<faces.size(); i++)
    {
        faceCache->preloadFace(faces[i], nside);
        if(DISPLAY_TEXTURE)
            textureCache->preloadFace(faces[i], nside);
    }
}

/*
void Tesselation::getFace(int faceNumber)
{
    Face* face = faceCache->getFace(faceNumber, nside);
    visibleFaces.append(face);
}
*/

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
        face = faceCache->getFace(facesv[i], nside);

        /* display texture */
        if(DISPLAY_TEXTURE)
        {
            texture = textureCache->getFace(facesv[i], nside);
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
            //qDebug() << "Displaying polarization vectors";
            polVectors = (PolarizationVectors*)overlayCache->getFace(facesv[i], nside, MapOverlay::POLARIZATION_VECTORS);
            polVectors->draw();
        }

        /* display grid */
        if(displayGrid)
        {
            glColor3f(1.0, 1.0, 1.0);
            grid->draw();
        }
    }
}

/* enable polarization vectors */
void Tesselation::showPolarizationVectors(bool show)
{
    displayPolarizationVectors = show;
}


void Tesselation::updateTextureThreshold(float min, float max)
{
    textureCache->updateTextureThreshold(min, max);
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
