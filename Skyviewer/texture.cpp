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


void Texture::create()
{
    QString name;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    qDebug() << "Face " << faceNumber << " with texture " << textureId;

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
    glBindTexture(GL_TEXTURE_2D, NULL);

    created = true;
}


void Texture::draw()
{
    if(!created)
        create();

    glBindTexture(GL_TEXTURE_2D, textureId);
}
