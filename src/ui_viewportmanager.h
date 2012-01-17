/********************************************************************************
** Form generated from reading UI file 'viewportmanager.ui'
**
** Created: Mon Jan 16 17:16:34 2012
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWPORTMANAGER_H
#define UI_VIEWPORTMANAGER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewportManager
{
public:
    QVBoxLayout *verticalLayout;
    QTreeWidget *treeViewports;
    QHBoxLayout *iconLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOpen;
    QPushButton *btnClose;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QComboBox *mapFieldSelector;

    void setupUi(QWidget *ViewportManager)
    {
        if (ViewportManager->objectName().isEmpty())
            ViewportManager->setObjectName(QString::fromUtf8("ViewportManager"));
        ViewportManager->resize(318, 300);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ViewportManager->sizePolicy().hasHeightForWidth());
        ViewportManager->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(ViewportManager);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        treeViewports = new QTreeWidget(ViewportManager);
        treeViewports->setObjectName(QString::fromUtf8("treeViewports"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(treeViewports->sizePolicy().hasHeightForWidth());
        treeViewports->setSizePolicy(sizePolicy1);
        treeViewports->setMaximumSize(QSize(16777215, 200));
        treeViewports->setFrameShape(QFrame::NoFrame);
        treeViewports->setAnimated(false);
        treeViewports->setColumnCount(3);
        treeViewports->header()->setVisible(true);

        verticalLayout->addWidget(treeViewports);

        iconLayout = new QHBoxLayout();
        iconLayout->setSpacing(0);
        iconLayout->setObjectName(QString::fromUtf8("iconLayout"));
        iconLayout->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        iconLayout->addItem(horizontalSpacer);

        btnOpen = new QPushButton(ViewportManager);
        btnOpen->setObjectName(QString::fromUtf8("btnOpen"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(btnOpen->sizePolicy().hasHeightForWidth());
        btnOpen->setSizePolicy(sizePolicy2);
        btnOpen->setLayoutDirection(Qt::RightToLeft);

        iconLayout->addWidget(btnOpen);

        btnClose = new QPushButton(ViewportManager);
        btnClose->setObjectName(QString::fromUtf8("btnClose"));
        sizePolicy2.setHeightForWidth(btnClose->sizePolicy().hasHeightForWidth());
        btnClose->setSizePolicy(sizePolicy2);
        btnClose->setLayoutDirection(Qt::RightToLeft);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/trash.gif"), QSize(), QIcon::Normal, QIcon::On);
        btnClose->setIcon(icon);

        iconLayout->addWidget(btnClose);


        verticalLayout->addLayout(iconLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(ViewportManager);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        mapFieldSelector = new QComboBox(ViewportManager);
        mapFieldSelector->setObjectName(QString::fromUtf8("mapFieldSelector"));
        mapFieldSelector->setEnabled(false);
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(mapFieldSelector->sizePolicy().hasHeightForWidth());
        mapFieldSelector->setSizePolicy(sizePolicy3);

        horizontalLayout_2->addWidget(mapFieldSelector);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(ViewportManager);

        QMetaObject::connectSlotsByName(ViewportManager);
    } // setupUi

    void retranslateUi(QWidget *ViewportManager)
    {
        ViewportManager->setWindowTitle(QApplication::translate("ViewportManager", "Form", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeViewports->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("ViewportManager", "Available Maps", 0, QApplication::UnicodeUTF8));
        btnOpen->setText(QString());
        btnClose->setText(QString());
        label->setText(QApplication::translate("ViewportManager", "Map Field:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ViewportManager: public Ui_ViewportManager {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWPORTMANAGER_H
