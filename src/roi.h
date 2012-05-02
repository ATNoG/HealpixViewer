#ifndef ROI_H
#define ROI_H

#include <QSet>
#include <QVector>
#include <QDebug>
#include <QGLBuffer>
#include "vertice.h"
#include "facevertices.h"
#include "configs.h"
#include <set>

using namespace std;

class ROI
{
    public:
        ROI(std::set<int> pixelIndexes, int nside);
        ~ROI();
        void draw();
        void createBuffer();

    private:
        std::set<int> pixelIndexes;
        void createVertexs();
        int nside;
        int totalPoints;

        GLfloat* vertexs;
        QGLBuffer* vertexBuffer;

        bool bufferInitialized;
        bool vertexsCalculated;
};

#endif // ROI_H
