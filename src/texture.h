#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtOpenGL>
#include <QDebug>
#include "colormap.h"
#include <chealpix.h>

using namespace std;

QColor NULLVAL_COLOR(128, 128, 128);

class Texture
{
public:
    Texture(int faceNumber, int nside);
    Texture(int faceNumber, int nside, ColorMap* colorMap);
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

    bool created;

    void create();
};

#endif // TEXTURE_H
