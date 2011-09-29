#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <QObject>
#include <QVector>
#include <QList>
#include <QMap>
#include <QQueue>
#include <QMutex>
#include <QFuture>
#include <QtConcurrentRun>
#include <QGLWidget>
#include "texture.h"
#include "healpixmap.h"

#define CACHE_MARGIN 5200//0//3000//12498


typedef QMap<int, Texture*> TextureCacheEntry;
typedef QMap<int, TextureCacheEntry*> TextureCacheTable;


class TextureCache : public QObject
{
    Q_OBJECT

public:
    TextureCache(HealpixMap* map, int minNside, int maxNside, int maxTiles = TEXTURECACHE_MAX_TILES);
    ~TextureCache();

    /* inform cache about the faces that are visible and the nside used, so cache can preview the next faces to preload */
    //void updateStatus(QVector<int> visibleFaces, int nside);

    /* get the face with faceNumber and for the given nside */
    Texture* getFace(int faceNumber, int nside);

    /* preload face */
    void preloadFace(int faceNumber, int nside);

    void updateStatus(QVector<int> faces, int nside);

    /* discard old faces on cache */
    bool cleanCache(int minSpace=0);

    void updateTextureThreshold(float min, float max);
    void changeMapField(HealpixMap::MapType field);

    void getTextureMinMax(float &min, float &max);

signals:
    void newFaceAvailable(Texture* face);
    void newFaceAvailable(bool cleanCache);

private:
    /* store face in the cache */
    bool storeFace(int faceNumber, int nside, Texture* face);
    /* discard face in cache */
    void discardFace(int faceNumber, int nside);
    /* calculate the new face */
    Texture* loadFace(int faceNumber, int nside);
    /* check if face is available on cache */
    bool checkFaceAvailable(int faceNumber, int nside);
    /* get face from cache */
    Texture* getFaceFromCache(int faceNumber, int nside);
    /* get best face available in cache */
    Texture* getBestFaceFromCache(int faceNumber, int nside);

    void generateBaseTextures();
    void invalidateCache();

    int Min_Nside;
    int Max_Nside;

    /* return the number of tiles necessary for display the face with nside */
    int calculateFaceTiles(int nside);

    TextureCacheTable textureCache;         /* structure to store cache faces */
    int maxTiles;            /* max vertices to store in cache */
    int availableTiles;      /* free space in cache */
    int marginTilesSpace;     /* margin of free space, after that start cleaning some old entries */

    HealpixMap* healpixMap;

    QList<int> cacheControl;
    QSet<int>  requestedFaces;
    QSet<int>  facesInUse;
    //QList<int> supportedNsides;

    QMutex cacheAccess;

    float minTex, maxTex;
};

#endif // TEXTURECACHE_H
