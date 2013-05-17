#ifndef HISTOGRAMWIDGET_H
#define HISTOGRAMWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QString>
#include "healpixmap.h"
#include "histogramviewer.h"
#include "mapviewer.h"
#include "colormap.h"
#include "configs.h"
#include "types.h"


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
    /* redraw histogram with new values */
    void updateHistogram();
    void updateHistogram(QList<int> selectedViewports);
    /* update viewport information */
    void updateMapInfo(int viewportId, mapInfo* info);
    /* unload viewport info */
    void unloadMapInfo(int viewportId);

private slots:
    /* called by lowerThreshold spinbox */
    void updateLowerThreshold(double value);
    /* called by higherThreshold spinbox */
    void updateHigherThreshold(double value);
    /* connected to apply button */
    void updateMap();
    /* called when colormap selector changes */
    void colorMapUpdate(int idx);
    /* called when scale selector changes */
    void scaleUpdate(int idx);
    /* update the color used to sentinel pixels */
    void sentinelColorUpdate();

signals:
    void histogramUpdated(ColorMap* colorMap, float min, float max, QColor sentinelColor, ScaleType scale, float factor, float offset);

private:
    Ui::HistogramWidget *ui;
    ColorMap *currentColorMap;
    ScaleType currentScale;

    float min, max;
    float factor, offset;

    /* rebuild histogram with new threshold */
    void updateHistogramThreshold();

    void fillColorMaps();
    void fillScaleOptions();
    void updateColorMap(ColorMap* cm);
    void updateScale(ScaleType scale);
    void updateSentinelColor(QColor color);
    void updateColorMapSelector(ColorMap *colorMap);
    void updateScaleSelector(ScaleType scale);
    void updateFactor(float factor);
    void updateOffset(float offset);

    ColorMap* getSelectedColorMap();

    /* called when a single map is selected */
    void setThresholds(float min, float max);

    QColor currentSentinelColor;
    QMap<int, mapInfo*> mapsInformation;
    QList<int> selectedViewports;
};

#endif // HISTOGRAMWIDGET_H
