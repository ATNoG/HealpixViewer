#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include "healpixmap.h"

class MapLoader : public QDialog
{
    Q_OBJECT

public:
    explicit MapLoader(QWidget *parent, QString filename, QList<HealpixMap::MapType> availableMaps);
    ~MapLoader();
    HealpixMap::MapType getSelectedMapType();

private:
    QList<HealpixMap::MapType> availableMaps;
    QComboBox* comboBox;
    QVBoxLayout* vboxlayout;
    QLabel* lbltitle;
    QPushButton* openButton;
};

#endif // MAPLOADER_H
