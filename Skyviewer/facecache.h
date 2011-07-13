#ifndef FACECACHE_H
#define FACECACHE_H

#include <QVector>
#include <QList>
#include <QMap>
#include <QMutex>
#include "face.h"

#define MAX_TILES 12500
#define CACHE_MARGIN 3000//12498
//2500

typedef QMap<int, Face*> CacheEntry;
typedef QMap<int, CacheEntry> Cache;


struct lruEntry
{
    int faceNumber;
    int nside;
};

class FaceCache
{

public:
    FaceCache(int maxTiles = MAX_TILES);

    /* inform cache about the faces that are visible and the nside used, so cache can preview the next faces to preload */
    void updateStatus(QVector<int> visibleFaces, int nside);

    /* get the face with faceNumber and for the given nside */
    Face* getFace(int faceNumber, int nside);

    /* preload face */
    void preloadFace(int faceNumber, int nside);

    static FaceCache *instance()
    {
        if(!s_instance)
            s_instance = new FaceCache;
        return s_instance;
    }

private:
    /* store face in the cache */
    bool storeFace(int faceNumber, int nside, Face* face);
    /* discard face in cache */
    void discardFace(int faceNumber, int nside);
    /* calculate the new face */
    Face* loadFace(int faceNumber, int nside);
    /* check if face is available on cache */
    bool checkFaceAvailable(int faceNumber, int nside);
    /* get face from cache */
    Face* getFaceFromCache(int faceNumber, int nside);
    /* discard old faces on cache */
    bool cleanCache(int minSpace=0);

    /* return the number of tiles necessary for display the face with nside */
    int calculateFaceTiles(int nside);

    Cache faceCache;         /* structure to store cache faces */
    int maxTiles;            /* max vertices to store in cache */
    int availableTiles;      /* free space in cache */
    int marginTilesSpace;     /* margin of free space, after that start cleaning some old entries */

    static FaceCache *s_instance;

    QList<int> cacheControl;
};

#endif // FACECACHE_H
