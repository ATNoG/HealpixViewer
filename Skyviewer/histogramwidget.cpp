#include "histogramwidget.h"
#include "ui_histogramwidget.h"

HistogramWidget::HistogramWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistogramWidget)
{
    ui->setupUi(this);

    /* connect spinboxs */
    connect(ui->lowerThreshold, SIGNAL(valueChanged(double)), this, SLOT(updateLowerThreshold(double)));
    connect(ui->higherThreshold, SIGNAL(valueChanged(double)), this, SLOT(updateHigherThreshold(double)));

    connect(ui->treeViewports, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(selectionChanged(QTreeWidgetItem*,int)));
    connect(ui->applyHistogram, SIGNAL(released()), this, SLOT(updateMap()));
    connect(ui->mapFieldSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMapField(int)));

    currentSelectedViewport = -1;
}


HistogramWidget::~HistogramWidget()
{
    delete ui;
}


void HistogramWidget::updateLowerThreshold(double value)
{
    min = value;
    ui->higherThreshold->setMinimum(min+ui->higherThreshold->singleStep());
    updateHistogramThreshold();
}

void HistogramWidget::updateHigherThreshold(double value)
{
    max = value;
    ui->lowerThreshold->setMaximum(max-ui->higherThreshold->singleStep());
    updateHistogramThreshold();
}

void HistogramWidget::changeMapField(int currentIdx)
{
    QVariant aux = ui->mapFieldSelector->itemData(currentIdx);
    HealpixMap::MapType field = getMapField(aux.toInt());

    if(currentSelectedViewport>=0)
    {
        if(mapsInformation[currentSelectedViewport]->currentField != field)
        {
            mapsInformation[currentSelectedViewport]->currentField = field;
            emit(mapFieldChanged(currentSelectedViewport, field));
        }
    }
}

void HistogramWidget::setThresholds(float _min, float _max)
{
    //qDebug() << "Setting thresholds to " << _min << "," << _max;

    min = _min;
    max = _max;

    /* update spinboxs */
    ui->lowerThreshold->blockSignals(true);
    ui->higherThreshold->blockSignals(true);
    ui->lowerThreshold->setMaximum(max-ui->higherThreshold->singleStep());
    ui->higherThreshold->setMinimum(min+ui->higherThreshold->singleStep());
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

void HistogramWidget::updateHistogramThreshold()
{
    /* rebuild histogram */
    ui->histogram->rebuildHistogram(min, max);
}

void HistogramWidget::updateMap()
{
    QList<int> selectedViewports = getSelectedViewports();

    /* update map informations */
    for(int i=0; i<selectedViewports.size(); i++)
    {
        mapsInformation[selectedViewports[i]]->min = min;
        mapsInformation[selectedViewports[i]]->max = max;
    }

    /* update threshold values for selected viewports */
    emit(thresholdUpdated(selectedViewports, min, max));
}

void HistogramWidget::updateHistogram()
{
    QList<int> selectedViewports = getSelectedViewports();

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
        float mapMin, mapMax;
        if(selectedViewports.size()==1)
        {
            mapMin = mapsInformation[selectedViewports[0]]->min;
            mapMax = mapsInformation[selectedViewports[0]]->max;

            if(mapMin==0 && mapMax==0)
                histogram->getMinMax(mapMin, mapMax);
        }
        else
        {
            histogram->getMinMax(mapMin, mapMax);
        }

        /* update thresholds */
        setThresholds(mapMin, mapMax);
    }
    else
    {
        ui->histogram->cleanupHistogram();
        ui->lowerThreshold->setDisabled(true);
        ui->higherThreshold->setDisabled(true);
        ui->applyHistogram->setDisabled(true);
    }
}


void HistogramWidget::addViewport(int viewportId, QString title, mapInfo *info)
{
    /* add viewport to tree */
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, title);
    item->setData(0, Qt::UserRole, viewportId);
    item->setCheckState(0, Qt::Unchecked);
    ui->treeViewports->addTopLevelItem(item);

    /* add information to map */
    mapsInformation[viewportId] = info;

    /* get map histogram */
    float mapMin, mapMax;
    Histogram *histo = new Histogram(info->values, info->nvalues);
    histo->getMinMax(mapMin, mapMax);

    /* send min and max to map */
    QList<int> viewports;
    viewports.append(viewportId);
    emit(thresholdUpdated(viewports, mapMin, mapMax));

    delete histo;

    if(getSelectedViewports().size()<=1)
    {
        loadViewportInfo(viewportId);
    }
}


void HistogramWidget::removeViewport(int viewportId)
{
    /* remove viewport from tree */
    QList<QTreeWidgetItem *> items = ui->treeViewports->findItems(QString("Healpixmap %1").arg(viewportId), Qt::MatchExactly);
    delete items[0];

    /* remove from information */
    mapsInformation.remove(viewportId);

    /* update histogram */
    updateHistogram();
}


void HistogramWidget::loadViewportInfo(int viewportId)
{
    /* update field selector for this viewport */
    updateFieldSelector(viewportId);


    QList<QTreeWidgetItem *> items;

    if(currentSelectedViewport>=0)
    {
        /* uncheck current checkbox */
        items = ui->treeViewports->findItems(QString("Healpixmap %1").arg(currentSelectedViewport), Qt::MatchExactly);
        items[0]->setCheckState(0, Qt::Unchecked);
    }

    currentSelectedViewport = viewportId;

    /* select this viewport in treeview */
    items = ui->treeViewports->findItems(QString("Healpixmap %1").arg(viewportId), Qt::MatchExactly);
    items[0]->setCheckState(0, Qt::Checked);
}


QList<int> HistogramWidget::getSelectedViewports()
{
    QList<int> selectedViewports;

    for(int i=0; i<ui->treeViewports->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item =  ui->treeViewports->topLevelItem(i);
        if(item->checkState(0)==Qt::Checked)
            selectedViewports.append(item->data(0, Qt::UserRole).toInt());
    }

    return selectedViewports;
}




void HistogramWidget::selectionChanged(QTreeWidgetItem*, int)
{
    /* get selected viewports in treeview */
    QList<int> selectedViewports = getSelectedViewports();

    if(selectedViewports.size()==1)
    {
        currentSelectedViewport = selectedViewports[0];

        /* only one viewport selected, show field selection */
        updateFieldSelector(currentSelectedViewport);
        ui->mapFieldSelector->setEnabled(true);
    }
    else
    {
        currentSelectedViewport = -1;

        /* disable field selection */
        ui->mapFieldSelector->setDisabled(true);
    }

    updateHistogram();
}


void HistogramWidget::updateFieldSelector(int viewportId)
{
    ui->mapFieldSelector->blockSignals(true);

    mapInfo *info = mapsInformation[viewportId];
    QList<HealpixMap::MapType> availableFiels = info->availableFields;

    int selectedIndex = 0;
    ui->mapFieldSelector->clear();
    for(int i=0; i<availableFiels.size(); i++)
    {
        /* add item to combobox */
        ui->mapFieldSelector->insertItem(i, HealpixMap::mapTypeToString(availableFiels[i]), availableFiels[i]);
        if(availableFiels[i]==info->currentField)
            selectedIndex = i;
    }

    /* change current item to selected field */
    ui->mapFieldSelector->setCurrentIndex(selectedIndex);

    ui->mapFieldSelector->blockSignals(false);
}
