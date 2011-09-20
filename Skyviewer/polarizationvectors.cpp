#include "polarizationvectors.h"

PolarizationVectors::PolarizationVectors(int faceNumber, int nside, HealpixMap* map)
{
    this->faceNumber = faceNumber;
    this->nside = nside;
    this->map = map;

    bufferCreated = false;
    vectorsBuffer = NULL;
}

PolarizationVectors::~ PolarizationVectors()
{
    if(vectorsBuffer!=NULL)
    {
        vectorsBuffer->release();
        vectorsBuffer->destroy();
        delete vectorsBuffer;
    }
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
    /*
    vectors[0] = -0.5;
    vectors[1] = 0;
    vectors[2] = 0;
    vectors[3] = 0.5;
    vectors[4] = 0;
    vectors[5] = 0;
    vectors[6] = 0;
    vectors[7] = 0.7;
    vectors[8] = 0.1;
    vectors[9] = 0.5;
    vectors[10] = -0.2;
    vectors[11] = 0.4;
    totalVectors = 2;*/

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

        delete vectors;
    }
    else
    {
        qDebug("Values not loaded yet");
        // TODO: throw exception
        build();
    }
}
