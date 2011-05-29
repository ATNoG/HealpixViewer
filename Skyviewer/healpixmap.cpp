#include "healpixmap.h"

#include <QDebug>
#include <chealpix.h>

HealpixMap::HealpixMap(QString path)
{
    // TODO: verify if file exists, etc..

    /* open fits file */
    float* map;
    char _coordsys[10];
    char _ordering[10];
    QByteArray ba = path.toLocal8Bit();

    /* read healpix map */
    map = read_healpix_map(ba.data(), &nside, _coordsys, _ordering);

    /* initialize variables */
    ordering = readOrdering(_ordering);
    coordsys = readCoordsys(_coordsys);
    npixels  = nside2npix(nside);

    qDebug() << "Healpixmap readed";
    qDebug() << " Nside = " << nside;
    qDebug() << " Npixels = " << npixels;
    qDebug() << " Ordering = " << _ordering << "(" << ordering << ")";
    qDebug() << " Coordsys = " << _coordsys << "(" << coordsys << ")";
}


HealpixMap::Ordering HealpixMap::readOrdering(char *value)
{
    if(!strcmp(value, "RING"))
        return RING;
    else if(!strcmp(value, "NESTED"))
        return NESTED;

    // TODO: throw exception if invalid order
    else
        return RING;
}


HealpixMap::Coordsys HealpixMap::readCoordsys(char* value)
{
    if(!strcmp(value, "C"))
        return CELESTIAL;
    else if(!strcmp(value, "E"))
        return ECLIPTIC;
    else if(!strcmp(value, "G"))
        return GALACTIC;

    // TODO: throw exception if a invalid coordsys received
    else
        return GALACTIC;
}


bool HealpixMap::isOrderRing()
{
    return ordering==RING;
}

bool HealpixMap::isOrderNested()
{
    return ordering==NESTED;
}

bool HealpixMap::isCoordsysCelestial()
{
    return coordsys==CELESTIAL;
}

bool HealpixMap::isCoordsysEcliptic()
{
    return coordsys==ECLIPTIC;
}

bool HealpixMap::isCoordsysGalactic()
{
    return coordsys==GALACTIC;
}
