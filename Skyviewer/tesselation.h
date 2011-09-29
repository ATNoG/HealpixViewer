#ifndef TESSELATION_H
#define TESSELATION_H

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
#include "grid.h"
#include "configs.h"

class Tesselation
{

struct faceInfo
{
    int faceNumber;
    int nside;
};

public:
    Tesselation(int nside, bool mollview, FaceCache* faceCache, TextureCache* textureCache, OverlayCache* overlayCache);
    ~Tesselation();
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
    void showGrid(bool show=true);

    void updateTextureThreshold(float min, float max);
    void changeMapField(HealpixMap::MapType field);

    void changeTo3D();
    void changeToMollweide();

private:
    int nside;
    //QMap<int, Face*> availableFaces;

    void createInitialTesselation();
    void createGrid();
    //void getFace(int faceNumber);

    bool mollview;
    bool displayPolarizationVectors;
    bool displayGrid;

    FaceCache* faceCache;
    TextureCache* textureCache;
    OverlayCache* overlayCache;
    QVector<int> facesv;
    QVector<Face*> visibleFaces;

    Grid *grid;

    HealpixMap* healpixmap;
    //HealpixMap::MapType mapType;
};

#endif // TESSELATION_H
