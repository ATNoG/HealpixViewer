#include "mapviewport.h"

MapViewport::MapViewport(QWidget *parent, QString title) :
    QWidget(parent)
{
    this->title = title;
    this->selected = false;  // by default viewport is not selected

    /* create toolbar */
    toolbar = new QToolBar;

    /* create the layout */
    hboxtoplayout = new QHBoxLayout;
    hboxtoplayout->setMargin(6);
    hboxbottomlayout = new QHBoxLayout;
    hboxbottomlayout->setMargin(0);
    vboxlayout = new QVBoxLayout;
    vboxlayout->setSpacing(0);
    vboxlayout->setMargin(5);   // set spacing between viewports
    setLayout(vboxlayout);

    titlewidget = new QWidget;
    titlewidget->setStyleSheet(QString("background-color: #c3c3c3; border-top-right-radius: 9px; border-top-left-radius: 9px; "));
    titlewidget->setMinimumHeight(24);
    titlewidget->setMaximumHeight(30);
    titlewidget->setLayout(hboxtoplayout);

    areawidget = new QWidget;
    areawidget->setLayout(hboxbottomlayout);

    /* create the widgets: title, checkbox and opengl area */
    QLabel* lbltitle = new QLabel;
    lbltitle->setText(title);
    QCheckBox* checkbox = new QCheckBox;
    QGLWidget* glwidget = new QGLWidget;

    /* add widgets */
    hboxtoplayout->addWidget(lbltitle);
    hboxtoplayout->addWidget(checkbox, 0, Qt::AlignRight);
    hboxbottomlayout->addWidget(glwidget);
    //hboxbottomlayout->addWidget(toolbar);
    vboxlayout->addWidget(titlewidget);
    vboxlayout->addWidget(areawidget);

    /* configure the size policy */
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /* connect events */
    connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(selectionChanged(bool)));
}


void MapViewport::loadHealpixMap(QString path)
{
    /* create new healpixmap */
    //healpixMap(path);

    // TODO: verify if no map is currently loaded
}


bool MapViewport::isSelected()
{
    return selected;
}

void MapViewport::selectViewport()
{
    selected = true;

    /* change titlebar color */
    // TODO: dont rewrite all properties again
    titlewidget->setStyleSheet(QString("background-color: %1; border-top-right-radius: 9px; border-top-left-radius: 9px; ").arg(COLOR_SELECTED));
}

void MapViewport::deselectViewport()
{
    selected = false;

    /* change titlebar color */
    // TODO: dont rewrite all properties again
    titlewidget->setStyleSheet(QString("background-color: %1; border-top-right-radius: 9px; border-top-left-radius: 9px; ").arg(COLOR_INACTIVE));
}


/* Slots */
void MapViewport::selectionChanged(bool selected)
{
    if(selected)
        selectViewport();
    else
        deselectViewport();
}
