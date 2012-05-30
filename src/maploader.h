#ifndef MAPLOADER_H
#define MAPLOADER_H

#include <QObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QSet>
#include <QPushButton>
#include <QVector>
#include "healpixmap.h"

class MapLoader : public QDialog
{
    Q_OBJECT

public:
    explicit MapLoader(QWidget *parent, QString filename, QList<HealpixMap::MapType> availableMaps);
    ~MapLoader();
    HealpixMap::MapType getSelectedMapType();
    QSet<HealpixMap::MapType> getSelectedMapTypes();

private:
    QList<HealpixMap::MapType> availableMaps;
    QGroupBox *groupBox;
    QVBoxLayout* vboxlayout;
    QLabel* lbltitle;
    QPushButton* openButton;
    QVector<QCheckBox*> checkboxs;
};

#endif // MAPLOADER_H
