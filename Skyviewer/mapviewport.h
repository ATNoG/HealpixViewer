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


/* forward declaration of WorkSpace class */
class WorkSpace;

class MapViewport : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewport(QWidget *parent, QString title, WorkSpace* _workspace);

    bool isSelected();
    void changeToMollview();
    void changeTo3D();
    void openMap(QString fitsfile);
    void closeMap();

signals:

public slots:
    void selectionChanged(bool selected);
    void synchronizeView(QEvent* event, int type, MapViewer* source);

private:
    QString title;
    QHBoxLayout *titlebarlayout;
    QVBoxLayout *vboxlayout;
    QWidget *titlewidget, *areawidget;
    QToolBar *toolbar;
    QCheckBox* checkbox;
    MapViewer* mapviewer;
    WorkSpace* workspace;

    bool selected;
    bool mollview;

    void selectViewport();
    void deselectViewport();
    void loadHealpixMap(QString path);
    void configureUI();
};

#endif // WORKSPACE_H
