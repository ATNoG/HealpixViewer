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
    lbltitle->setText("Opening " + filename);

    /* create combobox with available maps */
    groupBox = new QGroupBox("Select the wanted maps");
    QVBoxLayout *vbox = new QVBoxLayout;

    for(int i=0; i<availableMaps.size(); i++)
    {
        QCheckBox *checkBox = new QCheckBox(availableMaps[i]);
        checkboxs.append(checkBox);
        vbox->addWidget(checkBox);
    }

    groupBox->setLayout(vbox);

    /* create button */
    openButton = new QPushButton("Open");

    /* add widget to layout */
    vboxlayout->addWidget(lbltitle);
    vboxlayout->addWidget(groupBox);
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

QSet<HealpixMap::MapType> MapLoader::getSelectedMapTypes()
{
    QSet<HealpixMap::MapType> mapFields;

    for(int i=0; i<checkboxs.size(); i++)
    {
        if(checkboxs[i]->isChecked())
            mapFields.insert(availableMaps[i]);
    }

    return mapFields;
}
