#include "pixellutcache.h"

/* pointer to the singleton */
PixelLUTCache* PixelLUTCache::s_instance = NULL;


PixelLUTCache* PixelLUTCache::getInstance()
{
    if(!s_instance)
        s_instance = new PixelLUTCache;

    return s_instance;
}

PixelLUTCache::PixelLUTCache()
{
}

PixelLUTCache::~PixelLUTCache()
{
}

PixelLUT* PixelLUTCache::getLut(int nside)
{
    PixelLUT *lut;

    /* if lut already constructed for wanted nside return it */
    if(cache.find(nside)!=cache.end())
        lut = cache[nside];
    else
    {
        lut = new PixelLUT;

        lut->resize(nside2npix(nside));
        int  face = 0, x = 0, y = 0;
        long dy = nside;
        long pix;
        long k = 0;
        for(face = 0; face < 12; face++)
        {
            long xo = 0;//currentNside*(face % 4);
            long yo = 0;//currentNside*(face / 4);
            long face_offset = face*nside*nside;
            for(y = 0; y < nside; y++)
            {
                for(x = 0; x < nside; x++)
                {
                    k = x + xo + (y+yo)*dy + face_offset;
                    pix = xy2pix(x,y)  + face_offset;
                    // TODO!
                    if(false)
                    //if (!nest)
                        nest2ring(nside,pix,&pix);
                    lut->at(pix) = k;
                }
            }
        }

        cache[nside] = lut;
    }

    return lut;
}
