#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QList>
#include <QDebug>
#include "configs.h"

class Histogram
{

public:
    Histogram(float* values, int totalValues);
    Histogram(QList<float*> values, QList<int> totalValues);
    ~Histogram();

    /* rebuild histogram using new min and max thresholds */
    void configureThresholds(float min, float max);

    float getHistogramValue(float pos1, float pos2);

    void getMinMax(float &min, float &max);

private:
    float minValue;
    float maxValue;

    int *histogram;

    int numberBins;
    int maxbin;

    float *values;
    int totalValues;

    bool composite;

    /* setup histogram with new values */
    void setupHistogram(float* values, int totalValues);

    void calculateMinMax();

    void buildHistogram();
};

#endif // HISTOGRAM_H
