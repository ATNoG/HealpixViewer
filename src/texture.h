#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtOpenGL>
#include <QDebug>
#include "colormap.h"
#include <chealpix.h>
#include <healpix_map.h>
#include "configs.h"
#include "types.h"
#include "unique_ptr.hpp"

using namespace std;

class Texture
{
public:
    Texture(int faceNumber, int nside);
    Texture(int faceNumber, int nside, ColorMap* colorMap, QColor sentinelColor, ScaleType scale, float factor, float offset, bool isOverlay);
    ~Texture();

    void draw();
    void buildTexture(hv::unique_ptr<float[0]> data, float min, float max);
    void unbind();

private:
    int faceNumber;
    int nside;

    unsigned char* texture;
    GLuint textureId;
    ColorMap *colorMap;
    QColor sentinelColor;
    ScaleType scale;
    float factor, offset;

    bool created;
    bool isOverlay;

    void create();
};

#endif // TEXTURE_H
