#include "histogramviewer.h"

HistogramViewer::HistogramViewer(QWidget *parent) :
    QWidget(parent)
{
    int margin = 2;
    this->numberBins = NUMBER_BINS;
    this->_width = 300-2*margin;
    this->_height = 150-2*margin;
    this->histogram = NULL;
    this->values = NULL;

    initialized = false;

    ct = new ColorTable(2);
}


void HistogramViewer::setupHistogram(float* values, int totalValues, float min, float max)
{
    qDebug() << "Total values1 = " << totalValues;

    /* calculate min and max values */
    /*
    minValue = maxValue = values[0];
    for(long i=0; i<totalValues; i++)
    {
        if(values[i]>maxValue)
            maxValue = values[i];
        else if(values[i]<minValue)
            minValue = values[i];
    }
    */
    this->values = values;
    this->totalValues = totalValues;

    /* delete old histogram */
    if(histogram!=NULL)
        delete histogram;

    initialized = true;

    buildHistogram(min, max);

    /*
    for(int i=0; i<numberBins; i++)
    {
        if(histogram[i]>0)
        qDebug() << "Histogram[" << i << "] = " << histogram[i];
    }
    */
}


void HistogramViewer::buildHistogram(float min, float max)
{
    if(initialized)
    {
        /* allocate space for histogram */
        if(histogram==NULL)
            histogram = new int[numberBins];

        /* initialize values */
        for(int i=0; i<numberBins; i++)
        {
            histogram[i] = 0;
        }

        /* create histogram */
        maxbin = 0;
        qDebug() << "Total values2 = " << totalValues;
        for(long i=0; i<totalValues; i++)
        {
            int bin = ((values[i]-min)/(max-min)) * numberBins;
            if(bin>=0 && bin<numberBins)
            {
                histogram[bin]++;
                if(histogram[bin]>maxbin)
                    maxbin = histogram[bin];
            }
        }

        /* update drawing */
        update();
    }
}


void HistogramViewer::paintEvent(QPaintEvent*)
{
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
            float value = getHistogramBarSize(i);
            float color = (float)i/(float)_width;

            painter.setPen((*ct)(color));
            //qDebug() << "Value for pos " << i << " = " << (int)(_height*(1-value))-yMargin;
            painter.drawLine(xMargin+i, _height-yMargin, xMargin+i, (int)(_height*(1-value))-yMargin);
        }
    }
}


float HistogramViewer::getHistogramBarSize(int position)
{
    int bin0 = (int)(position*(1./(_width-1))*numberBins);
    int bin1 = (int)((position+1)*(1./(_width-1))*numberBins);

    if(bin1>=numberBins)
        bin1 = numberBins-1;

    float mean = 0;

    for(int bin=bin0; bin<=bin1; bin++)
    {
        mean += histogram[bin];
    }

    mean = mean/(bin1-bin0+1);

    return mean/maxbin;
}
