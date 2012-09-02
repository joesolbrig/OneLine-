/********************************************************************************
** Form generated from reading UI file 'pin_dialog.ui'
**
** Created: Fri Jun 8 21:49:08 2012
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PIN_DIALOG_H
#define UI_PIN_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_Pin_Dialog
{
public:
    QDialogButtonBox *buttonBox;
    QTextEdit *textEdit;

    void setupUi(QDialog *Pin_Dialog)
    {
        if (Pin_Dialog->objectName().isEmpty())
            Pin_Dialog->setObjectName(QString::fromUtf8("Pin_Dialog"));
        Pin_Dialog->resize(240, 92);
        buttonBox = new QDialogButtonBox(Pin_Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(150, 10, 81, 301));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        textEdit = new QTextEdit(Pin_Dialog);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 10, 121, 41));

        retranslateUi(Pin_Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Pin_Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Pin_Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Pin_Dialog);
    } // setupUi

    void retranslateUi(QDialog *Pin_Dialog)
    {
        Pin_Dialog->setWindowTitle(QApplication::translate("Pin_Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Pin_Dialog: public Ui_Pin_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PIN_DIALOG_H
