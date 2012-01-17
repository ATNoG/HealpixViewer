#ifndef VERTICE_H
#define VERTICE_H

#include <math.h>
#include <QGLViewer/qglviewer.h>
#include <QDebug>

class Vertice
{

public:
    Vertice();

    void setVertS(double theta, double phi, double rad = 1.);
    void setTex(double s, double t);
    void draw();

    float x,y,z;
    float s, t;

};

#endif // VERTICE_H
