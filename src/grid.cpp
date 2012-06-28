#include "grid.h"

Grid::Grid(QGLViewer* viewer, int dlong, int dlat)
{
    this->viewer = viewer;
    setConfiguration(dlong, dlat);

    color = QColor("white");
    labeling = GRID_LABELING;
    labelSize = GRID_LABEL_SIZE;

    this->mollweide = !DEFAULT_VIEW_3D;
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

void Grid::setLabeling(bool active, int size)
{
    this->labeling = active;
    this->labelSize = size;
}

void Grid::setColor(QColor color)
{
    this->color = color;
}

void Grid::changeToMollweide(bool mollweide)
{
    this->mollweide = mollweide;
}

void Grid::draw()
{
    glClientActiveTexture(GL_TEXTURE0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    if(mollweide)
        drawMollweide();
    else
        draw3D();
}


void Grid::draw3D(int points)
{
    float phi, theta;
    float cosphi, sinphi, costheta, sintheta;
    double x, y;
    float dtheta, dphi;
    float radius = 1.001;

    int nmerid = (int)(180/dlong);
    int nparal = (int)(180/dlat);

    dtheta = 2.0*M_PI / (float)points;
    dphi = 2.0*M_PI / (float)points;

    /* change color */
    double r,g,b;
    color.getRgbF(&r, &g, &b);
    glColor3d(r, g, b);

    /* draw meridians (longitude lines) */
    for (int i=0; i<nmerid; i++)
    {
        phi = (float) i*M_PI / nmerid;

        cosphi = cos(phi);
        sinphi = sin(phi);

        glBegin(GL_LINE_LOOP);
        for (int j=0; j<=points; j++)
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

    if(labeling)
    {
        /* draw parallels coordinates */
        float labelRadius = radius+0.01;

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
                viewer->renderText(x*labelRadius, y*labelRadius, costheta*labelRadius, QString("%1").arg(degrees).append(QString::fromUtf8("º")), QFont("Arial", labelSize));
            }
        }

        /* draw meridians coordinates */
        theta = M_PI/2 - (2*M_PI/nparal)/4;
        costheta = cos(theta);
        sintheta = sin(theta);

        for(int i=0; i<nmerid*2; i++)
        {
            phi = (float) - (i*M_PI / nmerid) - M_PI/2;

            sinphi = sin(phi);

            x = -sintheta*sinphi;
            y = sintheta*cos(phi);
            float degrees;

            if(i<=nmerid)
                degrees = (i*180 / nmerid);
            else
                degrees = - (nmerid*2-i) * 180 / nmerid;
            viewer->renderText(x*labelRadius, y*labelRadius, costheta*labelRadius, QString("%1").arg(degrees).append(QString::fromUtf8("º")), QFont("Arial", labelSize));
        }
    }
}

void Grid::drawMollweide(int points)
{
    float phi, theta;
    float cosphi, sinphi, costheta, sintheta;
    double x, y;
    float dtheta, dphi;
    double rad = 1/sqrt(2.);

    float xPos = -0.0001;

    int nmerid = (int)(181/dlong);
    int nparal = (int)(181/dlat);

    dtheta = M_PI / (float)points;
    dphi = 2.0*M_PI / (float)points;

    /* change color */
    double r,g,b;
    color.getRgbF(&r, &g, &b);
    glColor3d(r, g, b);

    /* draw meridians (longitude lines) */
    for (int i=0; i<=nmerid; i++)
    {
        phi = (float) i*M_PI / nmerid;
        cosphi = cos(phi);
        sinphi = sin(phi);

        int j;

        if(i!=nmerid)
        {
            glBegin(GL_LINE_LOOP);
            for (j=0; j<=points; j++)
            {
                theta = (float) j*dtheta - M_PI/2;
                toMollweide(theta, phi, x, y);
                glVertex3f(xPos, x*rad, y*rad);
            }
            glEnd();
        }

        glBegin(GL_LINE_LOOP);
        for (j=0; j<=points; j++)
        {
            theta = (float) j*dtheta - M_PI/2;
            toMollweide(theta, phi, x, y);
            glVertex3f(xPos, -x*rad, y*rad);
        }
        glEnd();
    }

    /* draw parallels (latitude lines) */
    for(int i=0; i<nparal; i++)
    {
        theta = (float) i*M_PI / nparal - M_PI/2;
        costheta = cos(theta);
        sintheta = sin(theta);

        glBegin(GL_LINE_LOOP);
        for (int j=0; j<=points; j++)
        {
            phi = (float) j*dphi - M_PI;
            toMollweide(theta, phi, x, y);
            glVertex3f(xPos, x*rad, y*rad);
        }
        glEnd();
    }

    if(labeling)
    {
        for(int i=0; i<nparal; i++)
        {
            theta = (float) i*M_PI / nparal - M_PI/2 + M_PI/128;
            phi = -M_PI/2 + (2*M_PI/nmerid)/4;

            if(theta>-M_PI/2 && theta<M_PI/2)
            {
                toMollweide(theta, phi, x, y);

                float degrees = (i*180 / nparal - 90);
                viewer->renderText(xPos, x*rad, y*rad, QString("%1").arg(degrees).append(QString::fromUtf8("º")), QFont("Arial", labelSize));
            }
        }

        /* draw meridians coordinates */
        theta = M_PI/2 - (2*M_PI/nparal)/4;

        for(int i=0; i<nmerid*2; i++)
        {
            phi = (float) (i*M_PI / nmerid) - M_PI + M_PI/64
                    ;
            theta = M_PI/8;

            toMollweide(theta, phi, x, y);

            float degrees = 180 - (i*360/(nmerid*2));

            viewer->renderText(xPos, x*rad, y*rad, QString("%1").arg(degrees).append(QString::fromUtf8("º")), QFont("Arial", labelSize));
        }
    }
}
