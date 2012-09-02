#include "scrollbarminimal.h"

ScrollBarMinimal::ScrollBarMinimal(QObject *parent) :
    QScrollBar(parent)
{

}


virtual void ScrollBarMinimal::paintEvent ( QPaintEvent * ){
    QRect r = this->geometry();
    int extend = maximum() - minimum();
    qreal relativeSize = r.height()/(extend);
    relativeSize = MIN((0.2), relativeSize);
    r.setHeight(geometry().height() * relativeSize);
    r.moveCenter(r.height()*);






}
