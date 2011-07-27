#include "tesselation.h"

Tesselation::Tesselation(int _nside, bool _mollview)
{
    nside = _nside;
    mollview = _mollview;

    faceCache = FaceCache::instance();

    createInitialTesselation();
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
}


/* preload faces */
void Tesselation::preloadFaces(QVector<int> faces, int nside)
{
    for(int i=0; i<faces.size(); i++)
    {
        faceCache->preloadFace(faces[i], nside);
    }
}

/*
void Tesselation::getFace(int faceNumber)
{
    Face* face = faceCache->getFace(faceNumber, nside);
    visibleFaces.append(face);
}
*/

/* Draw opengl primitives */
void Tesselation::draw()
{
    int totalFaces = facesv.size();
    Face* face;
    Texture* texture;

    /* get faces */
    for(int i=0; i<totalFaces; i++)
    {
        face = faceCache->getFace(facesv[i], nside);

        if(DISPLAY_TEXTURE)
        {
            //texture = textureCache->getTexture(facesv[i], nside);
            texture = new Texture(facesv[i], nside);
            texture->draw();
        }

        face->draw();
    }
}
