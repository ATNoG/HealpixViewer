#ifndef GRID_H
#define GRID_H

#include <math.h>
#include <GL/gl.h>
#include <QGLViewer/qglviewer.h>
#include <QDebug>

class Grid
{
public:
    Grid(QGLViewer* viewer, int dlong, int dlat);
    ~Grid();

    void setConfiguration(int dlong, int dlat);
    void draw();

private:
    int dlong;
    int dlat;
    QGLViewer* viewer;
};

#endif // GRID_H
