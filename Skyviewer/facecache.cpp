#include "facecache.h"


FaceCache* FaceCache::s_instance = 0;

FaceCache::FaceCache(int minNside, int maxNside, int maxTiles)
{
    this->Min_Nside = minNside;
    this->Max_Nside = maxNside;
    this->maxTiles = maxTiles;
    this->availableTiles = maxTiles;

    marginTilesSpace = CACHE_MARGIN;

    //supportedNsides << 64 << 128 << 256 << 512 << 1024 << 2048;

    generateBaseFaces();
}


FaceCache::~FaceCache()
{
    qDebug() << "Calling FaceCache destructor";

    QList<int> nsides = faceCache.keys();
    foreach(int nside, nsides)
    {
        FaceCacheEntry* entry = faceCache.value(nside);
        QList<int> faces = entry->keys();
        foreach(int faceNumber, faces)
        {
            Face *face = entry->value(faceNumber);
            delete face;
        }
        delete entry;
    }
}


/* PUBLIC INTERFACE */

/* get the face with faceNumber and for the given nside */
Face* FaceCache::getFace(int faceNumber, int nside)
{
    //qDebug() << "Getting face " << faceNumber << "(" << nside << ") from faceCache";
    Face* face;

    bool locked = true;
    int faceId = (faceNumber+1)*10000 + nside;

    /* request access to cache */
    cacheAccess.lock();

    /* nside BASE_NSIDE face are not inserted into lru list to never get deleted */
    if(nside>Min_Nside)
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
        if(nside==Min_Nside)
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
                //qDebug() << "Face " << faceNumber << " (" << nside << ") -> Miss";

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

    //printCache();

    /* release access to cache */
    cacheAccess.unlock();

    return face;

    //predictNeededFaces();
}


/* PRIVATE METHODS */

/* calculate the new face */
Face* FaceCache::loadFace(int faceNumber, int nside)
{
    //qDebug()<< "Face " << faceNumber << "(" << nside << ") - Loading";

    Face* face = new Face(faceNumber);

#if DEBUG > 0
    if(face==NULL)
        qDebug("Face is null");
#endif

    face->setRigging(nside, false);
    //face->createDisplayList();
    //face->createBuffer();

    /* store the face into the cache */
    bool clean = storeFace(faceNumber, nside, face);

#if DEBUG > 0
    qDebug() << "Face " << faceNumber << "(" << nside << ") - Loaded";
#endif

    /* remove the request */
    int faceId = (faceNumber+1)*10000 + nside;
    requestedFaces.remove(faceId);

    if(nside>Min_Nside)
        emit(newFaceAvailable(clean));
        //emit(newFaceAvailable(face));

    return face;
}

bool FaceCache::cleanCache(int minSpace)
{
    // TODO: discard the not used faces to get space for new faces - better algorithm

#if DEBUG > 0
    qDebug("-------------============Cleaning cache ==================-------------");
#endif

    int lru, fn, ns;
    int nFacesInUse = 0;

    /* request access to cache */
    cacheAccess.lock();

    //printCache();

#if DEBUG > 0
    qDebug() << "Margin tiles: " << marginTilesSpace;
    qDebug() << "Available tiles: " << availableTiles;
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
            fn = (lru/10000)-1;
            ns = lru-(fn+1)*10000;
            discardFace(fn, ns);
        }
        else
            nFacesInUse++;
    }

    //printCache();

    /* release cache access */
    cacheAccess.unlock();

    return true;
}

void FaceCache::preloadFace(int faceNumber, int nside)
{
    return;
    qDebug() << "Preload face " << faceNumber << "(" << nside << ")";

    /* request access to cache */
    cacheAccess.lock();

    //qDebug() << "Preload face " << faceNumber << "(" << nside << ") - has access";

    if(!checkFaceAvailable(faceNumber, nside))
    {
        int faceId = (faceNumber+1)*10000 + nside;

        if(!requestedFaces.contains(faceId))
        {
            //REMOVE qDebug() << "Face " << faceNumber << " (" << nside << ") -> Preloading";

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


    if(availableTiles>=neededTiles)
    {
        /* insert face on cache */
        FaceCacheEntry* faceEntry;

        cacheAccess.lock();

        if(faceCache.contains(nside))
            faceEntry = faceCache[nside];
        else
            faceEntry = new FaceCacheEntry;

        faceEntry->insert(faceNumber, face);
        faceCache.insert(nside, faceEntry);

        /* change number of tiles available */
        availableTiles -= neededTiles;

        cacheAccess.unlock();

        /* insert new face in queue */
        /*
        if(nside>64)
            newFaces.enqueue(face);
            */

        //return true;
    }
    else
    {
        qDebug("no space left on face cache");
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

#if DEBUG > 0
    qDebug() << "Discarding face " << faceNumber << " with nside " << nside;
#endif

    /* free the allocated memory for face */
    Face* face = getFaceFromCache(faceNumber, nside);
    delete face;

    /* remove the entry */
    FaceCacheEntry* entry = faceCache.value(nside);
    entry->remove(faceNumber);

    /* update available tiles */
    availableTiles += calculateFaceTiles(nside);
}

/* check if face is available on cache */
bool FaceCache::checkFaceAvailable(int faceNumber, int nside)
{
    //qDebug() << "check if available: face " << faceNumber << " nside " << nside;
    if(faceCache.contains(nside))
    {
        FaceCacheEntry* entry = faceCache.value(nside);
        if(entry->contains(faceNumber))
        {
            return true;
        }
    }
    return false;
}

/* get face from cache */
Face* FaceCache::getFaceFromCache(int faceNumber, int nside)
{
    FaceCacheEntry* entry = faceCache.value(nside);
    return entry->value(faceNumber);
}


Face* FaceCache::getBestFaceFromCache(int faceNumber, int nside)
{
    bool faceAvailable;
    do
    {
        faceAvailable = checkFaceAvailable(faceNumber, nside);
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

    //qDebug() << "Best face " << faceNumber << " in cache has nside " << nside;

    return getFaceFromCache(faceNumber, nside);
}


/* AUXILIAR FUNCTIONS */

/* return the number of tiles (tiles of nside64) necessary for display the face with nside */
int FaceCache::calculateFaceTiles(int nside)
{
    return pow(nside/Min_Nside, 2);
}


void FaceCache::printCache()
{
    QList<int> nsides = faceCache.keys();
    qDebug("=============== Face Cache ===============");
    //for(int i=0; i<faceCache.size(); i++)
    foreach(int nside, nsides)
    {
        cout << "Nside " << nside << ": ";
        FaceCacheEntry* entry = faceCache[nside];
        QList<int> faces = entry->keys();
        foreach(int face, faces)
        {
            cout << face << " ";
        }
        cout << endl;
    }
    qDebug("==========================================");
}


void FaceCache::generateBaseFaces()
{
    for(int i=0; i<12; i++)
        loadFace(i, Min_Nside);
}
