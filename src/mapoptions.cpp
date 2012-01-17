#include "mapoptions.h"
#include "ui_mapoptions.h"

MapOptions::MapOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapOptions)
{
    ui->setupUi(this);

    /* connect signals */
    connect(ui->checkPVectorsThreshold, SIGNAL(stateChanged(int)), this, SLOT(useThreholds(int)));
    connect(ui->applyButton, SIGNAL(released()), this, SLOT(applyOptions()));
}

MapOptions::~MapOptions()
{
    delete ui;

    QList<int> viewportIds = viewportOptions.keys();
    foreach(int viewportId, viewportIds)
    {
        delete viewportOptions[viewportId];
    }
}


/* PUBLIC SLOTS */
void MapOptions::updateSelectedMaps(QList<int> selectedViewports)
{
    this->selectedViewports = selectedViewports;
    updateOptions();
}

void MapOptions::updateMapInfo(int viewportId, mapInfo* info)
{
    qDebug() << "update map info called";

    if(viewportOptions.contains(viewportId))
    {
        delete viewportOptions[viewportId];
        viewportOptions.remove(viewportId);
    }

    mapOptions *options = new mapOptions;
    options->tesselationNside = TESSELATION_DEFAULT_NSIDE;
    options->textureNside = 1; /* automatic */
    options->pVectorsNside = 1; /* automatic */
    options->magnification = 1.0;
    options->minNside = info->minNside;
    options->maxNside = info->maxNside;
    options->minPolThreshold = -1;
    options->maxPolThreshold = -1;
    options->originalMinPolThreshold = info->minMag;
    options->originalMaxPolThreshold = info->maxMag;
    options->controlPolarization = info->hasPolarization;
    options->vectorsSpacing = DEFAULT_VECTORS_SPACING;

    viewportOptions.insert(viewportId, options);
}


/* unload viewport info */
void MapOptions::unloadMapInfo(int viewportId)
{
    delete viewportOptions[viewportId];
    viewportOptions.remove(viewportId);

    selectedViewports.removeAt(selectedViewports.indexOf(viewportId));

    if(selectedViewports.size()==0)
        this->setDisabled(true);
    else
        updateOptions();
}



/* PRIVATE SLOTS */

void MapOptions::useThreholds(int state)
{
    if(state==Qt::Checked)
    {
        ui->spinboxMinPolarization->setEnabled(true);
        ui->spinboxMaxPolarization->setEnabled(true);
    }
    else
    {
        ui->spinboxMinPolarization->setDisabled(true);
        ui->spinboxMaxPolarization->setDisabled(true);
    }
}


void MapOptions::applyOptions()
{
    mapOptions *options = new mapOptions;
    options->tesselationNside = ui->comboTesselationNside->itemData(ui->comboTesselationNside->currentIndex()).toInt();
    options->textureNside = ui->comboTextureNside->itemData(ui->comboTextureNside->currentIndex()).toInt();
    options->pVectorsNside = ui->comboPVectorsNside->itemData(ui->comboPVectorsNside->currentIndex()).toInt();
    options->magnification = ui->spinboxMagnification->value();
    options->vectorsSpacing = ui->comboVectorsSpacing->itemData(ui->comboVectorsSpacing->currentIndex()).toInt();

    switch(ui->checkPVectorsThreshold->checkState())
    {
        case Qt::Checked:
            options->minPolThreshold = ui->spinboxMinPolarization->value();
            options->maxPolThreshold = ui->spinboxMaxPolarization->value();
            break;
        case Qt::Unchecked:
            options->minPolThreshold = -1;
            options->maxPolThreshold = -1;
            break;
        case Qt::PartiallyChecked:
            options->minPolThreshold = -1;
            options->maxPolThreshold = -1;
            break;
    }

    emit(optionsChanged(options));

    /* update local information */

    for(int i=0; i<selectedViewports.size(); i++)
    {
        mapOptions *mapOpt = viewportOptions[selectedViewports[i]];

        if(options->tesselationNside!=0)
            mapOpt->tesselationNside = options->tesselationNside;

        if(options->textureNside!=0)
            mapOpt->textureNside = options->textureNside;

        if(options->pVectorsNside!=0)
            mapOpt->pVectorsNside = options->pVectorsNside;

        if(options->vectorsSpacing!=-1)
            mapOpt->vectorsSpacing = options->vectorsSpacing;

        mapOpt->magnification = options->magnification;

        if(ui->checkPVectorsThreshold->checkState()!=Qt::PartiallyChecked)
        {
            mapOpt->minPolThreshold = options->minPolThreshold;
            mapOpt->maxPolThreshold = options->maxPolThreshold;
        }
    }

    delete options;
}



/* PRIVATE */

void MapOptions::fillTesselationNsideCombobox(int min, int max, int currentNside)
{
    int i=0;

    /* clear current items */
    ui->comboTesselationNside->clear();

    ui->comboTesselationNside->insertItem(i++, "", 0);

    for(int nside=min; nside<=max; nside*=2)
    {
        QString valueText;
        valueText.setNum(nside);
        ui->comboTesselationNside->insertItem(i, valueText, nside);

        if(nside == currentNside)
            ui->comboTesselationNside->setCurrentIndex(i);

        i++;
    }

    if(currentNside<8)
        ui->comboTesselationNside->setCurrentIndex(currentNside);
}

void MapOptions::fillTextureNsideCombobox(int min, int max, int currentNside)
{
    int i=0;

    /* clear current items */
    ui->comboTextureNside->clear();

    /* add automatic option */
    ui->comboTextureNside->insertItem(i++, "", 0);
    ui->comboTextureNside->insertItem(i++, "Automatic", 1);

    for(int nside=min; nside<=max; nside*=2)
    {
        QString valueText;
        valueText.setNum(nside);
        ui->comboTextureNside->insertItem(i, valueText, nside);

        if(nside == currentNside)
            ui->comboTextureNside->setCurrentIndex(i);

        i++;
    }

    if(currentNside<8)
        ui->comboTextureNside->setCurrentIndex(currentNside);
}

void MapOptions::fillPVectorsNsideCombobox(int min, int max, int currentNside)
{
    int i=0;

    /* clear current items */
    ui->comboPVectorsNside->clear();

    /* add automatic option */
    ui->comboPVectorsNside->insertItem(i++, "", 0);
    ui->comboPVectorsNside->insertItem(i++, "Automatic", 1);

    for(int nside=min; nside<=max; nside*=2)
    {
        QString valueText;
        valueText.setNum(nside);
        ui->comboPVectorsNside->insertItem(i, valueText, nside);

        if(nside == currentNside)
            ui->comboPVectorsNside->setCurrentIndex(i);

        i++;
    }

    if(currentNside<8)
        ui->comboPVectorsNside->setCurrentIndex(currentNside);
}

void MapOptions::fillPVectorsSpacingCombobox(int min, int max, int value)
{
    int i=0;

    /* clear current items */
    ui->comboVectorsSpacing->clear();

    /* add automatic option */
    ui->comboVectorsSpacing->insertItem(i++, "", -1);

    for(int _value=min; _value<=max; )
    {
        QString valueText;
        valueText.setNum(_value);
        ui->comboVectorsSpacing->insertItem(i, valueText, _value);

        if(value == _value)
            ui->comboVectorsSpacing->setCurrentIndex(i);

        if(_value==0)
            _value = 2;
        else
            _value*=2;

        i++;
    }

    if(value==-1)
        ui->comboVectorsSpacing->setCurrentIndex(0);
}

/* update current options */
void MapOptions::updateOptions()
{
    if(selectedViewports.size()==0)
    {
        this->setDisabled(true);
    }
    else if(selectedViewports.size()==1)
    {
        this->setEnabled(true);

        /* disable tristate */
        ui->checkPVectorsThreshold->setTristate(false);

        mapOptions *options = viewportOptions[selectedViewports[0]];

        /* fill comboboxs */
        fillTesselationNsideCombobox(TESSELATION_MIN_NSIDE, TESSELATION_MAX_NSIDE, options->tesselationNside);
        fillTextureNsideCombobox(options->minNside, options->maxNside, options->textureNside);

        if(!options->controlPolarization)
        {
            /* disable polarization controls */
            ui->checkPVectorsThreshold->setChecked(false);
            ui->checkPVectorsThreshold->setDisabled(true);
            ui->comboPVectorsNside->setDisabled(true);
            ui->spinboxMagnification->setDisabled(true);
            ui->comboVectorsSpacing->setDisabled(true);
        }
        else
        {
            /* enable polization controls */
            ui->checkPVectorsThreshold->setEnabled(true);
            ui->comboPVectorsNside->setEnabled(true);
            ui->spinboxMagnification->setEnabled(true);
            ui->comboVectorsSpacing->setEnabled(true);

            fillPVectorsNsideCombobox(options->minNside, options->maxNside, options->pVectorsNside);
            fillPVectorsSpacingCombobox(MIN_VECTORS_SPACING, MAX_VECTORS_SPACING, options->vectorsSpacing);

            /* update magnification and mag thresholhds */
            ui->spinboxMagnification->setValue(options->magnification);

            if(options->minPolThreshold==-1 && options->maxPolThreshold==-1)
            {
                ui->spinboxMinPolarization->setValue(options->originalMinPolThreshold);
                ui->spinboxMaxPolarization->setValue(options->originalMaxPolThreshold);
                ui->checkPVectorsThreshold->setChecked(false);
            }
            else
            {
                ui->spinboxMinPolarization->setValue(options->minPolThreshold);
                ui->spinboxMaxPolarization->setValue(options->maxPolThreshold);
                ui->checkPVectorsThreshold->setChecked(true);
            }
        }
    }
    else
    {
        bool sameTesselationNside = true;
        bool sameTextureNside = true;
        bool samePVectorsNside = true;
        bool sameMagnification = true;
        bool sameSpacing = true;
        bool allUsingThreshold = true;
        bool allHavePolarization = viewportOptions[selectedViewports[0]]->controlPolarization;
        int tesselationNside = viewportOptions[selectedViewports[0]]->tesselationNside;
        int textureNside = viewportOptions[selectedViewports[0]]->textureNside;
        int pvectorsNside = viewportOptions[selectedViewports[0]]->pVectorsNside;
        int minTextureNside = viewportOptions[selectedViewports[0]]->minNside;
        int maxTextureNside = viewportOptions[selectedViewports[0]]->maxNside;
        int spacing = viewportOptions[selectedViewports[0]]->vectorsSpacing;
        double magnification = viewportOptions[selectedViewports[0]]->magnification;
        double minMag = viewportOptions[selectedViewports[0]]->minPolThreshold;
        double maxMag = viewportOptions[selectedViewports[0]]->maxPolThreshold;

        if(minMag==-1 && maxMag==-1)
            allUsingThreshold = false;

        mapOptions *options;

        for(int i=1; i<selectedViewports.size(); i++)
        {
            options = viewportOptions[selectedViewports[i]];

            if(sameTesselationNside && tesselationNside!=options->tesselationNside)
                sameTesselationNside = false;

            if(sameTextureNside && textureNside!=options->textureNside)
                sameTextureNside = false;

            if(samePVectorsNside && pvectorsNside!=options->pVectorsNside)
                samePVectorsNside = false;

            if(sameSpacing && spacing!=options->vectorsSpacing)
                sameSpacing = false;

            if(allHavePolarization && !options->controlPolarization)
                allHavePolarization = false;

            if(sameMagnification && magnification!=options->magnification)
                sameMagnification = false;

            if(options->minNside > minTextureNside)
                minTextureNside = options->minNside;

            if(options->maxNside < maxTextureNside)
                maxTextureNside = options->maxNside;

            if(allUsingThreshold)
            {
                if(options->minPolThreshold==-1 && options->maxPolThreshold==-1)
                    allUsingThreshold = false;
                else
                {
                    if(options->minPolThreshold < minMag)
                        minMag = options->minPolThreshold;
                    if(options->maxPolThreshold > maxMag)
                        maxMag = options->maxPolThreshold;
                }
            }
        }

        if(!sameTesselationNside)
            tesselationNside = 0;

        if(!sameTextureNside)
            textureNside = 0;

        fillTesselationNsideCombobox(TESSELATION_MIN_NSIDE, TESSELATION_MAX_NSIDE, tesselationNside);
        fillTextureNsideCombobox(minTextureNside, maxTextureNside, textureNside);

        if(allHavePolarization)
        {
            /* enable polization controls */
            ui->checkPVectorsThreshold->setEnabled(true);
            ui->comboPVectorsNside->setEnabled(true);
            ui->spinboxMagnification->setEnabled(true);

            if(!samePVectorsNside)
                pvectorsNside = 0;

            if(!sameSpacing)
                spacing = -1;

            fillPVectorsNsideCombobox(minTextureNside, maxTextureNside, pvectorsNside);
            fillPVectorsSpacingCombobox(MIN_VECTORS_SPACING, MAX_VECTORS_SPACING, spacing);

            if(sameMagnification)
                ui->spinboxMagnification->setValue(magnification);
            else
                ui->spinboxMagnification->setValue(1.0);

            if(allUsingThreshold)
            {
                ui->checkPVectorsThreshold->setCheckState(Qt::Checked);
                ui->spinboxMinPolarization->setValue(minMag);
                ui->spinboxMaxPolarization->setValue(maxMag);
            }
            else
            {
                ui->checkPVectorsThreshold->setCheckState(Qt::PartiallyChecked);
            }
        }
        else
        {
            /* disable polarization controls */
            ui->checkPVectorsThreshold->setChecked(false);
            ui->checkPVectorsThreshold->setDisabled(true);
            ui->comboPVectorsNside->setDisabled(true);
            ui->spinboxMagnification->setDisabled(true);
            ui->comboVectorsSpacing->setDisabled(true);
        }
    }

}



void MapOptions::updateTextureNside(int nside, int viewportId)
{
    //qDebug() << "Options: updating texture nside of viewport " << viewportId << " to " << nside;

    /* save info */
    mapOptions *options = viewportOptions[viewportId];
    options->textureNside = nside;

    /* update combobox */
    textureNsideUpdated();
}

void MapOptions::updateVectorsNside(int nside, int viewportId)
{
    //qDebug() << "Options: updating vectors nside of viewport " << viewportId << " to " << nside;

    /* save info */
    mapOptions *options = viewportOptions[viewportId];
    options->pVectorsNside = nside;

    /* update combobox */
    vectorsNsideUpdated();
}


void MapOptions::textureNsideUpdated()
{
    bool sameTextureNside = true;
    int textureNside = viewportOptions[selectedViewports[0]]->textureNside;
    int minTextureNside = viewportOptions[selectedViewports[0]]->minNside;
    int maxTextureNside = viewportOptions[selectedViewports[0]]->maxNside;

    mapOptions *options;
    for(int i=1; i<selectedViewports.size(); i++)
    {
        options = viewportOptions[selectedViewports[i]];

        if(sameTextureNside && textureNside!=options->textureNside)
            sameTextureNside = false;

        if(options->minNside > minTextureNside)
            minTextureNside = options->minNside;

        if(options->maxNside < maxTextureNside)
            maxTextureNside = options->maxNside;
    }

    if(!sameTextureNside)
        textureNside = 0;

    fillTextureNsideCombobox(minTextureNside, maxTextureNside, textureNside);
}

void MapOptions::vectorsNsideUpdated()
{
    bool sameVectorsNside = true;
    int vectorsNside = viewportOptions[selectedViewports[0]]->pVectorsNside;
    int minTextureNside = viewportOptions[selectedViewports[0]]->minNside;
    int maxTextureNside = viewportOptions[selectedViewports[0]]->maxNside;

    mapOptions *options;
    for(int i=1; i<selectedViewports.size(); i++)
    {
        options = viewportOptions[selectedViewports[i]];

        if(sameVectorsNside && vectorsNside!=options->pVectorsNside)
            sameVectorsNside = false;

        if(options->minNside > minTextureNside)
            minTextureNside = options->minNside;

        if(options->maxNside < maxTextureNside)
            maxTextureNside = options->maxNside;
    }

    if(!sameVectorsNside)
        vectorsNside = 0;

    fillPVectorsNsideCombobox(minTextureNside, maxTextureNside, vectorsNside);
}
