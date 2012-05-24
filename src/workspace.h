#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QWidget>
#include <QDebug>
#include <QMap>
#include "mapviewport.h"
#include "configs.h"


struct viewportPosition
{
    int row;
    int col;
    int cspan;
};


class WorkSpace : public QWidget
{
    Q_OBJECT
public:
    explicit WorkSpace(QWidget *parent = 0);
    ~WorkSpace();

    int getProjectionToSyncTo();
    bool hasViewports();


signals:
    void signalSyncZoom(float camPosition, MapViewer *viewer);
    void signalSyncPosition(Vec position, MapViewer *viewer);
    void signalSyncRotation(Quaternion rotation, MapViewer *viewer);
    void signalSyncKeyPress(QKeyEvent *e, MapViewer *viewer);

    void textureNsideUpdated(int nside, int viewport);
    void vectorsNsideUpdated(int nside, int viewport);
    void workspaceUpdated();

public slots:
    void changeToMollview();
    void changeTo3D();
    void changeSynchronization(bool on);
    void enableSynchronization();
    void disableSynchronization();

    void selectAllViewports();
    void deselectAllViewports();
    void resetViewports();

    void showPolarizationVectors(bool show);
    void showGrid(bool show);

    void addViewport(int viewportId, MapViewport *viewport);
    void removeViewport(int viewportId);

    void maximize(int viewportId);
    void restore();

    void multipleSelection();

    /* synchronization */
    void syncZoom(float camPosition, MapViewer *viewer);
    void syncPosition(Vec position, MapViewer *viewer);
    void syncRotation(Quaternion rotation, MapViewer *viewer);
    void syncKeyPress(QKeyEvent *e, MapViewer *viewer);
    void syncPixelSelection(std::set<int> pixels, int nside, bool add);

private:
    int numberViewports;
    QGridLayout *layout;
    QMap<int, MapViewport*> viewports;
    QMap<int, viewportPosition*> viewportPositions;
    bool synchronize;

    //void addWidgetToLayout(QWidget* widget);
    //void removeWidgetFromLayout(QWidget* widget);

    void reorganizeLayout();

    void calculateViewportPositions(int totalViewports);

    int mmc(int a, int b);
};

#endif // WORKSPACE_H
