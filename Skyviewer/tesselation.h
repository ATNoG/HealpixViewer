#ifndef TESSELATION_H
#define TESSELATION_H

#include <QVector>
#include <QFuture>
#include <QtConcurrentRun>
#include <chealpix.h>
#include <math.h>
#include <set>
#include "face.h"
#include "facecache.h"

class Tesselation
{

struct faceInfo
{
    int faceNumber;
    int nside;
};

public:
    Tesselation(int nside, bool mollview);
    void draw();
    void drawAllFaces();

    /* update to new nside */
    void updateNside(int nside);

    /* update viewport info */
    void updateVisibleFaces(QVector<int> facesv);

private:
    int nside;
    //QMap<int, Face*> availableFaces;

    void createInitialTesselation();
    void getFace(int faceNumber);
    void predictNeededFaces();

    bool mollview;

    FaceCache* faceCache;
    QVector<int> facesv;
    QVector<Face*> visibleFaces;
};

#endif // TESSELATION_H
