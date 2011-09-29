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
    PolarizationVectors(int faceNumber, int nside, bool mollweide, HealpixMap* map);
    ~PolarizationVectors();
    void draw();
    void build();

private:
    HealpixMap* map;
    int faceNumber;
    int nside;
    bool mollweide;

    bool bufferCreated;
    bool valuesLoaded;

    long totalVectors;
    float* vectors;
    QGLBuffer* vectorsBuffer;

    QList<int> numberVertices;

    void createBuffer();

    double toMollweide(const double phi, const double lambda, double &x, double &y);
    void convertToMollweide (double ax, double ay, double az, double &rx, double &ry, double &rz);
};

#endif // POLARIZATIONVECTORS_H
