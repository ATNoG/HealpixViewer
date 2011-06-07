#ifndef SKYMAP_H
#define SKYMAP_H

#include <QString>
#include <chealpix.h>
#include <fitsio.h>

using namespace std;


class HealpixMap
{
public:
    /* load a fits file into a skymap */
    HealpixMap(QString path);

    enum Ordering {NESTED, RING};
    enum Coordsys {CELESTIAL, ECLIPTIC, GALACTIC};

    bool isOrderRing();
    bool isOrderNested();
    bool isCoordsysCelestial();
    bool isCoordsysEcliptic();
    bool isCoordsysGalactic();

private:
    long nside;
    long npixels;
    Ordering ordering;
    Coordsys coordsys;

    Ordering readOrdering(char *value);
    Coordsys readCoordsys(char* value);
};

#endif // SKYMAP_H
