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
    ~FaceVertices();

    static FaceVertices *instance()
    {
        //if(!s_instance)
        //    s_instance = new FaceVertices;
        //return s_instance;
        return new FaceVertices;
    }

    QVector<Strip> getFaceVertices(int face, int nside, double radius=1.);

    QVector<Vertice> getPixelVertices(int pixelIndex, int nside, double radius=1.);

private:
    QVector<Strip> getRigging_EQ(int faceNumber, int nside, QVector<double> *costhetas, double rad);
    QVector<Strip> getRigging_NP(int faceNumber, int nside, QVector<double> *costhetas, double rad);
    QVector<Strip> getRigging_SP(int faceNumber, int nside, QVector<double> *costhetas, double rad);
    void getRingLatitudes(int nside, QVector<double> *costhetas_np, QVector<double> *costhetas_eq, QVector<double> *costhetas_sp);

    int ring(int pixIndex, int nside);
    QVector<double> integration_limits_in_costh(int i_th, int nside);
    QVector<double> pixel_boundaries(double i_th, double i_phi, int i_zone, double cos_theta, int nside);

    static FaceVertices *s_instance;
};


#endif // FACEVERTICES_H
