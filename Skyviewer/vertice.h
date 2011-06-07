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
    void draw();

private:
    float x,y,z;

};

#endif // VERTICE_H
