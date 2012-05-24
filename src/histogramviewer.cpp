#include "histogramviewer.h"

HistogramViewer::HistogramViewer(QWidget *parent) :
    QWidget(parent)
{
    this->histogram = NULL;
    ct = ColorMapManager::instance()->getDefaultColorMap();
}


HistogramViewer::~HistogramViewer()
{
    delete ct;
}


void HistogramViewer::setHistogram(Histogram *histogram)
{
    this->histogram = histogram;
}


void HistogramViewer::paintEvent(QPaintEvent*)
{
    int margin = 2;

    // TODO: dynamic width and height
    _width = WIDTH-2*margin;
    _height = HEIGHT-2*margin;

    QPainter painter(this);
    painter.setPen(QColor("Black"));

    /* draw axis */
    painter.drawLine(0, 0, 0, _height);
    painter.drawLine(0, _height, _width, _height);
    painter.drawLine(_width, _height, _width, 0);

    int xMargin = 1;
    int yMargin = 1;

    if(histogram!=NULL)
    {
        /* draw histogram */
        for(int i=0; i<_width-1; i++)
        {
            float pos1, pos2;
            pos1 = i*(1./(_width-1));
            pos2 = (i+1)*(1./(_width-1));

            float value = histogram->getHistogramValue(pos1, pos2);
            float color = (float)i/(float)_width;

            painter.setPen((*ct)(color));
            //qDebug() << "Value for pos " << i << " = " << (int)(_height*(1-value))-yMargin;

            if(value>0.0)
                painter.drawLine(xMargin+i, _height-yMargin, xMargin+i, (int)(_height*(1-value))-yMargin);
        }
    }
}


void HistogramViewer::cleanupHistogram()
{
    //qDebug() << "cleaning up histogram";
    if(histogram!=NULL)
    {
        delete histogram;
        histogram = NULL;
        update();
    }
}


void HistogramViewer::rebuildHistogram(float min, float max)
{
    //qDebug("HistogramViewer::rebuildHistogram");
    if(histogram!=NULL)
        histogram->configureThresholds(min, max);
    update();
}


/* update histogram color map */
void HistogramViewer::updateColorMap(ColorMap *colorMap)
{
    this->ct = colorMap;
    update();
}

void HistogramViewer::updateScale(ScaleType scale)
{
    if(histogram!=NULL)
        histogram->updateScale(scale);
    update();
}
