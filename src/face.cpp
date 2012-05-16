#include "face.h"


Face::Face()
{
    vertexBuffer = NULL;
}


Face::Face(int faceNumber, int nside, bool mollweide)
{
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->mollweide = mollweide;
    vertexsCalculated = false;
    bufferInitialized = false;
    vertexBuffer = NULL;
    textureBuffer = NULL;
}


Face::~Face()
{
    #if DEBUG > 0
        qDebug() << "Calling Face destructor";
    #endif

    //qDebug("Destroying face");
    if(vertexBuffer!=NULL)
    {
        vertexBuffer->release();
        vertexBuffer->destroy();
        delete vertexBuffer;
    }
    if(textureBuffer!=NULL)
    {
        textureBuffer->release();
        textureBuffer->destroy();
        delete textureBuffer;
    }
}


void Face::draw()
{

    if(GPU_BUFFER)
        if(!bufferInitialized)
            createBuffer();

    //QTime time1;
    //time1.start();


    if(!DISPLAY_TEXTURE)
    {
        if(!COLOR_PER_NSIDE)
        {
            switch(faceNumber)
            {
                case 0:
                    glColor3f(1.0, 0.0, 0.0);
                    break;
                case 1:
                    glColor3f(0.0, 1.0, 0.0);
                    break;
                case 2:
                    glColor3f(0.0, 0.0, 1.0);
                    break;
                case 3:
                    glColor3f(1.0, 1.0, 0.0);
                    break;
                case 4:
                    glColor3f(0.0, 1.0, 1.0);
                    break;
                case 5:
                    glColor3f(1.0, 0.0, 1.0);
                    break;
                case 6:
                    glColor3f(0.5, 1.0, 0.0);
                    break;
                case 7:
                    glColor3f(0.0, 1.0, 0.5);
                    break;
                case 8:
                    glColor3f(0.5, 1.0, 0.5);
                    break;
                case 9:
                    glColor3f(0.5, 0.0, 1.0);
                    break;
                case 10:
                    glColor3f(1.0, 0.0, 0.5);
                    break;
                case 11:
                    glColor3f(0.5, 0.5, 1.0);
                    break;
            }
        }
        else
        {
            switch(nside)
            {
                case 64:
                    glColor3f(1.0, 0.0, 0.0);
                    break;
                case 128:
                    glColor3f(0.0, 1.0, 0.0);
                    break;
                case 256:
                    glColor3f(0.0, 0.0, 1.0);
                    break;
                case 512:
                    glColor3f(1.0, 1.0, 0.0);
                    break;
                case 1024:
                    glColor3f(0.0, 1.0, 1.0);
                    break;
                case 2048:
                    glColor3f(1.0, 0.0, 1.0);
                    break;
            }
        }
    }



    //glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_FRONT, GL_LINE);
    //glPolygonMode(GL_BACK, GL_NONE);


    if(GPU_BUFFER)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        vertexBuffer->bind();
        glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

        textureBuffer->bind();

        glClientActiveTexture(GL_TEXTURE0);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

        glClientActiveTexture(GL_TEXTURE1);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

        if(!(mollweide && faceNumber==6))
        {
            for(int i=0; i<nside; i++)
            {
                glDrawArrays(GL_QUAD_STRIP, 2*(nside+1)*i, 2*(nside+1));
            }
        }
        else
        {
            long pos = 0;
            for(int i=0; i<numberVertices.size(); i++)
            {
                glDrawArrays(GL_QUAD_STRIP, pos, numberVertices[i]);
                pos+= numberVertices[i];
            }
        }

        vertexBuffer->release();
        textureBuffer->release();

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    else
    {
        /* use normal draw - very slow!! */
        QVector<Strip>::iterator stripIT;
        QVector<Vertice>::iterator verticesIT;

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

    //int ms = time1.elapsed();
    //qDebug() << "Time drawbuffer (nside " << nside << "): " << ms;
}


int Face::getFaceNumber()
{
    return faceNumber;
}


/* create vertex list */
void Face::createVertexs()
{
    int i=0, j=0;

    vertexs = new GLfloat[3*totalVertices];
    textureCoords = new GLfloat[2*totalVertices];

    if(vertexs==NULL || textureCoords==NULL)
        qDebug("Error allocation memory for vertexs");

    QVector<Strip>::iterator stripIT;
    QVector<Vertice>::iterator verticesIT;

    // draw each vertice of each strip
    for(stripIT=strips.begin(); stripIT!=strips.end(); ++stripIT)
    {
        for(verticesIT=stripIT->begin(); verticesIT!=stripIT->end(); verticesIT++)
        {
            vertexs[i] = verticesIT->x;
            vertexs[i+1] = verticesIT->y;
            vertexs[i+2] = verticesIT->z;
            i+=3;
            /*
            // Flipped: T and S changed!
            textureCoords[j] = verticesIT->t;
            textureCoords[j+1] = verticesIT->s;
            */
            textureCoords[j] = verticesIT->s;
            textureCoords[j+1] = verticesIT->t;
            j+=2;
        }
    }

    // free strips structure
    strips.clear();

    vertexsCalculated = true;
}



/* create opengl buffers */
void Face::createBuffer()
{
    //QTime time1;
    //time1.start();

    createVertexs();

    vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
    textureBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);

    vertexBuffer->create();
    textureBuffer->create();

    vertexBuffer->bind();
    vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
    vertexBuffer->allocate(vertexs, 3*totalVertices*sizeof(GLfloat));
    vertexBuffer->release();

    textureBuffer->bind();
    textureBuffer->setUsagePattern(QGLBuffer::StaticDraw);
    textureBuffer->allocate(textureCoords, 2*totalVertices*sizeof(GLfloat));
    textureBuffer->release();

    delete[] vertexs;
    delete[] textureCoords;

    bufferInitialized = true;

    //int ms = time1.elapsed();
    //qDebug() << "Time createBuffer (nside " << nside << "): " << ms;
}


void Face::setRigging(double radius)
{
    strips = FaceVertices::instance()->getFaceVertices(faceNumber, nside, radius);

    /* 2*(nside+1) per each strip */
    totalVertices = 2*(nside+1)*nside;
    this->nside = nside;

    if(mollweide)
        toMollweide(radius);

    if(faceNumber==6 && mollweide)
    {
        QVector<Strip>::iterator stripIT;
        long tot = 0;
        for(stripIT=strips.begin(); stripIT!=strips.end(); ++stripIT)
        {
            tot+= stripIT->size();
            numberVertices.append(stripIT->size());
        }

        totalVertices = tot;
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
    // copy the current list to temp storage
    QVector<Strip> oldstrips;
    oldstrips.resize(strips.size());
    QVector<Strip>::iterator srcqli = strips.begin();
    QVector<Strip>::iterator destqli = oldstrips.begin();

    for(int i=0; i<strips.size(); i++)
    {
        destqli->resize(srcqli->size());
        QVector<Vertice>::iterator srcpti = srcqli->begin();
        QVector<Vertice>::iterator destpti = destqli->begin();
        for(int j=0; j<srcqli->size(); j++)
            *destpti++ = *srcpti++;
        srcqli++;
        destqli++;
    }

    // Now refill it, making the break along the back as needed
    strips.resize(0);
    uint i = 0;
    for(srcqli = oldstrips.begin(); srcqli != oldstrips.end(); ++srcqli)
    {
        i++;

        bool patch = true;	// need to patch the end of the left and start of the right sides
        bool leftside = true;	// true while building the left side quadlist

        QVector<Vertice> newverts1;
        QVector<Vertice> newverts2;

        int jbreak = srcqli->size(); // vertex number of switch between left and right side
        jbreak = 2*i-2;
        for(int j = 0; j < srcqli->size(); j++ )
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
