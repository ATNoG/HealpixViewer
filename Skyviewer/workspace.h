#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QWidget>
#include <QGridLayout>
#include <QDebug>
#include <QList>
#include <QStringList>
#include "mapviewport.h"


class WorkSpace : public QWidget
{
    Q_OBJECT
public:
    explicit WorkSpace(QWidget *parent = 0, int numberViewports=1);
    void openFiles(QStringList filenames);

signals:
    void synchronizeMouseMove(QMouseEvent* e);
    void synchronizeMousePress(QMouseEvent* e);

public slots:
    void configureWorkspace(int numberViewports);
    void changeToMollview();
    void changeTo3D();
    void syncViewportsMouseMove(QMouseEvent* e);
    void syncViewportsMousePress(QMouseEvent* e);

private:
    int numberViewports;
    QGridLayout *gridlayout;
    QList<MapViewport*> viewports;

    void drawViewports(int oldnviewports, int newnviewports);
};

#endif // WORKSPACE_H
