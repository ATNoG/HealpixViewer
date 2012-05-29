#include "grid.h"

Grid::Grid(QGLViewer* viewer, int dlong, int dlat)
{
    this->viewer = viewer;
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
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

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

    /* draw parallels (latitude lines) */
    for(int i=0; i<nparal; i++)
    {
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

    /* draw coordinates */
    float labelRadius = radius+0.02;

    glColor3f(1.0, 1.0, 1.0);
    for(int i=1; i<nparal; i++)
    {
        theta = (float) i*M_PI / nparal;
        costheta = cos(theta);
        sintheta = sin(theta);
        for(int j=0; j<4; j++)
        {
            phi = j*(M_PI/2) + (2*M_PI/nmerid)/4;
            x = -sintheta*sin(phi);
            y = sintheta*cos(phi);
            //float degrees = -(theta * 180 / M_PI - 90);
            float degrees = - (i*180 / nparal - 90);
            viewer->renderText(x*labelRadius, y*labelRadius, costheta*labelRadius, QString("%1").arg(degrees).append(QString::fromUtf8("º")), QFont("Arial", 9));
        }
    }

    theta = M_PI/2 - (2*M_PI/nparal)/4;
    costheta = cos(theta);
    sintheta = sin(theta);

    for(int i=0; i<nmerid*2; i++)
    {
        phi = (float) i*M_PI / nmerid;

        sinphi = sin(phi);

        x = -sintheta*sinphi;
        y = sintheta*cos(phi);
        float degrees = - (i*180 / nparal - 180) + 90;
        viewer->renderText(x*labelRadius, y*labelRadius, costheta*labelRadius, QString("%1").arg(degrees).append(QString::fromUtf8("º")), QFont("Arial", 9));
    }
}
