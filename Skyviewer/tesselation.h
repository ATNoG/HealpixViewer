#ifndef TESSELATION_H
#define TESSELATION_H

#include <QVector>
#include <chealpix.h>
#include <math.h>
#include <set>
#include "face.h"
#include "facecache.h"

class Tesselation
{

public:
    Tesselation(int nside, bool mollview);
    void draw();
    void draw(set<int> facesv);

    /* update to new nside */
    void updateNside(int nside);

    /* update viewport info */
    void updateVisibleFaces(QVector<int> facesv);

private:
    int nside;
    //QVector<Face> facesv;
    QMap<int, Face*> availableFaces;


    void createTesselation();
    bool mollview;

    FaceCache* faceCache;
    QVector<Face*> visibleFaces;

    // TODO: delete
    QVector<int> facesv;
};

#endif // TESSELATION_H
