#ifndef MAPVIEWER_H
#define MAPVIEWER_H

#include <QGLViewer/qglviewer.h>
#include <QMouseEvent>
#include <QDebug>
#include <QMap>
#include <QVector>
#include <QSet>
#include <QtConcurrentRun>
#include <QGLBuffer>
#include <QFileDialog>
#include <math.h>
#include "healpixmap.h"
#include "tesselation.h"
#include "facevertices.h"
#include "maploader.h"

#define MIN_NSIDE 64
#define MAX_NSIDE 2048
#define EXP_NSIDE 6
#define MIN_ZOOM 0
#define MAX_ZOOM 10
#define ZOOM_LEVEL 2
#define INVISIBLE_X 0.37//0.25
#define CAMERA_ZOOM_INC 0.16
#define PRELOAD_FACES false
#define INITIAL_CAMERA_POSITION 2.8

#define DISPLAY_TEXTURE true

#define BUFFER_OFFSET(i) ((GLbyte *)NULL + (i))


using namespace qglviewer;


struct mapInfo
{
    float* values;
    int nvalues;
    QList<HealpixMap::MapType> availableFields;
    HealpixMap::MapType currentField;
    float min;
    float max;
};

class MapViewport;

class MapViewer : public QGLViewer
{
    Q_OBJECT

public:
    explicit MapViewer(QWidget *parent = 0);
    void changeToMollview();
    void changeTo3D();
    //void loadMap(HealpixMap* map);
    bool loadMap(QString fitsfile);
    void synchronize(QEvent* e, int type);
    bool zoomIn();
    bool zoomOut();
    void resetView();

    void showPolarizationVectors(bool show=true);

    void updateThreshold(float min, float max);
    void changeMapField(HealpixMap::MapType field);

    //void checkForUpdates();

    mapInfo* getMapInfo();

signals:
    void cameraChanged(QEvent* e, int type, MapViewer* viewer);
    void mapFieldChanged(float* values, int nValues);

protected:
    virtual void init(void);
    virtual void draw(void);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void wheelEvent(QWheelEvent* e);
    virtual void postSelection(const QPoint &point);


public slots:
    void checkForUpdates(bool cleanCache);
    //void checkForUpdates(void);

private:
    HealpixMap* skymap;
    Tesselation* tesselation;
    int currentNside;
    int currentZoomLevel;
    float cameraPosition;
    bool initialized;
    int maxNside;
    HealpixMap *healpixMap;
    HealpixMap::MapType mapType;

    QProgressDialog *progressDialog;

    QGLBuffer* buffer;
    GLfloat* vertexs;

    Camera* predictCamera;
    ManipulatedFrame* currentManipulatedFrame;

    int zoomToNside(int zoomLevel);
    bool faceIsInside(float ax, float ay, float bx, float by, float width, float height);
    void preloadFaces();

    void mousePressEvent(QMouseEvent* e, bool propagate);
    void mouseReleaseEvent(QMouseEvent* e, bool propagate);
    void mouseMoveEvent(QMouseEvent* e, bool propagate);
    void wheelEvent(QWheelEvent* e, bool propagate);

    enum MouseEvent {MOUSEPRESS, MOUSEMOVE, MOUSEWHEEL, MOUSERELEASE};

    void checkVisibility();

    QMap<int, QVector<Vec> > faceBoundaries;
    QVector<int> visibleFaces;

    FaceCache* faceCache;
    TextureCache* textureCache;
    OverlayCache* overlayCache;

    PolarizationVectors* polVectors;

};

#endif // MAPVIEWER_H
