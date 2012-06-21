#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include <QWidget>
#include "healpixmap.h"
#include "matrix.h"

namespace Ui {
class CoordinateSystem;
}

class CoordinateSystem : public QWidget
{
    Q_OBJECT
    
public:
    explicit CoordinateSystem(QWidget *parent = 0);
    ~CoordinateSystem();

signals:
    void coordSysChanged(HealpixMap::Coordsys in, HealpixMap::Coordsys out);

private slots:
    void applyOptions();

public slots:
    void updateSelectedMaps(QList<int> selectedViewports);
    
private:
    Ui::CoordinateSystem *ui;

    void fillAvailableCoordSystems();
    void enableControllers();
    void disableControllers();
};


class CoordinateSystemMatrices
{

public:
    CoordinateSystemMatrices();
    ~CoordinateSystemMatrices();

    static CoordinateSystemMatrices *instance()
    {
        if(!s_instance)
            s_instance = new CoordinateSystemMatrices();
        return s_instance;
    }

    /* ecliptic to galactic */
    double e2g[3][3];
    /* ecliptic to equatorial */
    double e2q[3][3];
    /* galactic to ecliptic */
    double g2e[3][3];
    /* galactic to equatorial */
    double g2q[3][3];
    /* equatorial to ecliptic */
    double q2e[3][3];
    /* equatorial to galactic */
    double q2g[3][3];
    /* no rotation */
    double norot[3][3];

private:
    static CoordinateSystemMatrices *s_instance;

};

#endif // COORDINATESYSTEM_H
