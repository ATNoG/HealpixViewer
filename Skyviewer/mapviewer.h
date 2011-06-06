#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QGLViewer/qglviewer.h>
#include <QDebug>
#include "healpixmap.h"

class MapViewer : public QGLViewer
{
    Q_OBJECT

public:
    explicit MapViewer(QWidget *parent = 0);
    void changeToMollview();
    void changeTo3D();

protected:
    void paintGL();

signals:

public slots:

private:
    HealpixMap* skymap;

};

#endif // MAPVIEWER_H
