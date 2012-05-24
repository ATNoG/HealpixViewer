#include "histogramwidget.h"
#include "ui_histogramwidget.h"

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistogramWidget)
{
    ui->setupUi(this);

    histogram = NULL;

    /* udpate default sentinel pixel color */
    currentSentinelColor = QColor(DEFAULT_SENTINEL_COLOR);
    QStyle* style = new QWindowsStyle;
    ui->selectSentinelColor->setStyle(style);
    ui->selectSentinelColor->setPalette(QPalette(currentSentinelColor));

    /* connect spinboxs and apply button */
    connect(ui->lowerThreshold, SIGNAL(valueChanged(double)), this, SLOT(updateLowerThreshold(double)));
    connect(ui->higherThreshold, SIGNAL(valueChanged(double)), this, SLOT(updateHigherThreshold(double)));
    connect(ui->applyHistogram, SIGNAL(released()), this, SLOT(updateMap()));
    connect(ui->colorMapSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(colorMapUpdate(int)));
    connect(ui->selectSentinelColor, SIGNAL(released()), this, SLOT(sentinelColorUpdate()));
    connect(ui->scaleSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(scaleUpdate(int)));

    fillColorMaps();
    fillScaleOptions();

    factor = 1.0;
    offset = 0.0;
}


HistogramWidget::~HistogramWidget()
{
    #if DEBUG > 0
        qDebug() << "Calling HistogramWidget destructor";
    #endif

    delete ui;

    if(histogram!=NULL)
        delete histogram;
}


/* PRIVATE SLOTS */

void HistogramWidget::colorMapUpdate(int)
{
    currentColorMap = getSelectedColorMap();

    updateColorMap(currentColorMap);
}

void HistogramWidget::scaleUpdate(int)
{
    currentScale = (ScaleType)ui->scaleSelector->itemData(ui->scaleSelector->currentIndex()).toInt();
    updateScale(currentScale);
}

void HistogramWidget::sentinelColorUpdate()
{
    QColor selectedColor = QColorDialog::getColor(currentSentinelColor);
    if(selectedColor.isValid())
    {
        currentSentinelColor = selectedColor;
        ui->selectSentinelColor->setPalette(QPalette(currentSentinelColor));
    }
}

/* called when lower threshold spinbox changes */
void HistogramWidget::updateLowerThreshold(double value)
{
    //qDebug() << "updating lower threshold";
    min = value;
    ui->higherThreshold->setMinimum(min+ui->higherThreshold->singleStep());
    updateHistogramThreshold();
}

/* called when higher threshold spinbox changes */
void HistogramWidget::updateHigherThreshold(double value)
{
    //qDebug() << "updating higher threshold";
    max = value;
    ui->lowerThreshold->setMaximum(max-ui->higherThreshold->singleStep());
    updateHistogramThreshold();
}

/* called when apply button is pressed. Emits signal 'histogramUpdated' */
void HistogramWidget::updateMap()
{
    /* update map informations */
    for(int i=0; i<selectedViewports.size(); i++)
    {
        mapsInformation[selectedViewports[i]]->min = min;
        mapsInformation[selectedViewports[i]]->max = max;
        mapsInformation[selectedViewports[i]]->colorMap = currentColorMap;
        mapsInformation[selectedViewports[i]]->scale = currentScale;
        mapsInformation[selectedViewports[i]]->sentinelColor = currentSentinelColor;
        mapsInformation[selectedViewports[i]]->factor = factor;
        mapsInformation[selectedViewports[i]]->offset = offset;
    }

    /* update threshold values for selected viewports */
    if(currentScale==LOGARITHMIC)
    {
        float logmin;
        logmin = min<=0 ? histogram->getMinLog() : log10(min);
        emit(histogramUpdated(currentColorMap, logmin, max, currentSentinelColor, currentScale, factor, offset));
    }
    else
    {
        emit(histogramUpdated(currentColorMap, min, max, currentSentinelColor, currentScale, factor, offset));
    }
}



/* PUBLIC SLOTS */

/* Update histogram after selection changed */
void HistogramWidget::updateHistogram(QList<int> selectedViewports)
{
    //qDebug("Viewport selection changed");
    this->selectedViewports = selectedViewports;
    updateHistogram();
}

void HistogramWidget::updateHistogram()
{
    //qDebug() << "--- update histogram";

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

        //if(histogram!=NULL)
            //delete histogram;

        histogram = new Histogram(values, totalValues);
        ui->histogram->setHistogram(histogram);

        /* if more then 1 viewports, select min and max
           if only 1 viewport, select min and max if saved previously, else min max */
        float mapMin, mapMax, mapMinAbs, mapMaxAbs;

        mapMinAbs = mapsInformation[selectedViewports[0]]->min;
        mapMaxAbs = mapsInformation[selectedViewports[0]]->max;

        ColorMap *colorMapToUse = mapsInformation[selectedViewports[0]]->colorMap;
        QColor sentinelColorToUse = mapsInformation[selectedViewports[0]]->sentinelColor;
        ScaleType scaleToUse = mapsInformation[selectedViewports[0]]->scale;
        bool usingSameColorMap = true;
        bool usingSameSentinelColor = true;
        bool usingSameScale = true;

        /* get min and max for each map */
        for(int i=0; i<selectedViewports.size(); i++)
        {
            mapInfo *info = mapsInformation[selectedViewports[i]];
            mapMin = info->min;
            mapMax = info->max;

            /* map has threshold defined, use them */
            if(mapMin<mapMinAbs)
                mapMinAbs = mapMin;
            if(mapMax>mapMaxAbs)
                mapMaxAbs = mapMax;

            if(colorMapToUse!=info->colorMap)
                usingSameColorMap = false;

            if(sentinelColorToUse!=info->sentinelColor)
                usingSameSentinelColor = false;

            if(scaleToUse!=info->scale)
                usingSameScale = false;
        }

        if(usingSameColorMap)
            currentColorMap = colorMapToUse;
        else
            currentColorMap = ColorMapManager::instance()->getDefaultColorMap();

        updateColorMap(currentColorMap);

        if(usingSameSentinelColor)
            updateSentinelColor(sentinelColorToUse);
        else
            updateSentinelColor(QColor(DEFAULT_SENTINEL_COLOR));

        if(usingSameScale)
            updateScale(scaleToUse);
        else
            updateScale(LINEAR);

        updateFactor(1.0);
        updateOffset(0.0);

        /* update thresholds */
        setThresholds(mapMinAbs, mapMaxAbs);
    }
    else
    {
        //qDebug() << "no viewports selected";
        ui->histogram->cleanupHistogram();
        ui->lowerThreshold->setDisabled(true);
        ui->higherThreshold->setDisabled(true);
        ui->applyHistogram->setDisabled(true);
        ui->colorMapSelector->setDisabled(true);
        ui->selectSentinelColor->setDisabled(true);
        ui->scaleSelector->setDisabled(true);
    }
}


/* update information about viewport, so when it needs to be displayed in histogram, the values will be correct */
void HistogramWidget::updateMapInfo(int viewportId, mapInfo* info)
{
    if(mapsInformation.contains(viewportId))
        mapsInformation.remove(viewportId);

    mapsInformation.insert(viewportId, info);

    //qDebug() << "-update map info";
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


ColorMap* HistogramWidget::getSelectedColorMap()
{
    int colorMapId = ui->colorMapSelector->itemData(ui->colorMapSelector->currentIndex()).toInt();
    return ColorMapManager::instance()->getColorMap(colorMapId);
}

/* redrwa histogram using new threshold */
void HistogramWidget::updateHistogramThreshold()
{
    //qDebug("HistogramWidget::updateHistogramThreshold");

    /* rebuild histogram */
    ui->histogram->rebuildHistogram(min, max);

    if(REALTIME_TEXTURES && selectedViewports.size()==1)
        updateMap();
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
    ui->colorMapSelector->setEnabled(true);
    ui->selectSentinelColor->setEnabled(true);
    ui->scaleSelector->setEnabled(true);
}


void HistogramWidget::fillColorMaps()
{
    ui->colorMapSelector->blockSignals(true);

    ColorMapManager *cmapManager = ColorMapManager::instance();
    QList<ColorMap*> colorMaps = cmapManager->getColorMaps();
    ColorMap *cm;

    ui->colorMapSelector->clear();
    for(int i=0; i<colorMaps.size(); i++)
    {
        cm = colorMaps[i];
        /* add item to combobox */
        ui->colorMapSelector->insertItem(i, cm->getName(), cm->getId());
    }

    /* change current item to default colormap */
    ui->colorMapSelector->setCurrentIndex(0);
    ui->colorMapSelector->blockSignals(false);
}


void HistogramWidget::fillScaleOptions()
{
    ui->scaleSelector->blockSignals(true);

    ui->scaleSelector->clear();
    ui->scaleSelector->insertItem(0, "Linear", LINEAR);
    ui->scaleSelector->insertItem(1, "asinh", ASINH);
    ui->scaleSelector->insertItem(2, "Logarithmic", LOGARITHMIC);

    /* change current item to default scale */
    ui->scaleSelector->setCurrentIndex(0);
    ui->scaleSelector->blockSignals(false);
}


void HistogramWidget::updateColorMapSelector(ColorMap *colorMap)
{
    ui->colorMapSelector->blockSignals(true);
    ui->colorMapSelector->setCurrentIndex(colorMap->getId());
    ui->colorMapSelector->blockSignals(false);
}

void HistogramWidget::updateScaleSelector(ScaleType scale)
{
    ui->scaleSelector->blockSignals(true);
    ui->scaleSelector->setCurrentIndex(scale);
    ui->scaleSelector->blockSignals(false);
}


void HistogramWidget::updateColorMap(ColorMap* cm)
{
    /* update histogramViewer to use current colormap */
    ui->histogram->updateColorMap(cm);

    /* update colormap selector */
    updateColorMapSelector(cm);
}


void HistogramWidget::updateScale(ScaleType scale)
{
    /* changes the scale used in histogram */
    ui->histogram->updateScale(scale);

    /* update scale selector */
    updateScaleSelector(scale);
}

void HistogramWidget::updateSentinelColor(QColor color)
{
    currentSentinelColor = color;
    ui->selectSentinelColor->setPalette(QPalette(currentSentinelColor));
}

void HistogramWidget::updateFactor(float factor)
{

}

void HistogramWidget::updateOffset(float offset)
{

}
