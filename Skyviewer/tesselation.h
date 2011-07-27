#ifndef TESSELATION_H
#define TESSELATION_H

#define DISPLAY_TEXTURE false

#include <QVector>
#include <QFuture>
#include <QtConcurrentRun>
#include <chealpix.h>
#include <math.h>
#include <set>
#include "face.h"
#include "facecache.h"
#include "texture.h"

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

    /* preload faces */
    void preloadFaces(QVector<int> faces, int nside);

private:
    int nside;
    //QMap<int, Face*> availableFaces;

    void createInitialTesselation();
    //void getFace(int faceNumber);

    bool mollview;

    FaceCache* faceCache;
    QVector<int> facesv;
    QVector<Face*> visibleFaces;
};

#endif // TESSELATION_H
