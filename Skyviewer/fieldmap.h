#ifndef PIXELMAP_H
#define PIXELMAP_H

#include <QDebug>
#include "healpixutil.h"

class FieldMap
{

public:
    FieldMap();

    void drawMap();
    float* downgradeMap(float* map, int oldNside, int newNside);
    //unsigned char* getTexture();

private:
    float* map;
    float* newMap;

    //bool buildLut();
    //void buildTexture();

    //PixLUT *lut;                                    /* pointer to lut */
    //PixLUTCache lut_cache_ring, lut_cache_nest;

    //unsigned char *texture;
    //int texture_res;

    int degradePixel(int pixel, int oldNside, int newNside);

};

#endif // PIXELMAP_H
