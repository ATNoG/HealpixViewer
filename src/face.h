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
    void createBuffer(bool rotationUpdated=false);
    void draw();
    int getFaceNumber();

    void setMollweideRotation(int xRot, int yRot);

private:
    int faceNumber;
    int nside;
    bool mollweide;
    int totalVertices;
    QVector<Strip> strips;
    bool bufferInitialized;
    bool vertexsCalculated;

    double xRot, yRot;

    void toMollweide(double rad);
    double toMollweide(const double phi, const double lambda, double &x, double &y);
    void toMollweideBackfaceSplit();

    inline void multiplyMatrices(double a[3][3], double b[3][3], double c[3][3], int m, int n, int p)
    {
        int i,j,k;
        double sum=0;
        for(i=0; i<m; i++)
        {
            for(j=0; j<p; j++)
            {
                for (k=0;k<n;k++)
                    sum=sum+(a[i][k]*b[k][j]);
                c[i][j]=sum;
                sum=0;
            }
        }
    }

    GLfloat* vertexs;
    GLfloat* textureCoords;
    QGLBuffer* vertexBuffer;
    QGLBuffer* textureBuffer;

    QList<int> numberVertices;
};

#endif // FACE_H
