
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include "individualIcon.h"
#include "iconspreadwindow.h"

IndividualIcon::IndividualIcon(MessageBarIcon *p, ListItem it) :
        QObject((QObject*)p), m_item(it) {
    m_hover = false;
    m_animating = false;
    m_magnification = 1;
    m_followingMouse = false;
    m_captured = false;
    m_delay_factor = 0;
    m_parent = p;

}

void IndividualIcon::paint( QPainter * painter,
             const QStyleOptionGraphicsItem * option, QWidget * ){

    //QGraphicsPixmapItem::paint(painter, option, widget);

    QRectF b = boundingRect();
    QIcon ic = m_item.getActualIcon();
    QRect iconRect;
    iconRect.setTopLeft(b.topLeft().toPoint());
    iconRect.setWidth(UI_STANDARD_ICON_WITH);
    iconRect.setHeight(UI_STANDARD_ICON_WITH);

    ic.paint(painter, iconRect);
    painter->save();
    if(this->isSelected()){
        QBrush brush1(option->palette.highlight());
        brush1.setStyle(Qt::SolidPattern);
        painter->setBrush(brush1);
        painter->drawRect(b);
    }
    QFont fnt = m_item.getDisplayFont();
//    QFontMetrics fm(fnt);
//    int fnt_h = fm.height();
    QRect r(0,0,0,0);
    r.setTop(b.height());
    r.setBottom(b.bottom());
    r.setWidth(b.width());


    painter->setClipRect(r);

    drawHtmlLine(painter, fnt, r, m_item.formattedName());

    painter->restore();

}


//Between 0 and 1
//void IndividualIcon::setSpreadPos(QPointF pt){
//    if(m_captured){
//        return;
//
//    }
//    float relSize = (m_parent->height() - pt.y())/2;
//    if(m_delay_factor){
//        //slow it somehow..
//        QPointF p = pos();
//        pt = p*(1- m_delay_factor) + pt*m_delay_factor;
//    }
//    if(!m_followingMouse){
//        m_delay_factor = m_delay_factor/2;
//        if(m_delay_factor < 0.1){
//            m_delay_factor = 0;
//        }
//    }
//    QTransform id;
//    QTransform tr = id.scale(relSize, relSize);
//    setPos(pt);
//    this->setTransform(tr);
//
////    QPoint diff = m_boundingRect.topLeft() - m_boundingRect.center();
////    m_boundingRect.setTopLeft(pt + diff);
//
//
//}
