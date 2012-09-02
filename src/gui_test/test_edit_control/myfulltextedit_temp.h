#ifndef TEXT_EDIT_CONTROL_H
#define TEXT_EDIT_CONTROL_H
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QBoxLayout>
//#include <Q3BoxLayout>

//#include "item.h"
#include "../../edit_controls.h"

#ifndef MAX
#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A,B) (((A) < (B)) ? (A) : (B))
#endif


#define SMALL_FONT_RATIO (0.8)

//Simulate a single text control with two different objects...
//This isn't part of the QT Window system, it just passes messages
//to the "real" QT objects... but it's a QObject so it can do that...

class MyTextEdit : public QTextEdit {
public:
    MyTextEdit(QWidget* p): QTextEdit(p){
        //this->setCursorWidth(3);

    }

    void takeKey(QKeyEvent* k){
        setFocus();
        QTextEdit::keyPressEvent(k);
    }


};



class MyFullTextEdit : public QWidget
{
    Q_OBJECT
    QRect m_geoRect;
public:
    MyFullTextEdit(QWidget* parent) : QWidget(parent,Qt::Widget), usingShortEdit(true){
        bigFont = QWidget::font();
        smallFont = bigFont;
        if(smallFont.pointSize() > 0){
            smallFont.setPointSize(smallFont.pointSize()* SMALL_FONT_RATIO);
        } else {
            smallFont.setPixelSize(smallFont.pixelSize() * SMALL_FONT_RATIO);
        }
        usingShortEdit =false;

        longEdit = new MyTextEdit(parent);
        shortEdit = new QLabel(parent);
        shortEdit->setMargin(0);
        shortEdit->setContentsMargins(0,0,0,0);
        longEdit->setContentsMargins(0,0,0,0);

        maxDisplayCharacters = 20;
        maxBigCharacters = 10;
        isVisible = true;
        currentLines = -1;
        maxLineHeight = 10;

        connect(longEdit,SIGNAL(textChanged()), this,SLOT(textChanged()));
        connect(longEdit,SIGNAL( cursorPositionChanged()),this,SLOT(cursorPosChange()));
        longEdit->setAcceptRichText(false);
        longEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        QTextDocument* d = longEdit->document();
        d->setDocumentMargin(0);
        QTextOption to;
        to.setWrapMode(QTextOption::WordWrap);
        d->setDefaultTextOption(to);
        longEdit->setLineWrapMode(QTextEdit::FixedPixelWidth);

        shortEdit->setEnabled(true);
        shortEdit->raise();
        shortEdit->setFocusProxy(this);
        this->raise();
        this->setEnabled(true);
        formatting=false;

        activateShortEdit();
        setText("Text");
    }

    ~MyFullTextEdit(){ }

    void setObjectName(const QString &name) {
        QObject::setObjectName(name);

    }
    //This will always be qt's html dialect
    void setText(QString t){
        itsText = t;
        shortEdit->setText(t);
        longEdit->setText(t);
        reformat();
    }

    void setGeometry(const QRect & r){
        float charWidth = ((float)fontMetrics().width("XuIXm",5))/5.0;
        maxBigCharacters = r.width()/(charWidth*2);
        maxDisplayCharacters = maxBigCharacters * 1.5;

        reformat();
        longEdit->setLineWrapColumnOrWidth(r.width());
        if(usingShortEdit){
            shortEdit->raise();
        } else {
            longEdit->raise();
        }
        longEdit->setFixedWidth(r.width());
        QTextDocument* d = longEdit->document();
        d->setTextWidth(r.width());
        m_geoRect = r;
        QFontMetrics fm(bigFont);
        m_geoRect.setHeight(fm.height());
        shortEdit->setGeometry(m_geoRect);
        longEdit->setGeometry(m_geoRect);
    }

    void activateWindow(){
        QWidget::activateWindow();
        if(usingShortEdit){
            shortEdit->activateWindow();
            longEdit->setVisible(false);
        } else {
            shortEdit->setVisible(false);
            longEdit->activateWindow();
        }
    }

    void setFocus(){
        QWidget::setFocus();
        if(!usingShortEdit){
            shortEdit->raise();
        } else {
            longEdit->setFocus();
        }
    }
    void raise(){
        QWidget::raise();
        if(usingShortEdit){
            shortEdit->raise();
            longEdit->setVisible(false);
        } else {
            longEdit->raise();
            shortEdit->setVisible(false);
        }
    }

    void show(){
        if(usingShortEdit){
            shortEdit->show();
            longEdit->setVisible(false);
        } else {
            shortEdit->setVisible(false);
            longEdit->show();
        }
    }

    QString text() {
        if(usingShortEdit){
            return shortEdit->text();
        } else {
            return longEdit->toPlainText();
        }
    }

    QPoint pos(){
        if(usingShortEdit){
            return shortEdit->pos();
        } else {
            return longEdit->pos();
        }
    }

    bool takingEditKeys() {
        return usingShortEdit;
    }

    void setMaxDisplayCharacters(int i){
        maxDisplayCharacters = i;
    }

protected:

    //This is just for testing... it will be "driven"
    //by it's parent once installed...
    void keyPressEvent(QKeyEvent* key){
        if(usingShortEdit){
            QString t = itsText;
            QString kys(key->text());
            if(key->key()== Qt::Key_Backspace){
                t.chop(1);

            } else {
                t.append(kys);
            }
            setText(t);
        } else {
            longEdit->takeKey(key);
        }
    }

private:

    //Config - changed from outside
    QRect defaultSize;
    int maxVisibleLines;
    int maxLineHeight;
    int maxBigCharacters;
    int maxDisplayCharacters;
    QString itsText;
    QFont bigFont;
    QFont smallFont;

    //Internal State - changeable internally
    QLabel* shortEdit;
    MyTextEdit* longEdit;
    bool usingShortEdit;
    bool isVisible;
    int currentLines;
    bool formatting;

    QFont curFont(){
        if(text().length() < maxBigCharacters){
            return bigFont; }
        else { return smallFont;}

    }

    void reformat(){
        formatting = true;
        QString t = text();
        shortEdit->setFont(curFont());
        longEdit->setFont(curFont());

        if(t.length() <= maxDisplayCharacters ){
            if(!usingShortEdit){ activateShortEdit(t); }
        } else {
            if(usingShortEdit){ activateLongEdit(t); }
        }
        formatting = false;
        shortEdit->setGeometry(m_geoRect);
        longEdit->setGeometry(m_geoRect);

    }

    void activateShortEdit(QString t=""){
        shortEdit->setGeometry(m_geoRect);
        longEdit->setGeometry(m_geoRect);
        if(!usingShortEdit){
            itsText = longEdit->toPlainText();
        }
        usingShortEdit = true;
        longEdit->setVisible(false);
        longEdit->setEnabled(false);
        longEdit->setFocusPolicy(Qt::NoFocus);

        setFocusPolicy(Qt::StrongFocus);
        activateWindow();
        raise();
        shortEdit->setVisible(isVisible);
        shortEdit->setText(t);
        shortEdit->setFocus();
        //setFocus();
    }

    void activateLongEdit(QString t){
        usingShortEdit = false;
        shortEdit->setVisible(false);
        testResize();
        setFocusPolicy(Qt::NoFocus);

        longEdit->setVisible(isVisible);
        longEdit->activateWindow();
        longEdit->setEnabled(true);
        longEdit->setFocusPolicy(Qt::StrongFocus);
        longEdit->setFocus();
        longEdit->setPlainText(t);
        longEdit->moveCursor(QTextCursor::End);
    }



    //
    void testResize(){
        QRect r = longEdit->geometry();
        int length = this->fontMetrics().width(text());
        int lines = length/r.width();
        if(currentLines != lines){
            if(lines > maxLineHeight){
                currentLines = maxLineHeight;
            } else {
                currentLines = lines;
                r.setHeight(this->fontMetrics().height()*(currentLines+1));
                longEdit->setGeometry(r);
                emit rectangleResizing(r);
            }
        }
    }

public slots:
    void textChanged(){
        if(!formatting){
            reformat();
            if(!usingShortEdit){
                testResize();
            }
        }
    }
    void cursorPosChange(){
        //Later we'll have internal items but not yet
    }

signals:
    void rectangleResizing(QRect&);
    void keyPressed(QKeyEvent* key);

};


#endif // TEXT_EDIT_CONTROL_H
