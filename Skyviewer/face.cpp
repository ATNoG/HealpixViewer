#include "face.h"


Face::Face()
{

}

Face::Face(int _faceNumber)
{
    faceNumber = _faceNumber;
}


void Face::setNumber(int _faceNumber)
{
    faceNumber = _faceNumber;
}


/* draw the face */
void Face::draw()
{
    Strip strip;
    glBegin(GL_QUAD_STRIP);

    /* draw each vertice of each strip */
    foreach(strip, strips)
    {
        Vertice v;
        foreach(v, strip)
        {
            v.draw();
        }
    }

    glEnd();
}


void Face::setRigging(int nside, QVector<double> &costhetas, double radius)
{
    if( faceNumber < 4)
        setRigging_NP(nside,costhetas,radius);
    else if( faceNumber > 7)
        setRigging_SP(nside,costhetas,radius);
    else
        setRigging_EQ(nside,costhetas,radius);
}


void Face::setRigging_EQ(int nside, QVector<double> &costhetas, double rad)
{
    Boundary lower;
    /* create lower boundary */
    lower.set_eq(-2./3., 0.5*M_PI*(faceNumber-4), 1);
    Boundary boundry0;
    Boundary boundry1;

    strips.resize(nside);

    int verticePos;

    /* foreach strip */
    for(int i = 0; i < nside; i++)
    {
        strips[i].resize(2*(nside+1));

        /* reset vertice position */
        verticePos = 0;

        boundry0.set_eq(costhetas[i],   lower(costhetas[i]),   -1);
        boundry1.set_eq(costhetas[i+1], lower(costhetas[i+1]), -1);

        /* foreach division in strip */
        for(int j = i; j <= i+nside; j++)
        {
            strips[i][verticePos].setVertS(acos(costhetas[j]),boundry0(costhetas[j]),rad);
            verticePos++;
            strips[i][verticePos].setVertS(acos(costhetas[j+1]),boundry1(costhetas[j+1]),rad);
            verticePos++;
        }
    }
}


void Face::setRigging_NP(const int nside, QVector<double> &costhetas, double rad)
{
    Boundary lower;
    Boundary boundry0;
    Boundary boundry1;

    int verticePos;

    strips.resize(nside);

    lower.set_eq( 0, 0.5*M_PI*(faceNumber+0.5), 1);

    for(int i = 0; i < nside; i++)
    {

        strips[i].resize(2*(nside+1));

        /* reset vertice position */
        verticePos = 0;

        bool boundry0_reset = false;
        bool boundry1_reset = false;
        boundry0.set_eq(costhetas[i],   lower(costhetas[i]),   -1);
        boundry1.set_eq(costhetas[i+1], lower(costhetas[i+1]), -1);

        for(int j = i; j <= i+nside; j++)
        {
            strips[i][verticePos].setVertS(acos(costhetas[j]),boundry0(costhetas[j]),rad);
            verticePos++;

            strips[i][verticePos].setVertS(acos(costhetas[j+1]),boundry1(costhetas[j+1]),rad);
            verticePos++;

            if( (costhetas[j] >= 2./3.) && ! boundry0_reset)
            {
                boundry0.set_np(costhetas[j],boundry0(costhetas[j]), faceNumber);
                boundry0_reset = true;
            }

            if( (costhetas[j+1 ]>= 2./3.) && ! boundry1_reset)
            {
                boundry1.set_np(costhetas[j+1],boundry1(costhetas[j+1]), faceNumber);
                boundry1_reset = true;
            }
        }
    }
}


void Face::setRigging_SP(const int nside, QVector<double> &costhetas, double rad)
{
    double phi_boundry = 0.5*M_PI*(faceNumber-7);
    Boundary boundry0;
    Boundary boundry1;
    int verticePos = 0;

    strips.resize(nside);

    for(int i = 0; i < nside; i++)
    {
        strips[i].resize(2*(nside+1));

        /* reset vertice position */
        verticePos = 0;

        bool boundry0_reset = false;
        bool boundry1_reset = false;
        boundry0.set_sp(costhetas[i],   phi_boundry, faceNumber);
        boundry1.set_sp(costhetas[i+1], phi_boundry, faceNumber);

        for(int j = i; j <= i+nside; j++)
        {
            strips[i][verticePos].setVertS(acos(costhetas[j]),boundry0(costhetas[j]),rad);
            verticePos++;

            strips[i][verticePos].setVertS(acos(costhetas[j+1]),boundry1(costhetas[j+1]),rad);
            verticePos++;

            if( (costhetas[j] > (-2./3. - 0.001)) && ! boundry0_reset)
            {
                boundry0.set_eq(costhetas[j],boundry0(costhetas[j]),-1);
                boundry0_reset = true;
            }
            if( (costhetas[j+1] > (-2./3. - 0.001)) && ! boundry1_reset)
            {
                boundry1.set_eq(costhetas[j+1],boundry1(costhetas[j+1]),-1);
                boundry1_reset = true;
            }
        }
    }
}
