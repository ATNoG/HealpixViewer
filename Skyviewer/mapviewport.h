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
    explicit MapViewport(QWidget *parent, QString title, WorkSpace* _workspace, int viewportId);

    bool isSelected();
    void changeToMollview();
    void changeTo3D();
    bool openMap(QString fitsfile);
    void closeMap();
    void selectViewport(bool changeCheckbox);
    void deselectViewport(bool changeCheckbox);
    void resetViewport();
    bool inUse();
    void showHistogram();
    void showPolarizationVectors(bool show);
    void updateThreshold(float min, float max);
    void changeMapField(HealpixMap::MapType field);

    mapInfo* getMapInfo();


signals:
    void mapFieldChanged(int viewportId, float* values, int nValues);

public slots:
    void selectionChanged(bool selected);
    void synchronizeView(QEvent* event, int type, MapViewer* source);
    void viewportUpdated(float* values, int nValues);

private:
    QString title;
    QHBoxLayout *titlebarlayout;
    QVBoxLayout *vboxlayout;
    QWidget *titlewidget, *areawidget;
    QToolBar *toolbar;
    QCheckBox* checkbox;
    MapViewer* mapviewer;
    WorkSpace* workspace;

    bool loaded;
    bool selected;
    bool mollview;

    int viewportId;

    void configureUI();
};

#endif // WORKSPACE_H
