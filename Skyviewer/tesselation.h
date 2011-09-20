#ifndef TESSELATION_H
#define TESSELATION_H

#define DISPLAY_TEXTURE true

#include <QVector>
#include <QFuture>
#include <QtConcurrentRun>
#include <chealpix.h>
#include <math.h>
#include <set>
#include "face.h"
#include "facecache.h"
#include "texture.h"
#include "texturecache.h"
#include "overlaycache.h"
#include "polarizationvectors.h"
#include "healpixmap.h"

class Tesselation
{

struct faceInfo
{
    int faceNumber;
    int nside;
};

public:
    Tesselation(int nside, bool mollview, FaceCache* faceCache, TextureCache* textureCache, OverlayCache* overlayCache);
    void draw();
    void drawAllFaces();

    /* update to new nside */
    void updateNside(int nside);

    /* update viewport info */
    void updateVisibleFaces(QVector<int> facesv);

    /* preload faces */
    void preloadFaces(QVector<int> faces, int nside);

    void setMap(HealpixMap* map);

    void showPolarizationVectors(bool show=true);

private:
    int nside;
    //QMap<int, Face*> availableFaces;

    void createInitialTesselation();
    //void getFace(int faceNumber);

    bool mollview;
    bool displayPolarizationVectors;

    FaceCache* faceCache;
    TextureCache* textureCache;
    OverlayCache* overlayCache;
    QVector<int> facesv;
    QVector<Face*> visibleFaces;

    HealpixMap* healpixmap;
    //HealpixMap::MapType mapType;
};

#endif // TESSELATION_H
