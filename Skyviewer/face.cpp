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

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticesIT;
    /* draw each vertice of each strip */
    for(stripIT=strips.begin(); stripIT!=strips.end(); ++stripIT)
    {
        glBegin(GL_QUAD_STRIP);

        for(verticesIT=stripIT->begin(); verticesIT!=stripIT->end(); verticesIT++)
        {
            verticesIT->draw();
        }

         glEnd();
    }
}


void Face::setRigging(int nside, QVector<double> &costhetas, bool mollview, double radius)
{
    if( faceNumber < 4)
        setRigging_NP(nside,costhetas,radius);
    else if( faceNumber > 7)
        setRigging_SP(nside,costhetas,radius);
    else
        setRigging_EQ(nside,costhetas,radius);


    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    int i = 0;

    /*
    qDebug() << "Face " << faceNumber;
    for(stripIT=strips.begin(); stripIT!=strips.end(); ++stripIT)
    {
        qDebug() << "  Strip " << i;
        int j = 0;
        for(verticeIT=stripIT->begin(); verticeIT!=stripIT->end(); ++verticeIT)
        {
            qDebug() << "    Vertice " << j;
            qDebug() << "       (x,y,z) = " << verticeIT->x << "," << verticeIT->y << "," << verticeIT->z << " (s,t) = "  << verticeIT->s << "," << verticeIT->t;
            j++;
        }
        i++;
    }
    */

    /* texture coordinates */
    stripIT = strips.begin();
    int j=0;
    for(int i = 0; i < nside; i++)
    {
        for(verticeIT=stripIT->begin(); verticeIT!=stripIT->end(); ++verticeIT)
        {
            verticeIT->s = 0.25*verticeIT->s + 0.25*(faceNumber % 4);
            verticeIT->t = 0.25*verticeIT->t + 0.25*(faceNumber / 4);
            j++;
        }
        stripIT++;
    }

    if(mollview)
        toMollweide(radius);
}


void Face::setRigging_EQ(int nside, QVector<double> &costhetas, double rad)
{
    double ds = 1./nside;
    double s = 0, t = 0;
    Boundary lower;
    /* create lower boundary */
    lower.set_eq(-2./3., 0.5*M_PI*(faceNumber-4), 1);
    Boundary boundry0;
    Boundary boundry1;

    strips.resize(nside);

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    /* foreach strip */
    stripIT = strips.begin();
    for(int i = 0; i < nside; i++)
    {
        t = 0;
        stripIT->resize(2*(nside+1));

        boundry0.set_eq(costhetas[i],   lower(costhetas[i]),   -1);
        boundry1.set_eq(costhetas[i+1], lower(costhetas[i+1]), -1);

        /* foreach division in strip */
        verticeIT = stripIT->begin();
        for(int j = i; j <= i+nside; j++)
        {
            verticeIT->setVertS(acos(costhetas[j]),boundry0(costhetas[j]),rad);
            verticeIT->setTex(s,t);
            verticeIT++;

            verticeIT->setVertS(acos(costhetas[j+1]),boundry1(costhetas[j+1]),rad);
            verticeIT->setTex(s+ds,t);
            verticeIT++;

            t += ds;
        }

        s += ds;
        stripIT++;
    }
}


void Face::setRigging_NP(const int nside, QVector<double> &costhetas, double rad)
{
    double ds = 1./nside;
    double s = 0, t = 0;
    Boundary lower;
    Boundary boundry0;
    Boundary boundry1;

    strips.resize(nside);

    lower.set_eq( 0, 0.5*M_PI*(faceNumber+0.5), 1);

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    /* foreach strip */
    stripIT = strips.begin();
    for(int i = 0; i < nside; i++)
    {
        t = 0;
        stripIT->resize(2*(nside+1));

        bool boundry0_reset = false;
        bool boundry1_reset = false;
        boundry0.set_eq(costhetas[i],   lower(costhetas[i]),   -1);
        boundry1.set_eq(costhetas[i+1], lower(costhetas[i+1]), -1);

        /* foreach division in strip */
        verticeIT = stripIT->begin();
        for(int j = i; j <= i+nside; j++)
        {
            verticeIT->setVertS(acos(costhetas[j]),boundry0(costhetas[j]),rad);
            verticeIT->setTex(s,t);
            verticeIT++;

            verticeIT->setVertS(acos(costhetas[j+1]),boundry1(costhetas[j+1]),rad);
            verticeIT->setTex(s+ds,t);
            verticeIT++;

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

            t += ds;
        }

        s += ds;
        stripIT++;
    }
}


void Face::setRigging_SP(const int nside, QVector<double> &costhetas, double rad)
{
    double ds = 1./nside;
    double s = 0, t = 0;
    double phi_boundry = 0.5*M_PI*(faceNumber-7);
    Boundary boundry0;
    Boundary boundry1;

    strips.resize(nside);

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    /* foreach strip */
    stripIT = strips.begin();
    for(int i = 0; i < nside; i++)
    {
        t = 0;
        stripIT->resize(2*(nside+1));

        bool boundry0_reset = false;
        bool boundry1_reset = false;
        boundry0.set_sp(costhetas[i],   phi_boundry, faceNumber);
        boundry1.set_sp(costhetas[i+1], phi_boundry, faceNumber);

        /* foreach division in strip */
        verticeIT = stripIT->begin();
        for(int j = i; j <= i+nside; j++)
        {
            verticeIT->setVertS(acos(costhetas[j]),boundry0(costhetas[j]),rad);
            verticeIT->setTex(s,t);
            verticeIT++;

            verticeIT->setVertS(acos(costhetas[j+1]),boundry1(costhetas[j+1]),rad);
            verticeIT->setTex(s+ds,t);
            verticeIT++;

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

            t += ds;
        }

        s += ds;
        stripIT++;
    }
}





void Face::toMollweide(double rad)
{
    double r = 1/sqrt(2.);
    double phi;
    double lambda;
    double x,y;

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticeIT;

    for(stripIT=strips.begin(); stripIT!=strips.end(); ++stripIT)
    {
        for(verticeIT=stripIT->begin(); verticeIT!=stripIT->end(); ++verticeIT)
        {
            phi = asin(verticeIT->z);
            lambda = atan2(verticeIT->y, verticeIT->x);
            toMollweide(phi, lambda, x, y);
            if( (faceNumber == 2 || faceNumber == 10) && x > 0) x *= -1;
            verticeIT->y = r*x;
            verticeIT->z = r*y;
            verticeIT->x = (rad < 1.) ? -(rad - 1.) : 0;
        }
    }

    if(faceNumber == 6)
        toMollweideBackfaceSplit();
}


void Face::toMollweideBackfaceSplit()
{
    if(faceNumber!=6)
        return;

    // copy the current list to temp storage
    QVector<Strip> oldstrips;
    oldstrips.resize(strips.size());
    QVector<Strip>::iterator srcqli = strips.begin();
    QVector<Strip>::iterator destqli = oldstrips.begin();

    for(uint i=0; i<strips.size(); i++)
    {
        destqli->resize(srcqli->size());
        QVector<Vertice>::iterator srcpti = srcqli->begin();
        QVector<Vertice>::iterator destpti = destqli->begin();
        for(uint j=0; j<srcqli->size(); j++)
            *destpti++ = *srcpti++;
        srcqli++;
        destqli++;
    }

    // Now refill it, making the break along the back as needed
    srcqli = oldstrips.begin();
    QVector<Strip>::iterator qli;
    strips.resize(0);
    uint i = 0;
    for(srcqli = oldstrips.begin(); srcqli != oldstrips.end(); ++srcqli)
    {
        i++;

        bool patch = true;	// need to patch the end of the left and start of the right sides
        bool leftside = true;	// true while building the left side quadlist

        QVector<Vertice> newverts1;
        QVector<Vertice> newverts2;

        uint jbreak = srcqli->size(); // vertex number of switch between left and right side
        jbreak = 2*i-2;
        for(uint j = 0; j < srcqli->size(); j++ )
        {

            Vertice p = (*srcqli)[j];
            if( leftside )
            {
                p.y = -fabs(p.y);
                newverts1.push_back(p);
                leftside = j < jbreak;
            }
            else if( patch )
            {
                p.y = -fabs(p.y);
                newverts1.push_back(p);

                Vertice p2;
                QVector<Strip>::iterator qli = srcqli;
                qli++;
                if( qli != oldstrips.end() )
                {
                    p2 = (*qli)[j+1];
                    p2.y = -fabs(p2.y);
                }
                else
                {
                    p2.y = -1.66597;
                    p2.z =  0.553293;
                    p2.s = 0.75;
                    p2.t = 0.5;
                }
                newverts1.push_back(p2);
                newverts1.push_back(p);


                // Patch the beginning of the right side of the split face
                p = (*srcqli)[j+1];
                p.y = fabs(p.y);
                if( srcqli != oldstrips.begin() )
                {
                    qli = srcqli;
                    qli--;
                    p2 = (*qli)[j];
                    p2.y = fabs(p2.y);
                }
                else
                {
                    p2.y =  1.66597;
                    p2.z = -0.553293;
                    p2.s = 0.5;
                    p2.t = 0.25;
                }
                newverts2.push_back(p);
                newverts2.push_back(p2);

                patch = false;
            }
            else
            {
                p.y = fabs(p.y);
                newverts2.push_back(p);
            }
        }
        strips.push_back(newverts1);
        strips.push_back(newverts2);
    }
}


/* TODO: put this function in another place ? */
double Face::toMollweide(const double phi, const double lambda, double &x, double &y)
{
    const double lambda0 = 0;
        const double r2 = sqrt(2.);
        double theta;
        x = phi;
        y = lambda;
/*
                        Start by solving 2theta + sin(2theta) == pi sin(phi), for theta

                                Upper limit.
*/
        if( fabs(x - M_PI/2) < 1e-6) {
                theta = M_PI/2;
        }
/*
                                Lower limit
*/
        else if (fabs(x + M_PI/2) < 1e-6 ) {
                theta = -M_PI/2;
        }
        else {
/*
                                Iterative computation using Newton's method.
*/
                const int nmax = 20;		// max number of iterations for Newton's method
                const double eps = 1e-6; 	// how well we solve for theta
                int i;
                double dtheta;
                double f,df;
                theta = x/2;									// The initial guess.
                f = 2*theta + sin(2*theta) - M_PI*sin(phi); 	// Solve for this function.
                for(i = 0; i < nmax; i++) {
                        if( fabs(f) < eps)
                                break;
                        df = 2*(1+cos(2*theta));
                        dtheta = -f/df;
                        theta += dtheta;
                        f = 2*theta + sin(2*theta) - M_PI*sin(x);
                }
        }
/*
                        Now that we have the theta, we can compute the cartesian coordinates.
*/
        x = 2. * r2 * (y - lambda0) * cos(theta) / M_PI;
        y = r2 * sin(theta);

        return theta;
}
