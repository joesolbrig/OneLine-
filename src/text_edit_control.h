#ifndef TEXT_EDIT_CONTROL_H
#define TEXT_EDIT_CONTROL_H
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QBoxLayout>
//#include <Q3BoxLayout>

#include "item.h"
#include "edit_controls.h"





//Simulate a single text control with two different objects...
//This isn't part of the QT Window system, it just passes messages
//to the "real" QT objects... but it's a QObject so it can do that...

class MyFullTextEdit : QObject {
    Q_OBJECT

public:
    MyFullTextEdit(QWidget* parent) : shortEdit(parent), longEdit(parent), usingShortEdit(true){
        connect(&longEdit,SIGNAL(contentsChange(int, int , int )), this,SLOT(textChange(int, int , int )));
        connect(&longEdit,SIGNAL(cursoPositionChange(QTextCursor)),this,SLOT(cursorPosChange(QTextCursor)));
    }
    void setObjectName(const QString &name) {
        QObject::setObjectName(name);

    }
    //This will always be qt's html dialect
    void setText(QString t){
        usingShortEdit = true;
        shortEdit.setText(t);
        longEdit.setText(t);
        reformat();
    }

    void setGeometry(const QRect & r){
        shortEdit.setGeometry(r);
        longEdit.setGeometry(r);
        reformat();

    }

    void activateWindow(){
        if(usingShortEdit){
            shortEdit.activateWindow();
        } else {
            longEdit.activateWindow();
        }
    }
    void setFocus(){
        if(usingShortEdit){
            shortEdit.setFocus();
        } else {
            longEdit.setFocus();
        }
    }
    void raise(){
        if(usingShortEdit){
            shortEdit.raise();
        } else {
            longEdit.raise();
        }
    }

    void show(){
        if(usingShortEdit){
            shortEdit.show();
        } else {
            longEdit.show();
        }
    }

    QString text() {
        if(usingShortEdit){
            return shortEdit.text();
        } else {
            return longEdit.document()->toHtml();
        }
    }
    QPoint pos(){
        if(usingShortEdit){
            return shortEdit.pos();
        } else {
            return longEdit.pos();
        }
    }

    bool takingEditKeys() {
        return usingShortEdit;
    }

private:

    //Config - changed from outside
    QRect defaultSize;
    int maxVisibleLines;
    int maxLineHeight;
    int maxDisplayCharacters;
    QFont bigFont;
    QFont smallFont;

    //Internal State - changeable internally
    QLabel shortEdit;
    QTextEdit longEdit;
    bool usingShortEdit;
    bool isVisible;
    int currentLines;

    void reformat(){
        QString t = text();
        if(t.length() <= maxDisplayCharacters){
            usingShortEdit = true;
            longEdit.setVisible(false);
            shortEdit.setVisible(isVisible);
            shortEdit.setText(t);
        } else {
            usingShortEdit = false;
            shortEdit.setVisible(false);
            longEdit.setVisible(isVisible);
            longEdit.setText(t);
            testResize();
            longEdit.activateWindow();
        }
    }

    //
    void testResize(){
        QTextDocument* d = longEdit.document();
        d->adjustSize();
        d->setTextWidth(shortEdit.size().width());
        int lines = d->lineCount();
        if(currentLines < maxLineHeight && currentLines != lines){
            QSizeF s = d->size(); //why isn't size size...?
            longEdit.resize(s.toSize());
            emit rectangleResizing(s);
            currentLines = MAX(maxLineHeight,currentLines);
        }
    }

//Interface



public slots:
    void textChange(int, int , int ){ reformat(); }
    void cursorPosChange(QTextCursor){
        //Later we'll have internal items but not yet
    }

signals:
    void rectangleResizing(QSizeF);

};

#endif // TEXT_EDIT_CONTROL_H
