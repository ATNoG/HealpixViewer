#ifndef GRID_H
#define GRID_H

#include <math.h>
#include <GL/gl.h>
#include <QDebug>

class Grid
{
public:
    Grid(int lines);

    void setNumberLines(int lines);
    void draw();

private:
    int lines;
};

#endif // GRID_H
