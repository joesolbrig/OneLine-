
#include "appearance_window.h"
#include "../globals.h"
#include "multiTextDisplay.h"

MultiTextDisplay::MultiTextDisplay(QWidget* parent): QWidget(parent,Qt::FramelessWindowHint),
        m_mainEdit(this),  m_explanationLabel(this),
        m_meaningIconWidget(this),
        //m_actionIconLabel(this,Qt::FramelessWindowHint),
        m_backgroundLabel(this) {

    //m_meaningIconWidget.setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    QFont f = QWidget::font();
    f.setPointSize(UI_BASE_FONT_SIZE);
    QWidget::setFont(f);
    m_mainEdit.setFont(f);

    f.setPointSize(UI_EXPLANATION_FONT_SIZE);
    m_explanationLabel.setFont(f);

    //Put this in our larger window eventually...
    m_iconSize = UI_MAIN_ICON_SIZE;
    m_iconRatio = 1;

    m_horizontalMargin = MAX(m_iconSize, UI_LEFT_MARGIN);
    m_imageMidsectionWidth = UI_IMAGE_MIDSECTION_WIDTH;
    m_backgroundCutHeight = UI_BACKGROUND_CUT_HEIGHT;
    QString imageFile(UI_IMAGE_FILE);
    qDebug() << "MultiTextDisplay imageFile: " << imageFile;
    m_backgroundImage = QImage(imageFile);
    setMargin(UI_BORDER_WINDOW_TOP_MARGIN);

    m_backgroundLabel.setupImage(getBackgroundImage(),getBackgroundCutHeight(),getImageMidsectionWidth());
    m_backgroundLabel.show();
    m_backgroundLabel.move(0,m_backgroundLabel.pos().y());
    m_explanationLabel.hide();

    //m_mainEdit.setObjectName("editLine");
    m_mainEdit.show();
    m_mainEdit.raise();

    //connect(&m_currentEdit, SIGNAL(keyPressed()), this,SLOT(insideKeyPress()));
    connect(&m_mainEdit, SIGNAL(rectangleResizing()), this,SLOT(rectangleResizing()),Qt::QueuedConnection);
    connect(&m_mainEdit, SIGNAL(textChangedSignal(QString, QString , int )),
            this,SLOT(textChangedSlot(QString, QString , int )));

    m_explanationFieldAdjust =-1;

    QRect r(0,0,m_backgroundLabel.baseWidth(), invisibleAreaHeight()+visibleDesiredHeight());
    qDebug() << "MultiTextDisplay starting geo:" << r;
    setGeometry(r);
    adjustForTextChange();

}


void MultiTextDisplay::positionTextEdit(){
    QSize sz = m_backgroundLabel.size();
    sz.setHeight(m_backgroundLabel.baseHeight());
    sz.setWidth(m_backgroundLabel.baseWidth());
    m_mainEdit.setAppropriateSize(sz);
    m_mainEdit.move(QPoint(getLeftMargin(), invisibleAreaHeight() + getTopMargin()));

}

void MultiTextDisplay::adjustForTextChange(bool forceAdjust){
    //order matters
    positionTextEdit();
    sizeParentHeightsForChildren(forceAdjust);
    placeBackgroundLabel();
    placeExplanationText();
    update();
}

void MultiTextDisplay::sizeParentHeightsForChildren(bool forceAdjust){
    int desiredVHeight = visibleDesiredHeight();

    int totalCurrentHeight = invisibleAreaHeight() + m_backgroundLabel.currentHeight();
    int tDesireHeight = totalDesiredHeight(); //bottom margin too

    if(tDesireHeight > totalCurrentHeight + 1
       ||(tDesireHeight < totalCurrentHeight - m_mainEdit.lineHeight()/2)
       ||forceAdjust){
        m_backgroundLabel.setCurrentHeight(desiredVHeight);
        setParentHeight(tDesireHeight);
    }
}

int MultiTextDisplay::visibleDesiredHeight(){
    int controlHeight = m_mainEdit.desiredHeight();
    int visibleDesiredHeight =getTopMargin() + controlHeight + getBottomMargin();
    visibleDesiredHeight = MAX(visibleDesiredHeight, m_backgroundLabel.baseHeight());
    return visibleDesiredHeight;
}

void MultiTextDisplay::setParentHeight(int height){
    QRect r = gMainWidget->geometry();
    int width = r.width();
    QSize s(width, height);
    gMainWidget->setPositionFromSize(s);
}

//void MultiTextDisplay::placeBackgroundLabel(){
//    int invisibleHeight = invisibleAreaHeight();
//    m_backgroundLabel.move(0, invisibleHeight);
//}

void MultiTextDisplay::placeBackgroundLabel(){
    m_backgroundLabel.move(0, invisibleAreaHeight());
    float left = getLeftMargin();
    float right = left + m_mainEdit.textWidth();
    float base = m_mainEdit.geometry().bottom() + getBottomMargin();
    base = MAX(base,m_backgroundLabel.currentHeight());
    m_backgroundLabel.setMainEditArea(left,right,base);
}


void MultiTextDisplay::placeExplanationText(){
    QRect explanR = m_explanationLabel.geometry();
    explanR.setHeight(m_mainEdit.lineHeight());
    explanR.setRight(visibleRect().right());
    explanR.moveBottom(visibleRect().bottom());
    explanR.setLeft(getLeftMargin() + UI_MAINBOX_BTM_EXPL_TEXT_OFFSET);
    m_explanationLabel.setGeometry(explanR);
    m_explanationLabel.show();
    m_explanationLabel.raise();
}







void MultiTextDisplay::setExplanationText(QString formattedText, int , int ){

    m_explanationTextWidth=-1;
    m_explanationLabel.setText(formattedText);
    m_explanationLabel.show();
    m_explanationLabel.raise();
    m_explanationFieldAdjust =-1;

}


QRect MultiTextDisplay::visibleRect(){
    return m_backgroundLabel.geometry();
}

QPoint MultiTextDisplay::visibleTopLeft(){
    return m_backgroundLabel.pos();
}

//--------------------XXXXXXXXXXXXXX

int MultiTextDisplay::invisibleAreaHeight(){
    return MAX(0,getIconSize()/2 - getBottomMargin() );
}

void MultiTextDisplay::setFlat(bool flat){
    m_backgroundLabel.setFlat(flat);
}

int MultiTextDisplay::totalDesiredHeight(){
    return invisibleAreaHeight() + visibleDesiredHeight();
}


void MultiTextDisplay::setOpacity(qreal o){
    m_backgroundLabel.setOpacity(o);
}

qreal MultiTextDisplay::getOpacity(){
    return m_backgroundLabel.getOpacity();
}

bool MultiTextDisplay::takingEditKeys(){
    return m_mainEdit.atLongEdit();
}

void MultiTextDisplay::setVisible(bool visible){
    QWidget::setVisible(visible);
    m_mainEdit.setVisible(visible);
    m_backgroundLabel.setVisible(visible);
    m_meaningIconWidget.setVisible(visible);
}

void MultiTextDisplay::setEditVisibility(bool visible){
    m_mainEdit.setVisible(visible);
}

bool MultiTextDisplay::setKeyPress(QKeyEvent* key){
    return m_mainEdit.takeKey(key);
}

void MultiTextDisplay::setKeyRelease(QKeyEvent* key){
    m_mainEdit.takeKeyRelease(key);
}



void MultiTextDisplay::setGeometry(QRect r){
    QWidget::setGeometry(r);
    m_mainEdit.setAppropriateSize();
    m_mainEdit.move(QPoint(r.left()+ getLeftMargin(),
        invisibleAreaHeight() + getTopMargin()));
    placeIconWidget();

}

void MultiTextDisplay::show(){
    QWidget::show();
    m_backgroundLabel.show();
    m_mainEdit.show();
    m_mainEdit.raise();
}

QImage MultiTextDisplay::getBackgroundImage() const {
    return m_backgroundImage;
}

void MultiTextDisplay::setBackgroundImage(QImage s) {
    m_backgroundImage = s;
}

int MultiTextDisplay::getIconSize() const {
    return m_iconSize;
}

void MultiTextDisplay::setIconSize(int s) {
    m_iconSize = s;
}

float MultiTextDisplay::getIconRatio() const {
    return m_iconRatio;
}
void MultiTextDisplay::setIconRatio(float s) {
    m_iconRatio = s;
}

int MultiTextDisplay::getTopMargin() const {
    int baseH = m_backgroundLabel.baseHeight();
    int baseTextH = m_mainEdit.lineHeight();
    int marginForCentering = (baseH - baseTextH)/2;
    return MAX(m_verticalMargin,marginForCentering);
}

int MultiTextDisplay::getBottomMargin() const {
    return m_verticalMargin;
}

void MultiTextDisplay::setMargin(int s) {
    int baseH = this->m_backgroundLabel.baseHeight();
    int baseTextH = this->m_mainEdit.lineHeight();
    m_verticalMargin = MAX(s, (baseH - baseTextH)/2);
    int curH = this->m_backgroundLabel.currentHeight();
    int requestedH = m_mainEdit.lineHeight() + m_verticalMargin*2;
    if(requestedH > curH){
        m_backgroundLabel.setCurrentHeight(requestedH);
    }
}

int MultiTextDisplay::getLeftMargin() const {
    return MAX(m_horizontalMargin,
               m_meaningIconWidget.geometry().width());
}

int MultiTextDisplay::getRightMargin() const {
    return (m_horizontalMargin);
}

void MultiTextDisplay::setRightMargin(int s) {
    m_horizontalMargin = s;
}

int MultiTextDisplay::getBackgroundCutHeight() const {
    return m_backgroundCutHeight;
}

void MultiTextDisplay::setBackgroundCutHeight(int s) {
    m_backgroundCutHeight = s;
}


int MultiTextDisplay::getImageMidsectionWidth() const {
    return m_imageMidsectionWidth;
}

void MultiTextDisplay::setImageMidsectionWidth(int s) {
    m_imageMidsectionWidth = s;
}

void MultiTextDisplay::setMainPlainText(QString s, bool forceAdjust){
    m_mainEdit.setPlaintText(s);
    adjustForTextChange(forceAdjust);
}

void MultiTextDisplay::setFeedbackText(QString s){
    m_mainEdit.setFeedbackText(s);
    adjustForTextChange();
}

void MultiTextDisplay::clearText(){
    return m_mainEdit.clearText();
}

bool MultiTextDisplay::hasText(){
    return m_mainEdit.hasText();
}

void MultiTextDisplay::setMainHtmlText(QString html, QString s ){
    m_mainEdit.setHtmlText(html, s);
    adjustForTextChange();
}

QString MultiTextDisplay::getMainText(){
    return m_mainEdit.text();
}

QString MultiTextDisplay::getMainHtmlText(){
    QString res = m_mainEdit.htmlText();
    qDebug() << "got html res:" << res;
    return res;
}

void MultiTextDisplay::setActiveIconPos(int pos){
    m_meaningIconWidget.setActiveIconPos(pos);
}


void MultiTextDisplay::placeIconWidget(){
//    qDebug() << "setIconPos()" ;
//    qDebug() << "totalDesiredHeight()" << totalDesiredHeight();
//    qDebug() << "invisibleAreaHeight()" << invisibleAreaHeight();
//    qDebug() << "visibleDesiredHeight" << visibleDesiredHeight();
//    qDebug() << "geometry: " << geometry();
    QRect r(UI_MAINBOX_LEFT_MARGIN,0, 0,0); //zero h&w 'cause it should set it own size...
    r.setBottom(invisibleAreaHeight() + visibleDesiredHeight()) ;//- getBottomMargin())

    //qDebug() << "icon rect:" << r;
    raise();
    m_meaningIconWidget.raise();
    m_meaningIconWidget.show();
    m_meaningIconWidget.setGeometry(r);
    m_meaningIconWidget.formatIcons();

}

void MultiTextDisplay::setExpressionIcon(QIcon i, int pos){
    QSize s(m_iconSize, m_iconSize);
    m_meaningIconWidget.setIcon(i.pixmap(s),pos);
    placeIconWidget();
}

int MultiTextDisplay::expressionIconCount(){
    return m_meaningIconWidget.expressionIconCount();
}

int MultiTextDisplay::activeExpressionIcon(){
    return m_meaningIconWidget.activeExpressionIcon();
}

void MultiTextDisplay::clearExpressionIcons(){
    m_meaningIconWidget.clearIcons();
    placeIconWidget();

}


void MultiTextDisplay::popIcon(){
    m_meaningIconWidget.popIcon();
    placeIconWidget();
}

void MultiTextDisplay::animateIconRight(){
    m_meaningIconWidget.animateRight();
    placeIconWidget();
}
void MultiTextDisplay::animateIconLeft(){
    m_meaningIconWidget.animateLeft();
}

//    int desiredHeight(){
//        int h = m_currentEdit.desiredHeight();
//        h = MAX(h, m_backgroundLabel.minimumHeight());
//        return h;
//    }

bool MultiTextDisplay::getAtFull(){
    return m_mainEdit.atLongEdit();
}



void MultiTextDisplay::rectangleResizing(){
    adjustForTextChange();
    emit rectangleResizingSignal();
}

void MultiTextDisplay::textChangedSlot(QString s, QString t, int i){
    emit textChanged(s, t, i);

}










