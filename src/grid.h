#ifndef GRID_H
#define GRID_H

#include <math.h>
#include <GL/gl.h>
#include <QGLViewer/qglviewer.h>
#include <QDebug>
#include "configs.h"

class Grid
{
public:
    Grid(QGLViewer* viewer, int dlong, int dlat);
    ~Grid();

    void setConfiguration(int dlong, int dlat);
    void setLabeling(bool active, int size=GRID_LABEL_SIZE);
    void setColor(QColor color);
    void draw();

private:
    int dlong;
    int dlat;
    bool labeling;
    int labelSize;
    QGLViewer* viewer;
    QColor color;
};

#endif // GRID_H
