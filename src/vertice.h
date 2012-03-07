#ifndef VERTICE_H
#define VERTICE_H

#include <math.h>

#ifdef DARWIN
#include <qglviewer.h>
#else
#include <QGLViewer/qglviewer.h>
#endif
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
