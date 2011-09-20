#ifndef MAPOVERLAY_H
#define MAPOVERLAY_H

#include "healpixmap.h"

class MapOverlay
{
public:
    virtual void draw() = 0;
    virtual void build() = 0;

    enum OverlayType {POLARIZATION_VECTORS};

private:
    int faceNumber;
    int nside;
};

#endif // MAPOVERLAY_H
