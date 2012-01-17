#include "grid.h"

Grid::Grid(int lines)
{
    this->lines = lines;
}

Grid::~Grid()
{
    qDebug() << "Calling Grid destructor";
}

void Grid::setNumberLines(int lines)
{
    this->lines = lines;
}

void Grid::draw()
{
    float phi, theta;
    float cosphi, sinphi, costheta, sintheta;
    float x, y;
    int points = 120;
    float dtheta, dphi;
    float radius = 1.001;

    dtheta = 2.0*M_PI / (float)points;
    dphi = 2.0*M_PI / (float)points;

    for (int i=0; i<lines; i++)
    {
        /* draw longitude lines */
        phi = (float) i*M_PI / lines;
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

        /* draw latitude lines */
        theta = (float) i*M_PI / lines;
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

    //qDebug("Drawing grid");
}
