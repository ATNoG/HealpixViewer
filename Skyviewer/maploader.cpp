#include "maploader.h"

MapLoader::MapLoader(QWidget *parent, QString filename, QList<HealpixMap::MapType> _availableMaps) :
    QDialog(parent)
{
    availableMaps = _availableMaps;

    // TODO: dont allow window to get closed....
    setWindowModality(Qt::WindowModal);

    /* create the layout */
    QVBoxLayout* vboxlayout = new QVBoxLayout;

    /* create title label */
    QLabel* lbltitle = new QLabel();
    lbltitle->setText("Opening " + filename + "\nSelect the wanted map");

    /* create combobox with available maps */
    comboBox = new QComboBox();
    for(int i=0; i<availableMaps.size(); i++)
    {
        comboBox->addItem(HealpixMap::mapTypeToString(availableMaps[i]), i);
    }

    /* create button */
    QPushButton* openButton = new QPushButton("Open");

    /* add widget to layout */
    vboxlayout->addWidget(lbltitle);
    vboxlayout->addWidget(comboBox);
    vboxlayout->addWidget(openButton, 0, Qt::AlignRight);
    setLayout(vboxlayout);

    /* connect button */
    connect(openButton, SIGNAL(clicked()), this, SLOT(accept()));

}

HealpixMap::MapType MapLoader::getSelectedMapType()
{
    return availableMaps[comboBox->currentIndex()];
}
