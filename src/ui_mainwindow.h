/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Fri Feb 3 13:43:05 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "histogramwidget.h"
#include "mapoptions.h"
#include "viewportmanager.h"
#include "workspace.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QAction *actionExit;
    QAction *actionOpen;
    QAction *actionReset;
    QAction *action3Dview;
    QAction *actionMollview;
    QAction *actionSynchronize;
    QAction *actionColors;
    QAction *actionExtract;
    QAction *actionSelectAll;
    QAction *actionDeselectAll;
    QAction *actionPolarizationVectors;
    QAction *actionShowGrid;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    WorkSpace *workspace;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *histogramTab;
    HistogramWidget *histogramControl;
    QWidget *optionstab;
    MapOptions *mapOptions;
    QFrame *frame;
    QVBoxLayout *verticalLayout_3;
    ViewportManager *viewportManager;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuAbout;
    QStatusBar *statusBar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(679, 1000);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(2);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/3dview.gif"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/open.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName(QString::fromUtf8("actionReset"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/reset.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionReset->setIcon(icon2);
        action3Dview = new QAction(MainWindow);
        action3Dview->setObjectName(QString::fromUtf8("action3Dview"));
        action3Dview->setIcon(icon);
        actionMollview = new QAction(MainWindow);
        actionMollview->setObjectName(QString::fromUtf8("actionMollview"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/mollview.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionMollview->setIcon(icon3);
        actionSynchronize = new QAction(MainWindow);
        actionSynchronize->setObjectName(QString::fromUtf8("actionSynchronize"));
        actionSynchronize->setCheckable(true);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/sync.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionSynchronize->setIcon(icon4);
        actionColors = new QAction(MainWindow);
        actionColors->setObjectName(QString::fromUtf8("actionColors"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/colormap.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionColors->setIcon(icon5);
        actionExtract = new QAction(MainWindow);
        actionExtract->setObjectName(QString::fromUtf8("actionExtract"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/extract.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionExtract->setIcon(icon6);
        actionSelectAll = new QAction(MainWindow);
        actionSelectAll->setObjectName(QString::fromUtf8("actionSelectAll"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/select-all.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionSelectAll->setIcon(icon7);
        actionDeselectAll = new QAction(MainWindow);
        actionDeselectAll->setObjectName(QString::fromUtf8("actionDeselectAll"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/deselect-all.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionDeselectAll->setIcon(icon8);
        actionPolarizationVectors = new QAction(MainWindow);
        actionPolarizationVectors->setObjectName(QString::fromUtf8("actionPolarizationVectors"));
        actionPolarizationVectors->setCheckable(true);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/pol-vectors.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionPolarizationVectors->setIcon(icon9);
        actionShowGrid = new QAction(MainWindow);
        actionShowGrid->setObjectName(QString::fromUtf8("actionShowGrid"));
        actionShowGrid->setCheckable(true);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/grid.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionShowGrid->setIcon(icon10);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        centralWidget->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        workspace = new WorkSpace(splitter);
        workspace->setObjectName(QString::fromUtf8("workspace"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(4);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(workspace->sizePolicy().hasHeightForWidth());
        workspace->setSizePolicy(sizePolicy2);
        splitter->addWidget(workspace);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(layoutWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(4);
        sizePolicy3.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy3);
        tabWidget->setMinimumSize(QSize(300, 316));
        tabWidget->setMaximumSize(QSize(300, 16777215));
        histogramTab = new QWidget();
        histogramTab->setObjectName(QString::fromUtf8("histogramTab"));
        sizePolicy1.setHeightForWidth(histogramTab->sizePolicy().hasHeightForWidth());
        histogramTab->setSizePolicy(sizePolicy1);
        histogramControl = new HistogramWidget(histogramTab);
        histogramControl->setObjectName(QString::fromUtf8("histogramControl"));
        histogramControl->setEnabled(true);
        histogramControl->setGeometry(QRect(9, 9, 278, 884));
        sizePolicy1.setHeightForWidth(histogramControl->sizePolicy().hasHeightForWidth());
        histogramControl->setSizePolicy(sizePolicy1);
        histogramControl->setStyleSheet(QString::fromUtf8(""));
        tabWidget->addTab(histogramTab, QString());
        optionstab = new QWidget();
        optionstab->setObjectName(QString::fromUtf8("optionstab"));
        sizePolicy1.setHeightForWidth(optionstab->sizePolicy().hasHeightForWidth());
        optionstab->setSizePolicy(sizePolicy1);
        mapOptions = new MapOptions(optionstab);
        mapOptions->setObjectName(QString::fromUtf8("mapOptions"));
        mapOptions->setGeometry(QRect(10, 10, 280, 391));
        sizePolicy1.setHeightForWidth(mapOptions->sizePolicy().hasHeightForWidth());
        mapOptions->setSizePolicy(sizePolicy1);
        mapOptions->setMinimumSize(QSize(280, 0));
        tabWidget->addTab(optionstab, QString());

        verticalLayout_2->addWidget(tabWidget);

        frame = new QFrame(layoutWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setMinimumSize(QSize(300, 0));
        frame->setMaximumSize(QSize(300, 16777215));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        viewportManager = new ViewportManager(frame);
        viewportManager->setObjectName(QString::fromUtf8("viewportManager"));
        sizePolicy1.setHeightForWidth(viewportManager->sizePolicy().hasHeightForWidth());
        viewportManager->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(viewportManager);


        verticalLayout_2->addWidget(frame);

        splitter->addWidget(layoutWidget);

        horizontalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 679, 26));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QString::fromUtf8("menuAbout"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(toolBar->sizePolicy().hasHeightForWidth());
        toolBar->setSizePolicy(sizePolicy4);
        toolBar->setMinimumSize(QSize(40, 300));
        toolBar->setAutoFillBackground(false);
        MainWindow->addToolBar(Qt::LeftToolBarArea, toolBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionExit);
        menuAbout->addAction(actionAbout);
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionReset);
        toolBar->addAction(action3Dview);
        toolBar->addAction(actionMollview);
        toolBar->addAction(actionSynchronize);
        toolBar->addAction(actionColors);
        toolBar->addAction(actionPolarizationVectors);
        toolBar->addAction(actionShowGrid);
        toolBar->addAction(actionSelectAll);
        toolBar->addAction(actionDeselectAll);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Healpix Viewer", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen->setText(QApplication::translate("MainWindow", "Open", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen->setToolTip(QApplication::translate("MainWindow", "Open Map", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionReset->setText(QApplication::translate("MainWindow", "Reset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionReset->setToolTip(QApplication::translate("MainWindow", "&Reset Views", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action3Dview->setText(QApplication::translate("MainWindow", "3D view", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action3Dview->setToolTip(QApplication::translate("MainWindow", "Switch to 3D", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionMollview->setText(QApplication::translate("MainWindow", "Mollview", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionMollview->setToolTip(QApplication::translate("MainWindow", "Switch to Mollview", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSynchronize->setText(QApplication::translate("MainWindow", "Synchronize views", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSynchronize->setToolTip(QApplication::translate("MainWindow", "Synchronize views", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionColors->setText(QApplication::translate("MainWindow", "Colors", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionColors->setToolTip(QApplication::translate("MainWindow", "Show Options", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExtract->setText(QApplication::translate("MainWindow", "Extract", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionExtract->setToolTip(QApplication::translate("MainWindow", "Extract ROI", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSelectAll->setText(QApplication::translate("MainWindow", "Select All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSelectAll->setToolTip(QApplication::translate("MainWindow", "Select All Viewports", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionDeselectAll->setText(QApplication::translate("MainWindow", "Deselect All", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionDeselectAll->setToolTip(QApplication::translate("MainWindow", "Deselect All", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionPolarizationVectors->setText(QApplication::translate("MainWindow", "Polarization Vectors", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionPolarizationVectors->setToolTip(QApplication::translate("MainWindow", "Show/Hide Polarization Vectors", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionShowGrid->setText(QApplication::translate("MainWindow", "Grid", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionShowGrid->setToolTip(QApplication::translate("MainWindow", "Show Grid", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(histogramTab), QApplication::translate("MainWindow", "Histogram", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(optionstab), QApplication::translate("MainWindow", "Options", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        menuAbout->setTitle(QApplication::translate("MainWindow", "Help", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "ToolBar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
