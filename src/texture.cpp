#include "texture.h"

Texture::Texture(int faceNumber, int nside)
{
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->colorMap = ColorMapManager::instance()->getDefaultColorMap();
    this->texture = NULL;

    created = false;
}


Texture::Texture(int faceNumber, int nside, ColorMap* colorMap)
{
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->colorMap = colorMap;
    this->texture = NULL;

    created = false;
}


Texture::~Texture()
{
    #if DEBUG > 0
        qDebug() << "Calling Texture destructor (" << faceNumber << "," << nside << ")";
    #endif

    if(texture!=NULL)
    {
        //qDebug() << "Texture deleted in destructor():  (" << faceNumber << "," << nside << ")" << texture;
        delete[] texture;
    }
}


void Texture::buildTexture(float* data, float minv, float maxv)
{
    float v = 0.0;
    long texk = 0;
    QColor color;
    texk = 0;

    texture = new unsigned char[nside*nside*3];

    int npixels = nside*nside;

    for(uint pix = 0; pix < npixels; pix++)
    {
        v = data[pix];

        if(v==HEALPIX_NULLVAL)
        {
            color = NULLPIX_COLOR;
        }
        else
        {
            if (v < minv) v = minv;
            if (v > maxv) v = maxv;
            v = (v-minv)/(maxv-minv);
            color = (*colorMap)(v);
        }

        texk = pix*3;

        texture[texk++] = color.red();
        texture[texk++] = color.green();
        texture[texk++] = color.blue();
        //texture[texk++] = 255;
    }

    delete[] data;
}


void Texture::create()
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, nside, nside, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    glBindTexture(GL_TEXTURE_2D, NULL);

    created = true;

    if(texture!=NULL)
    {
        //qDebug() << "Deleting texture in create(): (" << faceNumber << "," << nside << ")" << texture;
        delete[] texture;
        texture = NULL;
        //qDebug() << "Texture deleted in create(): (" << faceNumber << "," << nside << ")" << texture;
    }
    else
        qDebug("ERROR in TExture: texture is NULL");
}


void Texture::draw()
{
    if(!created)
        create();

    glBindTexture(GL_TEXTURE_2D, textureId);
}


void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, NULL);
}
