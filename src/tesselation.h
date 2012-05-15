#ifndef TESSELATION_H
#define TESSELATION_H

#include <QVector>
#include <QFuture>
#include <QtConcurrentRun>
#include <chealpix.h>
#include <math.h>
#include <set>
#include "roi.h"
#include "face.h"
#include "facecache.h"
#include "texture.h"
#include "texturecache.h"
#include "overlaycache.h"
#include "polarizationvectors.h"
#include "healpixmap.h"
#include "grid.h"
#include "configs.h"
#include <set>

class Tesselation
{

struct faceInfo
{
    int faceNumber;
    int nside;
};

public:
    Tesselation(int _textureNside, int _tessNside, int _pVecNside, bool mollview, FaceCache* faceCache, TextureCache* textureCache, OverlayCache* overlayCache, int maxNside);
    ~Tesselation();
    void draw();
    void drawAllFaces();

    /* update faces nside */
    void updateTesselationNside(int nside);

    /* update to new nside */
    void updateTextureNside(int nside);

    /* update vectors nside */
    void updateVectorsNside(int nside);

    /* update viewport info */
    void updateVisibleFaces(QVector<int> facesv);

    /* preload faces */
    void preloadFaces(QVector<int> faces, int nside);

    void setMap(HealpixMap* map);

    void showPolarizationVectors(bool show=true);
    void showGrid(bool show=true);

    void updateTextureThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor);
    void changeMapField(HealpixMap::MapType field);

    void changeTo3D();
    void changeToMollweide();

    void selectPixels(std::set<int> pixelIndexes, int nside);
    void unselectPixels(std::set<int> pixelIndexes);
    void clearROI();

private:
    int tesselationNside;
    int textureNside;
    int vectorsNside;
    //QMap<int, Face*> availableFaces;

    void createInitialTesselation();
    void createGrid();
    //void getFace(int faceNumber);

    bool mollview;
    bool displayPolarizationVectors;
    bool displayGrid;

    ROI* roi;
    FaceCache* faceCache;
    TextureCache* textureCache;
    OverlayCache* overlayCache;
    QVector<int> facesv;
    QVector<Face*> visibleFaces;
    ROIManager* manager;

    std::set<int> selectedPixels;

    Grid *grid;

    HealpixMap* healpixmap;
    //HealpixMap::MapType mapType;
};

#endif // TESSELATION_H
