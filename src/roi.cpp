#include "roi.h"

ROI::ROI(std::set<int> pixelIndexes, int nside)
{
    this->pixelIndexes = pixelIndexes;
    this->nside = nside;
    this->totalPoints = pixelIndexes.size();

    vertexBuffer = NULL;
    bufferInitialized = false;
}

ROI::~ROI()
{
    if(vertexBuffer!=NULL)
    {
        vertexBuffer->release();
        vertexBuffer->destroy();
        delete vertexBuffer;
    }
}

/* create vertex list */
void ROI::createVertexs()
{
    int i=0;
    vertexs = new GLfloat[3*4*totalPoints];

    FaceVertices* factory = FaceVertices::instance();

    std::set<int>::iterator pixelIT;
    for(pixelIT=pixelIndexes.begin(); pixelIT!=pixelIndexes.end(); pixelIT++)
    {
        QVector<Vertice> pixelVertices = factory->getPixelVertices(*pixelIT, nside);

        QVector<Vertice>::iterator verticesIT;
        for(verticesIT=pixelVertices.begin(); verticesIT!=pixelVertices.end(); verticesIT++)
        {
            vertexs[i] = verticesIT->x;
            vertexs[i+1] = verticesIT->y;
            vertexs[i+2] = verticesIT->z;
            i+=3;
        }

        vertexsCalculated = true;
    }

    pixelIndexes.clear();
}

void ROI::draw()
{
    if(GPU_BUFFER)
        if(!bufferInitialized)
            createBuffer();

    glColor3f(1.0, 0.0, 0.0);

    if(GPU_BUFFER)
    {
        glEnableClientState(GL_VERTEX_ARRAY);

        glColor3f(1.0, 0.0, 0.0);

        vertexBuffer->bind();
        glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));

        glDrawArrays(GL_QUADS, 0, 4*totalPoints);

        vertexBuffer->release();

        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        // use normal draw - very slow!!
        std::set<int>::iterator pixelIT;
        FaceVertices* factory = FaceVertices::instance();

        for(pixelIT=pixelIndexes.begin(); pixelIT!=pixelIndexes.end(); pixelIT++)
        {
            QVector<Vertice> pixelVertices = factory->getPixelVertices(*pixelIT, nside);

            QVector<Vertice>::iterator verticesIT;

            glBegin(GL_QUADS);
            for(verticesIT=pixelVertices.begin(); verticesIT!=pixelVertices.end(); verticesIT++)
            {
                verticesIT->draw();
            }
            glEnd();
        }
    }

    glColor3f(1.0, 1.0, 1.0);
}

/* create opengl buffers */
void ROI::createBuffer()
{
    createVertexs();

    vertexBuffer = new QGLBuffer(QGLBuffer::VertexBuffer);
    vertexBuffer->create();

    vertexBuffer->bind();
    vertexBuffer->setUsagePattern(QGLBuffer::StaticDraw);
    vertexBuffer->allocate(vertexs, 4*3*totalPoints*sizeof(GLfloat));
    vertexBuffer->release();

    delete[] vertexs;

    bufferInitialized = true;
}
