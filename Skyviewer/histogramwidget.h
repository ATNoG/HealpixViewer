#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QString>
#include "healpixmap.h"
#include "histogramviewer.h"
#include "mapviewer.h"


namespace Ui {
    class HistogramWidget;
}

class HistogramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistogramWidget(QWidget *parent = 0);
    ~HistogramWidget();

public slots:
    /* called when new viewport opened in workspace */
    void addViewport(int viewportId, QString name, mapInfo *info);
    /* called when viewport closed */
    void removeViewport(int viewportId);
    /* load map information for viewport */
    void loadViewportInfo(int viewportId);

private slots:
    /* called by lowerThreshold spinbox */
    void updateLowerThreshold(double value);
    /* called by higherThreshold spinbox */
    void updateHigherThreshold(double value);
    /* called when treeview selection changed */
    void selectionChanged(QTreeWidgetItem *item=NULL, int column=0);
    /* connected to apply button */
    void updateMap();
    /* connected to map field combobox */
    void changeMapField(int currentIdx);

signals:
    void thresholdUpdated(QList<int> viewports, float min, float max);
    void mapFieldChanged(int viewportId, HealpixMap::MapType field);


private:
    Ui::HistogramWidget *ui;

    float min, max;

    void updateFieldSelector(int viewportId);

    /* rebuild histogram with new threshold */
    void updateHistogramThreshold();

    /* redraw histogram with new values */
    void updateHistogram();

    /* called when a single map is selected */
    void setThresholds(float min, float max);

    /* return list of selected viewports in treeview */
    QList<int> getSelectedViewports();

    QMap<int, mapInfo*> mapsInformation;

    int currentSelectedViewport;
};

#endif // HISTOGRAMWIDGET_H
