/********************************************************************************
** Form generated from reading UI file 'mapoptions.ui'
**
** Created: Mon Jan 16 17:16:34 2012
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAPOPTIONS_H
#define UI_MAPOPTIONS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
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

QT_BEGIN_NAMESPACE

class Ui_MapOptions
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QComboBox *comboTextureNside;
    QLabel *label;
    QLabel *label_2;
    QComboBox *comboTesselationNside;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_3;
    QGridLayout *gridLayout_2;
    QLabel *label_4;
    QComboBox *comboPVectorsNside;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout;
    QDoubleSpinBox *spinboxMinPolarization;
    QDoubleSpinBox *spinboxMaxPolarization;
    QLabel *label_6;
    QDoubleSpinBox *spinboxMagnification;
    QCheckBox *checkPVectorsThreshold;
    QLabel *label_7;
    QComboBox *comboVectorsSpacing;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *applyButton;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *MapOptions)
    {
        if (MapOptions->objectName().isEmpty())
            MapOptions->setObjectName(QString::fromUtf8("MapOptions"));
        MapOptions->resize(289, 368);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MapOptions->sizePolicy().hasHeightForWidth());
        MapOptions->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(MapOptions);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        comboTextureNside = new QComboBox(MapOptions);
        comboTextureNside->setObjectName(QString::fromUtf8("comboTextureNside"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboTextureNside->sizePolicy().hasHeightForWidth());
        comboTextureNside->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(comboTextureNside, 1, 1, 1, 1);

        label = new QLabel(MapOptions);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 1, 0, 1, 1);

        label_2 = new QLabel(MapOptions);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        comboTesselationNside = new QComboBox(MapOptions);
        comboTesselationNside->setObjectName(QString::fromUtf8("comboTesselationNside"));
        sizePolicy1.setHeightForWidth(comboTesselationNside->sizePolicy().hasHeightForWidth());
        comboTesselationNside->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(comboTesselationNside, 0, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer_2 = new QSpacerItem(17, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_2);

        label_3 = new QLabel(MapOptions);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QFont font;
        font.setUnderline(true);
        label_3->setFont(font);

        verticalLayout->addWidget(label_3);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_4 = new QLabel(MapOptions);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        comboPVectorsNside = new QComboBox(MapOptions);
        comboPVectorsNside->setObjectName(QString::fromUtf8("comboPVectorsNside"));
        sizePolicy1.setHeightForWidth(comboPVectorsNside->sizePolicy().hasHeightForWidth());
        comboPVectorsNside->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(comboPVectorsNside, 0, 1, 1, 2);

        label_5 = new QLabel(MapOptions);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(label_5, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        spinboxMinPolarization = new QDoubleSpinBox(MapOptions);
        spinboxMinPolarization->setObjectName(QString::fromUtf8("spinboxMinPolarization"));
        spinboxMinPolarization->setEnabled(false);
        spinboxMinPolarization->setDecimals(3);
        spinboxMinPolarization->setMinimum(-1);
        spinboxMinPolarization->setMaximum(1);
        spinboxMinPolarization->setSingleStep(0.002);

        horizontalLayout->addWidget(spinboxMinPolarization);

        spinboxMaxPolarization = new QDoubleSpinBox(MapOptions);
        spinboxMaxPolarization->setObjectName(QString::fromUtf8("spinboxMaxPolarization"));
        spinboxMaxPolarization->setEnabled(false);
        spinboxMaxPolarization->setDecimals(3);
        spinboxMaxPolarization->setMinimum(-1);
        spinboxMaxPolarization->setMaximum(1);
        spinboxMaxPolarization->setSingleStep(0.002);

        horizontalLayout->addWidget(spinboxMaxPolarization);


        gridLayout_2->addLayout(horizontalLayout, 1, 2, 1, 1);

        label_6 = new QLabel(MapOptions);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 2, 0, 1, 1);

        spinboxMagnification = new QDoubleSpinBox(MapOptions);
        spinboxMagnification->setObjectName(QString::fromUtf8("spinboxMagnification"));
        spinboxMagnification->setMinimum(0.5);
        spinboxMagnification->setMaximum(4);
        spinboxMagnification->setSingleStep(0.02);
        spinboxMagnification->setValue(1);

        gridLayout_2->addWidget(spinboxMagnification, 2, 1, 1, 2);

        checkPVectorsThreshold = new QCheckBox(MapOptions);
        checkPVectorsThreshold->setObjectName(QString::fromUtf8("checkPVectorsThreshold"));
        checkPVectorsThreshold->setTristate(true);

        gridLayout_2->addWidget(checkPVectorsThreshold, 1, 1, 1, 1);

        label_7 = new QLabel(MapOptions);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 3, 0, 1, 1);

        comboVectorsSpacing = new QComboBox(MapOptions);
        comboVectorsSpacing->setObjectName(QString::fromUtf8("comboVectorsSpacing"));

        gridLayout_2->addWidget(comboVectorsSpacing, 3, 1, 1, 2);


        verticalLayout->addLayout(gridLayout_2);

        verticalSpacer_3 = new QSpacerItem(20, 6, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        applyButton = new QPushButton(MapOptions);
        applyButton->setObjectName(QString::fromUtf8("applyButton"));
        applyButton->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_2->addWidget(applyButton);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(MapOptions);

        QMetaObject::connectSlotsByName(MapOptions);
    } // setupUi

    void retranslateUi(QWidget *MapOptions)
    {
        MapOptions->setWindowTitle(QApplication::translate("MapOptions", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MapOptions", "Texture Nside", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MapOptions", "Sphere Nside", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MapOptions", "Polarization Vectors", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MapOptions", "Nside", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MapOptions", "Threshold", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MapOptions", "Magnification", 0, QApplication::UnicodeUTF8));
        checkPVectorsThreshold->setText(QString());
        label_7->setText(QApplication::translate("MapOptions", "Spacing", 0, QApplication::UnicodeUTF8));
        applyButton->setText(QApplication::translate("MapOptions", "Apply", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MapOptions: public Ui_MapOptions {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAPOPTIONS_H
