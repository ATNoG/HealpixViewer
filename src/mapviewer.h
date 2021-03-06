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
#include "fieldmap.h"
#include "coordinatesystem.h"

#include <fitsio2.h>
#include <healpix_base.h>
#include <healpix_map.h>
#include <healpix_map_fitsio.h>
#include <fitshandle.h>
#include <cxxutils.h>
#include "types.h"


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

struct gridOptions
{
    int meridians;
    int parallels;
    QColor color;
    bool labeling;
    int labelSize;
    bool automatic;
};

struct mapInfo
{
    hv::unique_ptr<float[0]> values;
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
    ScaleType scale;
    float factor;
    float offset;
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
    bool loadMap(QString fitsfile, HealpixMap::MapType type);
    bool zoomIn();
    bool zoomOut();
    void resetView();

    void showPolarizationVectors(bool show=true);
    void showGrid(bool show=true);

    void updateThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset);
    void changeMapField(HealpixMap::MapType field);
    void applyOptions(mapOptions *options);
    void applyGridOptions(gridOptions* options);
    void applyCoordSys(HealpixMap::Coordsys coordIn, HealpixMap::Coordsys coordOut);

    /* sync */
    void updateCameraPosition(float pos, bool signal=false, bool update=true);
    void updatePosition(Vec position);
    void updateRotation(Quaternion rotation);
    bool updateKeyPress(QKeyEvent *e);
    void updateSelection(std::set<int> pixels, int nside, bool add);

    //void checkForUpdates();

    mapInfo* getMapInfo();

signals:
    void signalZoomChanged(float camPosition, MapViewer *viewer);
    void signalRotationChanged(Quaternion rotation, MapViewer *viewer);
    void signalPositionChanged(Vec position, MapViewer *viewer);
    void signalKeyPressed(QKeyEvent* e, MapViewer* viewer);
    void signalSelectionChanged(std::set<int> pixels, int nside, bool add);

    void mapFieldChanged(mapInfo *info);
    void textureNsideUpdated(int nside);
    void vectorsNsideUpdated(int nside);


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
    bool automaticGraticule;

    float maxCameraX;
    float minCameraX;

    QPoint prevPoint;

    CameraConstraint *constraint;

    hv::unique_ptr<Camera> predictCamera;
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
    void addPixelsToSelection(std::set<int>, int nside);
    void removePixelsFromSelection(std::set<int>);
    void changeSelectionType(SelectionType);
    int calculatePixelIndex(const QPoint &point);
    int transformSameNside(std::vector<int>& pixels, std::vector<int> nsides);
    void updateGraticule();

    void exportSelectedArea();
    void exportSelectedAreaAsMask();

    //void exportSnapshot();
    void drawColorBar();

    int xRot, yRot;
    QPoint firstPos;
    void updateMollweideRotation();

    SelectionType selectionType;

    std::vector<int> auxiliarPixels;
    std::vector<int> auxiliarNsides;
    bool unselectSelection;

    QMap<int, QVector<Vec> > faceBoundaries;
    QVector<int> visibleFaces;

    FaceCache* faceCache;
    TextureCache* textureCache;
    OverlayCache* overlayCache;
    TextureCache* textureCacheOverlay;

    PolarizationVectors* polVectors;

    std::set<int> selectedPixels;
    QString filename;

    Grid* grid;

    Frame* coordSysFrame;
    Quaternion originalRotation;

    float overlayAlpha;
    ColorMap *colorMap;

    GLuint textureId;
};

#endif // MAPVIEWER_H
