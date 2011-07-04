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

#define MINZOOM 32
#define MAXZOOM 2048

using namespace std;


/* forward declaration of PixelMap */
class FieldMap;


class HealpixMap
{
public:
    /* load a fits file into a skymap */
    HealpixMap(QString path);

    enum Ordering {NESTED, RING};
    enum Coordsys {CELESTIAL, ECLIPTIC, GALACTIC};
    enum MapType  {I, Q, U, NObs};

    bool isOrderRing();
    bool isOrderNested();
    bool isCoordsysCelestial();
    bool isCoordsysEcliptic();
    bool isCoordsysGalactic();

    bool hasTemperature();
    bool hasPolarization();
    bool hasNObs();

    void changeCurrentMap(MapType mapType);
    FieldMap* getMap(MapType mapType);
    QList<MapType> getAvailableMaps();
    long getNumberPixels();

    bool zoomIn();
    bool zoomOut();

    void drawMap();

    static unsigned int NSide2Res  (unsigned int ns);

private:
    QString path;
    long maxNside, currentNside;
    long npixels;
    Ordering ordering;
    Coordsys coordsys;

    QList<MapType> availableMaps;
    MapType currentMap;

    Ordering parseOrdering(char *value);
    Coordsys parseCoordsys(char* value);

    void processFile(QString path);
    void readFITSPrimaryHeader(fitsfile *fptr);
    void readFITSExtensionHeader(fitsfile *fptr);
    void writeFITS(char* filename, char* tabname, float* temperature, int newnside);
    void writeFITSPrimaryHeader(fitsfile *fptr);
    void writeFITSExtensionHeader(fitsfile *fptr, int newnside);

    float *temperature;
};


inline unsigned int HealpixMap::NSide2Res (unsigned int ns)
{
        return (unsigned int) (0.4 + (log(double(ns)) / log(2.0)));
}


#endif // SKYMAP_H
