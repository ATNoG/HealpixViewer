#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QWidget>
#include <QGridLayout>
#include <QDebug>
#include <QList>
#include <QString>
#include <QStringList>
#include "mapviewport.h"

#define MAX_VIEWPORTS 9


class WorkSpace : public QWidget
{
    Q_OBJECT
public:
    explicit WorkSpace(int numberViewports=1, QWidget *parent = 0);
    void openFiles(QStringList filenames);

signals:
    void syncNeeded(QEvent* e, int type, MapViewer* source);
    void mapOpened(int viewportId, QString title, mapInfo *info);
    void mapClosed(int viewportId);
    void mapFieldChanged(int viewportId, float* values, int nValues);

public slots:
    void configureWorkspace(int numberViewports);
    void changeToMollview();
    void changeTo3D();
    void changeSynchronization(bool on);
    void syncViewports(QEvent* e, int type, MapViewer* viewer);
    void selectAllViewports();
    void deselectAllViewports();
    void resetViewports();

    void showPolarizationVectors(bool show);
    void showGrid(bool show);
    void updateMapField(int viewportId, HealpixMap::MapType field);
    void updateThreshold(QList<int> viewports, float min, float max);
// TODO: updateColorTable

private:
    int numberViewports;
    int usedViewports;
    QGridLayout *gridlayout;
    QList<MapViewport*> viewports;

    void drawViewports(int oldnviewports, int newnviewports);

    bool synchronize;
};

#endif // WORKSPACE_H
