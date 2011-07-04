#include "fieldmap.h"
#include <QGLViewer/qglviewer.h>
#include <QDebug>

FieldMap::FieldMap()
{
}


void FieldMap::drawMap()
{
//    glEnable(GL_TEXTURE_2D);

//    glTexImage2D(
//            GL_TEXTURE_2D, 0, 4,
//            texture_res, texture_res, 0,
//            GL_RGBA, GL_UNSIGNED_BYTE,
//            texture
//    );
}


//bool FieldMap::buildLut()
//{
//    //PixLUTCache &lut_cache = (ordering == HealpixMap::RING) ? lut_cache_ring : lut_cache_nest;
//    PixLUTCache &lut_cache = lut_cache_ring;
//    PixLUT &lut = lut_cache[nside];
//    lut.resize(12*nside*nside);
//    int  face = 0, x = 0, y = 0;
//    long dy = 4*nside;
//    long pix;
//    long k = 0;
//    for(face = 0; face < 12; face++)
//    {
//        long xo = nside*(face % 4);
//        long yo = nside*(face / 4);
//        long face_offset = face*nside*nside;
//        for(y = 0; y < nside; y++)
//        {
//            for(x = 0; x < nside; x++)
//            {
//                k = x + xo + (y+yo)*dy;
//                pix = xy2pix(x,y)  + face_offset;
//                //if (ordering == RING)
//                    //nest2ring(nside,pix,&pix);
//                lut[pix] = 4*k;
//            }
//        }
//    }
//    return true;
//}


//void FieldMap::buildTexture()
//{
//    /*
//    double minv=-0.000145, maxv=0.000145;
//    float v = 0.0;
//    long texk = 0;
//    QColor color, blank(255, 255, 255, 255);
//    texk = 0;

//    //ColorTable *ct = new ColorTable(2);

//    texture_res = 4*nside;
//    texture = new unsigned char[texture_res*texture_res*4];

//    buildLut();
//    if(ordering==RING)
//        lut = &lut_cache_ring[nside];
//    else
//        lut = &lut_cache_nest[nside];

//    int npixels = 12*4*4;
//    for(uint pix = 0; pix < npixels; pix++)
//    {
//        v = temperature[pix];

//        if (v < minv) v = minv;
//        if (v > maxv) v = maxv;
//        v = (v-minv)/(maxv-minv);
//        color = (*ct)(v);

//        texk = (*lut)[pix];
//        texture[texk++] = color.red();
//        texture[texk++] = color.green();
//        texture[texk++] = color.blue();
//        texture[texk++] = 255;

//        //if(pix<10)
//        //    qDebug() << "color " << pix << ": " << color.red() << ", " << color.green() << ", " << color.blue() << ", 255";
//    }
//*/

//}


//unsigned char* FieldMap::getTexture()
//{
//    return texture;
//}




float* FieldMap::downgradeMap(float* map, int oldNside, int newNside)
{
    /* dont downgrade */
    //if(newNside>=oldNside)
      //  return;

    long newNpixels = nside2npix(newNside);
    long nPixels = nside2npix(oldNside);

    //float *newMap = new float[newNpixels];
    newMap = new float[newNpixels];
    int *count = new int[newNpixels];

    /* initialize count to 0 */
    for(long i=0; i<newNpixels; i++)
        count[i] = 0;

    for(long i=0; i<nPixels; i++)
    {
        long newPos = degradePixel(i, oldNside, newNside);
        newMap[newPos] += map[i];

        count[newPos]++;
    }

    for(long i=0; i<newNpixels; i++)
    {
        newMap[i] = newMap[i]/count[i];
    }

    delete count;

    return newMap;
    //delete newMap;
}


int FieldMap::degradePixel(int pixel, int oldNside, int newNside)
{
    bool ring = true;
    long npixel;

    // TODO: get map ordering

    /* convert nest to nest needed ? */
    if(ring)
        ring2nest(oldNside, pixel, &npixel);
    else
        npixel = pixel;

    int newpos;
    double scaleDown = double(newNside)/double(oldNside);
    newpos = int(npixel * scaleDown * scaleDown);
    return newpos;
}
