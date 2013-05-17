#ifndef HISTOGRAMVIEWER_H
#define HISTOGRAMVIEWER_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include "histogram.h"
#include "colormap.h"

#define WIDTH 260
#define HEIGHT 150

class HistogramViewer : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramViewer(QWidget *parent = 0);
    ~HistogramViewer();

    void setHistogram(QList<float*> const& values, QList<int> const& totalValues);

    float getMinLog() const { return histogram->getMinLog(); }

    /* cleanup histogram */
    void cleanupHistogram();

    /* rebuild histogram using new min and max thresholds */
    void rebuildHistogram(float min, float max);

    /* update histogram color map */
    void updateColorMap(ColorMap *colorMap);

    /* udpdate scale used in histogram */
    void updateScale(ScaleType scale);

signals:

private:
    int _width, _height;

    ColorMap *ct;
    hv::unique_ptr<Histogram> histogram;
    ScaleType scale;

    void paintEvent(QPaintEvent *event);
};

#endif // HISTOGRAMVIEWER_H
