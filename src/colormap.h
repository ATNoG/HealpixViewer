#ifndef COLORTABLE_H
#define COLORTABLE_H


#include <QString>
#include <QColor>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <QPixmap>
#include <vector>
#include "configs.h"


using namespace std;


class ColorMap
{
public:
        ColorMap(int id, QString file);
        QColor operator[](float v) const;
        QString getName();
        int getId();

private:
        QString name;
        int id;
        unsigned int ncolors;
        std::vector<QColor> table;

        void readTable(QString file);
};



class ColorMapManager
{

public:
    ColorMapManager();
    ~ColorMapManager();

    static ColorMapManager *instance()
    {
        if(!s_instance)
            s_instance = new ColorMapManager();
        return s_instance;
    }

    QList<ColorMap*> getColorMaps();
    ColorMap* getColorMap(int id);
    ColorMap* getDefaultColorMap();

private:
    static ColorMapManager *s_instance;
    QMap<int, ColorMap*> colorMaps;
    QString basepath;

    void readColorMaps();
};

#endif
