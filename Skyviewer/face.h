#ifndef FACE_H
#define FACE_H

#include <QVector>
#include <QDebug>
#include <QGLBuffer>
#include <chealpix.h>
#include "vertice.h"
#include "facevertices.h"
#include "configs.h"

using namespace std;

class Face
{
public:
    Face();
    Face(int faceNumber);
    ~Face();
    void setRigging(int nside, bool mollview, double rad = 1.);
    void createVertexs();
    void createDisplayList();
    void createBuffer();
    void draw();
    int getFaceNumber();

private:
    int faceNumber;
    int nside;
    int totalVertices;
    QVector<Strip> strips;
    bool bufferInitialized;
    bool vertexsCalculated;


    void toMollweide(double rad);
    double toMollweide(const double phi, const double lambda, double &x, double &y);
    void toMollweideBackfaceSplit();
    void freeVertices();

    GLfloat* vertexs;
    GLfloat* textureCoords;
    QGLBuffer* vertexBuffer;
    QGLBuffer* textureBuffer;
};

#endif // FACE_H
