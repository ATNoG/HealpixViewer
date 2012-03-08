#include "facevertices.h"

/* pointer to the singleton */
//FaceVertices *FaceVertices::s_instance = 0;

FaceVertices::FaceVertices()
{
}


FaceVertices::~FaceVertices()
{
    #if DEBUG > 0
        qDebug() << "Calling FaceVertices destructor";
    #endif
}


QVector<Strip> FaceVertices::getFaceVertices(int faceNumber, int nside, double radius)
{
    // TODO: costhetats podem ser guardados por nside
    QVector<double> costhetas_eq, costhetas_np, costhetas_sp;
    getRingLatitudes(nside, &costhetas_np, &costhetas_eq, &costhetas_sp);

    if( faceNumber < 4)
        return getRigging_NP(faceNumber, nside, &costhetas_np, radius);
    else if( faceNumber > 7)
        return getRigging_SP(faceNumber, nside, &costhetas_sp, radius);
    else
        return getRigging_EQ(faceNumber, nside, &costhetas_eq, radius);
}


QVector<Strip> FaceVertices::getRigging_EQ(int faceNumber, int nside, QVector<double> *costhetas, double rad)
{
    double ds = 1./nside;
    double s = 0, t = 0;
    Boundary lower;
    /* create lower boundary */
    lower.set_eq(-2./3., 0.5*M_PI*(faceNumber-4), 1);
    Boundary boundry0;
    Boundary boundry1;

    QVector<Strip> strips;
    strips.resize(nside);

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    /* foreach strip */
    stripIT = strips.begin();
    for(int i = 0; i < nside && i<costhetas->size()-1; i++)
    {
        t = 0;
        stripIT->resize(2*(nside+1));

        boundry0.set_eq((*costhetas)[i],   lower((*costhetas)[i]),   -1);
        boundry1.set_eq((*costhetas)[i+1], lower((*costhetas)[i+1]), -1);

        /* foreach division in strip */
        verticeIT = stripIT->begin();
        for(int j = i; j <= i+nside && j<costhetas->size()-1; j++)
        {
            verticeIT->setVertS(acos((*costhetas)[j]),boundry0((*costhetas)[j]),rad);
            verticeIT->setTex(s,t);
            verticeIT++;

            verticeIT->setVertS(acos((*costhetas)[j+1]),boundry1((*costhetas)[j+1]),rad);
            verticeIT->setTex(s+ds,t);
            verticeIT++;

            t += ds;
        }

        s += ds;
        stripIT++;
    }

    return strips;
}


QVector<Strip> FaceVertices::getRigging_NP(int faceNumber, int nside, QVector<double> *costhetas, double rad)
{
    double ds = 1./nside;
    double s = 0, t = 0;
    Boundary lower;
    Boundary boundry0;
    Boundary boundry1;

    QVector<Strip> strips;
    strips.resize(nside);

    lower.set_eq( 0, 0.5*M_PI*(faceNumber+0.5), 1);

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    /* foreach strip */
    stripIT = strips.begin();
    for(int i = 0; i < nside && i<costhetas->size()-1; i++)
    {
        t = 0;
        stripIT->resize(2*(nside+1));

        bool boundry0_reset = false;
        bool boundry1_reset = false;
        boundry0.set_eq((*costhetas)[i],   lower((*costhetas)[i]),   -1);
        boundry1.set_eq((*costhetas)[i+1], lower((*costhetas)[i+1]), -1);

        /* foreach division in strip */
        verticeIT = stripIT->begin();
        for(int j = i; j <= i+nside && j<costhetas->size()-1; j++)
        {
            verticeIT->setVertS(acos((*costhetas)[j]),boundry0((*costhetas)[j]),rad);
            verticeIT->setTex(s,t);
            verticeIT++;

            verticeIT->setVertS(acos((*costhetas)[j+1]),boundry1((*costhetas)[j+1]),rad);
            verticeIT->setTex(s+ds,t);
            verticeIT++;

            if( ((*costhetas)[j] >= 2./3.) && ! boundry0_reset)
            {
                boundry0.set_np((*costhetas)[j],boundry0((*costhetas)[j]), faceNumber);
                boundry0_reset = true;
            }

            if( ((*costhetas)[j+1 ]>= 2./3.) && ! boundry1_reset)
            {
                boundry1.set_np((*costhetas)[j+1],boundry1((*costhetas)[j+1]), faceNumber);
                boundry1_reset = true;
            }

            t += ds;
        }

        s += ds;
        stripIT++;
    }

    return strips;
}


QVector<Strip> FaceVertices::getRigging_SP(int faceNumber, int nside, QVector<double> *costhetas, double rad)
{
    double ds = 1./nside;
    double s = 0, t = 0;
    double phi_boundry = 0.5*M_PI*(faceNumber-7);
    Boundary boundry0;
    Boundary boundry1;

    QVector<Strip> strips;
    strips.resize(nside);

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    /* foreach strip */
    stripIT = strips.begin();
    for(int i = 0; i < nside && i<costhetas->size()-1; i++)
    {
        t = 0;
        stripIT->resize(2*(nside+1));

        bool boundry0_reset = false;
        bool boundry1_reset = false;
        boundry0.set_sp((*costhetas)[i],   phi_boundry, faceNumber);
        boundry1.set_sp((*costhetas)[i+1], phi_boundry, faceNumber);

        /* foreach division in strip */
        verticeIT = stripIT->begin();
        for(int j = i; j <= i+nside && j<costhetas->size()-1; j++)
        {
            verticeIT->setVertS(acos((*costhetas)[j]),boundry0((*costhetas)[j]),rad);
            verticeIT->setTex(s,t);
            verticeIT++;

            verticeIT->setVertS(acos((*costhetas)[j+1]),boundry1((*costhetas)[j+1]),rad);
            verticeIT->setTex(s+ds,t);
            verticeIT++;

            if( ((*costhetas)[j] > (-2./3. - 0.001)) && ! boundry0_reset)
            {
                boundry0.set_eq((*costhetas)[j],boundry0((*costhetas)[j]),-1);
                boundry0_reset = true;
            }
            if( ((*costhetas)[j+1] > (-2./3. - 0.001)) && ! boundry1_reset)
            {
                boundry1.set_eq((*costhetas)[j+1],boundry1((*costhetas)[j+1]),-1);
                boundry1_reset = true;
            }

            t += ds;
        }

        s += ds;
        stripIT++;
    }

    return strips;
}



/* Get the latitude of all pixel rings */
void FaceVertices::getRingLatitudes(int nside, QVector<double> *costhetas_np, QVector<double> *costhetas_eq, QVector<double> *costhetas_sp)
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

    /* save cosThetas in reverse order */
    // TODO: maybe a better way is to reverse the above algorithm...
    unsigned int n;
    n = thetas_np.size();
    costhetas_np->resize(n);
    for(unsigned int i = 0; i < n; i++)
	(*costhetas_np)[i] = cos(thetas_np[n-i-1]);   
    
    n = thetas_eq.size();
    costhetas_eq->resize(thetas_eq.size());
    for(unsigned int i = 0; i < n; i++)
	(*costhetas_eq)[i] = cos(thetas_eq[n-i-1]);
   
    n = thetas_sp.size();
    costhetas_sp->resize(thetas_sp.size());
    for(unsigned int i = 0; i < n; i++)
	(*costhetas_sp)[i] = cos(thetas_sp[n-i-1]);
}
