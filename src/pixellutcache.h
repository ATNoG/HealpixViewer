#ifndef LUT_H
#define LUT_H

#include "healpixutil.h"
#include <stdio.h>

using namespace std;

typedef vector<long> PixelLUT;

class PixelLUTCache
{
    public:
        static PixelLUTCache *getInstance();
        PixelLUT* getLut(int nside);

    private:
        PixelLUTCache();
        ~PixelLUTCache();
        static PixelLUTCache *s_instance;

        map<int, PixelLUT* > cache;
};

#endif // LUT_H
