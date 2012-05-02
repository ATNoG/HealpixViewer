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

/* get vertices for a single pixel */
QVector<Vertice> FaceVertices::getPixelVertices(int pixelIndex, int nside, double radius)
{
    long int pixelRing;
    nest2ring(nside, pixelIndex, &pixelRing);

    QVector<Vertice> vertices;
    vertices.resize(4);

    double theta, phi;
    pix2ang_ring(nside, pixelRing, &theta, &phi);

    int i_zone = (int) (phi / M_PI_2);
    int ringno = ring(pixelRing, nside);

    int i_phi_count = std::min(ringno, (int)std::min(nside, 4*nside-ringno));
    int i_phi = 0;
    double phifac = M_PI_2 / i_phi_count;

    if(ringno>=(long)nside && ringno<=3*nside)
        i_phi = (int) (phi / phifac + ((ringno%2) / 2.0)) + 1;
    else
        i_phi = (int) (phi / phifac) + 1;

    i_phi = i_phi - (i_zone * i_phi_count);

    QVector<double> nms = integration_limits_in_costh(ringno, nside);

    double ntheta = acos(nms[0]);
    double stheta = acos(nms[2]);
    QVector<double> philr = pixel_boundaries(ringno, i_phi, i_zone, nms[0], nside);

    if(i_phi > ( i_phi_count / 2 ))
    {
        vertices[0].setVertS(ntheta, philr[1]);
    }
    else
    {
        vertices[0].setVertS(ntheta, philr[0]);
    }


    philr = pixel_boundaries(ringno, i_phi, i_zone, nms[2], nside);

    if(i_phi > ( i_phi_count / 2 ))
        vertices[2].setVertS(stheta, philr[1]);
    else
        vertices[2].setVertS(stheta, philr[0]);

    double mtheta = acos(nms[1]);
    philr = pixel_boundaries(ringno, i_phi, i_zone, nms[1], nside);
    vertices[1].setVertS(mtheta, philr[0]);
    vertices[3].setVertS(mtheta, philr[1]);

    return vertices;
}


int FaceVertices::ring(int ipix, int nside)
{
    long npface = (long)nside * (long)nside;
    long npix = 12 * npface;
    long ncap = 2 * (long)nside * ((long)nside - 1);// points in each polar cap

    int iring = 0;
    long ipix1 = ipix + 1;
    int ip;
    double hip, fihip = 0;
    if ( ipix1 <= ncap ) { // North Polar cap -------------
        hip = ipix1 / 2.0;
        fihip = (int) ( hip );
        iring = (int) ( sqrt(hip - sqrt(fihip)) ) + 1;// counted
        // from
        // North
        // pole
    }
    else
    {
        if ( ipix1 <= 2*nside * ( 5 * (long)nside + 1 ) )
        {
        // Equatorial region
            // ------
            ip = (int)(ipix1 - ncap - 1);
            iring = (int) (( ip / (4*nside) ) + (long)nside);// counted from North pole
        }
        else
        {
            // South Polar cap -----------------------------------
            ip = (int)(npix - ipix1 + 1L);
            hip = ip / 2.0;
            fihip = (int) ( hip );
            iring = (int) ( sqrt(hip - sqrt(fihip)) ) + 1;// counted
            // from
            // South
            // pole
            iring = (int)(4*nside - iring);
        }
    }
    return iring;
}

QVector<double> FaceVertices::integration_limits_in_costh(int i_th, int nside)
{
    double a, ab, b, r_n_side;
    long npface = (long)nside * (long)nside;

    // integration limits in cos(theta) for a given ring i_th
    // i > 0 !!!

    r_n_side = 1.0 * (long)nside;
    if ( i_th <= (long)nside )
    {
        ab = 1.0 - ( pow(i_th, 2.0) / 3.0 ) / (double) npface;
        b = 1.0 - ( pow(( i_th - 1 ), 2.0) / 3.0 ) / (double) npface;
        if (i_th == (long)nside)
            a = 2.0 * ( (long)nside - 1.0 ) / 3.0 / r_n_side;
        else
            a = 1.0 - pow(( i_th + 1 ), 2) / 3.0 / (double) npface;
    }
    else
    {
            if ( i_th < 3*nside )
            {
                ab = 2.0 * ( 2 * (long)nside - i_th ) / 3.0 / r_n_side;
                b = 2.0 * ( 2 * (long)nside - i_th + 1 ) / 3.0 / r_n_side;
                a = 2.0 * ( 2 * (long)nside - i_th - 1 ) / 3.0 / r_n_side;
            }
            else
            {
                if ( i_th == 3*nside )
                    b = 2.0 * ( -(long)nside + 1 ) / 3.0 / r_n_side;
                else
                    b = -1.0 + pow(( 4 * nside - i_th + 1 ), 2) / 3.0 / (double) npface;

                a = -1.0 + pow(( 4*(long)nside - i_th - 1 ), 2) / 3.0 / (double) npface;
                ab = -1.0 + pow(( 4*nside - i_th ), 2) / 3.0 / (double) npface;
            }

    }
    // END integration limits in cos(theta)

    QVector<double> result;
    result.resize(3);

    result[0] = b;
    result[1] = ab;
    result[2] = a;

    return result;
}

QVector<double> FaceVertices::pixel_boundaries(double i_th, double i_phi, int i_zone, double cos_theta, int nside)
{
    double sq3th, factor, jd, ju, ku, kd, phi_l, phi_r;
    double r_n_side = 1.0 * nside;
    int npface = (long)nside * (long)nside;

    // HALF a pixel away from both poles
    if ( abs(cos_theta) >= 1.0 - 1.0 / 3.0 / (double) npface )
    {
        phi_l = i_zone * M_PI_2;
        phi_r = ( i_zone + 1 ) * M_PI_2;
        QVector<double> ret;
        ret.resize(2);
        ret[0] = phi_l;
        ret[1] = phi_r;
        return ret;
    }
    // -------
    // NORTH POLAR CAP
    if ( 1.50 * cos_theta >= 1.0 )
    {
        sq3th = sqrt(3.0 * ( 1.0 - cos_theta ));
        factor = 1.0 / r_n_side / sq3th;
        jd = (double) ( i_phi );
        ju = jd - 1;
        ku = (double) ( i_th - i_phi );
        kd = ku + 1;

        phi_l = M_PI_2 * ( std::max(( ju * factor ), ( 1.0 - ( kd * factor ) )) + i_zone );
        phi_r = M_PI_2 * ( std::min(( 1.0 - ( ku * factor ) ), ( jd * factor )) + i_zone );

    }
    else
    {
        if ( -1.0 < 1.50 * cos_theta )
        {
            // -------
            // -------
            // EQUATORIAL ZONE
            double cth34 = 0.50 * ( 1.0 - 1.50 * cos_theta );
            double cth34_1 = cth34 + 1.0;
            int modfactor = (int) ( nside + ( fmod(i_th,2) ) );

            jd = i_phi - ( modfactor - i_th ) / 2.0;
            ju = jd - 1;
            ku = ( modfactor + i_th ) / 2.0 - i_phi;
            kd = ku + 1;

            phi_l = M_PI_2 * ( std::max(( cth34_1 - ( kd / r_n_side ) ),
                                            ( -cth34 + ( ju / r_n_side ) )) + i_zone );

            phi_r = M_PI_2 * ( std::min(( cth34_1 - ( ku / r_n_side ) ),
                                            ( -cth34 + ( jd / r_n_side ) )) + i_zone );
            // -------
            // -------
            // SOUTH POLAR CAP
        }
        else
        {
            sq3th = sqrt(3.0 * ( 1.0 + cos_theta ));
            factor = 1.0 / r_n_side / sq3th;
            long ns2 = 2 * nside;

            jd = i_th - ns2 + i_phi;
            ju = jd - 1;
            ku = ns2 - i_phi;
            kd = ku + 1;

            phi_l = M_PI_2 * ( std::max(( 1.0 - ( ns2 - ju ) * factor ),
                                            ( ( ns2 - kd ) * factor )) + i_zone );

            phi_r = M_PI_2 * ( std::min(( 1.0 - ( ns2 - jd ) * factor ),
                                            ( ( ns2 - ku ) * factor )) + i_zone );
        }// of SOUTH POLAR CAP
    }
    // and that's it
    // System.out.println(" nside:"+nside+" i_th:"+i_th+" i_phi:"+i_phi+"
    // izone:"+i_zone+" cos_theta:"+cos_theta+" phi_l:"+phi_l+"
    // phi_r:"+phi_r);

    QVector<double> ret;
    ret.resize(2);
    ret[0] = phi_l;
    ret[1] = phi_r;
    return ret;
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
