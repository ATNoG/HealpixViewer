#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtOpenGL>
#include <QDebug>
#include "colortable.h"

using namespace std;

class Texture
{
public:
    Texture(int faceNumber, int nside);
    ~Texture();

    void draw();
    void buildTexture(float* data, float min, float max);
    void unbind();

private:
    int faceNumber;
    int nside;

    unsigned char* texture;
    GLuint textureId;

    bool created;

    void create();
};

#endif // TEXTURE_H
