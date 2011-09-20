#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include "histogramviewer.h"

namespace Ui {
    class HistogramWidget;
}

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramWidget(QWidget *parent = 0);
    ~HistogramWidget();

    void setValues(float* values, int nValues);

public slots:
    void updateLowerThreshold(double value);
    void updateHigherThreshold(double value);


private:
    Ui::HistogramWidget *ui;

    float getMinThreshold();
    float getMaxThreshold();
};

#endif // HISTOGRAMWIDGET_H
