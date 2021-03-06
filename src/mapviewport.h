#ifndef MAPVIEWPORT_H
#define MAPVIEWPORT_H

#include <QWidget>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QLabel>
#include <QCheckBox>
#include <QDebug>
#include "mapviewer.h"
#include "histogramwidget.h"
#include "configs.h"
#include "types.h"



/* forward declaration of WorkSpace class */
class WorkSpace;

class MapViewport : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewport(QWidget *parent, QString title, int viewportId, const QGLWidget* shareWidget=NULL);
    ~MapViewport();

    bool isSelected();
    bool openMap(QString fitsfile, HealpixMap::MapType type);
    void selectViewport(bool changeCheckbox);
    void deselectViewport(bool changeCheckbox);
    bool isSynchronized();
    bool isMollweide();

    void updateThreshold(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset);
    void setWorkspace(WorkSpace *workspace);

    mapInfo* getMapInfo();

    void disconnectFromWorkspace();

    void applyOptions(mapOptions *options);
    void applyGridOptions(gridOptions *options);
    void applyCoordSys(HealpixMap::Coordsys coordIn, HealpixMap::Coordsys coordOut);
    void syncPixelSelection(std::set<int> pixels, int nside, bool add);

    bool isSelectionSource();
    bool isSelectionDestination();


signals:
    void mapFieldInfoChanged(int viewportId, mapInfo *info);
    void maximized(int viewportId);
    void restored();
    void closed(int viewportId);
    void selectionChanged(int viewportId, bool selected);
    void textureNsideUpdated(int nside, int viewportId);
    void vectorsNsideUpdated(int nside, int viewportId);
    void gridDisplayChanged(bool show);
    void signalChange3D();
    void signalChangeMollweide();
    void pvectorsDisplayChanged(bool show);

public slots:
    void updateSelection(bool selected);
    void viewportUpdated(mapInfo *info);

    void showGrid(bool show);
    void showPolarizationVectors(bool show);
    void resetViewport();
    void changeMapField(HealpixMap::MapType field);
    void changeMapField(int);
    void enableSynchronization(bool on);

    void changeToMollview();
    void changeTo3D();

private slots:
    void maximize();
    void restore();
    void close();
    void updateOptionTextureNside(int nside);
    void updateOptionVectorsNside(int nside);

    void updateCameraPosition(float position, MapViewer *viewer);
    void updatePosition(Vec position, MapViewer *viewer);
    void updateRotation(Quaternion rotation, MapViewer *viewer);
    void updateKeyPress(QKeyEvent *e, MapViewer *viewer);
    void pixelSelectionChanged(std::set<int> pixels, int nside, bool add);

    void activeSelectionAsSource();
    void activeSelectionAsDestination();


private:
    QString title;
    QHBoxLayout *titlebarlayout;
    QVBoxLayout *vboxlayout;
    QWidget *titlewidget, *areawidget;
    QToolBar *toolbar;
    QCheckBox* checkbox;
    MapViewer* mapviewer;
    WorkSpace* workspace;
    QMenu* fieldMenu;
    QMenu* actAsMenu;
    QMap<HealpixMap::MapType, QAction*> mapFieldsActions;
    QSignalMapper* signalMapper;
    QAction *actionMaximize, *actionRestore, *actionSync, *actionGrid, *actionPvectors, *action3D, *actionMollweide;
    QActionGroup* fieldsActionGroup;
    QActionGroup* actAsActionGroup;
    mapInfo* info;
    const QGLWidget* shareWidget;
    bool hasPolarization;
    bool synchronized;

    void fillMapField();
    void fillActAsMenu();
    void updateMapField(HealpixMap::MapType field);

    bool loaded;
    bool selected;
    bool mollview;

    bool actAsSource, actAsDestination;

    int viewportId;

    void configureUI();
};

#endif // WORKSPACE_H
