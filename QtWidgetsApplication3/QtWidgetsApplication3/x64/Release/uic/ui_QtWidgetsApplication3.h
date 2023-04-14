/********************************************************************************
** Form generated from reading UI file 'QtWidgetsApplication3.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTWIDGETSAPPLICATION3_H
#define UI_QTWIDGETSAPPLICATION3_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtWidgetsApplication3Class
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtWidgetsApplication3Class)
    {
        if (QtWidgetsApplication3Class->objectName().isEmpty())
            QtWidgetsApplication3Class->setObjectName("QtWidgetsApplication3Class");
        QtWidgetsApplication3Class->resize(600, 400);
        menuBar = new QMenuBar(QtWidgetsApplication3Class);
        menuBar->setObjectName("menuBar");
        QtWidgetsApplication3Class->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtWidgetsApplication3Class);
        mainToolBar->setObjectName("mainToolBar");
        QtWidgetsApplication3Class->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtWidgetsApplication3Class);
        centralWidget->setObjectName("centralWidget");
        QtWidgetsApplication3Class->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtWidgetsApplication3Class);
        statusBar->setObjectName("statusBar");
        QtWidgetsApplication3Class->setStatusBar(statusBar);

        retranslateUi(QtWidgetsApplication3Class);

        QMetaObject::connectSlotsByName(QtWidgetsApplication3Class);
    } // setupUi

    void retranslateUi(QMainWindow *QtWidgetsApplication3Class)
    {
        QtWidgetsApplication3Class->setWindowTitle(QCoreApplication::translate("QtWidgetsApplication3Class", "QtWidgetsApplication3", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtWidgetsApplication3Class: public Ui_QtWidgetsApplication3Class {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTWIDGETSAPPLICATION3_H
