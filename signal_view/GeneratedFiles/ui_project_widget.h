/********************************************************************************
** Form generated from reading UI file 'project_widget.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROJECT_WIDGET_H
#define UI_PROJECT_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ProjectWidget
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QListWidget *listWidget;

    void setupUi(QDockWidget *ProjectWidget)
    {
        if (ProjectWidget->objectName().isEmpty())
            ProjectWidget->setObjectName(QStringLiteral("ProjectWidget"));
        ProjectWidget->resize(285, 488);
        ProjectWidget->setTabletTracking(false);
        QIcon icon;
        icon.addFile(QStringLiteral(":/MainWindow/icon/project.png"), QSize(), QIcon::Normal, QIcon::Off);
        ProjectWidget->setWindowIcon(icon);
        ProjectWidget->setFloating(false);
        widget = new QWidget();
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        listWidget = new QListWidget(widget);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        verticalLayout->addWidget(listWidget);

        ProjectWidget->setWidget(widget);

        retranslateUi(ProjectWidget);

        QMetaObject::connectSlotsByName(ProjectWidget);
    } // setupUi

    void retranslateUi(QDockWidget *ProjectWidget)
    {
        ProjectWidget->setWindowTitle(QApplication::translate("ProjectWidget", "\351\241\271\347\233\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProjectWidget: public Ui_ProjectWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROJECT_WIDGET_H
