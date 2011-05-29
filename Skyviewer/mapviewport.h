#ifndef MAPVIEWPORT_H
#define MAPVIEWPORT_H

#include <QWidget>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QGLWidget>
#include <QLabel>
#include <QCheckBox>
#include <QDebug>

#define COLOR_SELECTED "#fa8252"
#define COLOR_INACTIVE "#c3c3c3"


class MapViewport : public QWidget
{
    Q_OBJECT
public:
    explicit MapViewport(QWidget *parent, QString title);

    bool isSelected();


signals:

public slots:
    void selectionChanged(bool selected);

private:
    QString title;
    QHBoxLayout *hboxtoplayout;
    QHBoxLayout *hboxbottomlayout;
    QVBoxLayout *vboxlayout;
    QLayout *verticallayout;
    QWidget *titlewidget, *areawidget;
    QToolBar *toolbar;
    bool selected;

    void selectViewport();
    void deselectViewport();
    void loadHealpixMap(QString path);
};

#endif // WORKSPACE_H
