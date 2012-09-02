#include <QPainter>
#include "fancylabel.h"
#include "fancyenclosingrect.h"
#include "appearance_window.h"

FancyLabel::FancyLabel(QWidget *parent) :
    QWidget(parent,  Qt::FramelessWindowHint) {

    //Tells Qt we'll paint with translucent colors
    setAttribute(Qt::WA_TranslucentBackground);
    m_generated = true;
    m_flat = false;
    setGen();
}

void FancyLabel::setGeometry(QRect r){
    QWidget::setGeometry(r);
    qDebug() << "FancyLabel::setGeometry" << r;
    setGen();

}

void FancyLabel::resize(QSize sz){
    QWidget::resize(sz);
    qDebug() << "FancyLabel::resize" << sz;
    setGen();
}

void FancyLabel::move(const QPoint p)
{
    QWidget::move(p);
    qDebug() << "FancyLabel::move" << p;
    setGen();
}

void FancyLabel::setGen(){
    if(m_generated){
        QRect r = gMainWidget->geometry();
        r.setHeight(height());
        r.moveTopLeft(QPoint(0,0));
        m_pPaths = FancyEnclosingRect::createStyledRects(r,MAIN_EDIT_NAME,m_flat);
    }
}


void FancyLabel::paintEvent(QPaintEvent *) {

    QPainter painter(this);
    if(!m_generated){
        painter.save();
        if(m_opacity !=1)
            { painter.setOpacity(m_opacity);}
        int newSectionHeight = m_sectionHeight +
                               (m_currentHeight - m_originalImage.height());
        QSize stretchSize(m_middle.width(),newSectionHeight);

        QImage scaledMiddle = m_middle.scaled(stretchSize, Qt::IgnoreAspectRatio);

        painter.drawImage(QPoint(0,0), m_top);
        int midTop = m_top.height();
        painter.drawImage(QPoint(0,midTop), scaledMiddle);
        int midBottom = midTop +scaledMiddle.height();
        painter.drawImage(QPoint(0,midBottom), m_bottom);
        painter.restore();
    } else {
        painter.save();
        for(int i=m_pPaths.count()-1; i>=0;i--){
            painter.fillPath(m_pPaths[i].first, m_pPaths[i].second );
            //painter.drawPixmap(m_marginRect,m_backgroundImage);
        }
        painter.restore();


    }


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

