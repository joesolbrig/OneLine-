/********************************************************************************
** Form generated from reading UI file 'edittestmainwindow.ui'
**
** Created: Mon May 23 22:08:15 2011
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITTESTMAINWINDOW_H
#define UI_EDITTESTMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include "listwithdisplay.h"
#include "messageiconwidget.h"
#include "myfulltextedit.h"

QT_BEGIN_NAMESPACE

class Ui_EditTestMainWindow
{
public:
    QWidget *centralWidget;
    MyFullTextEdit *textEdit;
    MessageIconWidget *IconWidget;
    ListWithDisplay *listDisplayWidget;
    QPushButton *parentButton;
    QPushButton *childButton;
    QPushButton *expandListButton;
    QPushButton *shrinkListButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *EditTestMainWindow)
    {
        if (EditTestMainWindow->objectName().isEmpty())
            EditTestMainWindow->setObjectName(QString::fromUtf8("EditTestMainWindow"));
        EditTestMainWindow->resize(541, 400);
        centralWidget = new QWidget(EditTestMainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        textEdit = new MyFullTextEdit(centralWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(50, 40, 181, 41));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(162, 119, 237, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        QBrush brush2(QColor(239, 229, 255, 255));
        brush2.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Light, brush2);
        QBrush brush3(QColor(200, 174, 246, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Midlight, brush3);
        QBrush brush4(QColor(81, 59, 118, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Dark, brush4);
        QBrush brush5(QColor(108, 79, 158, 255));
        brush5.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        QBrush brush6(QColor(255, 255, 255, 255));
        brush6.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush6);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
        QBrush brush7(QColor(208, 187, 246, 255));
        brush7.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush7);
        QBrush brush8(QColor(255, 255, 220, 255));
        brush8.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Active, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Light, brush2);
        palette.setBrush(QPalette::Inactive, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush6);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush7);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Inactive, QPalette::ToolTipText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Light, brush2);
        palette.setBrush(QPalette::Disabled, QPalette::Midlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Dark, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Mid, brush5);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::BrightText, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipBase, brush8);
        palette.setBrush(QPalette::Disabled, QPalette::ToolTipText, brush);
        textEdit->setPalette(palette);
        textEdit->setCursor(QCursor(Qt::IBeamCursor));
        IconWidget = new MessageIconWidget(centralWidget);
        IconWidget->setObjectName(QString::fromUtf8("IconWidget"));
        IconWidget->setGeometry(QRect(50, 130, 271, 101));
        QPalette palette1;
        QBrush brush9(QColor(158, 191, 176, 200));
        brush9.setStyle(Qt::SolidPattern);
        palette1.setBrush(QPalette::Active, QPalette::Base, brush9);
        palette1.setBrush(QPalette::Inactive, QPalette::Base, brush9);
        palette1.setBrush(QPalette::Disabled, QPalette::Base, brush6);
        IconWidget->setPalette(palette1);
        IconWidget->setCursor(QCursor(Qt::PointingHandCursor));
        listDisplayWidget = new ListWithDisplay(centralWidget);
        listDisplayWidget->setObjectName(QString::fromUtf8("listDisplayWidget"));
        listDisplayWidget->setGeometry(QRect(370, 20, 161, 251));
        parentButton = new QPushButton(centralWidget);
        parentButton->setObjectName(QString::fromUtf8("parentButton"));
        parentButton->setGeometry(QRect(270, 300, 101, 27));
        childButton = new QPushButton(centralWidget);
        childButton->setObjectName(QString::fromUtf8("childButton"));
        childButton->setGeometry(QRect(400, 300, 92, 27));
        expandListButton = new QPushButton(centralWidget);
        expandListButton->setObjectName(QString::fromUtf8("expandListButton"));
        expandListButton->setGeometry(QRect(10, 300, 92, 27));
        shrinkListButton = new QPushButton(centralWidget);
        shrinkListButton->setObjectName(QString::fromUtf8("shrinkListButton"));
        shrinkListButton->setGeometry(QRect(140, 300, 92, 27));
        EditTestMainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(EditTestMainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 541, 25));
        EditTestMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(EditTestMainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        EditTestMainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(EditTestMainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        EditTestMainWindow->setStatusBar(statusBar);

        retranslateUi(EditTestMainWindow);

        QMetaObject::connectSlotsByName(EditTestMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *EditTestMainWindow)
    {
        EditTestMainWindow->setWindowTitle(QApplication::translate("EditTestMainWindow", "EditTestMainWindow", 0, QApplication::UnicodeUTF8));
        parentButton->setText(QApplication::translate("EditTestMainWindow", "Go To Parents", 0, QApplication::UnicodeUTF8));
        childButton->setText(QApplication::translate("EditTestMainWindow", "Go To Childs", 0, QApplication::UnicodeUTF8));
        expandListButton->setText(QApplication::translate("EditTestMainWindow", "Expand list", 0, QApplication::UnicodeUTF8));
        shrinkListButton->setText(QApplication::translate("EditTestMainWindow", "shrink List", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EditTestMainWindow: public Ui_EditTestMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITTESTMAINWINDOW_H
