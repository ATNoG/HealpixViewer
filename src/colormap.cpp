#include <QCoreApplication>
#include <iostream>
#include "colormap.h"
#include "exceptions.h"

ColorMapManager* ColorMapManager::s_instance = 0;

ColorMap::ColorMap(int id, QString file)
{
    this->id = id;
    readTable(file);
}

int ColorMap::getId()
{
    return id;
}

QString ColorMap::getName()
{
    return name;
}

void ColorMap::readTable(QString file)
{
    QFile f(file);
    //FILE *f;

    char name[40];
    unsigned char values[768];

    ncolors = 256;
    int totalValues = 3*ncolors;

    f.open(QIODevice::ReadOnly);
    //f = fopen("colormaps/bw_linear.tbl", "r");

    f.read(name, 40);
    f.read((char*)values, totalValues);

    this->name = QString(name);

    //fread(name, 1, 40, f);
    //fread(values, 1, totalValues, f);

    table.resize(ncolors);
    for(uint i=0; i<ncolors; i++)
    {
        table[i].setRgb((int)values[i], (int)values[i+ncolors], (int)values[i+ncolors+ncolors]);
    }

    f.close();
    //fclose(f);
}

QColor ColorMap::operator[](float v) const
{
    v = v < 0 ? 0 : v;
    v = v > 1 ? 1 : v;
    uint idx = (uint)(v*(ncolors-1));
    return table[idx];
}


/***** ColorMap Manager *****/

ColorMapManager::ColorMapManager()
{
    this->basepath = "colormaps/";
    readColorMaps();
}

ColorMapManager::~ColorMapManager()
{
    for(int i=0; i<colorMaps.size(); i++)
        delete colorMaps[i];
}

QList<ColorMap*> ColorMapManager::getColorMaps()
{
    return colorMaps.values();
}

void ColorMapManager::readColorMaps()
{
    ColorMap *cm;

    /* read colormaps directory */
    QDir colorMapsDir(this->basepath);
    if(!colorMapsDir.exists())
    {
        this->basepath = QCoreApplication::applicationDirPath();

        #if defined(__APPLE__)
        this->basepath += "/../Resources/colormaps/";
        #elif defined(_WIN32)
        this->basepath += "/colormaps/";
        #else
        this->basepath += "/../share/healpixviewer/colormaps/";
        #endif

        colorMapsDir = QDir(this->basepath);
        if(!colorMapsDir.exists())
        {
            throw ColormapsNotFoundException("Colormaps directory not found!");
        }
    }

    QStringList mapFiles = colorMapsDir.entryList(QDir::Files);

    if(mapFiles.size() > 0)
    {
        for(int i=0; i<mapFiles.size(); i++)
        {
            qDebug() << "Reading colormap " << mapFiles[i];
            cm = new ColorMap(i, this->basepath+mapFiles[i]);
            colorMaps.insert(i, cm);
        }
    }
    else
    {
        throw ColormapsNotFoundException("No colormaps found!");
    }
}

ColorMap* ColorMapManager::getColorMap(int id)
{
    if(colorMaps.contains(id))
        return colorMaps.value(id);
    else
        return NULL;
}

ColorMap* ColorMapManager::getDefaultColorMap()
{
    return colorMaps.value(DEFAULT_COLOR_MAP);
}
