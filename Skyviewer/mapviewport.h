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

#define COLOR_SELECTED "#fa8252"
#define COLOR_INACTIVE "#c3c3c3"
#define DEFAULT_VIEW_3D true


class MapViewport : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewport(QWidget *parent, QString title);

    bool isSelected();
    void changeToMollview();
    void changeTo3D();
    void openMap(QString fitsfile);
    void closeMap();

signals:

public slots:
    void selectionChanged(bool selected);

private:
    QString title;
    QHBoxLayout *titlebarlayout;
    QVBoxLayout *vboxlayout;
    QWidget *titlewidget, *areawidget;
    QToolBar *toolbar;
    QCheckBox* checkbox;
    MapViewer* mapviewer;

    bool selected;
    bool mollview;

    void selectViewport();
    void deselectViewport();
    void loadHealpixMap(QString path);
    void configureUI();
};

#endif // WORKSPACE_H
