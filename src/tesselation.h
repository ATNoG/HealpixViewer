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
#include "types.h"
#include <set>

class Tesselation
{

struct faceInfo
{
    int faceNumber;
    int nside;
};

public:
    Tesselation(int _textureNside, int _tessNside, int _pVecNside, bool mollview, FaceCache* faceCache, TextureCache* textureCache, OverlayCache* overlayCache, Grid* grid, int maxNside, TextureCache* textureCacheOverlay);
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

    void updateTextureThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset);
    void changeMapField(HealpixMap::MapType field);

    void changeTo3D();
    void changeToMollweide();

    void updateAlpha(float alpha);

    void selectPixels(std::set<int> pixelIndexes, int nside);
    void unselectPixels(std::set<int> pixelIndexes);
    void clearROI();

    void setMollweideRotation(int xRot, int yRot);

    void setCoordSysFrame(Frame* frame);

private:
    int tesselationNside;
    int textureNside;
    int vectorsNside;

    bool secondMap;

    //QMap<int, Face*> availableFaces;

    int xRot, yRot;

    void createInitialTesselation();
    void createGrid();
    //void getFace(int faceNumber);

    bool mollview;
    bool displayPolarizationVectors;
    bool displayGrid;
    bool needToUpdateRotation;

    ROI* roi;
    FaceCache* faceCache;
    TextureCache* textureCache;
    TextureCache* textureCacheOverlay;
    OverlayCache* overlayCache;
    QVector<int> facesv;
    QVector<Face*> visibleFaces;
    ROIManager* manager;
    float overlayAlpha;

    std::set<int> selectedPixels;

    Grid *grid;

    HealpixMap* healpixmap;
    Frame* coordSysFrame;
    //HealpixMap::MapType mapType;
};

#endif // TESSELATION_H
