#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QWidget>
#include <QDebug>
#include <QMap>
#include "mapviewport.h"

#define MAX_WORKSPACE_VIEWPORTS 16


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

signals:
    void syncNeeded(QEvent* e, int type, MapViewer* source);

public slots:
    void changeToMollview();
    void changeTo3D();
    void changeSynchronization(bool on);
    void syncViewports(QEvent* e, int type, MapViewer* viewer);
    void selectAllViewports();
    void deselectAllViewports();
    void resetViewports();

    void showPolarizationVectors(bool show);
    void showGrid(bool show);

    void addViewport(int viewportId, MapViewport *viewport);
    void removeViewport(int viewportId);

    void maximize(int viewportId);
    void restore();


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
