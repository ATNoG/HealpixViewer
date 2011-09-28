#ifndef HISTOGRAMVIEWER_H
#define HISTOGRAMVIEWER_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include "colortable.h"
#include "histogram.h"

#define WIDTH 260
#define HEIGHT 150

class HistogramViewer : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramViewer(QWidget *parent = 0);
    ~HistogramViewer();

    void setHistogram(Histogram *histogram);

    /* cleanup histogram */
    void cleanupHistogram();

    /* rebuild histogram using new min and max thresholds */
    void rebuildHistogram(float min, float max);


signals:

private:
    int _width, _height;

    ColorTable *ct;
    Histogram *histogram;

    void paintEvent(QPaintEvent *event);
};

#endif // HISTOGRAMVIEWER_H
