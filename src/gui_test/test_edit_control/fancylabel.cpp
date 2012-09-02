#include <QPainter>
#include "fancylabel.h"

FancyLabel::FancyLabel(QWidget *parent) :
    QWidget(parent,  Qt::FramelessWindowHint) {

    //Tells Qt we'll paint with translucent colors
    setAttribute(Qt::WA_TranslucentBackground);
}

void FancyLabel::paintEvent(QPaintEvent *) {

    QPainter p(this);
    int newSectionHeight = m_sectionHeight +
                           (m_currentHeight - m_originalImage.height());
    QSize stretchSize(m_middle.width(),newSectionHeight);

    QImage scaledMiddle = m_middle.scaled(stretchSize, Qt::IgnoreAspectRatio);

    p.drawImage(QPoint(0,0), m_top);
    int midTop = m_top.height();
    p.drawImage(QPoint(0,midTop), scaledMiddle);
    int midBottom = midTop +scaledMiddle.height();
    p.drawImage(QPoint(0,midBottom), m_bottom);

}

void FancyLabel::createImageSections(){

    int upperMiddle = m_cutHeight - m_sectionHeight/2;
    QRect rTop(0, 0,
               m_originalImage.width(),
               upperMiddle);

    m_top = m_originalImage.copy(rTop);
    Q_ASSERT(m_top.height() == upperMiddle);

    int lowerMiddle = (m_cutHeight  + m_sectionHeight/2 +1);
    QRect rMiddle(0, upperMiddle +1,
                  m_originalImage.width(),
                  lowerMiddle - upperMiddle);

    m_middle =m_originalImage.copy(rMiddle);

    QRect rBottom(0, lowerMiddle+1,
                  m_originalImage.width(),
                  m_originalImage.height() - lowerMiddle);
    m_bottom =m_originalImage.copy(rBottom);

}

