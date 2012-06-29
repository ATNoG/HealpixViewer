#include "histogram.h"
#include <float.h>
Histogram::Histogram(hv::unique_array<float> values, int totalValues)
{
    //qDebug() << "construction histogram";
    this->numberBins = NUMBER_BINS;
    this->scale = LINEAR;

    /* single map histogram */
    composite = false;

    for(int i=0; i<totalValues; i++)
    {
        switch(scale)
        {
            case LINEAR:
                values[i] = values[i];
                break;
            case ASINH:
                values[i] = asinh(values[i]);
                break;
            case LOGARITHMIC:
                values[i] = log10(values[i]);
                break;
        }
    }

    setupHistogram(values.move(), totalValues, true);

    /* configure thresholds to min and max values */
    configureThresholds(minValue, maxValue);
}

Histogram::Histogram(QList<float*> valuesList, QList<int> nValues)
{
    //qDebug() << "construction mixed histogram";
    this->numberBins = NUMBER_BINS;
    this->valuesList = valuesList;
    this->nValues = nValues;
    this->scale = LINEAR;

    computeValues(true);

    /* configure thresholds to min and max values */
    configureThresholds(minValue, maxValue);
}


void Histogram::computeValues(bool calculateExtremes)
{
    //qDebug("Histogram::computeValues");

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
            switch(scale)
            {
                case LINEAR:
                    values[filledValues] = valuesList[i][j];
                    break;
                case ASINH:
                    values[filledValues] = asinh(valuesList[i][j]);
                    break;
                case LOGARITHMIC:
                    float aux = valuesList[i][j];
                    if(aux < minThresholdOriginal)
                        aux = minThresholdOriginal;
                    else if(aux > maxThresholdOriginal)
                        aux = maxThresholdOriginal;

                    values[filledValues] = log10(std::max(aux, (float)1.e-6*84));
                    break;
            }

            filledValues++;
        }
    }

    /* calculate min log */
    if(scale==LOGARITHMIC)
    {
        minLog = FLT_MAX;
        for(int i=0; i<totalValues; i++)
        {
            if(!approx<double>(values[i],Healpix_undef))
            {
                if(values[i] < minLog)
                    minLog = values[i];
            }
        }
    }

    /* setup histogram with new values */
    setupHistogram(values.move(), totalValues, calculateExtremes);
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

float Histogram::getMinLog()
{
    return minLog;
}

void Histogram::configureThresholds(float min, float max)
{
    //qDebug() << "Histogram::configureThresholds: " << min << "," << max;

    /* update original thresholds */
    minThresholdOriginal = min;
    maxThresholdOriginal = max;

    /* calculate threshold to use based on current scale */
    switch(scale)
    {
        case LINEAR:
            minThreshold = min;
            maxThreshold = max;
            break;
        case ASINH:
            minThreshold = asinh(min);
            maxThreshold = asinh(max);
            break;
        case LOGARITHMIC:
            minThreshold = (min>0) ? log10(min) : minLog;
            maxThreshold = (max>0) ? log10(max) : max;
            break;
    }

    //qDebug() << "Using Thresholds: " << minThreshold << "," << maxThreshold;


    /* rebuild histogram */
    buildHistogram();
}


void Histogram::updateScale(ScaleType scale)
{
    //qDebug("Histogram::updateScale");

    this->scale = scale;

    //qDebug() << "before computeValues original thresholds: " << minThresholdOriginal << "," << maxThresholdOriginal;

    /* compute new values */
    computeValues();

    //qDebug() << "after computeValues original thresholds: " << minThresholdOriginal << "," << maxThresholdOriginal;

    /* calculate new threshold based on the original */
    configureThresholds(minThresholdOriginal, maxThresholdOriginal);

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
        int bin = ((values[i]-minThreshold)/(maxThreshold-minThreshold)) * numberBins;
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
    min = FLT_MAX;
    max = Healpix_undef;

    for(long i=0; i<totalValues; i++)
    {
        if(!approx<double>(values[i],Healpix_undef))
        {
            if(values[i]>max)
                max = values[i];
            else if(values[i]<min)
            {
                min = values[i];
            }
        }
    }

    minValue = min;
    maxValue = max;

    //qDebug() << "Histogram::calculateMinMax: " << minValue << "," << maxValue;
}


void Histogram::setupHistogram(hv::unique_array<float> values, int totalValues, bool calculateExtremes)
{
    this->values = values.move();
    this->totalValues = totalValues;

    if(calculateExtremes)
    {
        /* get min and max for map */
        calculateMinMax();
    }

    /* delete old histogram */
    histogram.reset();

    //qDebug("Histogram::setupHistogram");
}
