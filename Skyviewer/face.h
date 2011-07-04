#ifndef FACE_H
#define FACE_H

#include <QVector>
#include <QDebug>
#include <chealpix.h>
#include "vertice.h"
#include "facevertices.h"

using namespace std;


class Face
{
public:
    Face();
    Face(int faceNumber);
    void setRigging(int nside, bool mollview, double rad = 1.);
    void draw();

private:
    int faceNumber;
    QVector<Strip> strips;

    void toMollweide(double rad);
    double toMollweide(const double phi, const double lambda, double &x, double &y);
    void toMollweideBackfaceSplit();
};

#endif // FACE_H
