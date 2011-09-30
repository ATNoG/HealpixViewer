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



/* forward declaration of WorkSpace class */
class WorkSpace;

class MapViewport : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewport(QWidget *parent, QString title, int viewportId, const QGLWidget* shareWidget=NULL);
    ~MapViewport();

    bool isSelected();
    bool openMap(QString fitsfile);
    void selectViewport(bool changeCheckbox);
    void deselectViewport(bool changeCheckbox);

    void updateThreshold(ColorMap* colorMap, float min, float max);
    void setWorkspace(WorkSpace *workspace);

    mapInfo* getMapInfo();

    void disconnectFromWorkspace();


signals:
    void mapFieldInfoChanged(int viewportId, mapInfo *info);
    void maximized(int viewportId);
    void restored();
    void closed(int viewportId);
    void selectionChanged(int viewportId, bool selected);

public slots:
    void updateSelection(bool selected);
    void synchronizeView(QEvent* event, int type, MapViewer* source);
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
    QMap<HealpixMap::MapType, QAction*> mapFieldsActions;
    QSignalMapper* signalMapper;
    QAction *actionMaximize, *actionRestore, *actionSync, *actionGrid, *actionPvectors, *action3D, *actionMollweide;
    QActionGroup* fieldsActionGroup;
    mapInfo* info;
    const QGLWidget* shareWidget;
    bool hasPolarization;

    void fillMapField();
    void updateMapField(HealpixMap::MapType field);

    bool loaded;
    bool selected;
    bool mollview;

    int viewportId;

    void configureUI();
};

#endif // WORKSPACE_H
