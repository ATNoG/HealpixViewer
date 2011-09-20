#ifndef HISTOGRAMVIEWER_H
#define HISTOGRAMVIEWER_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include "colortable.h"

#define NUMBER_BINS 2094

class HistogramViewer : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramViewer(QWidget *parent = 0);

    QSize minimumSizeHint() const
    {
        return QSize(_width, _height);
    }

    QSize sizeHint() const
    {
        return QSize(_width, _height);
    }

    void setupHistogram(float* values, int totalValues, float min, float max);
    void buildHistogram(float min, float max);

signals:

private:
    int _height;
    int _width;
    float minValue;
    float maxValue;

    int *histogram;

    int numberBins;
    int maxbin;

    ColorTable *ct;

    float *values;
    int totalValues;

    bool initialized;

    void paintEvent(QPaintEvent *event);
    float getHistogramBarSize(int position);
};

#endif // HISTOGRAMVIEWER_H
