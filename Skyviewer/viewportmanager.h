#ifndef VIEWPORTMANAGER_H
#define VIEWPORTMANAGER_H

#include <QWidget>
#include <QSignalMapper>
#include "mapviewport.h"

#define MAXVIEWPORTS 20

namespace Ui {
    class ViewportManager;
}

class ViewportManager : public QWidget
{
    Q_OBJECT

public:
    explicit ViewportManager(QWidget *parent = 0);
    ~ViewportManager();

signals:
    void viewportShowOn(int viewportId, MapViewport *viewport); /* connect to Workspace::addViewport */
    void viewportShowOff(int viewportId);                       /* connect to Workspace::removeViewport */
    void histogramViewportsSelectedUpdated(QList<int> viewports);        /* connect to HistoWidget::updateHistogram */
    void mapUpdated(int viewportId, mapInfo *info);             /* connect to HistoWidget::updateMapInfo */
    void mapClosed(int viewportId);                             /* connect to HistoWidget::unloadMapInfo */

public slots:
    /* called when open button pressed */
    void openFiles();
    /* called when threshold updated in histogramWidget */
    void updateThresholds(float min, float max);
    //void updateColorTable()

private slots:
    /* connected to map field combobox */
    void mapFieldChanged(int currentIdx);
    /* called when trash button clicked */
    void closeViewports();
    /* called when treeview selection changed */
    void selectionChanged(void);
    /* called when treeview histogram selection changed */
    void histogramSelectionChanged(QTreeWidgetItem *, int);
    /* called when view state changes */
    void viewStateChanged(QWidget *item);
    /* called when viewport has new values */
    void updateMapInfo(int viewportId, mapInfo *info);

    void closeViewport(int viewportId);

private:
    Ui::ViewportManager *ui;
    int usedViewports;
    int viewportIdx;
    int currentSelectedViewport;
    QMap<int, MapViewport*> viewports;
    QMap<int, mapInfo*> mapsInformation;
    QString defaultTitle;
    QSignalMapper *signalMapper;
    bool isInWorkspace(int viewportId);
    QSet<int> displayedViewports;

    /* open files and create viewports */
    void openFiles(QStringList filenames);
    /* load map information for viewport */
    //void loadMapInfo(int viewportId);
    /* return list of histogram checked viewports in treeview */
    QList<int> getHistogramActiveViewports();
    /* return list of workspace checked viewports in treeview */
    QList<int> getWorkspaceActiveViewports();
    /* return list of selected viewports in treeview */
    QList<int> getSelectedViewports();

    /* update field selector with the available fields */
    void updateFieldSelector(int viewportId);
    /* update map to use this field */
    void updateMapField(int viewportId, HealpixMap::MapType field);

    void showViewport(int viewportId);
    void hideViewport(int viewporId);

    void addViewportToList(int viewportId, QString title, mapInfo *info);   // OK
    void removeViewportFromList(int viewportId);                            // OK
};

#endif // VIEWPORTMANAGER_H
