#include "coordinatesystem.h"
#include "ui_coordinatesystem.h"

#include <QDebug>

CoordinateSystem::CoordinateSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoordinateSystem)
{
    ui->setupUi(this);

    disableControllers();
    fillAvailableCoordSystems();

    connect(ui->applyButton, SIGNAL(released()), this, SLOT(applyOptions()));
}

CoordinateSystem::~CoordinateSystem()
{
    delete ui;
}

void CoordinateSystem::fillAvailableCoordSystems()
{
    QVector<HealpixMap::Coordsys> availableCoordSys;
    availableCoordSys.append(HealpixMap::GALACTIC);
    availableCoordSys.append(HealpixMap::ECLIPTIC);
    availableCoordSys.append(HealpixMap::CELESTIAL);

    ui->srcCoordSysCB->blockSignals(true);
    ui->dstCoordSysCB->blockSignals(true);

    for(int i=0; i<availableCoordSys.size(); i++)
    {
        ui->srcCoordSysCB->insertItem(i, HealpixMap::coordsysToString(availableCoordSys[i]), availableCoordSys[i]);
        ui->dstCoordSysCB->insertItem(i, HealpixMap::coordsysToString(availableCoordSys[i]), availableCoordSys[i]);
    }

    ui->srcCoordSysCB->blockSignals(false);
    ui->dstCoordSysCB->blockSignals(false);
}

void CoordinateSystem::updateSelectedMaps(QList<int> selectedViewports)
{
    // TODO: load coordsys info about those maps

    if(selectedViewports.size()>0)
        enableControllers();
    else
        disableControllers();
}

void CoordinateSystem::enableControllers()
{
    ui->srcCoordSysCB->setEnabled(true);
    ui->dstCoordSysCB->setEnabled(true);
    ui->applyButton->setEnabled(true);
}

void CoordinateSystem::disableControllers()
{
    ui->srcCoordSysCB->setEnabled(false);
    ui->dstCoordSysCB->setEnabled(false);
    ui->applyButton->setEnabled(false);
}


void CoordinateSystem::applyOptions()
{
    HealpixMap::Coordsys coordIn, coordOut;

    QVariant aux;

    aux = ui->srcCoordSysCB->itemData(ui->srcCoordSysCB->currentIndex());
    coordIn = getCoordSys(aux.toInt());

    aux = ui->dstCoordSysCB->itemData(ui->dstCoordSysCB->currentIndex());
    coordOut = getCoordSys(aux.toInt());

    emit(coordSysChanged(coordIn, coordOut));
}


/*** CoordinateSystemMatrices ***/

CoordinateSystemMatrices* CoordinateSystemMatrices::s_instance = 0;

CoordinateSystemMatrices::CoordinateSystemMatrices()
{
    /* ecliptic to equatorial */
    double eps = 23.452294 - 0.0130125 - 1.63889E-6 + 5.02778E-7;
    eps = eps * 3.141592653589793 / 180.;

    e2g[0][0] = -0.054882486;
    e2g[0][1] = -0.993821033;
    e2g[0][2] = -0.096476249;
    e2g[1][0] = 0.494116468;
    e2g[1][1] = -0.110993846;
    e2g[1][2] = 0.862281440;
    e2g[2][0] = -0.867661702;
    e2g[2][1] = -0.000346354;
    e2g[2][2] = 0.497154957;

    e2q[0][0] = 1.;
    e2q[0][1] = 0.;
    e2q[0][2] = 0.;
    e2q[1][0] = 0.;
    e2q[1][1] = cos(eps);
    e2q[1][2] = -1. * sin(eps);
    e2q[2][0] = 0.;
    e2q[2][1] = sin(eps);
    e2q[2][2] = cos(eps);

    norot[0][0] = 0.;
    norot[0][1] = 0.;
    norot[0][2] = 0.;
    norot[1][0] = 0.;
    norot[1][1] = 0.;
    norot[1][2] = 0.;
    norot[2][0] = 0.;
    norot[2][1] = 0.;
    norot[2][2] = 0.;

    matrixInverse(e2g, g2e, 3);
    multiplyMatrices<double>(g2e, e2q, g2q, 3, 3, 3);
    matrixInverse(e2q, q2e, 3);
    multiplyMatrices<double>(q2e, e2g, q2g, 3, 3, 3);
}
