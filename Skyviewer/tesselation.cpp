#include "tesselation.h"

Tesselation::Tesselation(int _nside, bool _mollview)
{
    nside = _nside;
    mollview = _mollview;

    faceCache = FaceCache::instance();

    createInitialTesselation();
}


void Tesselation::createInitialTesselation()
{
    /* initial faces */
    QVector<int> initialFaces;
    initialFaces.append(0);
    initialFaces.append(3);
    initialFaces.append(4);
    initialFaces.append(5);
    initialFaces.append(7);
    initialFaces.append(8);
    initialFaces.append(11);
    updateVisibleFaces(initialFaces);
}


/* Update nside used in tesselation */
void Tesselation::updateNside(int newNside)
{
    if(nside!=newNside)
    {
        this->nside = newNside;
    }
    qDebug("nside updated");
}

/* get visible faces */
void Tesselation::updateVisibleFaces(QVector<int> faces)
{
    //QVector<int>::iterator it;

    // TODO: dont delete faces that already exists

    //visibleFaces.clear();

    //qDebug("========================");

    this->facesv = faces;

    /*

    for(it=faces.begin(); it!=faces.end(); it++)
    {
        int faceNumber = *it;
        Face* face = faceCache->getFace(faceNumber, nside);

        //if(faceNumber==0 || faceNumber==4 || faceNumber==3)
            visibleFaces.append(face);

        //qDebug() << "Face " << faceNumber << " visible";
    }

    */
}


void Tesselation::predictNeededFaces()
{
    QVector<faceInfo*> facesToLoad;

    faceInfo fi;
    fi.faceNumber = 4;
    fi.nside = 2048;

    facesToLoad.append(&fi);

    for(int i=0; i<facesToLoad.size(); i++)
    {
        QFuture<void> future = QtConcurrent::run(faceCache, &FaceCache::preloadFace, facesToLoad[i]->faceNumber, facesToLoad[i]->nside);
    }
}


void Tesselation::getFace(int faceNumber)
{
    Face* face = faceCache->getFace(faceNumber, nside);
    visibleFaces.append(face);
}

/* Draw opengl primitives */
void Tesselation::draw()
{
    //QVector<QFuture<void> *> threads;
    QVector<QFuture<Face*> > threads;

    int i;
    int totalFaces = facesv.size();
    Face* face;

    /* get faces */
    for(i=0; i<totalFaces; i++)
    {
        //QFuture<void> future = QtConcurrent::run(this, &Tesselation::getFace, *it);
        //qDebug() << "Tesselation: getting face " << facesv[i];
        QFuture<Face*> future = QtConcurrent::run(faceCache, &FaceCache::getFace, facesv[i], nside);
        threads.append(future);
    }

    /* wait for all threads to finish */
    //qDebug("wait for finish");

    for(i=0; i<totalFaces; i++)
    {
        threads[i].waitForFinished();
    }

    //qDebug("threads finished");


    /* draw visible faces */
    for(i=0; i<totalFaces; i++)
    {
        //visibleFaces[i]->draw();
        face = threads[i].result();
        face->draw();
    }


    /* predict next faces */
    // predictNeededFaces();
}


void Tesselation::drawAllFaces()
{
    //for(int i=0; i<12; i++)
    //{
        //faces[i].draw();
/*
        if(facesv.contains(i))
            glColor4f(0.0, 1.0, 1.0, 0.3);
        else
            glColor3f(1.0, 0.0, 0.0);
*/

     //   faces[i].draw();
    //}

    /*
    glColor3f(1.0, 0.0, 0.0);
    faces[0].draw();
    glColor3f(0.0, 1.0, 0.0);
    faces[1].draw();
    glColor3f(0.0, 0.0, 1.0);
    faces[2].draw();
    glColor3f(1.0, 1.0, 0.0);
    faces[3].draw();
    glColor3f(0.0, 1.0, 1.0);
    faces[4].draw();
    glColor3f(1.0, 0.0, 1.0);
    faces[5].draw();
    glColor3f(0.5, 1.0, 0.0);
    faces[6].draw();
    glColor3f(0.0, 1.0, 0.5);
    faces[7].draw();
    glColor3f(0.5, 1.0, 0.5);
    faces[8].draw();
    glColor3f(0.5, 0.0, 1.0);
    faces[9].draw();
    glColor3f(1.0, 0.0, 0.5);
    faces[10].draw();
    glColor3f(0.5, 0.5, 1.0);
    faces[11].draw();
    */
}
