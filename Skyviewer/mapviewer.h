#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QGLViewer/qglviewer.h>
#include <QMouseEvent>
#include <QDebug>
#include <QMap>
#include <QVector>
#include <math.h>
#include "healpixmap.h"
#include "tesselation.h"
#include "facevertices.h"

#define BASE_NSIDE 64
#define EXP_NSIDE 6
#define MIN_ZOOM 0
#define MAX_ZOOM 10
#define ZOOM_LEVEL 2
#define INVISIBLE_X 0.25
#define CAMERA_ZOOM_INC 0.16


using namespace qglviewer;


class MapViewer : public QGLViewer
{
    Q_OBJECT

public:
    explicit MapViewer(QWidget *parent = 0);
    void changeToMollview();
    void changeTo3D();
    void loadMap(HealpixMap* map);
    void synchronize(QEvent* e, int type);
    bool zoomIn();
    bool zoomOut();
    void resetView();

signals:
    void cameraChanged(QEvent* e, int type, MapViewer* viewer);

protected:
    virtual void init(void);
    virtual void draw(void);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void wheelEvent(QWheelEvent* e);

public slots:

private:
    HealpixMap* skymap;
    Tesselation* tesselation;
    int currentNside;
    int currentZoomLevel;
    float cameraPosition;

    int zoomToNside(int zoomLevel);
    void resetZoom();

    enum MouseEvent {MOUSEPRESS, MOUSEMOVE, MOUSEWHEEL, MOUSERELEASE};

    void checkVisibility();

    QMap<int, QVector<Vec> > faceBoundaries;
    QVector<int> visibleFaces;

};

#endif // MAPVIEWER_H
