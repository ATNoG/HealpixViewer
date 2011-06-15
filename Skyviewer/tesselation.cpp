#include "tesselation.h"

Tesselation::Tesselation(int _nside, bool _mollview)
{
    nside = _nside;
    mollview = _mollview;
    init();
}


/* Get the latitude of all pixel rings */
void Tesselation::getRingLatitudes()
{
    double theta, phi, cosTheta;
    int pix=0, dpix=4;
    int npixels = nside2npix(nside);
    const double eps = 0.128/double(nside);

    QVector<double> thetas_np;
    QVector<double> thetas_eq;
    QVector<double> thetas_sp;

    /* add north pole point */
    thetas_np.append(0);

    /* iterate over the first pixel of each ring to get its theta, and calculate the latitude (costheta) */
    while(pix<npixels)
    {
        /* get pixel coordinates */
        pix2ang_ring(nside, pix, &theta, &phi);
        cosTheta = cos(theta);

        /* save ring latitude */
        if( cosTheta > -eps )
        {
            thetas_np.push_back(theta);
            //qDebug() << "cos=" << costheta << " sin=" << sintheta << " - NP";
        }
        if( fabs(cosTheta) <= (2./3.+eps) )
        {
            thetas_eq.push_back(theta);
            //qDebug() << "cos=" << costheta << " sin=" << sintheta << " - EQ";
        }
        if( cosTheta < eps )
        {
            thetas_sp.push_back(theta);
            //qDebug() << "cos=" << costheta << " sin=" << sintheta << " - SP";
        }


        /* jump to the first pixel of next ring */
        pix += dpix;

        /* calculate the next increment */
        /* in the polar regions, the number of pixels in ring decreases by 1 pixel
            per ring per face, as we move from north to south from equatorial zone. North to south from north polar cap, it increases
        */
        if(cosTheta > 2./3.)
            dpix += 4;
        else if(cosTheta < -2./3.)
            dpix -= 4;
    }
    /* add south pole point */
    thetas_sp.push_back(M_PI);


    unsigned int n = thetas_np.size();
    costhetas_np.resize(n);
    costhetas_eq.resize(n);
    costhetas_sp.resize(n);

    /* save cosThetas in reverse order */
    // TODO: maybe a better way is to reverse the above algorithm...
    for(unsigned int i = 0; i < n; i++)
    {
        costhetas_np[i] = cos(thetas_np[n-i-1]);
        costhetas_eq[i] = cos(thetas_eq[n-i-1]);
        costhetas_sp[i] = cos(thetas_sp[n-i-1]);
    }
}


void Tesselation::init()
{
    /* get ring latitudes */
    getRingLatitudes();

    /* create faces */
    for(unsigned int i=0; i<12; i++)
    {
        Face f(i);
        f.setNumber(i);
        faces.append(f);
    }

    /* TODO: input of setRigging ok */

    /* set face rigging */
    unsigned int i;
    for(i=0; i<4; i++)
        faces[i].setRigging(nside, costhetas_np, mollview);
    for(; i<8; i++)
        faces[i].setRigging(nside, costhetas_eq, mollview);
    for(; i<12; i++)
        faces[i].setRigging(nside, costhetas_sp, mollview);
}


/* Draw opengl primitives */
void Tesselation::draw()
{
    /* draw each face */
    for(int i=0; i<12; i++)
    {
        faces[i].draw();
    }

   // faces[0].draw();

/*
    for(int i=3; i<8; i++)
        faces[i].draw();*/



/*
    glColor3f(1.0, 0.0, 0.0);
    faces[0].draw();
    glColor3f(0.0, 1.0, 0.0);
    faces[1].draw();
    glColor3f(0.0, 0.0, 1.0);
    faces[2].draw();
    glColor3f(1.0, 1.0, 0.0);
    faces[3].draw();
    glColor3f(0.0, 1.0, 1.0);
    faces[4].draw();
    glColor3f(1.0, 0.0, 1.0);
    faces[5].draw();
    glColor3f(0.5, 1.0, 0.0);
    faces[6].draw();
    glColor3f(0.0, 1.0, 0.5);
    faces[7].draw();
    glColor3f(0.5, 1.0, 0.5);
    faces[8].draw();
    glColor3f(0.5, 0.0, 1.0);
    faces[9].draw();
    glColor3f(1.0, 0.0, 0.5);
    faces[10].draw();
    glColor3f(0.5, 0.5, 1.0);
    faces[11].draw();*/
}
