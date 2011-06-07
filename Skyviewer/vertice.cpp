#include "vertice.h"

Vertice::Vertice()
{
    x = y = z = 0;
}


/* convert spheric coordinates, to plan coordinates */
void Vertice::setVertS(double theta, double phi, double rad)
{
    x = rad * cos(phi) * sin(theta);
    y = rad * sin(phi) * sin(theta);
    z = rad * cos(theta);
}


void Vertice::draw()
{
    glVertex3f(x, y, z);
}
