#ifndef HEALPIXUTIL_H
#define HEALPIXUTIL_H

#include <vector>
#include <map>
#include <chealpix.h>

typedef std::vector<long> PixLUT;
typedef std::map<int, PixLUT > PixLUTCache;


inline long xy2pix(long ix, long iy)
{
        static int x2pix[128];
        static int y2pix[128];
        static bool xy2pix_called = false;
        if( ! xy2pix_called ) {
                mk_xy2pix(x2pix, y2pix);
        xy2pix_called = true;
        }
        //int ix_low = (int)fmod(ix,128);
        int ix_low = ix % 128;
        int ix_hi  = ix / 128;
        int iy_low = iy % 128;
        int iy_hi  = iy / 128;
        return (x2pix[ix_hi]+y2pix[iy_hi]) * (128 * 128)
                        + (x2pix[ix_low]+y2pix[iy_low]);
}

#endif // HEALPIXUTIL_H
