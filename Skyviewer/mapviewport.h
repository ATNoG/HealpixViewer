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

#define COLOR_SELECTED "#fa8252"
#define COLOR_INACTIVE "#c3c3c3"
#define DEFAULT_VIEW_3D true


/* forward declaration of WorkSpace class */
class WorkSpace;

class MapViewport : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewport(QWidget *parent, QString title, int viewportId);
    ~MapViewport();

    bool isSelected();
    void changeToMollview();
    void changeTo3D();
    bool openMap(QString fitsfile);
    void selectViewport(bool changeCheckbox);
    void deselectViewport(bool changeCheckbox);

    void updateThreshold(float min, float max);
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
    QAction *actionMaximize, *actionRestore, *actionSync;
    QActionGroup* fieldsActionGroup;
    mapInfo* info;

    void fillMapField();
    void updateMapField(HealpixMap::MapType field);

    bool loaded;
    bool selected;
    bool mollview;

    int viewportId;

    void configureUI();
};

#endif // WORKSPACE_H
