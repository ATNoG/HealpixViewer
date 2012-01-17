#ifndef COLORTABLE_H
#define COLORTABLE_H


#include <QString>
#include <QColor>
#include <QPixmap>
#include <vector>


class ColorMap
{
public:
        ColorMap();
        ColorMap(int id);
        QColor operator[](unsigned int ind) const;
        QColor operator[](float v) const;
        QColor operator()(float v) const;
        int getSize (void) const;
        QString getName();
        QPixmap getPixmap();
        int getId();
protected:
        QString name;			//!< Name of the table.
        int id;
        unsigned int ncols;		//!< Number of colors in this table
        std::vector<QColor> table;	//!< The color table
private:
        void define_table (float intab[][3]);
};



inline QColor ColorMap::operator[](unsigned int ind) const
{
        return table[(ind >= ncols) ? (ncols - 1) : ind];
}


inline QColor ColorMap::operator()(float v) const
{
        return operator[](v);
}



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
    QList<ColorMap*> colorMaps;

    void readColorMaps();
};

#endif
