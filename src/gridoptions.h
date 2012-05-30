#ifndef GRIDOPTIONS_H
#define GRIDOPTIONS_H

#include <QWidget>
#include <QColor>
#include <QColorDialog>
#include <QDebug>
#include <QStyle>
#include <QWindowsStyle>
#include "configs.h"
#include "mapviewer.h"

namespace Ui {
class GridOptions;
}

class GridOptions : public QWidget
{
    Q_OBJECT
    
public:
    explicit GridOptions(QWidget *parent = 0);
    ~GridOptions();

public slots:
    void updateSelectedMaps(QList<int> selectedViewports);

signals:
    void gridOptionsChanged(gridOptions *options);

private slots:
    void applyOptions();
    void colorUpdate();
    
private:
    Ui::GridOptions *ui;
    QList<int> selectedViewports;

    void enableControllers();
    void disableControllers();

    QColor currentColor;
};

#endif // GRIDOPTIONS_H
