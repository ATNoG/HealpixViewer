#include "fieldmap.h"
#include "exceptions.h"
#include <QDebug>


FieldMap::FieldMap(float* map, int nside, bool nest)
{
    this->map = map;
    this->currentNside = nside;
    this->nest = nest;

    if(!nest)
        convertToNest();

    lut_cache = PixelLUTCache::getInstance();

    // orderMap();
}


FieldMap::~FieldMap()
{
    #if DEBUG > 0
        qDebug() << "Calling FieldMap destructor";
    #endif
}

void FieldMap::convertToNest()
{
    qDebug("Converting to nest");

    long totalPixels = currentNside*currentNside*12;
    float* nestedMap;

    try
    {
        nestedMap = new float[totalPixels];
    }
    catch(const std::bad_alloc &)
    {
        throw FieldMapException("Not enough memory");
    }

    long npixel;

    for(long pixel=0; pixel<totalPixels; pixel++)
    {
        ring2nest(currentNside, pixel, &npixel);
        nestedMap[npixel] = map[pixel];
    }

    /* release memory used for ring map */
    delete[] map;

    /* map points now to a nested map */
    map = nestedMap;
    nest = true;
}


float* FieldMap::downgradeMap(int newNside)
{
    //qDebug() << "Downgrading map to " << newNside;

    int oldNside = currentNside;

    /* dont downgrade */
    if(newNside>=currentNside)
    {
        qDebug("Not downgraded!!!");
        // TODO: shouldnt return an exception ?
        return map;
    }

    long newNpixels = nside2npix(newNside);
    long nPixels = nside2npix(oldNside);

    //qDebug() << "Old nside is " << oldNside;

    float *newMap;
    int *count;

    try
    {
        newMap = new float[newNpixels];
        count = new int[newNpixels];
    }
    catch(const std::bad_alloc &)
    {
        delete[] map;
        delete[] newMap;
        delete[] count;
        throw FieldMapException("Not enough memory");
    }

    /* initialize count to 0 */
    for(long i=0; i<newNpixels; i++)
        count[i] = 0;

    for(long i=0; i<newNpixels; i++)
        newMap[i] = 0;

    for(long i=0; i<nPixels; i++)
    {
        long newPos = degradePixel(i, oldNside, newNside);
        newMap[newPos] += map[i];

        count[newPos]++;
    }

    for(long i=0; i<newNpixels; i++)
    {
        newMap[i] = newMap[i]/count[i];
    }

    delete[] count;

    return newMap;
}


int FieldMap::degradePixel(int pixel, int oldNside, int newNside)
{
    long npixel;

    // TODO: get map ordering

    /* convert nest to nest needed ? */
    if(!nest)
        ring2nest(oldNside, pixel, &npixel);
    else
        npixel = pixel;

    int newpos;
    double scaleDown = double(newNside)/double(oldNside);
    newpos = int(npixel * scaleDown * scaleDown);

    return newpos;
}


float* FieldMap::orderMap(float *values, int nside)
{
    /*
    buildLut();

    if(nest)
        lut = &lut_cache_nest[currentNside];
    else
        lut = &lut_cache_ring[currentNside];

    long npixels = currentNside*currentNside*12;

    orderedMap = new float[npixels];
    long texk;

    for(int pix = 0; pix < npixels; pix++)
    {
        texk = (*lut)[pix];
        orderedMap[texk] = map[pix];
    }

    QString outputFilename = "pixels.txt";
    QFile outputFile(outputFilename);
    outputFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&outputFile);

    for(int pix = 0; pix < npixels; pix++)
    {
        outStream << orderedMap[pix] << "\n";
    }
    outputFile.close();
    */

    PixelLUT *lut = lut_cache->getLut(nside);

    float *orderedValues;
    long npixels = nside2npix(nside);

    try
    {
        orderedValues = new float[npixels];
    }
    catch(const std::bad_alloc &)
    {
        delete[] map;
        throw FieldMapException("Not enough memory");
    }

    long texturepos;


    for(int pix = 0; pix < npixels; pix++)
    {
        texturepos = lut->at(pix);
        orderedValues[texturepos] = values[pix];
    }

    return orderedValues;
}


void FieldMap::generateDowngrades(QString path, QString prefix, int minNside)
{
    int nside = currentNside;
    float *values, *orderedValues;

    //qDebug() << "Current nside is " << nside;

    /* for each nside create a file */
    while(nside>=minNside)
    {
        /* verify if need to downgrade the map */
        if(nside<currentNside)
        {
            values = downgradeMap(nside);

            /* delete previous map */
            delete[] map;
            currentNside = nside;
            /* work with the downgraded map, so new downgrades are faster */
            map = values;

            orderedValues = orderMap(values, nside);
        }
        else
        {
            orderedValues = orderMap(map, nside);
        }

        //qDebug() << "Generating downgrade for " << nside;

        QString nsideStr;
        nsideStr.setNum(nside);
        QString filepath = path + "/" + prefix + "_" + nsideStr;
        saveFieldMap(filepath, orderedValues, nside2npix(nside));

        /*
        if(nside==512 && prefix=="I")
            for(int i=0; i<20; i++)
                    qDebug() << orderedValues[i];*/

        delete[] orderedValues;

        /* calculate next nside */
        nside = nside/2;
    }

    delete[] map;
}

void FieldMap::saveFieldMap(QString filepath, float* values, int nvalues)
{
    //qDebug() << "Writing values to file " << filepath;
    QFile file(filepath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write((const char*)values, nvalues*sizeof(float));
        file.close();
    }
    else
    {
        throw FieldMapException("Can't write map into cache");
    }
}
