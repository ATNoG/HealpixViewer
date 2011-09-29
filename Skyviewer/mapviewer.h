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
#include "configs.h"


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
    explicit MapViewer(QWidget *parent = 0, const QGLWidget* shareWidget=NULL);
    ~MapViewer();
    void changeToMollview();
    void changeTo3D();
    //void loadMap(HealpixMap* map);
    bool loadMap(QString fitsfile);
    void synchronize(QEvent* e, int type);
    bool zoomIn();
    bool zoomOut();
    void resetView();

    void showPolarizationVectors(bool show=true);
    void showGrid(bool show=true);

    void updateThreshold(float min, float max);
    void changeMapField(HealpixMap::MapType field);

    //void checkForUpdates();

    mapInfo* getMapInfo();

signals:
    void cameraChanged(QEvent* e, int type, MapViewer* viewer);
    void mapFieldChanged(mapInfo *info);

protected:
    virtual void init(void);
    virtual void draw(void);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void wheelEvent(QWheelEvent* e);
    virtual void postSelection(const QPoint &point);
    virtual void resizeGL(int width, int height);
    virtual void keyPressEvent(QKeyEvent *);


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
    bool displayCurrentNside;
    int maxNside;
    HealpixMap *healpixMap;
    HealpixMap::MapType mapType;

    QProgressDialog *progressDialog;

    CameraConstraint *constraint;

    Camera* predictCamera;
    ManipulatedFrame* currentManipulatedFrame;

    int zoomToNside(int zoomLevel);
    bool faceIsInside(float ax, float ay, float bx, float by, float width, float height);
    void preloadFaces();
    void checkNside();

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
