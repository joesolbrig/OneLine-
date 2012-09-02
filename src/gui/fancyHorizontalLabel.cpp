#include <QPainter>
#include "fancyHorizontallabel.h"

FancyHorizontalLabel::FancyHorizontalLabel(QWidget *parent) :
    QWidget(parent,  Qt::FramelessWindowHint) {

    //Tells Qt we'll paint with translucent colors
    setAttribute(Qt::WA_TranslucentBackground);
}

void FancyHorizontalLabel::paintEvent(QPaintEvent *) {

    QPainter p(this);
    if(m_opacity !=1)
        { p.setOpacity(m_opacity);}
    int newSectionWidth = m_sectionWidth +
                           (m_currentWidth - m_originalImage.width());
    QSize stretchSize(newSectionWidth, m_middle.height());

    QImage scaledMiddle = m_middle.scaled(stretchSize, Qt::IgnoreAspectRatio);

    p.drawImage(QPoint(0,0), m_left);
    int midTop = m_left.width();
    p.drawImage(QPoint(0,midTop), scaledMiddle);
    int midBottom = midTop +scaledMiddle.width();
    p.drawImage(QPoint(0,midBottom), m_right);

}

void FancyHorizontalLabel::createImageSections(){

    int upperMiddle = m_cutWidth - m_sectionWidth/2;
    QRect rLeft(0, 0,
               upperMiddle,
               m_originalImage.height() );

    m_left = m_originalImage.copy(rLeft);
    Q_ASSERT(m_left.width() == upperMiddle);

    int lowerMiddle = (m_cutWidth  + m_sectionWidth/2 +1);
    QRect rMiddle(upperMiddle +1, 0,
                  lowerMiddle - upperMiddle,
                  m_originalImage.height() );

    m_middle =m_originalImage.copy(rMiddle);

    QRect rRight(lowerMiddle+1, 0,
                  m_originalImage.width() - lowerMiddle ,
                  m_originalImage.height());
    m_right =m_originalImage.copy(rRight);

}

