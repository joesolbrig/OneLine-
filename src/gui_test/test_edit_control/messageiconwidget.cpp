
#include "messageiconwidget.h"
#include <QList>
#include "item.h"


void GnomeSignalReceiver::setTargetWindow(MessageIconWidget* t){
    targetWindow = t;
    connect(this, SIGNAL(itemsFound(QList<CatItem> )),t, SLOT(itemsFound(QList<CatItem> )));
}

MessageBarIcon::MessageBarIcon(QObject *p, ListItem it) :
        QObject(p), m_item(it)  {
    //setMouseTracking(true);
    setAcceptsHoverEvents(true);
    //resize(sz);
    this->setPixmap(it.getActualIcon().pixmap(
            ((MessageIconWidget*)parent())->getIconSize(),QIcon::Active ));
    m_magnification = 1;
    connect(this, SIGNAL(itemSelected(ListItem)),p, SLOT(itemClicked(ListItem)));
    prepareGeometryChange();
    m_animation = new IconAnimation(this);
    m_animating = false;
    m_hover = false;
}


void MessageBarIcon::paint( QPainter * painter,
             const QStyleOptionGraphicsItem * option, QWidget * widget ){

    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->save();
    QRectF b = boundingRect();
    QFont fnt = m_item.getDisplayFont();
    QFontMetrics fm(fnt);
    int fnt_h = fm.height();
    QRect r(0,0,0,0);
    r.setTop(b.height() - ( fnt_h + ((MessageIconWidget*)parent())->getInnerPadding()));
    r.setBottom(b.bottom());
    r.setWidth(b.width());
//    QBrush brush1(QColor(255, 20, 25, 255));
//    brush1.setStyle(Qt::SolidPattern);
//    painter->setBrush(brush1);
//    painter->drawRect(r);

    painter->setClipRect(r);

    //qDebug() << "test onto rect: " << r;
    drawHtmlLine(painter, fnt, r, m_item.formattedName());

    painter->restore();

}

QRectF MessageBarIcon::boundingRect(){
    QRectF b = QGraphicsPixmapItem::boundingRect();
    QSize s = ((MessageIconWidget*)parent())->getChildSize();
    b.setWidth(s.width());
    b.setHeight(s.height());
    return b;
}

void MessageBarIcon::hoverEnterEvent(QGraphicsSceneHoverEvent *){
    if(!m_hover){
        m_hover =true;
        if(!m_animating )
            {m_animation->startAnimation();}
    }


//        this->setPixmap(m_item.getActualIcon().pixmap(
//                ((MessageIconWidget*)parent())->getIconSize(),QIcon::Active));
//        prepareGeometryChange();
//        update();
//    }
}

void MessageBarIcon::hoverLeaveEvent(QGraphicsSceneHoverEvent *){
    if(m_hover){
        m_hover =false;
//        this->setPixmap(m_item.getActualIcon().pixmap(
//                ((MessageIconWidget*)parent())->getIconSize(),QIcon::Disabled));
//        prepareGeometryChange();
//        update();
    }
}
