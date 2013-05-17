#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QList>
#include <QDebug>
#include "configs.h"
#include "unique_ptr.hpp"
#include <chealpix.h>
#include <cxxutils.h>
#include <healpix_map.h>
#include "types.h"

class Histogram
{
    Histogram();
    Histogram(Histogram const&);
    Histogram& operator=(Histogram const&);

public:
    Histogram(hv::unique_ptr<float[0]> values, int totalValues);
    Histogram(QList<float*> values, QList<int> totalValues);
    ~Histogram();

    /* rebuild histogram using new min and max thresholds */
    void configureThresholds(float min, float max);

    /* change the used scale */
    void updateScale(ScaleType scale);

    float getHistogramValue(float pos1, float pos2);

    void getMinMax(float &min, float &max);
    float getMinLog();

private:
    float minThreshold, minThresholdOriginal;
    float maxThreshold, maxThresholdOriginal;
    float minValue;
    float maxValue;
    float minLog;
    ScaleType scale;
    QList<float*> valuesList;
    QList<int> nValues;

    hv::unique_ptr<int[0]> histogram;

    int numberBins;
    int maxbin;

    hv::unique_ptr<float[0]> values;
    int totalValues;

    bool composite;

    /* setup histogram with new values */
    void setupHistogram(hv::unique_ptr<float[0]> values, int totalValues, bool calculateExtremes=false);

    void calculateMinMax();

    void buildHistogram();

    void computeValues(bool calculateExtremes=false);
};

#endif // HISTOGRAM_H
