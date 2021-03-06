#ifndef OVERLAYCACHE_H
#define OVERLAYCACHE_H

#include <QObject>
#include <QVector>
#include <QList>
#include <QMap>
#include <QQueue>
#include <QMutex>
#include <QFuture>
#include <QtConcurrentRun>
#include <QGLWidget>
#include "healpixmap.h"
#include "mapoverlay.h"
#include "polarizationvectors.h"

#define CACHE_MARGIN 5200//0//3000//12498


typedef QMap<int, MapOverlay*> OverlayCacheEntry;
typedef QMap<int, OverlayCacheEntry*> OverlayCacheTable;


class OverlayCache : public QObject
{
    Q_OBJECT

public:
    OverlayCache(HealpixMap* map, int minNside, int maxNside, int maxTiles = OVERLAYCACHE_MAX_TILES);
    ~OverlayCache();

    /* inform cache about the faces that are visible and the nside used, so cache can preview the next faces to preload */
    //void updateStatus(QVector<int> visibleFaces, int nside);

    /* get the overlay for faceNumber and for the given nside */
    MapOverlay* getFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);

    /* preload face */
    void preloadFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);

    void updateStatus(QVector<int> faces, int nside, bool mollweide, MapOverlay::OverlayType type);

    /* discard old faces on cache */
    bool cleanCache(int minSpace=0);

    void setPolarizationMagThreshold(double minMag, double maxMag);
    void setPolarizationMagnification(double magnification);
    void setVectorsSpacing(int spacing);

signals:
    void newFaceAvailable(MapOverlay* face);
    void newFaceAvailable(bool cleanCache);

private:
    /* store face in the cache */
    bool storeFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type, MapOverlay* overlay);
    /* discard face in cache */
    void discardFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);
    /* calculate the new face */
    MapOverlay* loadFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);
    /* check if face is available on cache */
    bool checkFaceAvailable(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);
    /* get face from cache */
    MapOverlay* getFaceFromCache(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);
    /* get best face available in cache */
    MapOverlay* getBestFaceFromCache(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);
    /* get cache index */
    int getCacheIndex(MapOverlay::OverlayType type, int nside, bool mollweide);
    /* calculate face id used in cache */
    long calculateFaceId(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type);

    void decodeFaceId(long faceId, int &faceNumber, int &nside, bool &mollweide, MapOverlay::OverlayType &type);

    int Min_Nside;
    int Max_Nside;

    /* return the number of tiles necessary for display the face with nside */
    int calculateFaceTiles(int nside);

    void printCache();

    void generateBaseOverlays();
    void invalidateCache();

    OverlayCacheTable overlayCache;             /* structure to store cache overlays */
    //OverlayCacheTable overlayCacheMollweide;    /* structure to store mollweide cache overlays */
    int maxTiles;                           /* max vertices to store in cache */
    int availableTiles;                     /* free space in cache */
    int marginTilesSpace;                   /* margin of free space, after that start cleaning some old entries */

    double minMag, maxMag, magnification;
    int vectorsSpacing;

    HealpixMap* healpixMap;

    QList<long> cacheControl;
    QSet<long>  requestedFaces;
    QSet<int>  facesInUse;

    QMutex cacheAccess;
};

#endif // OVERLAYCACHE_H
