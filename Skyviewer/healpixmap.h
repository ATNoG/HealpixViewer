#ifndef SKYMAP_H
#define SKYMAP_H

#include <QString>
#include <vector>
#include <map>
#include <QColor>
#include "colortable.h"
#include <QGLViewer/qglviewer.h>
#include <chealpix.h>
#include <fitsio.h>

using namespace std;


typedef std::vector<long> PixLUT;
typedef std::map<int, PixLUT > PixLUTCache;

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

    float* getTemperature();
    long getNumberPixels();
    unsigned char* getTexture();
    void drawMap();

private:
    long nside;
    long npixels;
    Ordering ordering;
    Coordsys coordsys;

    Ordering readOrdering(char *value);
    Coordsys readCoordsys(char* value);



    float *temperature;
    unsigned char *texture;
    int texture_res;
    PixLUT *lut;
    PixLUTCache lut_cache_ring, lut_cache_nest;

    void buildTexture();
    bool buildLut();
    long xy2pix(long ix, long iy);
};



#endif // SKYMAP_H
