#include "gridoptions.h"
#include "ui_gridoptions.h"

GridOptions::GridOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GridOptions)
{
    ui->setupUi(this);

    /* default color */
    currentColor = QColor("white");

    /* connect apply button */
    connect(ui->applyButton, SIGNAL(released()), this, SLOT(applyOptions()));
    /* connect color button */
    connect(ui->colorButton, SIGNAL(released()), this, SLOT(colorUpdate()));
    /* connect automatic checkbox */
    connect(ui->automaticCheckbox, SIGNAL(stateChanged(int)), this, SLOT(automaticChanged(int)));

    /* load default values */
    ui->meridiansSelector->setValue(GRID_MERIDIANS);
    ui->parallelsSelector->setValue(GRID_PARALLELS);
    ui->labelSizeSelector->setValue(GRID_LABEL_SIZE);
    ui->labellingCheckbox->setChecked(GRID_LABELING);

    /* set the color in button */
    QStyle* style = new QWindowsStyle;
    ui->colorButton->setStyle(style);
    ui->colorButton->setPalette(QPalette(currentColor));

    automatic = true;

    /* disable controllers because no viewport is selected */
    disableControllers();
}

GridOptions::~GridOptions()
{
    delete ui;
}

void GridOptions::applyOptions()
{
    gridOptions *options = new gridOptions;
    options->automatic = automatic;
    options->parallels = ui->parallelsSelector->value();
    options->meridians = ui->meridiansSelector->value();
    options->color = currentColor;
    options->labeling = ui->labellingCheckbox->isChecked();
    options->labelSize = ui->labelSizeSelector->value();
    emit(gridOptionsChanged(options));
}

void GridOptions::colorUpdate()
{
    QColor selectedColor = QColorDialog::getColor(currentColor);
    if(selectedColor.isValid())
    {
        currentColor = selectedColor;
        ui->colorButton->setPalette(QPalette(currentColor));
    }
}

void GridOptions::updateSelectedMaps(QList<int> selectedViewports)
{
    this->selectedViewports = selectedViewports;
    // TODO: load grid info about those maps

    if(selectedViewports.size()>0)
        enableControllers();
    else
        disableControllers();
}

void GridOptions::enableControllers()
{
    if(automatic)
    {
        ui->meridiansSelector->setEnabled(false);
        ui->parallelsSelector->setEnabled(false);
    }
    else
    {
        ui->meridiansSelector->setEnabled(true);
        ui->parallelsSelector->setEnabled(true);
    }
    ui->colorButton->setEnabled(true);
    ui->labellingCheckbox->setEnabled(true);
    ui->labelSizeSelector->setEnabled(true);
    ui->applyButton->setEnabled(true);
    ui->automaticCheckbox->setEnabled(true);
}

void GridOptions::disableControllers()
{
    ui->meridiansSelector->setEnabled(false);
    ui->parallelsSelector->setEnabled(false);
    ui->colorButton->setEnabled(false);
    ui->labellingCheckbox->setEnabled(false);
    ui->labelSizeSelector->setEnabled(false);
    ui->applyButton->setEnabled(false);
    ui->automaticCheckbox->setEnabled(false);
}

void GridOptions::automaticChanged(int status)
{
    if(status == Qt::Checked)
    {
        ui->meridiansSelector->setEnabled(false);
        ui->parallelsSelector->setEnabled(false);
        automatic = true;
    }
    else
    {
        ui->meridiansSelector->setEnabled(true);
        ui->parallelsSelector->setEnabled(true);
        automatic = false;
    }
}
