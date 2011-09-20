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
}


void HistogramWidget::updateLowerThreshold(double value)
{
    ui->histogram->buildHistogram(value, getMaxThreshold());
}

void HistogramWidget::updateHigherThreshold(double value)
{
    ui->histogram->buildHistogram(getMinThreshold(), value);
}

void HistogramWidget::setValues(float* values, int nValues)
{
    ui->lowerThreshold->setValue(0.0);
    ui->higherThreshold->setValue(2.0);
    (ui->histogram)->setupHistogram(values, nValues, getMinThreshold(), getMaxThreshold());
}

float HistogramWidget::getMinThreshold()
{
    return ui->lowerThreshold->value();
}

float HistogramWidget::getMaxThreshold()
{
    return ui->higherThreshold->value();
}

HistogramWidget::~HistogramWidget()
{
    delete ui;
}
