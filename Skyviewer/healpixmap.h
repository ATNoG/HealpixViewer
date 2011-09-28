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
#include "histogram.h"
#include <QGLViewer/qglviewer.h>
#include <chealpix.h>
#include <fitsio.h>
#include <healpixutil.h>
#include <QGLViewer/vec.h>
#include <math.h>
#include "configs.h"

using namespace std;
using namespace qglviewer;


/* forward declaration of PixelMap */
class FieldMap;


class HealpixMap
{
public:
    /* load a fits file into a skymap */
    HealpixMap(QString path, int minNside);
    ~HealpixMap();

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
    float* getPolarizationVectors(int faceNumber, int nside, long &totalVectors);
    float* getFullMap(int nside);

    int getMaxNside();

    void changeCurrentMap(MapType type);
    MapType getCurrentMapField();

    void getMinMax(float &min, float &max);

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

    QMap<int, float> meanPolMagnitudes;
    QMap<int, float> maxPolMagnitudes;
    QMap<int, float> devPolMagnitudes;

    Ordering parseOrdering(char *value);
    Coordsys parseCoordsys(char* value);

    void processFile(QString path, bool generateMaps);
    void readFITSPrimaryHeader(fitsfile *fptr);
    void readFITSExtensionHeader(fitsfile *fptr);
    void writeFITS(char* filename, char* tabname, float* temperature, int newnside);
    void writeFITSPrimaryHeader(fitsfile *fptr);
    void writeFITSExtensionHeader(fitsfile *fptr, int newnside);

    float* calculatePolarizationVector(double theta, double phi, double angle, double mag, double pixsize, double minMag, double maxMag);
    Vec spinVector(const Vec &v0, const Vec &vin, double psi);

    void writeMapInfo();
    void readMapInfo();
    bool checkMapCache();


    float* readMapCache(int nside, MapType mapType, int firstPosition=0, int length=0);

    float min, max;

    int minNSide;

    QString getMapType(MapType type);

    QProgressDialog* loadingDialog;
};


inline unsigned int HealpixMap::NSide2Res (unsigned int ns)
{
    return (unsigned int) (0.4 + (log(double(ns)) / log(2.0)));
}


inline HealpixMap::MapType getMapField(int i)
{
    switch(i)
    {
        case HealpixMap::I:
            return HealpixMap::I;
        case HealpixMap::Q:
            return HealpixMap::Q;
        case HealpixMap::U:
            return HealpixMap::U;
        case HealpixMap::NObs:
            return HealpixMap::NObs;
    }
}

Q_DECLARE_METATYPE(HealpixMap::MapType);

#endif // SKYMAP_H
