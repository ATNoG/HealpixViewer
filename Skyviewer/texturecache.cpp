#include "texturecache.h"

TextureCache::TextureCache(HealpixMap* map, int minNside, int maxNside, int maxTiles)
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

/* get the face with faceNumber and for the given nside */
Texture* TextureCache::getFace(int faceNumber, int nside)
{
    Texture* face;

    /* verify the maximum nside available */
    // TODO: throw exception ?
    if(nside>MAX_NSIDE)
        nside = MAX_NSIDE;

    bool locked = true;
    int faceId = (faceNumber+1)*10000 + nside;

    /* request access to cache */
    cacheAccess.lock();

    /* nside MIN_NSIDE face are not inserted into lru list to never get deleted */
    if(nside>MIN_NSIDE)
    {
        /* update lru info */
        cacheControl.removeAll(faceId);
        cacheControl.push_back(faceId);
    }

    //qDebug() << "TextureCache: Getting face " << faceNumber << "(" << nside << ")";

    /* check if available in cache */
    if(!checkFaceAvailable(faceNumber, nside))
    {
        /* initial nside, so wait for texture to load */
        if(nside==MIN_NSIDE)
        //if(true)
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
                //REMOVE qDebug() << "Texture " << faceNumber << " (" << nside << ") -> Miss";

                requestedFaces.insert(faceId);

                /* release accesss to cache */
                cacheAccess.unlock();
                locked = false;

                /* create the face (new thread) */
                QtConcurrent::run(this, &TextureCache::loadFace, faceNumber, nside);
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


/* PRIVATE METHODS */

/* calculate the new face */
Texture* TextureCache::loadFace(int faceNumber, int nside)
{
    //REMOVE qDebug()<< "TextureCache: loading face " << faceNumber << "(" << nside << ")";

    Texture* texture = new Texture(faceNumber, nside);
    if(texture==NULL)
        qDebug("Texture is null");

    texture->buildTexture(healpixMap->getFaceValues(faceNumber, nside));

    /* store the face into the cache */
    bool clean = storeFace(faceNumber, nside, texture);

    qDebug() << "Texture " << faceNumber << "(" << nside << ") - Loaded";

    /* remove the request */
    int faceId = (faceNumber+1)*10000 + nside;
    requestedFaces.remove(faceId);

    if(nside>MIN_NSIDE)
        emit(newFaceAvailable(clean));
        //emit(newFaceAvailable(face));

    //qDebug("Returning texture");

    return texture;
}

bool TextureCache::cleanCache(int minSpace)
{
    // TODO: discard the not used faces to get space for new faces - better algorithm

    qDebug("============Cleaning cache ==================");

    int lru, fn, ns;
    int nFacesInUse = 0;

    /* request access to cache */
    cacheAccess.lock();

    qDebug() << "TextureCache Margin tiles: " << marginTilesSpace;
    qDebug() << "TextureCache Available tiles: " << availableTiles;

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

void TextureCache::preloadFace(int faceNumber, int nside)
{
    //qDebug() << "Preload face " << faceNumber << "(" << nside << ") - waiting";

    // TODO: throw exception ?
    if(nside>MAX_NSIDE)
        nside = MAX_NSIDE;

    /* request access to cache */
    cacheAccess.lock();

    //qDebug() << "Preload face " << faceNumber << "(" << nside << ") - has access";

    if(!checkFaceAvailable(faceNumber, nside))
    {
        int faceId = (faceNumber+1)*10000 + nside;

        if(!requestedFaces.contains(faceId))
        {
            //REMOVE qDebug() << "Texture " << faceNumber << " (" << nside << ") -> Preloading";

            requestedFaces.insert(faceId);

            /* release access to cache */
            cacheAccess.unlock();

            //qDebug() << "Requested Face " << faceId;

            /* create the face (new thread) */
            QtConcurrent::run(this, &TextureCache::loadFace, faceNumber, nside);
        }
    }
    else
    {
        //qDebug("Nothing to preload");

        /* release access to cache */
        cacheAccess.unlock();
    }
}


void TextureCache::updateStatus(QVector<int> faces, int nside)
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
bool TextureCache::storeFace(int faceNumber, int nside, Texture* face)
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
        TextureCacheEntry* faceEntry;

        if(textureCache.contains(nside))
            faceEntry = textureCache[nside];
        else
            faceEntry = new TextureCacheEntry;

        faceEntry->insert(faceNumber, face);
        textureCache.insert(nside, faceEntry);

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
void TextureCache::discardFace(int faceNumber, int nside)
{
    // TODO: do this in an atomic operation */

    qDebug() << "Discarding texture " << faceNumber << " with nside " << nside;

    /* free the allocated memory for face */
    Texture* face = getFaceFromCache(faceNumber, nside);
    delete face;

    /* remove the entry */
    TextureCacheEntry* entry = textureCache.value(nside);
    entry->remove(faceNumber);

    /* update available tiles */
    availableTiles += calculateFaceTiles(nside);
}

/* check if face is available on cache */
bool TextureCache::checkFaceAvailable(int faceNumber, int nside)
{
    //qDebug() << "check if available: face " << faceNumber << " nside " << nside;
    if(textureCache.contains(nside))
    {
        TextureCacheEntry* entry = textureCache.value(nside);
        if(entry->contains(faceNumber))
        {
            return true;
        }
    }
    return false;
}

/* get face from cache */
Texture* TextureCache::getFaceFromCache(int faceNumber, int nside)
{
    TextureCacheEntry* entry = textureCache.value(nside);
    return entry->value(faceNumber);
}


Texture* TextureCache::getBestFaceFromCache(int faceNumber, int nside)
{
    bool faceAvailable;
    do
    {
        faceAvailable = checkFaceAvailable(faceNumber, nside);
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

    return getFaceFromCache(faceNumber, nside);
}


/* AUXILIAR FUNCTIONS */

/* return the number of tiles (tiles of nside64) necessary for display the face with nside */
int TextureCache::calculateFaceTiles(int nside)
{
    return pow(nside/MIN_NSIDE, 2);
}
