/********************************************************************************
** Form generated from reading UI file 'open_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPEN_DIALOG_H
#define UI_OPEN_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_OpenDialog
{
public:
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *editFilename;
    QToolButton *buttonOpen;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *editRate;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_3;
    QComboBox *comboType;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonOk;
    QPushButton *buttonCancel;

    void setupUi(QDialog *OpenDialog)
    {
        if (OpenDialog->objectName().isEmpty())
            OpenDialog->setObjectName(QStringLiteral("OpenDialog"));
        OpenDialog->resize(410, 146);
        horizontalLayout_6 = new QHBoxLayout(OpenDialog);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(OpenDialog);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(80, 0));
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label);

        editFilename = new QLineEdit(OpenDialog);
        editFilename->setObjectName(QStringLiteral("editFilename"));

        horizontalLayout->addWidget(editFilename);

        buttonOpen = new QToolButton(OpenDialog);
        buttonOpen->setObjectName(QStringLiteral("buttonOpen"));

        horizontalLayout->addWidget(buttonOpen);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(OpenDialog);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(80, 0));
        label_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label_2);

        editRate = new QLineEdit(OpenDialog);
        editRate->setObjectName(QStringLiteral("editRate"));
        editRate->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(editRate);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_3 = new QLabel(OpenDialog);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(80, 0));
        label_3->setAlignment(Qt::AlignCenter);

        horizontalLayout_5->addWidget(label_3);

        comboType = new QComboBox(OpenDialog);
        comboType->setObjectName(QStringLiteral("comboType"));

        horizontalLayout_5->addWidget(comboType);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_5);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        buttonOk = new QPushButton(OpenDialog);
        buttonOk->setObjectName(QStringLiteral("buttonOk"));

        horizontalLayout_3->addWidget(buttonOk);

        buttonCancel = new QPushButton(OpenDialog);
        buttonCancel->setObjectName(QStringLiteral("buttonCancel"));

        horizontalLayout_3->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout_3);


        horizontalLayout_6->addLayout(verticalLayout);


        retranslateUi(OpenDialog);
        QObject::connect(buttonOk, SIGNAL(clicked()), OpenDialog, SLOT(accept()));
        QObject::connect(buttonCancel, SIGNAL(clicked()), OpenDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(OpenDialog);
    } // setupUi

    void retranslateUi(QDialog *OpenDialog)
    {
        OpenDialog->setWindowTitle(QApplication::translate("OpenDialog", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        label->setText(QApplication::translate("OpenDialog", "\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
        buttonOpen->setText(QApplication::translate("OpenDialog", "...", nullptr));
        label_2->setText(QApplication::translate("OpenDialog", "\351\207\207\346\240\267\347\216\207", nullptr));
        editRate->setText(QApplication::translate("OpenDialog", "1", nullptr));
        label_3->setText(QApplication::translate("OpenDialog", "\346\225\260\346\215\256\346\240\274\345\274\217", nullptr));
        buttonOk->setText(QApplication::translate("OpenDialog", "\347\241\256\345\256\232", nullptr));
        buttonCancel->setText(QApplication::translate("OpenDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OpenDialog: public Ui_OpenDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPEN_DIALOG_H
