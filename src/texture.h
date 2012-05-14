#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtOpenGL>
#include <QDebug>
#include "colormap.h"
#include <chealpix.h>
#include <healpix_map.h>
#include "configs.h"

using namespace std;

class Texture
{
public:
    Texture(int faceNumber, int nside);
    Texture(int faceNumber, int nside, ColorMap* colorMap, QColor sentinelColor);
    ~Texture();

    void draw();
    void buildTexture(float* data, float min, float max);
    void unbind();

private:
    int faceNumber;
    int nside;

    unsigned char* texture;
    GLuint textureId;
    ColorMap *colorMap;
    QColor sentinelColor;

    bool created;

    void create();
};

#endif // TEXTURE_H
