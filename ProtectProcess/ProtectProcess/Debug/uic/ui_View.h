/********************************************************************************
** Form generated from reading UI file 'View.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEW_H
#define UI_VIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ViewClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ViewClass)
    {
        if (ViewClass->objectName().isEmpty())
            ViewClass->setObjectName(QString::fromUtf8("ViewClass"));
        ViewClass->resize(600, 400);
        menuBar = new QMenuBar(ViewClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        ViewClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ViewClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        ViewClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(ViewClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        ViewClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ViewClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ViewClass->setStatusBar(statusBar);

        retranslateUi(ViewClass);

        QMetaObject::connectSlotsByName(ViewClass);
    } // setupUi

    void retranslateUi(QMainWindow *ViewClass)
    {
        ViewClass->setWindowTitle(QApplication::translate("ViewClass", "View", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewClass: public Ui_ViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEW_H
