#include "overlaycache.h"

OverlayCache::OverlayCache(HealpixMap* map, int minNside, int maxNside, int maxTiles)
{
    this->MIN_NSIDE = minNside;
    this->MAX_NSIDE = maxNside;
    this->maxTiles = maxTiles;
    this->availableTiles = maxTiles;

    marginTilesSpace = CACHE_MARGIN;

    //supportedNsides << 64 << 128 << 256 << 512 << 1024 << 2048;

    /* open healpix map */
    healpixMap = map;
}


/* PUBLIC INTERFACE */

/* get the overlay for faceNumber and for the given nside */
MapOverlay* OverlayCache::getFace(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    MapOverlay* overlay;

    /* verify the maximum nside available */
    // TODO: throw exception ?
    if(nside>MAX_NSIDE)
        nside = MAX_NSIDE;

    bool locked = true;
    long faceId = calculateFaceId(faceNumber, nside, type);

    /* request access to cache */
    cacheAccess.lock();

    /* nside BASE_NSIDE face are not inserted into lru list to never get deleted */
    if(nside>MIN_NSIDE)
    {
        /* update lru info */
        cacheControl.removeAll(faceId);
        cacheControl.push_back(faceId);
    }

    //qDebug() << "OverlayCache: Getting face " << faceNumber << "(" << nside << ")";

    /* check if available in cache */
    if(!checkFaceAvailable(faceNumber, nside, type))
    {
        /* initial nside, so wait for overlay to load */
        if(nside==MIN_NSIDE)
        //if(true)
        {
            /* release accesss to cache */
            cacheAccess.unlock();
            locked = false;

            /* wait until to load face and then return it */
            return loadFace(faceNumber, nside, type);
        }
        else
        {
            /* face already requested ? */
            if(!requestedFaces.contains(faceId))
            {
                //REMOVE qDebug() << "Overlay " << faceNumber << " (" << nside << ") -> Miss";

                requestedFaces.insert(faceId);

                /* release accesss to cache */
                cacheAccess.unlock();
                locked = false;

                /* create the face (new thread) */
                QtConcurrent::run(this, &OverlayCache::loadFace, faceNumber, nside, type);
            }
        }
    }

    /* request accesss to cache */
    if(!locked)
        cacheAccess.lock();

    /* return best available nside for wanted face */
    overlay = getBestFaceFromCache(faceNumber, nside, type);

    /* release access to cache */
    cacheAccess.unlock();

    return overlay;

    //predictNeededFaces();
}


/* PRIVATE METHODS */

/* calculate the new face */
MapOverlay* OverlayCache::loadFace(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    //REMOVE qDebug()<< "OverlayCache: loading face " << faceNumber << "(" << nside << ")";
    MapOverlay* overlay = NULL;

    switch(type)
    {
        case MapOverlay::POLARIZATION_VECTORS:
            overlay = new PolarizationVectors(faceNumber, nside, healpixMap);
            break;
        default:
            // TODO: throw exception
            qDebug("OverlayCache: overlay type not supported");
    }

#if DEBUG > 0
    if(overlay==NULL)
        qDebug("Overlay is null");
#endif

    overlay->build();

    /* store the face into the cache */
    bool clean = storeFace(faceNumber, nside, type, overlay);

#if DEBUG > 0
    qDebug() << "Overlay " << faceNumber << "(" << nside << ") - Loaded";
#endif

    /* remove the request */
    long faceId = calculateFaceId(faceNumber, nside, type);
    requestedFaces.remove(faceId);

    if(nside>MIN_NSIDE)
        emit(newFaceAvailable(clean));
        //emit(newFaceAvailable(face));

    //qDebug("Returning overlay");

    return overlay;
}

bool OverlayCache::cleanCache(int minSpace)
{
    // TODO: discard the not used faces to get space for new faces - better algorithm

#if DEBUG > 0
    qDebug("============Cleaning cache ==================");
#endif

    long lru;
    int fn, ns, _type;
    MapOverlay::OverlayType type;
    int nFacesInUse = 0;

    /* request access to cache */
    cacheAccess.lock();

#if DEBUG > 0
    qDebug() << "OverlayCache Margin tiles: " << marginTilesSpace;
    qDebug() << "OverlayCache Available tiles: " << availableTiles;
#endif

    while(availableTiles<marginTilesSpace && cacheControl.size()>nFacesInUse)
    //while(availableTiles<marginTilesSpace && !cacheControl.empty())
    //while(minSpace>=availableTiles-marginTilesSpace && !cacheControl.empty())
    {
        lru = cacheControl.front();
        cacheControl.pop_front();

        if(!facesInUse.contains(lru))
        {
            /* face is not in use, so can be deleted */
            fn = (lru/1000000)-1;
            ns = (lru-(fn+1)*1000000) / 100;
            _type = lru-(fn+1)*1000000-ns*100;
            type = (MapOverlay::OverlayType)_type;
            discardFace(fn, ns, type);
        }
        else
            nFacesInUse++;
    }

    /* release cache access */
    cacheAccess.unlock();

    return true;
}

void OverlayCache::preloadFace(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    //qDebug() << "Preload overlay " << faceNumber << "(" << nside << ") - waiting";

    // TODO: throw exception ?
    if(nside>MAX_NSIDE)
        nside = MAX_NSIDE;

    /* request access to cache */
    cacheAccess.lock();

    //qDebug() << "Preload overlay " << faceNumber << "(" << nside << ") - has access";

    if(!checkFaceAvailable(faceNumber, nside, type))
    {
        long faceId = calculateFaceId(faceNumber, nside, type);

        if(!requestedFaces.contains(faceId))
        {
            //REMOVE qDebug() << "Overlay " << faceNumber << " (" << nside << ") -> Preloading";

            requestedFaces.insert(faceId);

            /* release access to cache */
            cacheAccess.unlock();

            //qDebug() << "Requested Face " << faceId;

            /* create the face (new thread) */
            QtConcurrent::run(this, &OverlayCache::loadFace, faceNumber, nside, type);
        }
    }
    else
    {
        //qDebug("Nothing to preload");

        /* release access to cache */
        cacheAccess.unlock();
    }
}


void OverlayCache::updateStatus(QVector<int> faces, int nside)
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
bool OverlayCache::storeFace(int faceNumber, int nside, MapOverlay::OverlayType type, MapOverlay* overlay)
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
        OverlayCacheEntry* faceEntry;

        int cacheIdx = getCacheIndex(type, nside);

        if(overlayCache.contains(cacheIdx))
            faceEntry = overlayCache[cacheIdx];
        else
            faceEntry = new OverlayCacheEntry;

        faceEntry->insert(faceNumber, overlay);
        overlayCache.insert(cacheIdx, faceEntry);

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
        qDebug("no space left on overlay cache");
        //return false;
    }

    if(availableTiles<marginTilesSpace)
        return true;
    else
        return false;
}

/* discard face in cache */
void OverlayCache::discardFace(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    // TODO: do this in an atomic operation */

#if DEBUG > 0
    qDebug() << "Discarding overlay " << faceNumber << " with nside " << nside;
#endif

    /* free the allocated memory for face */
    MapOverlay* overlay = getFaceFromCache(faceNumber, nside, type);
    delete overlay;

    int cacheIdx = getCacheIndex(type, nside);

    /* remove the entry */
    OverlayCacheEntry* entry = overlayCache.value(cacheIdx);
    entry->remove(faceNumber);

    /* update available tiles */
    availableTiles += calculateFaceTiles(nside);
}

/* check if face is available on cache */
bool OverlayCache::checkFaceAvailable(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    int cacheIdx = getCacheIndex(type, nside);

    //qDebug() << "check if available: face " << faceNumber << " nside " << nside << " cacheIdx=" << cacheIdx;
    if(overlayCache.contains(cacheIdx))
    {
        OverlayCacheEntry* entry = overlayCache.value(cacheIdx);
        if(entry->contains(faceNumber))
        {
            return true;
        }
    }
    return false;
}

/* get face from cache */
MapOverlay* OverlayCache::getFaceFromCache(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    int cacheIdx = getCacheIndex(type, nside);

    OverlayCacheEntry* entry = overlayCache.value(cacheIdx);
    return entry->value(faceNumber);
}


MapOverlay* OverlayCache::getBestFaceFromCache(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    bool faceAvailable;
    do
    {
        faceAvailable = checkFaceAvailable(faceNumber, nside, type);
        if(!faceAvailable)
        {
            if(nside > MIN_NSIDE)
                nside = nside/2;
            /* search for downgraded nside */
            //int pos = supportedNsides.indexOf(nside);
            //if(pos>0)
            //    nside = supportedNsides[pos-1];
        }
    }while(!faceAvailable);

    return getFaceFromCache(faceNumber, nside, type);
}


/* AUXILIAR FUNCTIONS */

/* return the number of tiles (tiles of nside64) necessary for display the face with nside */
int OverlayCache::calculateFaceTiles(int nside)
{
    return pow(nside/MIN_NSIDE, 2);
}

int OverlayCache::getCacheIndex(MapOverlay::OverlayType type, int nside)
{
    return nside*100 + type;
}

long OverlayCache::calculateFaceId(int faceNumber, int nside, MapOverlay::OverlayType type)
{
    return (faceNumber+1)*1000000 + nside*100 + type;
}
