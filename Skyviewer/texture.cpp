#include "texture.h"

Texture::Texture(int faceNumber, int nside)
{
    this->faceNumber = faceNumber;
    this->nside = nside;

    created = false;
}


Texture::~Texture()
{

}


void Texture::buildTexture(float* data)
{

    //double minv=-0.000145, maxv=0.000145;
    //double minv=-1.0, maxv=3.0;
    double minv=0.0, maxv=2.0;
    float v = 0.0;
    long texk = 0;
    QColor color;
    texk = 0;

    ColorTable *ct = new ColorTable(2);

    texture = new unsigned char[nside*nside*3];

    int npixels = nside*nside;

    for(uint pix = 0; pix < npixels; pix++)
    {
        v = data[pix];

        if (v < minv) v = minv;
        if (v > maxv) v = maxv;
        v = (v-minv)/(maxv-minv);
        color = (*ct)(v);

        texk = pix*3;

        texture[texk++] = color.red();
        texture[texk++] = color.green();
        texture[texk++] = color.blue();
        //texture[texk++] = 255;
    }

/*
    for(int i=0; i<nside*nside; i++)
    {
        int pos = 3*i;
        color.setHsv((int)(360*((int)(i/nside))/nside), 255, 255);
        //color.setHsv((int)(360*(faceNumber)/12), 255, 255);
        //if(i%nside<10)
        //    color.setRgb(255, 0, 0);
        //else
        //    color.setRgb(0, 0, 255);

        texture[pos++] = color.red();
        texture[pos++] = color.green();
        texture[pos++] = color.blue();
    }
*/

}


void Texture::create()
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //qDebug() << "Face " << faceNumber << " with texture " << textureId;

    /*
    QString name;

    if(faceNumber<4)
        name = "/home/zeux/Pictures/Vietnam-Halong_Bay-New7Wonders_of_Nature-original.jpg";
    else if(faceNumber<8)
        name = "/home/zeux/Pictures/IMG_0068.JPG";
    else
        name = "/home/zeux/Pictures/IMG_0083.JPG";

    QImage img(name);
    QImage glImg = QGLWidget::convertToGLFormat(img);  // flipped 32bit RGBA

    // Bind the img texture...
    glTexImage2D(GL_TEXTURE_2D, 0, 4, glImg.width(), glImg.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, glImg.bits());
    */


    /*
    QColor color;
    texture = new unsigned char[nside*nside*3];
    for(int i=0; i<nside*nside; i++)
    {
        int pos = 3*i;
        //color.setHsv((int)(360*((int)(i/nside))/nside), 255, 255);
        if(i<2*nside)
            color.setRgb(255, 0, 0);
        else
            color.setRgb(0, 0, 255);
        texture[pos++] = color.red();
        texture[pos++] = color.green();
        texture[pos++] = color.blue();
    }
    */

    glTexImage2D(GL_TEXTURE_2D, 0, 3, nside, nside, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    glBindTexture(GL_TEXTURE_2D, NULL);

    created = true;
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
