#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QGLViewer/qglviewer.h>
#include <QMouseEvent>
#include <QDebug>
#include "healpixmap.h"
#include "tesselation.h"

class MapViewer : public QGLViewer
{
    Q_OBJECT

public:
    explicit MapViewer(QWidget *parent = 0);
    void changeToMollview();
    void changeTo3D();
    void loadMap(HealpixMap* map);
    void synchronize(QMouseEvent* e, int type);

signals:
    void synchronizeMouseMove(QMouseEvent* e);
    void synchronizeMousePress(QMouseEvent* e);

protected:
    virtual void init(void);
    virtual void draw(void);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    //virtual void animate (void);
    //virtual void postSelection (const QPoint &pt);

signals:

public slots:

private:
    HealpixMap* skymap;
    Tesselation* tesselation;
};

#endif // MAPVIEWER_H
