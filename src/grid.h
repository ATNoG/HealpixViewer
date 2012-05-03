#ifndef GRID_H
#define GRID_H

#include <math.h>
#include <GL/gl.h>
#include <QDebug>

class Grid
{
public:
    Grid(int dlong, int dlat);
    ~Grid();

    void setConfiguration(int dlong, int dlat);
    void draw();

private:
    int dlong;
    int dlat;
};

#endif // GRID_H
