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
    void synchronize(QEvent* e, int type);

signals:
    void cameraChanged(QEvent* e, int type);
    //void objectMoved(QMouseEvent* e, int type);
    //void objectZoomed(QWheelEvent* e);

protected:
    virtual void init(void);
    virtual void draw(void);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void wheelEvent(QWheelEvent* e);

signals:

public slots:

private:
    HealpixMap* skymap;
    Tesselation* tesselation;

    enum MouseEvent {MOUSEPRESS, MOUSEMOVE, MOUSEWHEEL};
};

#endif // MAPVIEWER_H
