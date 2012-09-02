#ifndef TEXT_EDIT_CONTROL_H
#define TEXT_EDIT_CONTROL_H
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QBoxLayout>
#include <QDebug>

//#include "item.h"
#include "constants.h"
#include "edit_controls.h"

//DUPLICATES.... should remove ...

#ifndef MAX
#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A,B) (((A) < (B)) ? (A) : (B))
#endif


QString htmlize(QString str);
QString titlize(QString str);
QString htmlFontFormat(QString str, QString font_str);

//END DUPLICATES

#define SMALL_FONT_RATIO (0.8)

//Simulate a single text control with two different objects...
//This isn't part of the QT Window system, it just passes messages
//to the "real" QT objects... but it's a QObject so it can do that...


//For the current item or keys...
class MyDisplayItem : public QLabel //QLineEdit
{
        Q_OBJECT
    public:
        MyDisplayItem(QWidget* parent = 0) :
            QLabel(parent) {
                QLabel::setTextFormat(Qt::RichText);
                QLabel::setWordWrap(false);
                setFocusPolicy(Qt::NoFocus);
                setAlignment(Qt::AlignHCenter);
                //setReadOnly(true);
                //setAttribute(Qt::WA_InputMethodEnabled);
            }
            void keyPressEvent(QKeyEvent* key);
            // This is how you pick up the tab key
            bool focusNextPrevChild(bool next) {
                next = next; // Remove compiler warning
                QKeyEvent key(QEvent::KeyPress, Qt::Key_Tab, NULL);
                emit keyPressed(&key);
                return true;
            }
            void focusOutEvent ( QFocusEvent * evt) {
                    emit focusOut(evt);
            }

            void inputMethodEvent(QInputMethodEvent *e) {
                    QLabel::inputMethodEvent(e);
                    //QLineEdit::inputMethodEvent(e);
                    if (e->commitString() != "") {
                            emit inputMethod(e);
                    }
            }
            void setText(const QString s);

            bool takingEditKeys(){return false;}
            void setKeyPress(QKeyEvent*e){
                emit keyPressed(e);
            }

            int getLines(){
                return 1;
            }

    signals:
        void keyPressed(QKeyEvent*);
        void focusOut(QFocusEvent* evt);
        void inputMethod(QInputMethodEvent *e);
};

class FlexTextEdit : public QTextEdit {
    public:
        FlexTextEdit(QWidget* p);
        void takeKey(QKeyEvent* k);
        void takeKeyRelease(QKeyEvent* k);
        void setGeometry(QRect);
        void restoreCursor();

        int m_cursorPos;
};

class MyFullTextEdit : public QObject //: public QWidget
{
    Q_OBJECT
public:
    MyFullTextEdit(QWidget* parent);
    ~MyFullTextEdit();

    void setVisible(bool v=true){
        m_shortEdit->setVisible(v);
        m_longEdit->setVisible(v);
    }

    void restoreCursor();
    int desiredLines();
    int desiredHeight();
    int lineHeight() const;
    int lineMargin() const;
    qreal textWidth();
    bool atLongEdit();
    void setFocusProxy(QWidget* parent);
    void setFont(const QFont &f);
    void setObjectName(const QString &name);
    QMargins contentsMargin();
    void setContentsMargin(QMargins m);

    int getLineCount();
    //This will be plain text
    void setPlaintText(QString t);
    void setFeedbackText(QString t);
    bool hasText();
    void clearText();
    
    //This will always be qt's html dialect
    void setHtmlText(QString html, QString plain);
    bool takeKey(QKeyEvent* key);

    //Even if we're on short-edit, we may
    //want to feed the long-edit the release key
    //it needs...
    void takeKeyRelease(QKeyEvent* key);
    QString text();
    QString htmlText();
    void setAppropriateSize(QSize s=QSize());
    void setLongTextWidth(int w);
    QRect geometry();
    void move(QPoint p);

    void activateWindow();
    void setFocus();
    void raise();
    void show();
    QPoint pos();
    bool takingEditKeys();
    void setMaxDisplayCharacters(int i);

protected:

    void keyPressEvent(QKeyEvent* key);

private:

    //Config - changed from outside
    QRect m_desiredGeom;
    QRect m_defaultSize;
    int m_maxVisibleLines;
    int m_maxLines;
    int m_maxBigCharacters;
    int m_maxDisplayCharacters;
    QString m_text;
    QString m_htmlText;
    QFont m_bigFont;
    QFont m_smallFont;

    //Internal State - changeable internally
    QLabel* m_shortEdit;
    FlexTextEdit* m_longEdit;
    bool m_usingShortEdit;
    bool m_isVisible;
    int m_currentInternalLines;
    bool m_formatting;
    bool m_forceShort;
    bool m_htmlFormat;
    bool m_userFeedBack;

    QFont curFont();
    void selectEditControl();
    void activateShortEdit(QString t="");
    void activateLongEdit(QString t);
    void testResize();

public slots:
    void textChangedSlot();
    void cursorPosChange();

signals:
    void rectangleResizing();
    void keyPressed(QKeyEvent* key);
    void textChangedSignal(QString , QString , int );

};


#endif // TEXT_EDIT_CONTROL_H
