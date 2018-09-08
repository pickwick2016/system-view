/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClass
{
public:
    QAction *actionOpen;
    QAction *actionClose;
    QAction *actionExit;
    QAction *actionProjectPane;
    QAction *actionAbout;
    QAction *actionScript1;
    QAction *actionFreq;
    QAction *actionWaterfall;
    QAction *actionWave;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_2;
    QMdiArea *mdiArea;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_3;
    QMenu *menu_4;
    QMenu *menu_5;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindowClass)
    {
        if (MainWindowClass->objectName().isEmpty())
            MainWindowClass->setObjectName(QStringLiteral("MainWindowClass"));
        MainWindowClass->resize(861, 550);
        QIcon icon;
        icon.addFile(QStringLiteral(":/MainWindow/icon/monitor.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindowClass->setWindowIcon(icon);
        actionOpen = new QAction(MainWindowClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/MainWindow/icon/document-open.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpen->setIcon(icon1);
        actionClose = new QAction(MainWindowClass);
        actionClose->setObjectName(QStringLiteral("actionClose"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/MainWindow/icon/document-remove.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClose->setIcon(icon2);
        actionExit = new QAction(MainWindowClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/MainWindow/icon/system-quit.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon3);
        actionProjectPane = new QAction(MainWindowClass);
        actionProjectPane->setObjectName(QStringLiteral("actionProjectPane"));
        actionProjectPane->setCheckable(true);
        actionProjectPane->setChecked(true);
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/MainWindow/icon/project.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionProjectPane->setIcon(icon4);
        actionAbout = new QAction(MainWindowClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/MainWindow/icon/information_2.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon5);
        actionScript1 = new QAction(MainWindowClass);
        actionScript1->setObjectName(QStringLiteral("actionScript1"));
        actionFreq = new QAction(MainWindowClass);
        actionFreq->setObjectName(QStringLiteral("actionFreq"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/MainWindow/icon/freq.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFreq->setIcon(icon6);
        actionWaterfall = new QAction(MainWindowClass);
        actionWaterfall->setObjectName(QStringLiteral("actionWaterfall"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/MainWindow/icon/waterfall.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWaterfall->setIcon(icon7);
        actionWave = new QAction(MainWindowClass);
        actionWave->setObjectName(QStringLiteral("actionWave"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/MainWindow/icon/wave.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWave->setIcon(icon8);
        centralWidget = new QWidget(MainWindowClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout_2 = new QVBoxLayout(centralWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(2, 2, 2, 2);
        mdiArea = new QMdiArea(centralWidget);
        mdiArea->setObjectName(QStringLiteral("mdiArea"));
        mdiArea->setFocusPolicy(Qt::StrongFocus);
        mdiArea->setViewMode(QMdiArea::TabbedView);
        mdiArea->setTabsClosable(true);

        verticalLayout_2->addWidget(mdiArea);

        MainWindowClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindowClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 861, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QStringLiteral("menu_3"));
        menu_4 = new QMenu(menuBar);
        menu_4->setObjectName(QStringLiteral("menu_4"));
        menu_5 = new QMenu(menuBar);
        menu_5->setObjectName(QStringLiteral("menu_5"));
        MainWindowClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindowClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindowClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindowClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindowClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_5->menuAction());
        menuBar->addAction(menu_4->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menu->addAction(actionOpen);
        menu->addAction(actionClose);
        menu->addSeparator();
        menu->addAction(actionExit);
        menu_2->addAction(actionAbout);
        menu_3->addAction(actionProjectPane);
        menu_4->addAction(actionScript1);
        menu_5->addAction(actionWaterfall);
        menu_5->addAction(actionWave);
        menu_5->addAction(actionFreq);
        mainToolBar->addAction(actionOpen);
        mainToolBar->addAction(actionClose);
        mainToolBar->addAction(actionExit);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionWaterfall);
        mainToolBar->addAction(actionWave);
        mainToolBar->addAction(actionFreq);

        retranslateUi(MainWindowClass);
        QObject::connect(actionExit, SIGNAL(triggered()), MainWindowClass, SLOT(close()));

        QMetaObject::connectSlotsByName(MainWindowClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClass)
    {
        MainWindowClass->setWindowTitle(QApplication::translate("MainWindowClass", "Signal View", nullptr));
        actionOpen->setText(QApplication::translate("MainWindowClass", "\346\211\223\345\274\200", nullptr));
        actionClose->setText(QApplication::translate("MainWindowClass", "\345\205\263\351\227\255", nullptr));
        actionExit->setText(QApplication::translate("MainWindowClass", "\351\200\200\345\207\272", nullptr));
        actionProjectPane->setText(QApplication::translate("MainWindowClass", "\351\241\271\347\233\256", nullptr));
        actionAbout->setText(QApplication::translate("MainWindowClass", "\345\205\263\344\272\216", nullptr));
        actionScript1->setText(QApplication::translate("MainWindowClass", "\350\204\232\346\234\254", nullptr));
        actionFreq->setText(QApplication::translate("MainWindowClass", "\347\236\254\346\227\266\351\242\221\350\260\261", nullptr));
        actionWaterfall->setText(QApplication::translate("MainWindowClass", "\347\200\221\345\270\203\345\233\276", nullptr));
        actionWave->setText(QApplication::translate("MainWindowClass", "\346\263\242\345\275\242\345\233\276", nullptr));
        menu->setTitle(QApplication::translate("MainWindowClass", "\346\226\207\344\273\266", nullptr));
        menu_2->setTitle(QApplication::translate("MainWindowClass", "\345\270\256\345\212\251", nullptr));
        menu_3->setTitle(QApplication::translate("MainWindowClass", "\347\252\227\345\217\243", nullptr));
        menu_4->setTitle(QApplication::translate("MainWindowClass", "\346\217\222\344\273\266", nullptr));
        menu_5->setTitle(QApplication::translate("MainWindowClass", "\350\247\206\345\233\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClass: public Ui_MainWindowClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
