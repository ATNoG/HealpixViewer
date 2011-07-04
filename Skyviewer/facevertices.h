#ifndef FACEVERTICES_H
#define FACEVERTICES_H

#include <QDebug>
#include <QVector>
#include <chealpix.h>
#include "boundary.h"
#include "vertice.h"


typedef QVector<Vertice> Strip;

class FaceVertices
{


public:
    FaceVertices();

    static FaceVertices *instance()
    {
        //if(!s_instance)
        //    s_instance = new FaceVertices;
        //return s_instance;
        return new FaceVertices;
    }

    QVector<Strip> getFaceVertices(int face, int nside, double radius=1.);

private:
    QVector<Strip> getRigging_EQ(int faceNumber, int nside, QVector<double> *costhetas, double rad);
    QVector<Strip> getRigging_NP(int faceNumber, int nside, QVector<double> *costhetas, double rad);
    QVector<Strip> getRigging_SP(int faceNumber, int nside, QVector<double> *costhetas, double rad);
    void getRingLatitudes(int nside, QVector<double> *costhetas_np, QVector<double> *costhetas_eq, QVector<double> *costhetas_sp);

    static FaceVertices *s_instance;
};


#endif // FACEVERTICES_H
