#include "texturecache.h"

TextureCache::TextureCache(HealpixMap* map, int minNside, int maxNside, int maxTiles)
{
    this->Min_Nside = minNside;
    this->Max_Nside = maxNside;
    this->maxTiles = maxTiles;
    this->availableTiles = maxTiles;

    marginTilesSpace = CACHE_MARGIN;

    //supportedNsides << 64 << 128 << 256 << 512 << 1024 << 2048;

    /* open healpix map */
    healpixMap = map;

    healpixMap->getMinMax(minTex, maxTex);

    generateBaseTextures();
}


TextureCache::~TextureCache()
{
    qDebug() << "Calling TextureCache destructor";

    QList<int> nsides = textureCache.keys();
    foreach(int nside, nsides)
    {
        TextureCacheEntry* entry = textureCache.value(nside);
        QList<int> faces = entry->keys();
        foreach(int faceNumber, faces)
        {
            Texture *texture = entry->value(faceNumber);
            delete texture;
        }
        delete entry;
    }
}


/* PUBLIC INTERFACE */

/* get the face with faceNumber and for the given nside */
Texture* TextureCache::getFace(int faceNumber, int nside)
{
    Texture* face;

    /* verify the maximum nside available */
    // TODO: throw exception ?
    if(nside>Max_Nside)
        nside = Max_Nside;

    bool locked = true;
    int faceId = (faceNumber+1)*10000 + nside;

    /* request access to cache */
    cacheAccess.lock();

    /* nside Min_Nside face are not inserted into lru list to never get deleted */
    if(nside>Min_Nside)
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
        if(nside==Min_Nside)
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

#if DEBUG > 0
    if(texture==NULL)
        qDebug("Texture is null");
#endif

    texture->buildTexture(healpixMap->getFaceValues(faceNumber, nside), minTex, maxTex);

    /* store the face into the cache */
    bool clean = storeFace(faceNumber, nside, texture);

#if DEBUG > 0
    qDebug() << "Texture " << faceNumber << "(" << nside << ") - Loaded";
#endif

    /* remove the request */
    int faceId = (faceNumber+1)*10000 + nside;
    requestedFaces.remove(faceId);

    if(nside>Min_Nside)
        emit(newFaceAvailable(clean));
        //emit(newFaceAvailable(face));

    //qDebug("Returning texture");

    return texture;
}

bool TextureCache::cleanCache(int minSpace)
{
    // TODO: discard the not used faces to get space for new faces - better algorithm

#if DEBUG > 0
    qDebug("============Cleaning cache ==================");
#endif

    int lru, fn, ns;
    int nFacesInUse = 0;

    /* request access to cache */
    cacheAccess.lock();

#if DEBUG > 0
    qDebug() << "TextureCache Margin tiles: " << marginTilesSpace;
    qDebug() << "TextureCache Available tiles: " << availableTiles;
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

    /* release cache access */
    cacheAccess.unlock();

    return true;
}

void TextureCache::preloadFace(int faceNumber, int nside)
{
    //qDebug() << "Preload face " << faceNumber << "(" << nside << ") - waiting";

    // TODO: throw exception ?
    if(nside>Max_Nside)
        nside = Max_Nside;

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
        if(checkFaceAvailable(faceNumber, nside))
            discardFace(faceNumber, nside);

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

        //qDebug() << "Texture (" << faceNumber << "," << nside << ") stored  - " << face;

        //return true;
    }
    else
    {
        qDebug("no space left on texture cache");
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

#if DEBUG > 0
    qDebug() << "Discarding texture " << faceNumber << " with nside " << nside;
#endif

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
            if(nside > Min_Nside)
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
    return pow(nside/Min_Nside, 2);
}




void TextureCache::updateTextureThreshold(float min, float max)
{
    if(minTex!=min || maxTex!=max)
    {
        minTex = min;
        maxTex = max;

        //qDebug("\n\n=============Regenerating texture for new threshold==================");

        invalidateCache();
    }
}


void TextureCache::changeMapField(HealpixMap::MapType field)
{
    if(healpixMap->getCurrentMapField()!=field)
    {
        healpixMap->changeCurrentMap(field);
        healpixMap->getMinMax(minTex, maxTex);
        invalidateCache();
    }
}


void TextureCache::invalidateCache()
{

    /* generate baseNside textures */
    for(int i=0; i<12; i++)
        loadFace(i, Min_Nside);

    /* invalidate current textures */
    QMap<int, QList<int> > facesToDelete;

    cacheAccess.lock();
    QList<int> nsides = textureCache.keys();
    for(int i=0; i<nsides.size(); i++)
    {
        if(nsides[i]>Min_Nside)
        {
            QList<int> faces = textureCache[nsides[i]]->keys();
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
            discardFace(faces[j], nsides[i]);
    }

    /* generate textures in use */
    QSetIterator<int> i(facesInUse);
    while(i.hasNext())
    {
        int faceId = i.next();
        int fn = (faceId/10000)-1;
        int ns = faceId-(fn+1)*10000;

        if(ns>Min_Nside)
        {
            //qDebug() << "Invalidate Cache - loading texture (" << fn << "," << ns << ")";
            /* add to requested faces */
            int faceId = (fn+1)*10000 + ns;

            cacheAccess.lock();
            requestedFaces.insert(faceId);
            cacheAccess.unlock();

            /* load texture in thread */
            QtConcurrent::run(this, &TextureCache::loadFace, fn, ns);
        }
    }
}

void TextureCache::getTextureMinMax(float &min, float &max)
{
    min = minTex;
    max = maxTex;
}

void TextureCache::generateBaseTextures()
{
    for(int i=0; i<12; i++)
        loadFace(i, Min_Nside);
}
