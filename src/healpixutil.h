#ifndef HEALPIXUTIL_H
#define HEALPIXUTIL_H

#include <vector>
#include <map>
#include <set>
#include <chealpix.h>
#include <cmath>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct Range{
    long first;
    long last;
};

inline long xy2pix(long ix, long iy)
{
        static int x2pix[128];
        static int y2pix[128];
        static bool xy2pix_called = false;
        if( ! xy2pix_called ) {
                mk_xy2pix(x2pix, y2pix);
        xy2pix_called = true;
        }
        //int ix_low = (int)fmod(ix,128);
        int ix_low = ix % 128;
        int ix_hi  = ix / 128;
        int iy_low = iy % 128;
        int iy_hi  = iy / 128;
        return (x2pix[ix_hi]+y2pix[iy_hi]) * (128 * 128)
                        + (x2pix[ix_low]+y2pix[iy_low]);
}

inline long ringNum(int nside, double z)
{
    long iring = 0;

    iring = (long) round(nside * (2.0 - 1.5 * z));

    /* north cap */
    if ( z > (2./3.) )
    {
        iring = (long) round(nside * sqrt(3.0 * ( 1.0 - z )));
        if ( iring == 0 )
                iring = 1;
    }
    /* south cap */
    if ( z < (-2./3.) )
    {
        iring = (long) round(nside * sqrt(3.0 * ( 1.0 + z )));
        if ( iring == 0 )
                iring = 1;
        iring = 4 * nside - iring;
    }
    return iring;
}

inline double MODULO(double a, double b)
{
    double res = 0.;
    long k = 0;
    if (a > 0.) {
            if (b > 0.) {
                    k = (long) (a / b);
                    res = a - k * b;
                    return res;
            }
            if (b < 0.) {
                    k = (long) rint(a / b);
                    res = a - k * b;
                    return res;
            }
    }
    if (a <= 0.) {
            if (b <= 0.) {
                    k = (long) (a / b);
                    res = a - k * b;
                    return res;
            }
            if (b > 0.) {
                    k = (long) rint(a / b);
                    res = a - k * b;
                    return res;
            }
    }
    return res;
}

/**
 * computes the intersection di of 2 intervals d1 (= [a1,b1]) and d2 (=
 * [a2,b2]) on the periodic domain (=[A,B] where A and B arbitrary) ni is
 * the resulting number of intervals (0,1, or 2) if a1 <b1 then d1 = {x |a1 <=
 * x <= b1} if a1>b1 then d1 = {x | a1 <=x <= B U A <=x <=b1}
 *
 * @param d1
 *            double[] first interval
 * @param d2
 *            double[] second interval
 * @return double[] one or two intervals intersections
 */
inline void intrs_intrv(double di[], double d1[], double d2[], int &elems)
{
        double epsilon = 1.0e-10;
        double dk[] = { -1.0e9, -1.0e9, -1.0e9, -1.0e9 };
        int ik = 0;
        bool tr12, tr21, tr34, tr43, tr13, tr31, tr24, tr42, tr14, tr32;
        /*                                             */

        //qDebug() << "d1= " << d1[0] << "," << d1[1] << "   d2= " << d2[0] << "," << d2[1];

        tr12 = ( d1[0] < d1[1] + epsilon );
        tr21 = !tr12; // d1[0] >= d1[1]
        tr34 = ( d2[0] < d2[1] + epsilon );
        tr43 = !tr34; // d2[0]>d2[1]
        tr13 = ( d1[0] < d2[0] + epsilon ); // d2[0] can be in interval
        tr31 = !tr13; // d1[0] in longerval
        tr24 = ( d1[1] < d2[1] + epsilon ); // d1[1] upper limit
        tr42 = !tr24; // d2[1] upper limit
        tr14 = ( d1[0] < d2[1] + epsilon ); // d1[0] in interval
        tr32 = ( d2[0] < d1[1] + epsilon ); // d2[0] in interval

        ik = 0;
        /* d1[0] lower limit case 1 */
        if ( ( tr34 && tr31 && tr14 ) || ( tr43 && ( tr31 || tr14 ) ) ) {
                ik++; // ik = 1;
                dk[ik - 1] = d1[0]; // a1

        }
        /* d2[0] lower limit case 1 */
        if ( ( tr12 && tr13 && tr32 ) || ( tr21 && ( tr13 || tr32 ) ) ) {
                ik++; // ik = 1
                dk[ik - 1] = d2[0]; // a2

        }
        /* d1[1] upper limit case 2 */
        if ( ( tr34 && tr32 && tr24 ) || ( tr43 && ( tr32 || tr24 ) ) ) {
                ik++; // ik = 2
                dk[ik - 1] = d1[1]; // b1

        }
        /* d2[1] upper limit case 2 */
        if ( ( tr12 && tr14 && tr42 ) || ( tr21 && ( tr14 || tr42 ) ) ) {
                ik++; // ik = 2
                dk[ik - 1] = d2[1]; // b2

        }
        //di = new double[1];
        di[0] = 0.;
        switch ( ik ) {

                case 2:
                        elems = 2;

                        di[0] = dk[0] - epsilon;
                        di[1] = dk[1] + epsilon;
                        break;
                case 4:
                        elems = 4;
                        di[0] = dk[0] - epsilon;
                        di[1] = dk[3] + epsilon;
                        di[2] = dk[1] - epsilon;
                        di[3] = dk[2] + epsilon;
                        break;
        }
}


inline double round(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}


/*
inline std::set<int> inRingNested(long iz, double phi0, double dphi, int nside, bool conservative=false)
{
    std::set<int> aux = inRing(iz, phi0, dphiring, nside);
    std::set<int>::iterator it;

    std::set<int> result;
    for(it=aux.begin(); it!=aux.end(); it++)
    {
        //long nestIpix;
        //ring2nest(nside, *it, &nestIpix);

        for(long ipix= )
        result.insert(ring2nest(ipix));
    }

    aux.clear();
    return result;
}
*/


/**
 * returns the list of pixels in RING scheme with latitude in [phi0 -
 * dpi, phi0 + dphi] on the ring iz in [1, 4*nside -1 ] The pixel id numbers
 * are in [0, 12*nside^2 - 1] the indexing is in RING, unless nest is set to
 * 1
 * NOTE: this is the f90 code 'in_ring' method ported to java with 'conservative' flag to false
 *
 * @param nside
 *            long the map resolution
 * @param iz
 *            long ring number
 * @param phi0
 *            double
 * @param dphi
 *            double
 * @param res result
 */
inline std::set<int> inRing(long iz, double phi0, double dphi, int nside, bool conservative=false)
{
    std::set<int> res;

    double nl3 = 3*nside;
    double nl4 = 4*nside;
    long npix = nside2npix(nside);
    long ncap = 2 * (long)nside * ( (long)nside - 1 );

    bool take_all = false;
    bool to_top = false;

    double epsilon = 1e-12;
    // java calculation jitter
    double shift = 0.;
    long ir = 0;
    long kshift, nr, ipix1, ipix2;//nir1, nir2,
    long ip_low = 0, ip_hi = 0; //,in, nir;

//		long inext;
    double phi_low = MODULO(phi0 - dphi, (2*M_PI)) - epsilon; // phi min,															  // excluding
                                                                                                                          // 2pi period
    // excluding
    double phi_hi = phi0 + dphi + epsilon;

    // this was being moduloed but why ?? around the 2pi that casues a problem
    double phi_hi_mod = MODULO(phi0 + dphi, (2*M_PI)) + epsilon;

//
    if (abs(dphi - M_PI) < epsilon)  take_all = true;
    // what happens when phi_hi wraps round ??

    /* identifies ring number */
    if ((iz >= nside) && (iz <= nl3)) { // equatorial region
        ir = iz - nside + 1; // in [1, 2*nside + 1]
        ipix1 = ncap + nl4 * (ir - 1); // lowest pixel number in the
                                                                             // ring
        ipix2 = ipix1 + nl4 - 1; // highest pixel number in the ring
        kshift = (long) MODULO(ir, 2.);

        nr = nl4;
    }
    else {
        if (iz < nside) { // north pole
                ir = iz;
                ipix1 = 2 * ir * (ir - 1); // lowest pixel number
                ipix2 = ipix1 + (4 * ir) - 1; // highest pixel number
        } else { // south pole
                ir = 4 * nside - iz;

                ipix1 = npix - 2 * ir * (ir + 1); // lowest pixel number
                ipix2 = ipix1 + 4 * ir - 1;       // highest pixel number
        }
        nr = ir * 4;
        kshift = 1;
    }

    // Construct the pixel list
    if (take_all) {
        /*
        Range r1;
        r1.first = ipix1;
        r1.last = ipix2;
        res.insert(r1);
        */
        for(int i=ipix1; i<=ipix2; i++)
            res.insert(i);
            //res.appendRange(ipix1,ipix2);
        return res;
    }

    shift = kshift / 2.0;

    // conservative : include every intersected pixel, even if the
    // pixel center is out of the [phi_low, phi_hi] region
    if (conservative) {
            ip_low = (long) round((nr * phi_low) / (2*M_PI) - shift);
            ip_hi = (long) round((nr * phi_hi) / (2*M_PI) - shift);

            ip_low = (long) MODULO(ip_low, nr); // in [0, nr - 1]
            if (ip_hi > nr) { // ifit is =nr then this sets it to zero - not good
                    ip_hi = (long) MODULO(ip_hi, nr); // in [0, nr - 1]
            }
//			System.out.println("ip_low="+ip_low+" ip_hi="+ip_hi);
    }
    else { // strict: includes only pixels whose center is in
        //                                                    [phi_low,phi_hi]

        ip_low = (long) ceil(((double)nr * phi_low) / (2*M_PI) - shift);
        ip_hi = (long)(((double)nr * phi_hi_mod) / (2*M_PI) - shift);
        if (ip_hi < ip_low && iz==1){//this is not good - problem on pole with direction.
           ip_hi = (long)(((double)nr * phi_hi) / (2*M_PI) - shift);
        }
        if (ip_low == ip_hi + 1)
            ip_low = ip_hi;

        if ((ip_low - ip_hi == 1) && (dphi * (double)nr < M_PI))
        {
            // the interval is too small ( and away from pixel center)
            // so no pixels is included in the list

            qDebug("the interval is too small and avay from center");

            return res; // return empty list
        }

        ip_low = min(ip_low, nr - 1);
        ip_hi = max(ip_hi, (long)0);
    }

    //
    if (ip_low > ip_hi)
        to_top = true;

    if (to_top)
    {
        ip_low += ipix1;
        ip_hi += ipix1;

        /*
        Range r2, r3;
        r2.first = ipix1;
        r2.last = ip_hi;
        res.insert(r2);
        */

        for(int i=ipix1; i<=ip_hi; i++)
            res.insert(i);

        /*
        r3.first = ip_low;
        r3.last = ipix2;
        res.insert(r3);
        */

        for(int i=ip_low; i<=ipix2; i++)
            res.insert(i);

        //res.appendRange(ipix1,ip_hi);
        //res.appendRange(ip_low,ipix2);
    }
    else
    {
        if (ip_low < 0 )
        {
            ip_low = abs(ip_low);

            /*
            Range r4, r5;
            r4.first = ipix1;
            r4.last = ipix1+ip_hi;
            res.insert(r4);
            */

            for(int i=ipix1; i<=(ipix1+ip_hi); i++)
                res.insert(i);

            /*
            r5.first = ipix2-ip_low+1;
            r5.last = ipix2;
            res.insert(r5);
            */

            for(int i=(ipix2-ip_low+1); i<=ipix2; i++)
                res.insert(i);

            //res.appendRange(ipix1, ipix1+ip_hi);
            //res.appendRange(ipix2-ip_low +1, ipix2);

            return res;
        }
        ip_low += ipix1;
        ip_hi += ipix1;

        /*
        Range r6;
        r6.first = ip_low;
        r6.last = ip_hi;
        res.insert(r6);
        */

        for(int i=ip_low; i<=ip_hi; i++)
            res.insert(i);

        //res.appendRange(ip_low,ip_hi);
    }

    return res;
}


inline double asinh(double value)
{
    double returned;

    if(value>0)
       returned = log(value + sqrt(value * value + 1));
    else
       returned = -log(-value + sqrt(value * value + 1));

    return(returned);
}


inline double toMollweide(double phi, double lambda, double &x, double &y)
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
           // qDebug() << "Upper limit";
                theta = M_PI/2;
        }
/*
                                Lower limit
*/
        else if (fabs(x + M_PI/2) < 1e-6 ) {
            //qDebug() << "Lower limit";
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

#endif // HEALPIXUTIL_H
