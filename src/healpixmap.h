#ifndef SKYMAP_H
#define SKYMAP_H

#include <QString>
#include <vector>
#include <map>
#include <QMutex>
#include <QColor>
#include <QFileInfo>
#include <QDir>
#include <QTextOStream>
#include <QProgressDialog>
#include "histogram.h"
#include <QGLViewer/qglviewer.h>
#include <chealpix.h>
#include <fitsio.h>
#include "healpixutil.h"
#include <error_handling.h>
#include <cxxutils.h>
#include <healpix_base.h>
#include <QGLViewer/vec.h>
#include <math.h>
#include <set>
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

    typedef QString MapType;

    bool isOrderRing();
    bool isOrderNested();
    bool isCoordsysCelestial();
    bool isCoordsysEcliptic();
    bool isCoordsysGalactic();
    bool hasNObs();
    bool hasPolarization();

    bool generateDowngrades();

    QList<MapType> getAvailableMaps();
    long getNumberPixels();

    hv::unique_ptr<float[0]> getFaceValues(int faceNumber, int nside);
    hv::unique_ptr<float[0]> getPolarizationVectors(int faceNumber, int nside, double minMag, double maxMag, double magnification, int spacing, long &totalVectors);
    hv::unique_ptr<float[0]> getFullMap(int nside);

    int getMaxNside();

    void changeCurrentMap(MapType type);
    MapType getCurrentMapField();
    Coordsys getCoordSystem();

    void getMinMax(float &min, float &max);
    void getMagMinMax(double &min, double &max);

    static unsigned int NSide2Res  (unsigned int ns);
    static QString coordsysToString(Coordsys coord);

    void angle2pix(double theta, double phi, int nside, long &pix);

    std::set<int> query_disc(int pixel1, int pixel2, int nside);
    std::set<int> query_triangle(int pixel1, int pixel2, int pixel3, int nside);
    std::set<int> query_polygon(std::vector<int> points, int nside);

private:
    QString path;
    QString filename;
    QString cachePath;
    long maxNside;
    long npixels;
    Ordering ordering;
    Coordsys coordsys;

    bool createCache;
    bool cacheCreated;

    bool QFieldExist, UFieldExist, NObsFieldExist;
    MapType QField, UField, NObsField;

    QList<MapType> availableMaps;
    MapType currentMapType;

    QMap<int, float> meanPolMagnitudes;
    QMap<int, float> maxPolMagnitudes;
    QMap<int, float> devPolMagnitudes;

    Ordering parseOrdering(char *value);
    Coordsys parseCoordsys(char* value);

    void checkFields();
    void processFile(QString path, bool generateMaps);
    void readFITSPrimaryHeader(fitsfile *fptr);
    void readFITSExtensionHeader(fitsfile *fptr);
    void writeFITS(char* filename, char* tabname, float* temperature, int newnside);
    void writeFITSPrimaryHeader(fitsfile *fptr);
    void writeFITSExtensionHeader(fitsfile *fptr, int newnside);

    hv::unique_ptr<float[0]> calculatePolarizationVector(double theta, double phi, double angle, double mag, double pixsize, double minMag, double maxMag, double magnification);
    Vec spinVector(const Vec &v0, const Vec &vin, double psi);

    void writeMapInfo();
    void readMapInfo();
    void removeCache();
    void abort();

    QMutex cacheAccess;

    hv::unique_ptr<float[0]> readMapCache(int nside, MapType mapType, int firstPosition=0, int length=0);

    float min, max;

    int minNSide;

    QString getMapType(MapType type);

    hv::unique_ptr<QProgressDialog> loadingDialog;
};


inline unsigned int HealpixMap::NSide2Res (unsigned int ns)
{
    return (unsigned int) (0.4 + (log(double(ns)) / log(2.0)));
}

inline HealpixMap::Coordsys getCoordSys(int i)
{
    switch(i)
    {
        case HealpixMap::GALACTIC:
            return HealpixMap::GALACTIC;
        case HealpixMap::ECLIPTIC:
            return HealpixMap::ECLIPTIC;
        case HealpixMap::CELESTIAL:
            return HealpixMap::CELESTIAL;
    }
}

Q_DECLARE_METATYPE(HealpixMap::MapType);

#endif // SKYMAP_H
