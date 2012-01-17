#ifndef PIXELMAP_H
#define PIXELMAP_H

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include "healpixutil.h"

using namespace std;

typedef vector<long> PixelLUT;
typedef map<int, PixLUT* > PixelLUTCache;

class FieldMap
{

public:
    FieldMap(float* map, int nside, bool nest);
    ~FieldMap();
    float* getFaceValues(int faceNumber);
    float* downgradeMap(int newNside);
    void generateDowngrades(QString path, QString prefix, int minNside);

private:
    float* map;
    int currentNside;
    bool nest;

    void convertToNest();
    float* orderMap(float* values, int nside);

    PixelLUT* getLut(int nside);

    PixelLUTCache lut_cache;

    int degradePixel(int pixel, int oldNside, int newNside);
    void saveFieldMap(QString filepath, float* values, int nvalues);
};

#endif // PIXELMAP_H
