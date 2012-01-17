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


/* set texture coordinates */
void Vertice::setTex(double _s, double _t)
{
    s = _s;
    t = _t;
}


void Vertice::draw()
{
    //qDebug() << "drawing vertice in pos: " << x << "," << y << "," << z << " - text in pos " << s << "," << t;
    /* get texture pixel */
    glTexCoord2f(s, t);
    /* draw vertice */
    glVertex3f(x, y, z);
}
