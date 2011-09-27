#include "histogramwidget.h"
#include "ui_histogramwidget.h"

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistogramWidget)
{
    ui->setupUi(this);

    /* connect spinboxs and apply button */
    connect(ui->lowerThreshold, SIGNAL(valueChanged(double)), this, SLOT(updateLowerThreshold(double)));
    connect(ui->higherThreshold, SIGNAL(valueChanged(double)), this, SLOT(updateHigherThreshold(double)));
    connect(ui->applyHistogram, SIGNAL(released()), this, SLOT(updateMap()));
}


HistogramWidget::~HistogramWidget()
{
    delete ui;
}


/* PRIVATE SLOTS */

/* called when lower threshold spinbox changes */
void HistogramWidget::updateLowerThreshold(double value)
{
    min = value;
    ui->higherThreshold->setMinimum(min+ui->higherThreshold->singleStep());
    updateHistogramThreshold();
}

/* called when higher threshold spinbox changes */
void HistogramWidget::updateHigherThreshold(double value)
{
    max = value;
    ui->lowerThreshold->setMaximum(max-ui->higherThreshold->singleStep());
    updateHistogramThreshold();
}

/* called when apply button is pressed. Emits signal 'thresholdUpdated' */
void HistogramWidget::updateMap()
{
    /* update map informations */
    for(int i=0; i<selectedViewports.size(); i++)
    {
        mapsInformation[selectedViewports[i]]->min = min;
        mapsInformation[selectedViewports[i]]->max = max;
    }

    /* update threshold values for selected viewports */
    emit(thresholdUpdated(min, max));
}



/* PUBLIC SLOTS */

/* Update histogram after selection changed */
void HistogramWidget::updateHistogram(QList<int> selectedViewports)
{
    this->selectedViewports = selectedViewports;
    updateHistogram();
}

void HistogramWidget::updateHistogram()
{
    if(selectedViewports.size()>0)
    {
        /* construct new histogram */
        QList<float*> values;
        QList<int> totalValues;

        for(int i=0; i<selectedViewports.size(); i++)
        {
            mapInfo *info = mapsInformation[selectedViewports[i]];

            values.append(info->values);
            totalValues.append(info->nvalues);
        }

        Histogram *histogram = new Histogram(values, totalValues);
        ui->histogram->setHistogram(histogram);

        /* if more then 1 viewports, select min and max
           if only 1 viewport, select min and max if saved previously, else min max */
        float mapMin, mapMax, mapMinAbs, mapMaxAbs;

        mapMinAbs = mapsInformation[selectedViewports[0]]->min;
        mapMaxAbs = mapsInformation[selectedViewports[0]]->max;

        /* get min and max for each map */
        for(int i=0; i<selectedViewports.size(); i++)
        {
            mapMin = mapsInformation[selectedViewports[i]]->min;
            mapMax = mapsInformation[selectedViewports[i]]->max;

            /* map has threshold defined, use them */
            if(mapMin<mapMinAbs)
                mapMinAbs = mapMin;
            if(mapMax>mapMaxAbs)
                mapMaxAbs = mapMax;
        }

        /* update thresholds */
        setThresholds(mapMinAbs, mapMaxAbs);
    }
    else
    {
        ui->histogram->cleanupHistogram();
        ui->lowerThreshold->setDisabled(true);
        ui->higherThreshold->setDisabled(true);
        ui->applyHistogram->setDisabled(true);
    }
}


/* update information about viewport, so when it needs to be displayed in histogram, the values will be correct */
void HistogramWidget::updateMapInfo(int viewportId, mapInfo* info)
{
    if(mapsInformation.contains(viewportId))
        mapsInformation.remove(viewportId);

    mapsInformation.insert(viewportId, info);

    updateHistogram();
}


/* unload viewport info */
void HistogramWidget::unloadMapInfo(int viewportId)
{
    mapsInformation.remove(viewportId);
    selectedViewports.removeAt(selectedViewports.indexOf(viewportId));

    updateHistogram();
}



/* PRIVATE */

/* redrwa histogram using new threshold */
void HistogramWidget::updateHistogramThreshold()
{
    /* rebuild histogram */
    ui->histogram->rebuildHistogram(min, max);
}

/* set new thresholds */
void HistogramWidget::setThresholds(float _min, float _max)
{
    min = _min;
    max = _max;

    /* calculate step and decimals places */
    float step = (_max-_min)/100;
    float aux;
    int decimals;

    //qDebug() << "Diff = " << step;
    if(step<=10)
    {
        decimals = 0;
        aux = step;
        while(aux<1.0)
        {
            aux = aux*10;
            decimals++;
        }
        step = 1 / pow(10, decimals);
        decimals++;
    }
    else
    {
        int a = 0;
        decimals = 0;
        aux = step;
        while(aux>=10)
        {
            aux = aux/10;
            a++;
        }
        a--;
        step = floor(aux) * pow(10, a);
    }

    //qDebug() << "Threshold set to " << _min << "," << _max;
    //qDebug() << "Step = " << step;
    //qDebug() << "Decimals = " << decimals;

    /* update spinboxs */
    ui->lowerThreshold->blockSignals(true);
    ui->higherThreshold->blockSignals(true);
    ui->lowerThreshold->setDecimals(decimals);
    ui->higherThreshold->setDecimals(decimals);
    ui->lowerThreshold->setSingleStep(step);
    ui->higherThreshold->setSingleStep(step);
    ui->lowerThreshold->setMaximum(max-step);
    ui->higherThreshold->setMinimum(min+step);
    ui->lowerThreshold->setValue(min);
    ui->higherThreshold->setValue(max);
    ui->lowerThreshold->blockSignals(false);
    ui->higherThreshold->blockSignals(false);

    /* update histogram */
    updateHistogramThreshold();

    /* enable spinboxs and button */
    ui->lowerThreshold->setEnabled(true);
    ui->higherThreshold->setEnabled(true);
    ui->applyHistogram->setEnabled(true);
}
