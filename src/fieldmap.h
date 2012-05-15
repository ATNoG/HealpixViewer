#ifndef PIXELMAP_H
#define PIXELMAP_H

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include "pixellutcache.h"
#include <healpix_map.h>

using namespace std;


class FieldMap
{

public:
    FieldMap(float* map, int nside, bool nest);
    ~FieldMap();
    float* getFaceValues(int faceNumber);
    float* downgradeMap(int newNside);
    void generateDowngrades(QString path, QString prefix, int minNside);
    static int degradePixel(int pixel, int oldNside, int newNside);

private:
    float* map;
    int currentNside;
    bool nest;

    void convertToNest();
    float* orderMap(float* values, int nside);

    PixelLUTCache* lut_cache;
    void saveFieldMap(QString filepath, float* values, int nvalues);
};

#endif // PIXELMAP_H
