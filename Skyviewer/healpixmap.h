#ifndef SKYMAP_H
#define SKYMAP_H

#include <QString>
#include <vector>
#include <map>
#include <QColor>
#include <QFileInfo>
#include <QDir>
#include <QTextOStream>
#include <QProgressDialog>
#include "colortable.h"
#include <QGLViewer/qglviewer.h>
#include <chealpix.h>
#include <fitsio.h>
#include <healpixutil.h>

#define MINZOOM 32
#define MAXZOOM 2048

#define CACHE_DIR "cache"

using namespace std;


/* forward declaration of PixelMap */
class FieldMap;

/*
typedef enum MapType
{
    I = 0,
    Q = 1,
    U = 2,
    NObs = 3
} MapType;
*/


class HealpixMap
{
public:
    /* load a fits file into a skymap */
    HealpixMap(QString path);

    enum Ordering {NESTED, RING};
    enum Coordsys {CELESTIAL, ECLIPTIC, GALACTIC};
    enum MapType  {I, Q, U, P, NObs};

    bool isOrderRing();
    bool isOrderNested();
    bool isCoordsysCelestial();
    bool isCoordsysEcliptic();
    bool isCoordsysGalactic();

    bool hasTemperature();
    bool hasPolarization();
    bool hasNObs();

    bool generateDowngrades();

    QList<MapType> getAvailableMaps();
    long getNumberPixels();

    float* getFaceValues(int faceNumber, int nside);
    float* getPolarizationVectors(int faceNumber, int nside);

    int getMaxNside();

    void changeCurrentMap(MapType type);

    static unsigned int NSide2Res  (unsigned int ns);
    static QString mapTypeToString(MapType type);

private:
    QString path;
    QString filename;
    QString cachePath;
    QString cacheInfo;
    long maxNside;
    long npixels;
    Ordering ordering;
    Coordsys coordsys;

    QList<MapType> availableMaps;
    MapType currentMapType;

    Ordering parseOrdering(char *value);
    Coordsys parseCoordsys(char* value);

    void processFile(QString path, bool generateMaps);
    void readFITSPrimaryHeader(fitsfile *fptr);
    void readFITSExtensionHeader(fitsfile *fptr);
    void writeFITS(char* filename, char* tabname, float* temperature, int newnside);
    void writeFITSPrimaryHeader(fitsfile *fptr);
    void writeFITSExtensionHeader(fitsfile *fptr, int newnside);

    void writeMapInfo();
    void readMapInfo();
    bool checkMapCache();

    float* readMapCache(int nside, MapType mapType, int firstPosition=0, int length=0);

    //float *temperature;
    //FieldMap* temperatureMap;

    QString getMapType(MapType type);

    QProgressDialog* loadingDialog;
};


inline unsigned int HealpixMap::NSide2Res (unsigned int ns)
{
    return (unsigned int) (0.4 + (log(double(ns)) / log(2.0)));
}


#endif // SKYMAP_H
