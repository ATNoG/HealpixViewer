#include "texture.h"

Texture::Texture(int faceNumber, int nside)
{
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->colorMap = ColorMapManager::instance()->getDefaultColorMap();
    this->sentinelColor = QColor(DEFAULT_SENTINEL_COLOR);
    this->texture = NULL;
    this->isOverlay = false;

    created = false;
}


Texture::Texture(int faceNumber, int nside, ColorMap* colorMap, QColor sentinelColor, ScaleType scale, float factor, float offset, bool isOverlay)
{
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->colorMap = colorMap;
    this->sentinelColor = sentinelColor;
    this->scale = scale;
    this->factor = factor;
    this->offset = offset;
    this->texture = NULL;
    this->isOverlay = isOverlay;

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


void Texture::buildTexture(hv::unique_ptr<float[0]> data, float minv, float maxv)
{
    float v = 0.0;
    long texk = 0;
    QColor color;

    texture = new unsigned char[nside*nside*3];
    int npixels = nside*nside;

    double log_min, log_max;
    log_min = minv;
    log_max = maxv;


    /* apply thresholds */
    for(uint pix = 0; pix < npixels; pix++)
    {
        if(!approx<double>(data[pix],Healpix_undef))
        {

            /* apply thresholds */
            if (data[pix] < minv) data[pix] = minv;
            if (data[pix] > maxv) data[pix] = maxv;

            if(scale == LOGARITHMIC)
            {
                data[pix] = log10(max(data[pix], (float)1.e-6*abs(maxv)));
                /* *1 */
                //if(data[pix]<log_min)
                //    log_min = data[pix];
            }
        }
    }

    if(scale == LOGARITHMIC)
    {
        /*
        if(minv>0)
            log_min = log10(minv);
            */

        if(maxv>0)
            log_max = log10(maxv);

        //qDebug() << "(face " << faceNumber << " " << nside << ") use log_min: " << log_min;
    }

    //qDebug() << "Texture, using thresholds: " << log_min << "," << log_max;


    for(uint pix = 0; pix < npixels; pix++)
    {
        v = data[pix];

        if(approx<double>(v,Healpix_undef))
        {
            color = sentinelColor;
        }
        else
        {
            if(scale == LOGARITHMIC)
            {
                v = (v-log_min)/(log_max-log_min);
            }
            else if(scale == ASINH)
            {
                double asinh_min, asinh_max;
                asinh_min = asinh(minv);
                asinh_max = asinh(maxv);
                v = (asinh(v)-asinh_min)/(asinh_max-asinh_min);
            }
            else
            {
                v = (v-minv)/(maxv-minv);
            }

            color = (*colorMap)[v];
        }

        texk = pix*3;

        texture[texk++] = color.red();
        texture[texk++] = color.green();
        texture[texk++] = color.blue();
    }
}


void Texture::create()
{
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, nside, nside, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);

    if(isOverlay)
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
    else
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_REPLACE);

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

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureId);
}


void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, NULL);
    //glActiveTexture(GL_TEXTURE0);
    //glClientActiveTexture(GL_TEXTURE0);
}
