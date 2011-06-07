/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Jun 6 10:59:27 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QAction *actionExit;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionReset;
    QAction *action3Dview;
    QAction *actionMollview;
    QAction *actionViewports;
    QAction *actionSynchronize;
    QAction *actionColors;
    QAction *actionExtract;
    QWidget *centralWidget;
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
        MainWindow->resize(679, 384);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionReset = new QAction(MainWindow);
        actionReset->setObjectName(QString::fromUtf8("actionReset"));
        action3Dview = new QAction(MainWindow);
        action3Dview->setObjectName(QString::fromUtf8("action3Dview"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icons/3dview.gif"), QSize(), QIcon::Normal, QIcon::Off);
        action3Dview->setIcon(icon);
        actionMollview = new QAction(MainWindow);
        actionMollview->setObjectName(QString::fromUtf8("actionMollview"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/icons/mollview.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionMollview->setIcon(icon1);
        actionViewports = new QAction(MainWindow);
        actionViewports->setObjectName(QString::fromUtf8("actionViewports"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/icons/viewports.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionViewports->setIcon(icon2);
        actionSynchronize = new QAction(MainWindow);
        actionSynchronize->setObjectName(QString::fromUtf8("actionSynchronize"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/images/icons/sync.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionSynchronize->setIcon(icon3);
        actionColors = new QAction(MainWindow);
        actionColors->setObjectName(QString::fromUtf8("actionColors"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/images/icons/colormap.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionColors->setIcon(icon4);
        actionExtract = new QAction(MainWindow);
        actionExtract->setObjectName(QString::fromUtf8("actionExtract"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/images/icons/extract.gif"), QSize(), QIcon::Normal, QIcon::Off);
        actionExtract->setIcon(icon5);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 170, 255);"));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 679, 25));
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
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(toolBar->sizePolicy().hasHeightForWidth());
        toolBar->setSizePolicy(sizePolicy1);
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
        toolBar->addAction(actionSave);
        toolBar->addAction(actionReset);
        toolBar->addAction(action3Dview);
        toolBar->addAction(actionMollview);
        toolBar->addAction(actionViewports);
        toolBar->addAction(actionSynchronize);
        toolBar->addAction(actionColors);
        toolBar->addAction(actionExtract);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Healpix Viewer", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionAbout->setToolTip(QApplication::translate("MainWindow", "&About", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExit->setText(QApplication::translate("MainWindow", "Exit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("MainWindow", "&Exit", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionOpen->setText(QApplication::translate("MainWindow", "&Open", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionOpen->setToolTip(QApplication::translate("MainWindow", "&Open Map", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSave->setText(QApplication::translate("MainWindow", "&Save", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSave->setToolTip(QApplication::translate("MainWindow", "&Save", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionReset->setText(QApplication::translate("MainWindow", "&Reset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionReset->setToolTip(QApplication::translate("MainWindow", "&Reset Views", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action3Dview->setText(QApplication::translate("MainWindow", "&3D view", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action3Dview->setToolTip(QApplication::translate("MainWindow", "&Switch to 3D", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionMollview->setText(QApplication::translate("MainWindow", "&Mollview", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionMollview->setToolTip(QApplication::translate("MainWindow", "&Switch to Mollview", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionViewports->setText(QApplication::translate("MainWindow", "&Viewports", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionViewports->setToolTip(QApplication::translate("MainWindow", "&Choose Viewports", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSynchronize->setText(QApplication::translate("MainWindow", "&Synchronize views", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSynchronize->setToolTip(QApplication::translate("MainWindow", "&Synchronize views", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionColors->setText(QApplication::translate("MainWindow", "&Colors", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionColors->setToolTip(QApplication::translate("MainWindow", "&Select colors", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionExtract->setText(QApplication::translate("MainWindow", "&Extract", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionExtract->setToolTip(QApplication::translate("MainWindow", "&Extract ROI", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
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
