#include "maploader.h"

MapLoader::MapLoader(QWidget *parent, QString filename, QList<HealpixMap::MapType> _availableMaps) :
    QDialog(parent)
{
    availableMaps = _availableMaps;

    // TODO: dont allow window to get closed....
    setWindowModality(Qt::WindowModal);

    /* create the layout */
    vboxlayout = new QVBoxLayout;

    /* create title label */
    lbltitle = new QLabel();
    lbltitle->setText("Opening " + filename + "\nSelect the wanted map");

    /* create combobox with available maps */
    comboBox = new QComboBox();
    for(int i=0; i<availableMaps.size(); i++)
    {
        comboBox->addItem(HealpixMap::mapTypeToString(availableMaps[i]), i);
    }

    /* create button */
    openButton = new QPushButton("Open");

    /* add widget to layout */
    vboxlayout->addWidget(lbltitle);
    vboxlayout->addWidget(comboBox);
    vboxlayout->addWidget(openButton, 0, Qt::AlignRight);
    setLayout(vboxlayout);

    /* connect button */
    connect(openButton, SIGNAL(clicked()), this, SLOT(accept()));
}

MapLoader::~MapLoader()
{
    #if DEBUG > 0
        qDebug() << "Calling MapLoader destructor";
    #endif
}

HealpixMap::MapType MapLoader::getSelectedMapType()
{
    return availableMaps[comboBox->currentIndex()];
}
