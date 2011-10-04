#include "polarizationvectors.h"

PolarizationVectors::PolarizationVectors(int faceNumber, int nside, bool mollweide, HealpixMap* map, double minMag, double maxMag, double magnification, int vectorsSpacing)
{
    //qDebug() << "Const. PolVects with nside " << nside;
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->map = map;
    this->mollweide = mollweide;
    this->minMag = minMag;
    this->maxMag = maxMag;
    this->magnification = magnification;
    this->vectorsSpacing = vectorsSpacing;

    bufferCreated = false;
    vectorsBuffer = NULL;
    vectors = NULL;
}

PolarizationVectors::~ PolarizationVectors()
{
    //qDebug() << "Calling PolarizationVectors destructor";

    if(vectorsBuffer!=NULL)
    {
        vectorsBuffer->release();
        vectorsBuffer->destroy();
        delete vectorsBuffer;
    }
    if(vectors!=NULL)
        delete[] vectors;
}

void PolarizationVectors::draw()
{
    /* check if buffer was already constructed */
    if(!bufferCreated)
        createBuffer();

    glEnableClientState(GL_VERTEX_ARRAY);

    vectorsBuffer->bind();
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

    glColor3f(1.0, 1.0, 1.0);
    glDrawArrays(GL_LINES, 0, 2*totalVectors);

    vectorsBuffer->release();

    glDisableClientState(GL_VERTEX_ARRAY);
}

void PolarizationVectors::build()
{
    vectors = map->getPolarizationVectors(faceNumber, nside, minMag, maxMag, magnification, vectorsSpacing, totalVectors);

    if(mollweide)
    {
        int pointer = 0;
        int aux = totalVectors;
        totalVectors = 0;
        /* convert to mollweide projection */
        for(int i=0; i<aux*3*2; i+=6)
        {
            double l, ax, ay, az, bx, by, bz;
            convertToMollweide((double)vectors[i], (double)vectors[i+1], (double)vectors[i+2], ax, ay, az);
            convertToMollweide((double)vectors[i+3], (double)vectors[i+4], (double)vectors[i+5], bx, by, bz);
            l = sqrt(((ax-bx) * (ax-bx)) + ((ay-by) * (ay-by)) + ((az-bz) * (az-bz)));

            if (l < 2.2)
            {
                vectors[pointer] = ax;
                vectors[pointer+1] = ay;
                vectors[pointer+2] = az;
                vectors[pointer+3] = bx;
                vectors[pointer+4] = by;
                vectors[pointer+5] = bz;
                pointer+=6;
                totalVectors++;
            }
        }
    }

    valuesLoaded = true;
}

void PolarizationVectors::createBuffer()
{
    /* if values already loaded, create the buffer */
    if(valuesLoaded)
    {
        /* create the buffer */
        vectorsBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
        vectorsBuffer->create();
        /* allocate values on buffer */
        vectorsBuffer->bind();
        vectorsBuffer->setUsagePattern(QGLBuffer::StaticDraw);
        vectorsBuffer->allocate(vectors, 3*2*totalVectors*sizeof(GLfloat));
        vectorsBuffer->release();

        bufferCreated = true;

        delete[] vectors;
        vectors = NULL;
    }
    else
    {
        //qDebug("Values not loaded yet");
        // TODO: throw exception
        build();
    }
}


void PolarizationVectors::convertToMollweide(double ax, double ay, double az, double &rx, double &ry, double &rz)
{
    double phi, lambda, x, y;
    double r = 1/sqrt(2.);
    phi = asin(az);
    lambda = atan2(ay, ax);
    toMollweide(phi, lambda, x, y);
    if( (faceNumber == 2 || faceNumber == 10) && x > 0) x *= -1;
    ry = r*x;
    rz = r*y;
    rx = 0.0001;
}


double PolarizationVectors::toMollweide(const double phi, const double lambda, double &x, double &y)
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
