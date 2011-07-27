#include "facecache.h"

QMutex lruMutex;
QMutex cacheMutex;

FaceCache* FaceCache::s_instance = 0;

FaceCache::FaceCache(int maxTiles)
{
    this->maxTiles = maxTiles;
    this->availableTiles = maxTiles;

    marginTilesSpace = CACHE_MARGIN;

    supportedNsides << 64 << 128 << 256 << 512 << 1024 << 2048;
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

    bool locked = true;
    int faceId = (faceNumber+1)*10000 + nside;

    /* request access to cache */
    cacheAccess.lock();

    /* nside BASE_NSIDE face are not inserted into lru list to never get deleted */
    if(nside>BASENSIDE)
    {
        /* update lru info */
        cacheControl.removeAll(faceId);
        cacheControl.push_back(faceId);
    }

    //qDebug() << "FaceCache: Getting face " << faceNumber << "(" << nside << ")";

    /* check if available in cache */
    if(!checkFaceAvailable(faceNumber, nside))
    {        
        /* initial nside, so wait for texture to load */
        if(nside==BASENSIDE)
        {
            /* release accesss to cache */
            cacheAccess.unlock();
            locked = false;

            /* wait until to load face and then return it */
            return loadFace(faceNumber, nside);
        }
        else
        {
            /* face already requested ? */
            if(!requestedFaces.contains(faceId))
            {
                // TODO: descomentar
                qDebug() << "Face " << faceNumber << " (" << nside << ") -> Miss";

                requestedFaces.insert(faceId);

                /* release accesss to cache */
                cacheAccess.unlock();
                locked = false;

                /* create the face (new thread) */
                QtConcurrent::run(this, &FaceCache::loadFace, faceNumber, nside);
            }
        }
    }

    /* request accesss to cache */
    if(!locked)
        cacheAccess.lock();

    /* return best available nside for wanted face */
    face = getBestFaceFromCache(faceNumber, nside);

    /* release access to cache */
    cacheAccess.unlock();

    return face;

    //predictNeededFaces();
}


Face* FaceCache::waitForUpdates()
{
    /* block in queue of finished jobs */
    return newFaces.dequeue();
}


/* PRIVATE METHODS */

/* calculate the new face */
Face* FaceCache::loadFace(int faceNumber, int nside)
{
    //qDebug()<< "------ loading face " << faceNumber << "(" << nside << ")";

    Face* face = new Face(faceNumber);
    if(face==NULL)
        qDebug("Face is null");

    face->setRigging(nside, false);
    //face->createDisplayList();
    //face->createBuffer();

    //qDebug() << "Face loaded " << faceNumber;


    /* store the face into the cache */
    bool clean = storeFace(faceNumber, nside, face);

    qDebug() << "Face " << faceNumber << "(" << nside << ") - Loaded";

    if(nside>BASENSIDE)
        emit(newFaceAvailable(clean));
        //emit(newFaceAvailable(face));

    return face;
}

bool FaceCache::cleanCache(int minSpace)
{
    // TODO: discard the not used faces to get space for new faces - better algorithm

    qDebug("============Cleaning cache ==================");

    int lru, fn, ns;
    int nFacesInUse = 0;

    /* request access to cache */
    cacheAccess.lock();

    qDebug() << "Margin tiles: " << marginTilesSpace;
    qDebug() << "Available tiles: " << availableTiles;

    while(availableTiles<marginTilesSpace && cacheControl.size()>nFacesInUse)
    //while(availableTiles<marginTilesSpace && !cacheControl.empty())
    //while(minSpace>=availableTiles-marginTilesSpace && !cacheControl.empty())
    {
        lru = cacheControl.front();
        cacheControl.pop_front();

        if(!facesInUse.contains(lru))
        {
            /* face is not in use, so can be deleted */
            fn = (lru/10000)-1;
            ns = lru-(fn+1)*10000;
            discardFace(fn, ns);
        }
        else
            nFacesInUse++;
    }

    /* release cache access */
    cacheAccess.unlock();

    return true;
}

void FaceCache::preloadFace(int faceNumber, int nside)
{
    //qDebug() << "Preload face " << faceNumber << "(" << nside << ") - waiting";

    /* request access to cache */
    cacheAccess.lock();

    //qDebug() << "Preload face " << faceNumber << "(" << nside << ") - has access";

    if(!checkFaceAvailable(faceNumber, nside))
    {
        int faceId = (faceNumber+1)*10000 + nside;

        if(!requestedFaces.contains(faceId))
        {
            // TODO: descomentar
            qDebug() << "Face " << faceNumber << " (" << nside << ") -> Preloading";

            requestedFaces.insert(faceId);

            /* release access to cache */
            cacheAccess.unlock();

            //qDebug() << "Requested Face " << faceId;

            /* create the face (new thread) */
            QtConcurrent::run(this, &FaceCache::loadFace, faceNumber, nside);
        }
    }
    else
    {
        //qDebug("Nothing to preload");

        /* release access to cache */
        cacheAccess.unlock();
    }
}


void FaceCache::updateStatus(QVector<int> faces, int nside)
{
    int faceId;

    cacheAccess.lock();

    facesInUse.clear();
    for(int i=0; i<faces.size(); i++)
    {
        faceId = (faces[i]+1)*10000 + nside;
        facesInUse.insert(faceId);
    }

    cacheAccess.unlock();
}


// TODO: create a proccess list with priorities


/* CACHE OPERATIONS FUNCTIONS */

/* store face in the cache */
bool FaceCache::storeFace(int faceNumber, int nside, Face* face)
{

    int neededTiles = calculateFaceTiles(nside);

    /*
    //qDebug() << "Storing face " << faceNumber << "(" << nside << ")";
    //qDebug() << "Needed Tiles: " << neededTiles << ", availableTiles: " << availableTiles;

    // TODO: concurrence
    if(neededTiles>=availableTiles-marginTilesSpace)
    {
        // discart old faces
        cleanCache(neededTiles);
    }
    */

    // TODO: descomentar
    //qDebug() << "face stored " << faceNumber << " (" << nside << ")";


    QMutexLocker locker(&cacheAccess);

    if(availableTiles>=neededTiles)
    {
        /* insert faca on cache */
        CacheEntry faceEntry;

        if(faceCache.contains(nside))
            faceEntry = faceCache[nside];

        faceEntry.insert(faceNumber, face);
        faceCache.insert(nside, faceEntry);

        /* change number of tiles available */
        availableTiles -= neededTiles;

        /* insert new face in queue */
        /*
        if(nside>64)
            newFaces.enqueue(face);
            */

        //return true;
    }
    else
    {
        qDebug("no space left on cache");
        //return false;
    }

    if(availableTiles<marginTilesSpace)
        return true;
    else
        return false;
}

/* discard face in cache */
void FaceCache::discardFace(int faceNumber, int nside)
{
    // TODO: do this in an atomic operation */

    qDebug() << "Discarding face " << faceNumber << " with nside " << nside;

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


Face* FaceCache::getBestFaceFromCache(int faceNumber, int nside)
{
    bool faceAvailable;
    do
    {
        faceAvailable = checkFaceAvailable(faceNumber, nside);
        if(!faceAvailable)
        {
            /* search for downgraded nside */
            int pos = supportedNsides.indexOf(nside);
            if(pos>0)
                nside = supportedNsides[pos-1];
        }
    }while(!faceAvailable);

    return getFaceFromCache(faceNumber, nside);
}


/* AUXILIAR FUNCTIONS */

/* return the number of tiles (tiles of nside64) necessary for display the face with nside */
int FaceCache::calculateFaceTiles(int nside)
{
    return pow(nside/BASENSIDE, 2);
}
