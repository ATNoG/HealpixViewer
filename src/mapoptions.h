#ifndef MAPOPTIONS_H
#define MAPOPTIONS_H

#include <QWidget>
#include <QDebug>
#include "configs.h"
#include "mapviewer.h"


namespace Ui {
    class MapOptions;
}

class MapOptions : public QWidget
{
    Q_OBJECT

public:
    explicit MapOptions(QWidget *parent = 0);
    ~MapOptions();

signals:
    void optionsChanged(mapOptions* options);

public slots:
    void updateSelectedMaps(QList<int> selectedViewports);
    /* update info about opened map */
    void updateMapInfo(int viewportId, mapInfo* info);
    /* unload viewport info */
    void unloadMapInfo(int viewportId);

    void updateTextureNside(int nside, int viewportId);
    void updateVectorsNside(int nside, int viewportId);

private slots:
    void useThreholds(int state);
    void applyOptions();

private:
    Ui::MapOptions *ui;
    QMap<int, mapOptions*> viewportOptions;
    QList<int> selectedViewports;

    void fillTesselationNsideCombobox(int min, int max, int nside);
    void fillTextureNsideCombobox(int min, int max, int nside);
    void fillPVectorsNsideCombobox(int min, int max, int nside);
    void fillPVectorsSpacingCombobox(int min, int max, int value);
    void updateOptions();

    void textureNsideUpdated();
    void vectorsNsideUpdated();
};

#endif // MAPOPTIONS_H
