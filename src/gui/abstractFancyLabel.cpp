#include "abstractFancyLabel.h"
#include





void AbstractFancyLabel::setImageHeight(int height){
    d->setItemHeight(height);
}

int AbstractFancyLabel::getImageHeight(){
    return d->getImageHeight();
}

int AbstractFancyLabel::baseHeight(){
    return d->baseHeight();
}

void AbstractFancyLabel::setOpacity(qreal o){
    d->setOpacity(o);
}

qreal AbstractFancyLabel::getOpacity(){
    return d->getOpacity();
}

void AbstractFancyLabel::animateToOpacity(qreal l){
    d->animateToOpacity(l);
}

void AbstractFancyLabel::showConnectingRegion(bool show=true){
    d->showConnectingRegion(show);
}

void AbstractFancyLabel::setExplanPopoutArea(float left, float right, float base) {
    d->setExplanPopoutArea(left,right, base);
}

void AbstractFancyLabel::setMainEditArea(float left, float right, float base) {
    d->setMainEditArea(left, right, base);
}

void AbstractFancyLabel::paintEvent(QPaintEvent *event){
    d->paintEvent(event);
}



RectangleLabel::RectangleLabel(QWidget *parent) :
    QWidget(parent,  Qt::FramelessWindowHint) {

    //Tells Qt we'll paint with translucent colors
    setAttribute(Qt::WA_TranslucentBackground);

}

void RectangleLabel::paintEvent(QPaintEvent *) {

    QPainter p(this);
    p.save();
    if(m_opacity !=1)
        { p.setOpacity(m_opacity);}
    int newSectionHeight = m_sectionHeight +
                           (m_currentHeight - m_originalImage.height());
    QSize stretchSize(m_middle.width(),newSectionHeight);

    QImage scaledMiddle = m_middle.scaled(stretchSize, Qt::IgnoreAspectRatio);

    p.drawImage(QPoint(0,0), m_top);
    int midTop = m_top.height();
    p.drawImage(QPoint(0,midTop), scaledMiddle);
    int midBottom = midTop +scaledMiddle.height();
    p.drawImage(QPoint(0,midBottom), m_bottom);


    p.restore();

    if(!m_illustrativeRegion.isEmpty()){
        p.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::RoundCap,
                            Qt::RoundJoin));
        if(p.opacity() !=1)
            { p.setOpacity(1);}
        QLinearGradient gradient(0, 0, 0, 100);
        gradient.setColorAt(0.0, Qt::darkBlue);
        gradient.setColorAt(1.0, Qt::gray);
        p.setBrush(gradient);

        p.drawPath(m_illustrativeRegion);
    }
}

