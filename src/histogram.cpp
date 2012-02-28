#include "histogram.h"

Histogram::Histogram(hv::unique_array<float> values, int totalValues)
{
    this->numberBins = NUMBER_BINS;

    /* single map histogram */
    composite = false;

    setupHistogram(values.move(), totalValues);
}

Histogram::Histogram(QList<float*> valuesList, QList<int> nValues)
{
    this->numberBins = NUMBER_BINS;

    /* check if histogram is from 1 or more maps */
    if(valuesList.size()>1)
        composite = true;
    else
        composite = false;

    /* calculate total number of values */
    long totalValues = 0;
    for(int i=0; i<nValues.size(); i++)
    {
        totalValues += nValues[i];
    }

    /* create new array for hold information */
    hv::unique_array<float> values(new float[totalValues]);

    /* fill values array */
    long filledValues = 0;
    for(int i=0; i<valuesList.size(); i++)
    {
        // TODO: try to use memcpy instead
        for(int j=0; j<nValues[i]; j++)
        {
            values[filledValues] = valuesList[i][j];
            filledValues++;
        }
    }

    setupHistogram(values.move(), totalValues);
}


Histogram::~Histogram()
{
    #if DEBUG > 0
        qDebug() << "Calling Histogram destructor";
    #endif
}


void Histogram::getMinMax(float &_min, float &_max)
{
    _min = minValue;
    _max = maxValue;
}


void Histogram::configureThresholds(float min, float max)
{
    minValue = min;
    maxValue = max;

    /* rebuild histogram */
    buildHistogram();
}


void Histogram::buildHistogram()
{
    /* allocate space for histogram */
    if(!histogram)
        histogram.reset(new int[numberBins]);

    /* initialize values */
    for(int i=0; i<numberBins; i++)
        histogram[i] = 0;

    /* create histogram */
    maxbin = 0;

    for(long i=0; i<totalValues; i++)
    {
        int bin = ((values[i]-minValue)/(maxValue-minValue)) * numberBins;
        if(bin>=0 && bin<numberBins)
        {
            histogram[bin]++;
            if(histogram[bin]>maxbin)
                maxbin = histogram[bin];
        }
    }

    /*
    for(int i=0; i<numberBins; i++)
        if(histogram[i]>0)
            qDebug() << "Histogram[" << i << "] = " << histogram[i];
    */
}


float Histogram::getHistogramValue(float pos1, float pos2)
{
    int bin0 = (int)(pos1*numberBins);
    int bin1 = (int)(pos2*numberBins);

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



/* private methods */

void Histogram::calculateMinMax()
{
    /* calculate min and max values */
    float min, max;
    min = max = values[0];
    for(long i=0; i<totalValues; i++)
    {
        if(values[i]>max)
            max = values[i];
        else if(values[i]<min)
            min = values[i];
    }

    minValue = min;
    maxValue = max;
}


void Histogram::setupHistogram(hv::unique_array<float> values, int totalValues)
{
    this->values = values.move();
    this->totalValues = totalValues;

    /* get min and max for map */
    calculateMinMax();

    /* delete old histogram */
    histogram.reset();

    configureThresholds(minValue, maxValue);
}
