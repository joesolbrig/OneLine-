/********************************************************************************
** Form generated from reading UI file 'actionpreview.ui'
**
** Created: Sat Dec 4 18:54:08 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACTIONPREVIEW_H
#define UI_ACTIONPREVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtWebKit/QWebView>

QT_BEGIN_NAMESPACE

class Ui_ActionPreview
{
public:
    QGridLayout *gridLayout;
    QWebView *htmlPreviewView;
    QProgressBar *progressBar;
    QPushButton *pushButton;

    void setupUi(QDialog *ActionPreview)
    {
        if (ActionPreview->objectName().isEmpty())
            ActionPreview->setObjectName(QString::fromUtf8("ActionPreview"));
        ActionPreview->resize(412, 341);
        gridLayout = new QGridLayout(ActionPreview);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        htmlPreviewView = new QWebView(ActionPreview);
        htmlPreviewView->setObjectName(QString::fromUtf8("htmlPreviewView"));
        htmlPreviewView->setUrl(QUrl("about:blank"));

        gridLayout->addWidget(htmlPreviewView, 0, 0, 1, 2);

        progressBar = new QProgressBar(ActionPreview);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setValue(24);

        gridLayout->addWidget(progressBar, 1, 0, 1, 1);

        pushButton = new QPushButton(ActionPreview);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 1, 1, 1, 1);


        retranslateUi(ActionPreview);

        QMetaObject::connectSlotsByName(ActionPreview);
    } // setupUi

    void retranslateUi(QDialog *ActionPreview)
    {
        ActionPreview->setWindowTitle(QApplication::translate("ActionPreview", "Dialog", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("ActionPreview", " Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ActionPreview: public Ui_ActionPreview {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACTIONPREVIEW_H
