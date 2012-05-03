#include "grid.h"

Grid::Grid(int dlong, int dlat)
{
    setConfiguration(dlong, dlat);
}

Grid::~Grid()
{
    #if DEBUG > 0
        qDebug() << "Calling Grid destructor";
    #endif
}

void Grid::setConfiguration(int dlong, int dlat)
{
    this->dlong = dlong;
    this->dlat  = dlat;
}

void Grid::draw()
{
    float phi, theta;
    float cosphi, sinphi, costheta, sintheta;
    float x, y;
    int points = 120;
    float dtheta, dphi;
    float radius = 1.001;

    int nmerid = (int)(181/dlong);
    int nparal = (int)(181/dlat);

    dtheta = 2.0*M_PI / (float)points;
    dphi = 2.0*M_PI / (float)points;

    /* draw meridians (longitude lines) */
    for (int i=0; i<nmerid; i++)
    {
        phi = (float) i*M_PI / nmerid;
        cosphi = cos(phi);
        sinphi = sin(phi);

        glBegin(GL_LINE_LOOP);
        for (int j=0; j<points; j++)
        {
            theta = (float) j*dtheta;
            costheta = cos(theta);
            sintheta = sin(theta);
            x = -sinphi*sintheta;
            y = cosphi*sintheta;
            glVertex3f(x*radius, y*radius, costheta*radius);
        }
        glEnd();
    }

    qDebug() << "total paralelos: " << nparal;

    /* draw parallels (latitude lines) */
    for(int i=0; i<nparal; i++)
    {
        qDebug() << "paralelo " << i;
        theta = (float) i*M_PI / nparal;
        costheta = cos(theta);
        sintheta = sin(theta);

        glBegin(GL_LINE_LOOP);
        for (int j=0; j<points; j++)
        {
            phi = (float) j*dphi;
            x = -sintheta*sin(phi);
            y = sintheta*cos(phi);
            glVertex3f(x*radius, y*radius, costheta*radius);
        }
        glEnd();
    }
}
