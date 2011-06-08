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
    void syncNeeded(QEvent* e, int type);

public slots:
    void configureWorkspace(int numberViewports);
    void changeToMollview();
    void changeTo3D();
    void changeSynchronization(bool on);
    void syncViewports(QEvent* e, int type);

private:
    int numberViewports;
    QGridLayout *gridlayout;
    QList<MapViewport*> viewports;

    void drawViewports(int oldnviewports, int newnviewports);

    bool synchronize;
};

#endif // WORKSPACE_H
