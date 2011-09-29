#ifndef FACECACHE_H
#define FACECACHE_H

#include <QObject>
#include <QVector>
#include <QList>
#include <QMap>
#include <QQueue>
#include <QMutex>
#include <QFuture>
#include <QtConcurrentRun>
#include <QGLWidget>
#include "face.h"

#define CACHE_MARGIN 5200//0//3000//12498

typedef QMap<int, Face*> FaceCacheEntry;
typedef QMap<int, FaceCacheEntry*> FaceCacheTable;


class FaceCache : public QObject
{
    Q_OBJECT

public:
    FaceCache(int minNside, int maxNside, int maxTiles = FACECACHE_MAX_TILES);
    ~FaceCache();

    /* inform cache about the faces that are visible and the nside used, so cache can preview the next faces to preload */
    //void updateStatus(QVector<int> visibleFaces, int nside);

    /* get the face with faceNumber and for the given nside */
    Face* getFace(int faceNumber, int nside, bool mollweide=false);

    /* preload face */
    void preloadFace(int faceNumber, int nside, bool mollweide=false);

    void updateStatus(QVector<int> faces, int nside, bool mollweide=false);

    static FaceCache *instance(int minNside, int maxNside)
    {
        if(!s_instance)
            s_instance = new FaceCache(minNside, maxNside);
        return s_instance;
    }

    /* discard old faces on cache */
    bool cleanCache(int minSpace=0);

signals:
    void newFaceAvailable(Face* face);
    void newFaceAvailable(bool cleanCache);

private:
    /* store face in the cache */
    bool storeFace(int faceNumber, int nside, bool mollweide, Face* face);
    /* discard face in cache */
    void discardFace(int faceNumber, int nside, bool mollweide);
    /* calculate the new face */
    Face* loadFace(int faceNumber, int nside, bool mollweide);
    /* check if face is available on cache */
    bool checkFaceAvailable(int faceNumber, int nside, bool mollweide);
    /* get face from cache */
    Face* getFaceFromCache(int faceNumber, int nside, bool mollweide);
    /* get best face available in cache */
    Face* getBestFaceFromCache(int faceNumber, int nside, bool mollweide);

    void generateBaseFaces();

    int Min_Nside;
    int Max_Nside;

    /* return the number of tiles necessary for display the face with nside */
    int calculateFaceTiles(int nside);

    void printCache();

    FaceCacheTable faceCache;           /* structure to store cache faces */
    FaceCacheTable faceCacheMollweide;  /* structure to store mollweide cache faces */
    int maxTiles;            /* max vertices to store in cache */
    int availableTiles;      /* free space in cache */
    int marginTilesSpace;     /* margin of free space, after that start cleaning some old entries */

    static FaceCache *s_instance;

    QList<int> cacheControl;
    QSet<int>  requestedFaces;
    QSet<int>  facesInUse;
    //QList<int> supportedNsides;

    QMutex cacheAccess;
};

#endif // FACECACHE_H
