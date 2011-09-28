#include "polarizationvectors.h"

PolarizationVectors::PolarizationVectors(int faceNumber, int nside, HealpixMap* map)
{
    //qDebug() << "Const. PolVects with nside " << nside;
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->map = map;

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
    vectors = map->getPolarizationVectors(faceNumber, nside, totalVectors);
    // qDebug() << "Building pol vectors (" << nside << ") total=" << totalVectors;

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

        qDebug() << "Delete Polarization vector " << faceNumber << "(" << nside << ") allocated at " << vectors;
        delete[] vectors;
        vectors = NULL;
    }
    else
    {
        qDebug("Values not loaded yet");
        // TODO: throw exception
        build();
    }
}
