#include "overlaycache.h"

OverlayCache::OverlayCache(HealpixMap* map, int minNside, int maxNside, int maxTiles)
{
    this->Min_Nside = minNside;
    this->Max_Nside = maxNside;
    this->maxTiles = maxTiles;
    this->availableTiles = maxTiles;

    this->magnification = 1.0;
    this->vectorsSpacing = DEFAULT_VECTORS_SPACING;

    map->getMagMinMax(this->minMag, this->maxMag);

    marginTilesSpace = CACHE_MARGIN;

    //supportedNsides << 64 << 128 << 256 << 512 << 1024 << 2048;

    /* open healpix map */
    healpixMap = map;

    if(map->hasPolarization())
        generateBaseOverlays();
}


OverlayCache::~OverlayCache()
{
    #if DEBUG > 0
        qDebug() << "Calling OverlayCache destructor";
    #endif

    QList<int> nsides = overlayCache.keys();
    foreach(int nside, nsides)
    {
        OverlayCacheEntry* entry = overlayCache.value(nside);
        QList<int> faces = entry->keys();
        foreach(int faceNumber, faces)
        {
            PolarizationVectors *polvectors = (PolarizationVectors*)entry->value(faceNumber);
            delete polvectors;
        }
        delete entry;
    }
/*
    nsides = overlayCacheMollweide.keys();
    foreach(int nside, nsides)
    {
        OverlayCacheEntry* entry = overlayCacheMollweide.value(nside);
        QList<int> faces = entry->keys();
        foreach(int faceNumber, faces)
        {
            PolarizationVectors *polvectors = (PolarizationVectors*)entry->value(faceNumber);
            delete polvectors;
        }
        delete entry;
    }*/
}


/* PUBLIC INTERFACE */

/* get the overlay for faceNumber and for the given nside */
MapOverlay* OverlayCache::getFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    MapOverlay* overlay;

    /* verify the maximum nside available */
    // TODO: throw exception ?
    if(nside>Max_Nside)
        nside = Max_Nside;

    bool locked = true;
    long faceId = calculateFaceId(faceNumber, nside, mollweide, type);

    /* request access to cache */
    cacheAccess.lock();

    /* nside BASE_NSIDE face are not inserted into lru list to never get deleted */
    if(nside>Min_Nside)
    {
        /* update lru info */
        cacheControl.removeAll(faceId);
        cacheControl.push_back(faceId);
    }

    //qDebug() << "OverlayCache: Getting face " << faceNumber << "(" << nside << ")";

    /* check if available in cache */
    if(!checkFaceAvailable(faceNumber, nside, mollweide, type))
    {
        /* initial nside, so wait for overlay to load */
        if(nside==Min_Nside)
        {
            /* release accesss to cache */
            cacheAccess.unlock();
            locked = false;

            /* wait until to load face and then return it */
            return loadFace(faceNumber, nside, mollweide, type);
        }
        else
        {
            /* face already requested ? */
            if(!requestedFaces.contains(faceId))
            {
                //qDebug() << "Overlay " << faceNumber << " (" << nside << ") -> Miss";

                requestedFaces.insert(faceId);

                /* release accesss to cache */
                cacheAccess.unlock();
                locked = false;

                /* create the face (new thread) */
                QtConcurrent::run(this, &OverlayCache::loadFace, faceNumber, nside, mollweide, type);
            }
        }
    }

    /* request accesss to cache */
    if(!locked)
        cacheAccess.lock();

    /* return best available nside for wanted face */
    overlay = getBestFaceFromCache(faceNumber, nside, mollweide, type);

    /* release access to cache */
    cacheAccess.unlock();

    return overlay;

    //predictNeededFaces();
}


/* PRIVATE METHODS */

/* calculate the new face */
MapOverlay* OverlayCache::loadFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    //REMOVE qDebug()<< "OverlayCache: loading face " << faceNumber << "(" << nside << ")";
    MapOverlay* overlay = NULL;

    switch(type)
    {
        case MapOverlay::POLARIZATION_VECTORS:
            overlay = new PolarizationVectors(faceNumber, nside, mollweide, healpixMap, minMag, maxMag, magnification, vectorsSpacing);
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
    bool clean = storeFace(faceNumber, nside, mollweide, type, overlay);

#if DEBUG > 0
    qDebug() << "Overlay " << faceNumber << "(" << nside << ") - Loaded";
#endif

    /* remove the request */
    long faceId = calculateFaceId(faceNumber, nside, mollweide, type);
    requestedFaces.remove(faceId);

    if(nside>Min_Nside)
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
    int fn, ns;
    MapOverlay::OverlayType type;
    int nFacesInUse = 0;
    bool mollweide;

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
            decodeFaceId(lru, fn, ns, mollweide, type);
            discardFace(fn, ns, mollweide, type);
        }
        else
            nFacesInUse++;
    }

    /* release cache access */
    cacheAccess.unlock();

    return true;
}

void OverlayCache::preloadFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    //qDebug() << "Preload overlay " << faceNumber << "(" << nside << ") - waiting";

    // TODO: throw exception ?
    if(nside>Max_Nside)
        nside = Max_Nside;

    /* request access to cache */
    cacheAccess.lock();

    //qDebug() << "Preload overlay " << faceNumber << "(" << nside << ") - has access";

    if(!checkFaceAvailable(faceNumber, nside, mollweide, type))
    {
        long faceId = calculateFaceId(faceNumber, nside, mollweide, type);

        if(!requestedFaces.contains(faceId))
        {
            //REMOVE qDebug() << "Overlay " << faceNumber << " (" << nside << ") -> Preloading";

            requestedFaces.insert(faceId);

            /* release access to cache */
            cacheAccess.unlock();

            //qDebug() << "Requested Face " << faceId;

            /* create the face (new thread) */
            QtConcurrent::run(this, &OverlayCache::loadFace, faceNumber, nside, mollweide, type);
        }
    }
    else
    {
        //qDebug("Nothing to preload");

        /* release access to cache */
        cacheAccess.unlock();
    }
}


void OverlayCache::updateStatus(QVector<int> faces, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    int faceId;

    cacheAccess.lock();

    facesInUse.clear();
    for(int i=0; i<faces.size(); i++)
    {
        faceId = calculateFaceId(faces[i], nside, mollweide, type);
        facesInUse.insert(faceId);
    }

    cacheAccess.unlock();
}


// TODO: create a proccess list with priorities


/* CACHE OPERATIONS FUNCTIONS */

/* store face in the cache */
bool OverlayCache::storeFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type, MapOverlay* overlay)
{
    OverlayCacheTable *cache = &overlayCache;
    /*
    if(!mollweide)
        cache = &overlayCache;
    else
        cache = &overlayCacheMollweide;
        */

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

        int cacheIdx = getCacheIndex(type, nside, mollweide);

        if(cache->contains(cacheIdx))
            faceEntry = cache->value(cacheIdx);
        else
            faceEntry = new OverlayCacheEntry;

        faceEntry->insert(faceNumber, overlay);
        cache->insert(cacheIdx, faceEntry);

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
void OverlayCache::discardFace(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    // TODO: do this in an atomic operation */

    OverlayCacheTable *cache = &overlayCache;

#if DEBUG > 0
    qDebug() << "Discarding overlay " << faceNumber << " with nside " << nside;
#endif

    if(checkFaceAvailable(faceNumber, nside, mollweide, type))
    {
        /* free the allocated memory for face */
        MapOverlay* overlay = getFaceFromCache(faceNumber, nside, mollweide, type);

        if(overlay!=NULL)
        {
            delete overlay;

            int cacheIdx = getCacheIndex(type, nside, mollweide);

            /* remove the entry */
            OverlayCacheEntry* entry = cache->value(cacheIdx);
            entry->remove(faceNumber);

            /* update available tiles */
            availableTiles += calculateFaceTiles(nside);
        }
    }
}

/* check if face is available on cache */
bool OverlayCache::checkFaceAvailable(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    int cacheIdx = getCacheIndex(type, nside, mollweide);


    OverlayCacheTable *cache = &overlayCache;
    /*
    if(!mollweide)
        cache = &overlayCache;
    else
        cache = &overlayCacheMollweide;
        */

    //qDebug() << "check if available: face " << faceNumber << " nside " << nside << " cacheIdx=" << cacheIdx;
    if(cache->contains(cacheIdx))
    {
        OverlayCacheEntry* entry = cache->value(cacheIdx);
        if(entry->contains(faceNumber))
        {
            return true;
        }
    }
    return false;
}

/* get face from cache */
MapOverlay* OverlayCache::getFaceFromCache(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    int cacheIdx = getCacheIndex(type, nside, mollweide);
    OverlayCacheTable *cache = &overlayCache;

    MapOverlay *overlay = NULL;

    if(cache->contains(cacheIdx))
    {
        OverlayCacheEntry* entry = cache->value(cacheIdx);

        if(entry->contains(faceNumber))
            overlay = entry->value(faceNumber);
    }

    return overlay;
}


MapOverlay* OverlayCache::getBestFaceFromCache(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    //qDebug() << "Getting best face from cache " << faceNumber << ", " << nside;

    bool faceAvailable;
    do
    {
        faceAvailable = checkFaceAvailable(faceNumber, nside, mollweide, type);
        if(!faceAvailable)
        {
            if(nside > Min_Nside)
                nside = nside/2;
            /* search for downgraded nside */
            //int pos = supportedNsides.indexOf(nside);
            //if(pos>0)
            //    nside = supportedNsides[pos-1];
        }
    }while(!faceAvailable);

    return getFaceFromCache(faceNumber, nside, mollweide, type);
}


void OverlayCache::setPolarizationMagThreshold(double minMag, double maxMag)
{
    if(minMag!=this->minMag || maxMag!=this->maxMag)
    {
        //qDebug() << "==> Update mag threshold: " << minMag << "," << maxMag;
        this->minMag = minMag;
        this->maxMag = maxMag;
        invalidateCache();
    }
}


void OverlayCache::setPolarizationMagnification(double magnification)
{
    if(magnification!=this->magnification)
    {
        //qDebug() << "==> Update magnification";
        this->magnification = magnification;
        invalidateCache();
    }
}


void OverlayCache::setVectorsSpacing(int spacing)
{
    if(spacing!=this->vectorsSpacing)
    {
        this->vectorsSpacing = spacing;
        invalidateCache();
    }
}


/* AUXILIAR FUNCTIONS */

/* return the number of tiles (tiles of nside64) necessary for display the face with nside */
int OverlayCache::calculateFaceTiles(int nside)
{
    return pow((double) nside/Min_Nside, 2);
}

int OverlayCache::getCacheIndex(MapOverlay::OverlayType type, int nside, bool mollweide)
{
    return nside*1000 + type*10 + (mollweide?1:0);
}

long OverlayCache::calculateFaceId(int faceNumber, int nside, bool mollweide, MapOverlay::OverlayType type)
{
    return (faceNumber+1)*10000000 + nside*1000 + type*10 + (mollweide?1:0);
}

void OverlayCache::decodeFaceId(long faceId, int &faceNumber, int &nside, bool &mollweide, MapOverlay::OverlayType &type)
{
    int _type, _mollweide;
    faceNumber = (faceId/10000000)-1;
    nside = (faceId-(faceNumber+1)*10000000) / 1000;
    _type = (faceId-(faceNumber+1)*10000000-nside*1000)/10;
    type = (MapOverlay::OverlayType)_type;
    _mollweide = faceId-(faceNumber+1)*10000000-nside*1000-_type*10;
    mollweide = (_mollweide==1?true:false);
}

void OverlayCache::generateBaseOverlays()
{
    /* 3d overlays */
    for(int i=0; i<12; i++)
        loadFace(i, Min_Nside, false, MapOverlay::POLARIZATION_VECTORS);

    /* Mollweide overlays */
    for(int i=0; i<12; i++)
        loadFace(i, Min_Nside, true, MapOverlay::POLARIZATION_VECTORS);
}



void OverlayCache::invalidateCache()
{
    //qDebug() << "Invalidating overlay cache";

    /* generate base overlays */
    generateBaseOverlays();

    /* invalidate current overlays */
    QMap<int, QList<int> > facesToDelete;

    cacheAccess.lock();
    QList<int> nsides = overlayCache.keys();

    for(int i=0; i<nsides.size(); i++)
    {
        if((nsides[i]/1000)>Min_Nside)
        {
            QList<int> faces = overlayCache[nsides[i]]->keys();
            for(int j=0; j<faces.size(); j++)
            {
                facesToDelete[nsides[i]].append(faces[j]);
            }
        }
    }
    cacheAccess.unlock();

    nsides = facesToDelete.keys();
    for(int i=0; i<nsides.size(); i++)
    {
        QList<int> faces = facesToDelete[nsides[i]];
        for(int j=0; j<faces.size(); j++)
        {
            discardFace(faces[j], nsides[i]/1000, true, MapOverlay::POLARIZATION_VECTORS);
            discardFace(faces[j], nsides[i]/1000, false, MapOverlay::POLARIZATION_VECTORS);
        }
    }

    /* generate overlays in use */
    QSetIterator<int> i(facesInUse);
    while(i.hasNext())
    {
        int fn, ns;
        bool moll;
        MapOverlay::OverlayType type;

        int faceId = i.next();
        decodeFaceId(faceId, fn, ns, moll, type);

        if(ns>Min_Nside)
        {
            // add to requested faces
            cacheAccess.lock();
            requestedFaces.insert(faceId);
            cacheAccess.unlock();

            // load texture in thread
            QtConcurrent::run(this, &OverlayCache::loadFace, fn, ns, moll, type);
        }
    }
}
