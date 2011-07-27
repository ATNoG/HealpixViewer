#ifndef TEXTURE_H
#define TEXTURE_H

#include <QtOpenGL>
#include <QDebug>

using namespace std;

class Texture
{
public:
    Texture(int faceNumber, int nside);
    ~Texture();

    void draw();

private:
    int faceNumber;
    int nside;

    float* texture;
    GLuint textureId;

    bool created;

    void create();
};

#endif // TEXTURE_H
