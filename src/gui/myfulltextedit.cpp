#include "myfulltextedit.h"

FlexTextEdit::FlexTextEdit(QWidget* p): QTextEdit(p){
    m_cursorPos =0;
    //this->setCursorWidth(3);
}

void FlexTextEdit::takeKey(QKeyEvent* k){
    //setFocus();

    qDebug() << "MyFullTextEdit:: pre-take key pos: " << textCursor().position();
    QTextEdit::keyPressEvent(k);
    //m_cursorPos = textCursor().position();
    qDebug() << "take key pos: " << textCursor().position();
}

void FlexTextEdit::takeKeyRelease(QKeyEvent* k){
    //setFocus();
    qDebug() << "MyFullTextEdit:: take key release start pos: " << textCursor().position();
    QTextEdit::keyReleaseEvent(k);
    m_cursorPos = textCursor().position();
    qDebug() << "take key release end pos: " << m_cursorPos;
}

void FlexTextEdit::setGeometry(QRect r)
{
    QTextEdit::setGeometry(r);
}

void FlexTextEdit::restoreCursor()
{
    QTextCursor tc = this->textCursor();
    tc.setPosition(m_cursorPos+1);
    setTextCursor(tc);
    qDebug() << "FlexTextEdit::setGeometry restoreCursor: " << m_cursorPos;
}

MyFullTextEdit::MyFullTextEdit(QWidget* parent) :
        //QWidget(parent,Qt::Widget),
        m_usingShortEdit(true){
    m_desiredGeom = parent->geometry();
    m_longEdit = new FlexTextEdit(parent);
    m_shortEdit = new QLabel(parent);

    m_usingShortEdit =true;
    m_forceShort = false;
    m_userFeedBack = false;
    m_htmlFormat = false;


    m_shortEdit->setMargin(0);
    m_shortEdit->setContentsMargins(0,0,0,0);
    m_longEdit->setContentsMargins(0,0,0,0);

    m_maxDisplayCharacters = 20;
    m_maxBigCharacters = 10;
    m_isVisible = true;
    m_currentInternalLines = -1;
    m_maxLines = 10;

    qDebug() << "expanding edit make connect";
    connect(m_longEdit,SIGNAL(textChanged()), this,SLOT(textChangedSlot()));
    //connect(m_longEdit,SIGNAL( cursorPositionChanged()),this,SLOT(cursorPosChange()));
    m_longEdit->setAcceptRichText(false);
    m_longEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QTextDocument* d = m_longEdit->document();
    d->setDocumentMargin(0);
    d->setDefaultFont(m_smallFont);
    QTextOption to;
    to.setWrapMode(QTextOption::WordWrap);
    d->setDefaultTextOption(to);
    m_longEdit->setLineWrapMode(QTextEdit::FixedPixelWidth);
    setLongTextWidth(m_desiredGeom.width());

    m_shortEdit->setEnabled(true);
    m_shortEdit->raise();
    m_shortEdit->setFocusProxy(parent);
    m_shortEdit->setAlignment(Qt::AlignTop);
    m_formatting=false;
    activateShortEdit();
    Q_ASSERT(m_usingShortEdit);
    setPlaintText("");
}

MyFullTextEdit::~MyFullTextEdit(){ }

void MyFullTextEdit::restoreCursor()
{
    m_longEdit->restoreCursor();
}

//
void MyFullTextEdit::testResize(){
    Q_ASSERT(!m_usingShortEdit);
    int estimatedLines = m_longEdit->document()->lineCount();
    QRect r = m_longEdit->geometry();
    //Darn document only give hard line break count, which means something but...
    QFontMetrics fm(this->m_smallFont);
    int adjustedLength = MAX(fm.width(text()),UI_MIN_TEXT_EDIT_LENGTH)*(1.3)+10;
    int width = MAX(r.width(),UI_MIN_TEXT_EDIT_WIDTH);
    int linesByWidth = (adjustedLength)/(width);
    estimatedLines = MAX(estimatedLines, linesByWidth);
    qDebug() << "MyFullTextEdit:: test resize lines: " << estimatedLines << ":" << m_currentInternalLines;

    if(m_currentInternalLines != estimatedLines ){
        m_currentInternalLines = estimatedLines;
    }
    if(r.height() != desiredHeight()){
        emit rectangleResizing();
    }
}

int MyFullTextEdit::desiredLines(){
    return MAX(MIN(m_maxLines, m_currentInternalLines),1);
}

int MyFullTextEdit::desiredHeight(){
    int innerHeight;
    if(m_usingShortEdit){
        QFontMetrics fm(m_smallFont);
        innerHeight = fm.height();
    } else {
//        QTextDocument* d = m_longEdit->document();
//        QAbstractTextDocumentLayout tl = d->documentLayout();
        //QFontMetrics fms();
        innerHeight = m_longEdit->fontMetrics().lineSpacing()*(MAX(desiredLines()+1, (1.5)));
    }
    return innerHeight;
}

int MyFullTextEdit::lineHeight() const {
    return m_longEdit->fontMetrics().lineSpacing();
}

int MyFullTextEdit::lineMargin() const {
    return m_longEdit->fontMetrics().lineSpacing() - m_longEdit->fontMetrics().height();
}

qreal MyFullTextEdit::textWidth(){
    int width;
    if(m_usingShortEdit){
        QFontMetrics fm(m_shortEdit->font());
        width = fm.boundingRect(text()).width();
    } else {
        width = m_longEdit->geometry().width();
    }
    width = MAX(0,width);
    return width;
}

bool MyFullTextEdit::atLongEdit(){
    return !m_usingShortEdit;
}

void MyFullTextEdit::setFocusProxy(QWidget* parent){
    m_shortEdit->setFocusProxy(parent);
    m_longEdit->setFocusProxy(parent);
}

void MyFullTextEdit::setFont(const QFont &f){
    m_bigFont = f;
    m_smallFont = m_bigFont;
    if(m_smallFont.pointSize() > 0){
        m_smallFont.setPointSize(m_smallFont.pointSize()* SMALL_FONT_RATIO);
    } else {
        m_smallFont.setPixelSize(m_smallFont.pixelSize() * SMALL_FONT_RATIO);
    }
    m_shortEdit->setFont(m_bigFont);
    m_longEdit->setFont(m_bigFont);

}

void MyFullTextEdit::setObjectName(const QString &name) {
    QObject::setObjectName(name);
}

QMargins MyFullTextEdit::contentsMargin(){
    Q_ASSERT(false);
    if(m_usingShortEdit){
        return m_shortEdit->contentsMargins();
    } else {
        return m_longEdit->contentsMargins();
    }
}

void MyFullTextEdit::setContentsMargin(QMargins m){
    Q_ASSERT(false);
    if(m_usingShortEdit){
        m_shortEdit->setContentsMargins(m);
    } else {
        m_longEdit->setContentsMargins(m);
    }
}

//    void getContentsMargins ( int * left, int * top, int * right, int * bottom ) const{
//        if(usingShortEdit){
//            shortEdit->setContentsMargins(left, top, right, bottom );
//        } else {
//            longEdit->setContentsMargins(left, top, right, bottom );
//        }
//    }


int MyFullTextEdit::getLineCount(){
    Q_ASSERT(false);
    return 1;
}

//This will be plain text
void MyFullTextEdit::setPlaintText(QString t){
    m_forceShort = false;
    m_htmlFormat = false;
    m_text = t;
    m_htmlText = titlize(t);
    m_shortEdit->setTextFormat(Qt::PlainText);
    m_shortEdit->setText(t);
    m_userFeedBack = false;

    m_longEdit->setText(t);
    m_longEdit->moveCursor(QTextCursor::End);
    selectEditControl();
}

void MyFullTextEdit::setFeedbackText(QString t){
    m_forceShort = true;
    m_htmlFormat = false;
    m_text = "";
    m_htmlText = titlize(t);
    m_shortEdit->setTextFormat(Qt::PlainText);
    m_shortEdit->setText(t);

    m_userFeedBack = true;

    m_longEdit->setText("");
    selectEditControl();
}

bool MyFullTextEdit::hasText(){
    QString text = this->htmlText();
    if(text.isEmpty()){
        return false;
    }
    if(text ==titlize(QString())){
        return false;
    }
    return true;
}

void MyFullTextEdit::clearText(){
    //order matters!
    setPlaintText("");
    m_htmlText="";
    m_text = "";
    m_shortEdit->setText("");
    m_longEdit->setText("");
}

//This will always be qt's html dialect
void MyFullTextEdit::setHtmlText(QString html, QString plain){
    m_forceShort = true;
    m_htmlFormat = true;
    m_text = plain;
    m_htmlText = html;
    m_shortEdit->setTextFormat(Qt::RichText);
    m_shortEdit->setText(html);
    m_longEdit->setText(m_text);

    m_userFeedBack = true;

    selectEditControl();
}

bool MyFullTextEdit::takeKey(QKeyEvent* key){
    m_forceShort = false;
    m_htmlFormat = false;

    if(m_usingShortEdit){
        QString t = m_text;
        QString kys(key->text());
        if(key->key()== Qt::Key_Backspace){
            t.chop(1);
        } else if (kys.isEmpty()){
            return false;
        } else {
            t.append(kys);
        }
        setPlaintText(t);
    } else {
        m_longEdit->takeKey(key);
    }
    return true;
}

//Even if we're on short-edit, we may
//want to feed the long-edit the release key
//it needs...
void MyFullTextEdit::takeKeyRelease(QKeyEvent* key){
    m_longEdit->takeKeyRelease(key);
}

QString MyFullTextEdit::text() {
    if(m_usingShortEdit){
        Q_ASSERT(m_shortEdit->textFormat() == Qt::RichText
                 || m_shortEdit->text() == m_text||
                 m_userFeedBack);
        return m_text;
    } else {
        return m_longEdit->toPlainText();
    }
}

QString MyFullTextEdit::htmlText() {
    QString res;
    if(m_usingShortEdit){
        if(m_htmlFormat){
            qDebug() << "MyFullTextEdit:: have html text:" << m_htmlText;
            res = m_htmlText;
            qDebug() << "got res:" << res;
        } else {
            res = htmlize(QString("<p>") + m_shortEdit->text()+ QString("</p>"));
        }
    } else {
        res = htmlize(QString("<p>") + m_longEdit->toPlainText() + QString("</p>"));
    }
    qDebug() << "got res:" << res;
    return res;
}


QRect MyFullTextEdit::geometry(){
    if(m_usingShortEdit){
        return m_shortEdit->geometry();
    } else {
        return m_longEdit->geometry();
    }
}

void MyFullTextEdit::move(QPoint p){
    if(m_usingShortEdit){
        p.setY(p.y() + lineMargin());
        m_shortEdit->move(p);;
    } else {
        m_longEdit->move(p);
    }
}


void MyFullTextEdit::setAppropriateSize(QSize size){

    QFontMetrics bigFontMetrics(m_bigFont);
    QFontMetrics smallFontMetrics(m_smallFont);
    QSize desiredSize;
    if(size.isEmpty()){
        desiredSize = geometry().size();
    } else { desiredSize = size;}

    if(m_usingShortEdit){
        desiredSize.setHeight(bigFontMetrics.height());
    } else {
        desiredSize.setHeight(desiredHeight());
    }

    if(geometry().size().height()==desiredSize.height() &&
                m_longEdit->geometry().width()==desiredSize.width()){
        return;
    }

    if(m_longEdit->geometry().width()!=desiredSize.width()){
        float charWidth =  bigFontMetrics.averageCharWidth(); //((float)fm.width("XuIXm",5))/5.0;
        m_maxBigCharacters = desiredSize.width()/(charWidth+5);
        charWidth =  smallFontMetrics.averageCharWidth(); //((float)fm.width("XuIXm",5))/5.0;
        m_maxDisplayCharacters = desiredSize.width()/(charWidth+5);
        //m_maxDisplayCharacters = m_maxBigCharacters * 1.5;
        selectEditControl();
        if(!m_usingShortEdit){
            setLongTextWidth(desiredSize.width());
        }
    }
    m_shortEdit->resize(desiredSize);
    m_longEdit->resize(desiredSize);
    qDebug() << "MyFullTextEdit::setFullGeometry end pos: " <<
            m_longEdit->m_cursorPos << ":"<< m_longEdit->textCursor().position();


}

void MyFullTextEdit::setLongTextWidth(int w){
    m_longEdit->setLineWrapColumnOrWidth(w);
    m_longEdit->setFixedWidth(w);
    QTextDocument* d = m_longEdit->document();
    d->setTextWidth(w);
    d->setDocumentMargin(qreal(0));
}


void MyFullTextEdit::activateWindow(){
    if(m_usingShortEdit){
        m_longEdit->setVisible(false);
    } else {
        m_shortEdit->setVisible(false);
        m_longEdit->activateWindow();
    }
}

void MyFullTextEdit::setFocus(){
    if(!m_usingShortEdit){
        m_shortEdit->raise();
    } else {
        m_longEdit->setFocus();
    }
}
void MyFullTextEdit::raise(){
    if(m_usingShortEdit){
        m_shortEdit->raise();
        m_longEdit->setVisible(false);
    } else {
        m_longEdit->raise();
        m_shortEdit->setVisible(false);
    }
}

void MyFullTextEdit::show(){
    if(m_usingShortEdit){
        m_shortEdit->show();
        m_longEdit->setVisible(false);
    } else {
        m_shortEdit->setVisible(false);
        m_longEdit->show();
    }
}

QPoint MyFullTextEdit::pos(){
    if(m_usingShortEdit){
        return m_shortEdit->pos();
    } else {
        return m_longEdit->pos();
    }
}

bool MyFullTextEdit::takingEditKeys() {
    return m_usingShortEdit;
}

void MyFullTextEdit::setMaxDisplayCharacters(int i){
    m_maxDisplayCharacters = i;
}



//This is called only just for testing... it will be "driven"
//by it's parent once installed...
void MyFullTextEdit::keyPressEvent(QKeyEvent* key){
    takeKey(key);
}


QFont MyFullTextEdit::curFont(){
    if(text().length() < m_maxBigCharacters){
        return m_bigFont; }
    else { return m_smallFont;}
}

void MyFullTextEdit::selectEditControl(){
    m_formatting = true;
    QString t = text();
    m_shortEdit->setFont(curFont());
    m_longEdit->setFont(curFont());

    if(t.length() <= m_maxDisplayCharacters || m_forceShort){
        if(!m_usingShortEdit){ activateShortEdit(t); }
    } else {
        if(m_usingShortEdit){ activateLongEdit(t); }
    }
    m_formatting = false;
}

void MyFullTextEdit::activateShortEdit(QString t){
    //m_shortEdit->setGeometry(m_desiredGeom);
    //m_longEdit->setGeometry(m_desiredGeom);
    if(!m_usingShortEdit){
        m_text = m_longEdit->toPlainText();
        m_htmlText = titlize(m_longEdit->toPlainText());
    }
    m_usingShortEdit = true;
    m_userFeedBack = false;
    emit textChangedSignal(m_text, QString(""), 0);
    m_longEdit->setVisible(false);
    m_longEdit->setEnabled(false);
    m_longEdit->setFocusPolicy(Qt::NoFocus);

    m_shortEdit->setVisible(m_isVisible);
    m_shortEdit->setText(t);
    m_shortEdit->raise();

}

void MyFullTextEdit::activateLongEdit(QString t){
    m_usingShortEdit = false;
    m_shortEdit->setVisible(false);
    testResize();

    m_longEdit->setVisible(m_isVisible);
    m_longEdit->activateWindow();
    m_longEdit->setEnabled(true);
    m_longEdit->setFocusPolicy(Qt::StrongFocus);
    m_longEdit->setFocus();
    m_longEdit->setPlainText(t);
    m_longEdit->moveCursor(QTextCursor::End);
    m_longEdit->raise();
}



void MyFullTextEdit::textChangedSlot(){
    int cursorPos =0;
    if(!m_formatting){
        selectEditControl();
        if(!m_usingShortEdit){
            testResize();
            cursorPos = this->m_longEdit->textCursor().position();
        }
    }
    if(!m_usingShortEdit){
        emit textChangedSignal(this->text(), QString(""), cursorPos);
    }

}
void MyFullTextEdit::cursorPosChange(){
    //Later we'll have internal items but not yet
}



