#include "facecache.h"

FaceCache* FaceCache::s_instance = 0;

FaceCache::FaceCache(int maxTiles)
{
    this->maxTiles = maxTiles;
    this->availableTiles = maxTiles;

    marginTilesSpace = CACHE_MARGIN;
}


/* PUBLIC INTERFACE */

/* inform cache about the faces that are visible and the nside used, so cache can preview the next faces to preload */
/*
void FaceCache::updateStatus(QVector<int> visibleFaces, int nside)
{

}
*/

/* get the face with faceNumber and for the given nside */
Face* FaceCache::getFace(int faceNumber, int nside)
{
    Face* face;

    /* update lru info */
    int lru = (faceNumber+1)*10000 + nside;
    cacheControl.removeAll(lru);
    cacheControl.push_back(lru);

    //qDebug() << "cache control:";
    //for(int i=0; i<cacheControl.size(); i++)
    //    qDebug() << cacheControl[i];

    //qDebug() << "Getting face " << faceNumber << "(" << nside << ")";

    /* check if available in cache */
    if(checkFaceAvailable(faceNumber, nside))
    {
        qDebug() << "Face " << faceNumber << " available on cache";
        /* face is available on cache, just return it */
        face = getFaceFromCache(faceNumber, nside);
    }
    else
    {
        qDebug() << "Face " << faceNumber << " miss!";

        /* create the face */
        face = loadFace(faceNumber, nside);

        /* load the face into the cache */
        storeFace(faceNumber, nside, face);
    }

    return face;

    //predictNeededFaces();
}



/* PRIVATE METHODS */

/* calculate the new face */
Face* FaceCache::loadFace(int faceNumber, int nside)
{
    Face* face = new Face(faceNumber);
    face->setRigging(nside, false);

    return face;
}

bool FaceCache::cleanCache(int minSpace)
{
    // TODO: discard the not used faces to get space for new faces

    qDebug("cleaning cache");

    int lru = cacheControl.front();
    cacheControl.pop_front();

    qDebug() << "pop " << lru;
    int fn, ns;
    fn = (lru/10000)-1;
    ns = lru-(fn+1)*10000;

    qDebug() << "going to delete face " << fn << " (" << ns << ")";
    discardFace(fn, ns);
}

void FaceCache::predictNeededFaces()
{
    // TODO: algorithm to predict the next faces that will be needed
}


// TODO: create a proccess list with priorities


/* CACHE OPERATIONS FUNCTIONS */

/* store face in the cache */
bool FaceCache::storeFace(int faceNumber, int nside, Face* face)
{
    int neededTiles = calculateFaceTiles(nside);

    qDebug() << "Storing face " << faceNumber << "(" << nside << ")";
    qDebug() << "Needed Tiles: " << neededTiles << ", availableTiles: " << availableTiles;

    // TODO: concurrence
    if(neededTiles>=availableTiles-marginTilesSpace)
    {
        /* discart old faces */
        cleanCache();
    }

    qDebug("face stored");

    /* insert faca on cache */
    CacheEntry faceEntry;

    if(faceCache.contains(nside))
        faceEntry = faceCache[nside];

    faceEntry.insert(faceNumber, face);
    faceCache.insert(nside, faceEntry);

    /* change number of tiles available */
    availableTiles -= neededTiles;

    return true;
}

/* discard face in cache */
void FaceCache::discardFace(int faceNumber, int nside)
{
    // TODO: do this in an atomic operation */

    /* free the allocated memory for face */
    Face* face = getFaceFromCache(faceNumber, nside);
    delete face;

    /* remove the entry */
    CacheEntry entry = faceCache.value(nside);
    entry.remove(nside);

    /* update available tiles */
    availableTiles += calculateFaceTiles(nside);
}

/* check if face is available on cache */
bool FaceCache::checkFaceAvailable(int faceNumber, int nside)
{
    //qDebug() << "check if available: face " << faceNumber << " nside " << nside;
    if(faceCache.contains(nside))
    {
        CacheEntry entry = faceCache.value(nside);
        if(entry.contains(faceNumber))
        {
            return true;
        }
    }
    return false;
}

/* get face from cache */
Face* FaceCache::getFaceFromCache(int faceNumber, int nside)
{
    CacheEntry entry = faceCache.value(nside);
    return entry.value(faceNumber);
}


/* AUXILIAR FUNCTIONS */

/* return the number of tiles (tiles of nside64) necessary for display the face with nside */
int FaceCache::calculateFaceTiles(int nside)
{
    return pow(nside/64, 2);
}
