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
#include <QDialog>
#include <math.h>
#include "healpixmap.h"
#include "tesselation.h"
#include "facevertices.h"
#include "maploader.h"
#include "configs.h"

#include <fitsio2.h>
#include <healpix_base.h>
#include <healpix_map.h>
#include <healpix_map_fitsio.h>
#include <fitshandle.h>
#include <cxxutils.h>



using namespace qglviewer;

enum SelectionType { SINGLE_POINT, DISC, TRIANGLE, POLYGON };

struct mapOptions
{
    int tesselationNside;
    int textureNside;
    int pVectorsNside;
    int pVectorsThreshold;
    float minPolThreshold;
    float maxPolThreshold;
    float originalMinPolThreshold;
    float originalMaxPolThreshold;
    float magnification;
    int minNside, maxNside;
    bool controlPolarization;
    int vectorsSpacing;
};


struct mapInfo
{
    float* values;
    int nvalues;
    QList<HealpixMap::MapType> availableFields;
    HealpixMap::MapType currentField;
    int minNside;
    int maxNside;
    float min;
    float max;
    double minMag;
    double maxMag;
    bool hasPolarization;
    ColorMap *colorMap;
    QColor sentinelColor;
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
    bool zoomIn();
    bool zoomOut();
    void resetView();

    void showPolarizationVectors(bool show=true);
    void showGrid(bool show=true);

    void updateThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor);
    void changeMapField(HealpixMap::MapType field);
    void applyOptions(mapOptions *options);

    /* sync */
    void updateCameraPosition(float pos, bool signal=false, bool update=true);
    void updatePosition(Vec position);
    void updateRotation(Quaternion rotation);
    void updateKeyPress(QKeyEvent *e);

    //void checkForUpdates();

    mapInfo* getMapInfo();

signals:
    void signalZoomChanged(float camPosition, MapViewer *viewer);
    void signalRotationChanged(Quaternion rotation, MapViewer *viewer);
    void signalPositionChanged(Vec position, MapViewer *viewer);
    void signalKeyPressed(QKeyEvent* e, MapViewer* viewer);

    void mapFieldChanged(mapInfo *info);
    void textureNsideUpdated(int nside);
    void vectorsNsideUpdated(int nside);


protected:
    virtual void init(void);
    virtual void draw(void);
    //virtual void mousePressEvent(QMouseEvent* e);
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
    int tesselationNside;
    int currentVectorsNside;
    float cameraPosition;
    bool initialized;
    bool displayInfo;
    int maxNside;
    HealpixMap *healpixMap;
    HealpixMap::MapType mapType;
    bool mollweide;
    bool showPVectors;
    bool automaticTextureNside;
    bool automaticPVectorsNside;
    int pvectorsNsideFactor;

    float maxCameraX;
    float minCameraX;

    QPoint prevPoint;

    CameraConstraint *constraint;

    Camera* predictCamera;
    ManipulatedFrame* currentManipulatedFrame;

    bool faceIsInside(float ax, float ay, float bx, float by, float width, float height);
    void preloadFaces();
    void checkNside();
    void updateNside(int nside, bool signal=false);
    void updateVectorsNside(int nside, bool signal=false);
    void computeMaxCameraDistance();

    void checkVisibility();
    void sceneUpdated(bool update=true);
    void changeProjectionConstraints();
    void updateMouseSensitivity();
    void addPixelsToSelection(std::set<int>);
    void removePixelsFromSelection(std::set<int>);
    void changeSelectionType(SelectionType);
    int calculatePixelIndex(const QPoint &point);

    void exportSelectedArea();

    SelectionType selectionType;
    int firstPix, secondPix;
    std::vector<int> polygonPixels;
    bool unselectSelection;

    QMap<int, QVector<Vec> > faceBoundaries;
    QVector<int> visibleFaces;

    FaceCache* faceCache;
    TextureCache* textureCache;
    OverlayCache* overlayCache;

    PolarizationVectors* polVectors;

    std::set<int> selectedPixels;
};

#endif // MAPVIEWER_H
