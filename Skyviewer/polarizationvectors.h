#ifndef POLARIZATIONVECTORS_H
#define POLARIZATIONVECTORS_H

#include <QGLBuffer>
#include "mapoverlay.h"
#include <healpixutil.h>
#include <math.h>
#include "configs.h"


class PolarizationVectors : public MapOverlay
{

public:
    PolarizationVectors(int faceNumber, int nside, HealpixMap* map);
    ~PolarizationVectors();
    void draw();
    void build();

private:
    HealpixMap* map;
    int faceNumber;
    int nside;

    bool bufferCreated;
    bool valuesLoaded;

    long totalVectors;
    float* vectors;
    QGLBuffer* vectorsBuffer;

    void createBuffer();
};

#endif // POLARIZATIONVECTORS_H
