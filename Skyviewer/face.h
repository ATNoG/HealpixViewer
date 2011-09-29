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
    Face(int faceNumber, int nside, bool mollweide);
    ~Face();
    void setRigging(double rad = 1.);
    void createVertexs();
    void createDisplayList();
    void createBuffer();
    void draw();
    int getFaceNumber();

private:
    int faceNumber;
    int nside;
    bool mollweide;
    int totalVertices;
    QVector<Strip> strips;
    bool bufferInitialized;
    bool vertexsCalculated;

    void toMollweide(double rad);
    double toMollweide(const double phi, const double lambda, double &x, double &y);
    void toMollweideBackfaceSplit();

    GLfloat* vertexs;
    GLfloat* textureCoords;
    QGLBuffer* vertexBuffer;
    QGLBuffer* textureBuffer;

    QList<int> numberVertices;
};

#endif // FACE_H
