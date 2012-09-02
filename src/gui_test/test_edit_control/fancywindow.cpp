#include <QPixmap>
#include "fancywindow.h"


//QImage stretchFromLine(QImage in, int newHeight,
//                              int lineHeight,
//                              int sectionWidth = SECTION_HEIGHT){
//    Q_ASSERT(newHeight > in.width());
//
//    QImage top = in.copy(0, 0, m_original.width(),
//                        lineHeight - sectionWidth);
//    QImage section =in.copy(0, lineHeight - sectionWidth+1,
//                        m_original.width(), lineHeight + sectionWidth-1);
//    QImage bottom =int.copy(lineHeight + sectionWidth,in.height(),
//                        in.width(),
//                        in.height() - (lineHeight + sectionWidth));
//
//    int extraHeight = newHeight - in.height();
//
//    QImage scaledSection = section.scaledToHeight(extraHeight);
//
//    QImage newImage(newHeight, in.width());
//    QPainter p(newImage);
//    p.drawImage(0,0, top);
//    p.drawImage(0,lineHeight + sectionWidth, scaledSection);
//    p.drawImage(lineHeight + sectionWidth+newHeight, bottom);
//    return newImage;
//
//}

FancyWindow::FancyWindow(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint ) {
    //this->setAutoFillBackground(false); - default

    //Tell Qt we're going to be doing translucent painting...
    setAttribute(Qt::WA_TranslucentBackground);
    QPixmap a;



}


void FancyWindow::mouseMoveEvent(QMouseEvent *event) {

}


void FancyWindow::mousePressEvent(QMouseEvent *event) {

}

void FancyWindow::paintEvent(QPaintEvent *event) {

    QImage scaledSection = section.scaledToHeight(extraHeight);

    QPainter p(event);
    p.drawImage(0,0, top);
    p.drawImage(0,lineHeight + sectionWidth, scaledSection);
    p.drawImage(lineHeight + sectionWidth+newHeight, bottom);


}

void FancyWindow::resizeEvent(QResizeEvent *event) {

}


void FancyWindow::calculateWindowSize() {

}

