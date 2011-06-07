#ifndef FACE_H
#define FACE_H

#include <QVector>
#include <QDebug>
#include "vertice.h"
#include "boundary.h"

using namespace std;

typedef QVector<Vertice> Strip;

class Face
{
public:
    Face();
    Face(int faceNumber);
    void setNumber(int faceNumber);
    void setRigging(int nside, QVector<double> &costhetas, double rad = 1.);
    void draw();

private:
    int faceNumber;
    QVector<Strip> strips;

    void setRigging_NP(int nside, QVector<double> &costhetas, double rad = 1.);
    void setRigging_EQ(int nside, QVector<double> &costhetas, double rad = 1.);
    void setRigging_SP(int nside, QVector<double> &costhetas, double rad = 1.);
};

#endif // FACE_H
