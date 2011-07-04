#include "tesselation.h"

Tesselation::Tesselation(int _nside, bool _mollview)
{
    nside = _nside;
    mollview = _mollview;

    faceCache = FaceCache::instance();

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

    //createTesselation();
}


void Tesselation::createTesselation()
{
    QVector<double> costhetas_np;
    QVector<double> costhetas_eq;
    QVector<double> costhetas_sp;





    /* teste */
    /*
    Face* face;
    qDebug() << "Asking for face 0";
    face = faceCache->getFace(0, 64);
    qDebug() << "Asking for face 1";
    face = faceCache->getFace(1, 64);
    qDebug() << "Asking for face 2";
    face = faceCache->getFace(2, 64);
    qDebug() << "Asking for face 3";
    face = faceCache->getFace(3, 64);
    qDebug() << "Asking for face 4";
    face = faceCache->getFace(4, 64);
    qDebug() << "Asking for face 5";
    face = faceCache->getFace(5, 64);
    */

    /* get ring latitudes */
    //getRingLatitudes(nside, &costhetas_np, &costhetas_eq, &costhetas_sp);

    /* create faces */
    /*
    for(unsigned int i=0; i<12; i++)
    {
        //TODO: delete this cicle
        Face f(i);
        f.setRigging(nside, false);
        faces.append(f);
    }
    */

    /* TODO: input of setRigging ok */

    /* set face rigging */
    /*
    unsigned int i;
    for(i=0; i<4; i++)
        faces[i].setRigging(nside, mollview);
    for(; i<8; i++)
        faces[i].setRigging(nside, mollview);
    for(; i<12; i++)
        faces[i].setRigging(nside, mollview);
        */
}


/* Update nside used in tesselation */
void Tesselation::updateNside(int newNside)
{
    if(nside!=newNside)
    {
        this->nside = newNside;
        createTesselation();
    }
    qDebug("nside updated");
}

/* get visible faces */
void Tesselation::updateVisibleFaces(QVector<int> faces)
{
    QVector<int>::iterator it;

    // TODO: dont delete faces that already exists

    visibleFaces.clear();

    //qDebug("========================");

    facesv = faces;

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

/* Draw opengl primitives */
void Tesselation::draw()
{
    /* draw each face */


    //qDebug("===============");
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
    QVector<Face*>::iterator it;
    for(it=visibleFaces.begin(); it!=visibleFaces.end(); it++)
    {
        (*it)->draw();
    }
    */

    QVector<int>::iterator it;

    for(it=facesv.begin(); it!=facesv.end(); it++)
    {
        int faceNumber = *it;
        Face* face = faceCache->getFace(faceNumber, nside);

        //if(faceNumber==0 || faceNumber==4 || faceNumber==3)
            //visibleFaces.append(face);

        //qDebug() << "Face " << faceNumber << " visible";

        face->draw();
    }


    // TODO: draw just visible faces
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



/* Draw opengl primitives */
void Tesselation::draw(set<int> faceNumbers)
{
    /*
    set<int>::iterator it;
    for(it=faceNumbers.begin(); it!=faceNumbers.end(); it++)
    {
        int faceNumber = *it;
        facesv[faceNumber].draw();
    }
    */
}
