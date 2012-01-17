/********************************************************************************
** Form generated from reading UI file 'histogramwidget.ui'
**
** Created: Mon Jan 16 17:16:34 2012
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HISTOGRAMWIDGET_H
#define UI_HISTOGRAMWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "histogramviewer.h"

QT_BEGIN_NAMESPACE

class Ui_HistogramWidget
{
public:
    QVBoxLayout *verticalLayout;
    HistogramViewer *histogram;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QDoubleSpinBox *higherThreshold;
    QDoubleSpinBox *lowerThreshold;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *colorMapSelector;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *applyHistogram;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *HistogramWidget)
    {
        if (HistogramWidget->objectName().isEmpty())
            HistogramWidget->setObjectName(QString::fromUtf8("HistogramWidget"));
        HistogramWidget->resize(318, 600);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(HistogramWidget->sizePolicy().hasHeightForWidth());
        HistogramWidget->setSizePolicy(sizePolicy);
        HistogramWidget->setMinimumSize(QSize(0, 600));
        verticalLayout = new QVBoxLayout(HistogramWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        histogram = new HistogramViewer(HistogramWidget);
        histogram->setObjectName(QString::fromUtf8("histogram"));
        histogram->setMinimumSize(QSize(300, 150));
        histogram->setMaximumSize(QSize(300, 150));
        histogram->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 255);"));

        verticalLayout->addWidget(histogram);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        label_3 = new QLabel(HistogramWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_3, 0, 3, 1, 1);

        higherThreshold = new QDoubleSpinBox(HistogramWidget);
        higherThreshold->setObjectName(QString::fromUtf8("higherThreshold"));
        higherThreshold->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(higherThreshold->sizePolicy().hasHeightForWidth());
        higherThreshold->setSizePolicy(sizePolicy1);
        higherThreshold->setAccelerated(true);
        higherThreshold->setMinimum(-150);
        higherThreshold->setMaximum(400000);
        higherThreshold->setSingleStep(1e-06);

        gridLayout->addWidget(higherThreshold, 1, 3, 1, 1);

        lowerThreshold = new QDoubleSpinBox(HistogramWidget);
        lowerThreshold->setObjectName(QString::fromUtf8("lowerThreshold"));
        lowerThreshold->setEnabled(false);
        sizePolicy1.setHeightForWidth(lowerThreshold->sizePolicy().hasHeightForWidth());
        lowerThreshold->setSizePolicy(sizePolicy1);
        lowerThreshold->setAccelerated(true);
        lowerThreshold->setMinimum(-150);
        lowerThreshold->setMaximum(400000);
        lowerThreshold->setSingleStep(1e-06);

        gridLayout->addWidget(lowerThreshold, 1, 0, 1, 1);

        label_2 = new QLabel(HistogramWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        colorMapSelector = new QComboBox(HistogramWidget);
        colorMapSelector->setObjectName(QString::fromUtf8("colorMapSelector"));
        colorMapSelector->setEnabled(false);
        sizePolicy1.setHeightForWidth(colorMapSelector->sizePolicy().hasHeightForWidth());
        colorMapSelector->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(colorMapSelector);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        applyHistogram = new QPushButton(HistogramWidget);
        applyHistogram->setObjectName(QString::fromUtf8("applyHistogram"));
        applyHistogram->setEnabled(false);
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(applyHistogram->sizePolicy().hasHeightForWidth());
        applyHistogram->setSizePolicy(sizePolicy2);

        horizontalLayout_3->addWidget(applyHistogram);


        verticalLayout->addLayout(horizontalLayout_3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(HistogramWidget);

        QMetaObject::connectSlotsByName(HistogramWidget);
    } // setupUi

    void retranslateUi(QWidget *HistogramWidget)
    {
        HistogramWidget->setWindowTitle(QApplication::translate("HistogramWidget", "Form", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("HistogramWidget", "Max", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("HistogramWidget", "Min", 0, QApplication::UnicodeUTF8));
        applyHistogram->setText(QApplication::translate("HistogramWidget", "Apply", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class HistogramWidget: public Ui_HistogramWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HISTOGRAMWIDGET_H
