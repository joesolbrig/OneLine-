
#include "messageiconwidget.h"
#include <QList>
#include "item.h"
#include "appearance_window.h"

MessageBarIcon::MessageBarIcon(QObject *p, ListItem it) :
        QObject(p), m_item(it)  {
    //setMouseTracking(true);
    setAcceptsHoverEvents(true);
    //m_parent = (MessageIconWidget*)p;

    m_pixmap_sz = ((MessageIconWidget*)parent())->getIconSize();
    setPixmap(it.getActualIcon().pixmap( m_pixmap_sz,QIcon::Active));

    m_magnification = 0;
    connect(this, SIGNAL(itemClicked(ListItem, bool)),p, SLOT(itemClicked(ListItem, bool)));
    prepareGeometryChange();
    m_animation = new IconAnimation(this);
    m_animating = false;
    m_hovered = false;
    m_activated = false;
    QRectF f = boundingRect();
    setOffset((abs(f.width()-m_pixmap_sz.width()))/2,f.top());

    m_textItem = new QGraphicsTextItem(this);
    QFont fnt = it.getDisplayFont();
    fnt.setPointSize(fnt.pointSize() +1);
    m_textItem->setFont(fnt);
    QRectF r = boundingRect();
    r.moveTopLeft(QPoint(0,0));
    m_textItem->setTextWidth(r.width()*3);
    //QFontMetrics fm(it.getDisplayFont());
    //qreal fnt_h = fm.height();
    //m_textItem->setPos(r.bottom() - fnt_h,r.left());
    m_textItem->setPos(r.topLeft());
    m_textItem->setZValue(zValue()+1);
    m_textItem->setHtml(tagAs(QString(m_item.hotkeyChar()),"center"));
    m_textItem->show();
    m_textItem->setAcceptHoverEvents(false);

    ((MessageIconWidget*)parent())->addItem(m_textItem);

}

MessageBarIcon::~MessageBarIcon(){
    if(m_animation){
        m_animation->deleteLater();
    }
}


void MessageBarIcon::paint( QPainter * painter,
             const QStyleOptionGraphicsItem * option, QWidget * widget ){

    QRectF b = QGraphicsPixmapItem::boundingRect();//boundingRect();
    //b.setHeight(m_pixmap_sz.height());
    if(m_activated){
        painter->save();
        QPen pn;
        pn.setColor(UI_SELECTED_MINI_ICON_BACKGROUND);
        pn.setWidth(UI_SELECTED_MINI_ICON_PENWIDTH);
        painter->setPen(pn);
        painter->drawRect(b.toRect());
        painter->restore();
    }
    //QGraphicsPixmapItem::paint(painter, option, widget);
    QGraphicsPixmapItem::paint(painter, option, widget);
    painter->save();

//    if(m_hovered || m_magnification!=0){
//        QFont fnt = m_item.getDisplayFont();
//        QFontMetrics fm(fnt);
//        int fnt_h = fm.height();
//        QRect r(b.toRect());

//        QRect r(m_savedRectF.toRect());
//        r.moveTop(r.bottom()+fnt_h*(1.5));
//        r.setHeight(fnt_h*2);
        //r.setWidth(r.width()*4);
//        r.moveCenter(r.center());
//        r.setTop(r.height() - ( fnt_h ));//+ ((MessageIconWidget*)parent())->getInnerPadding();
//        r.setBottom(r.bottom());
        //painter->setClipRect(r);
//        drawHtmlLine(painter, fnt, r, m_item.formattedName());
//    } else if(gMainWidget->st_altKeyDown){
//        QFont fnt = m_item.getDisplayFont();
//        QRect r(b.toRect());
//        drawHtmlLine(painter, fnt, r, m_item.hotkeyChar());
//    }
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
    if(!m_hovered){
        m_hovered =true;
        m_textItem->setHtml(tagAs(m_item.formattedName(false),"center"));
        if(!m_animating ) {
            m_savedRectF = QGraphicsPixmapItem::boundingRect();
            setTransformOriginPoint(m_savedRectF.center().x(), m_savedRectF.bottom());
            m_animation->startAnimation();
        }
    }

//    if(m_parent->m_hoverText){
//        m_parent->m_scene->removeItem(m_parent->m_hoverText);;
//        delete m_parent->m_hoverText;
//        m_parent->m_hoverText =0;
//    }
//    QFont fnt = m_item.getDisplayFont();
//    m_parent->m_hoverText =m_parent->m_scene->addText(m_item.formattedName(),fnt);
//    m_parent->m_hoverText->setPos(boundingRect().bottomLeft());

    update();


//        this->setPixmap(m_item.getActualIcon().pixmap(
//                ((MessageIconWidget*)parent())->getIconSize(),QIcon::Active));
//        prepareGeometryChange();
//        update();
//    }
}

void MessageBarIcon::hoverLeaveEvent(QGraphicsSceneHoverEvent *){
    if(m_hovered){
        m_hovered =false;
        m_textItem->setHtml(tagAs(QString(m_item.hotkeyChar()),"center"));
//        this->setPixmap(m_item.getActualIcon().pixmap(
//                ((MessageIconWidget*)parent())->getIconSize(),QIcon::Disabled));
//        prepareGeometryChange();
//        update();
    }
//    if(m_parent->m_hoverText){
//        m_parent->m_scene->removeItem(m_parent->m_hoverText);;
//        delete m_parent->m_hoverText;
//        m_parent->m_hoverText =0;
//    }
    update();
}

void MessageBarIcon::mousePressEvent(QGraphicsSceneMouseEvent *){
    m_activated = !m_activated;
    emit itemClicked(m_item, m_activated);
}


MessageIconWidget::MessageIconWidget(QWidget* par, ListItem psuedoParentItem): QWidget(par) {
    if(psuedoParentItem.isEmpty()){
        ListItem pI("builtin://dummy");
        pI.setCustomPluginValue(CUSTOM_MSG_PADDING, UI_DEFAULT_MINI_ICON_PADDING);
        pI.setCustomPluginValue(CUSTOM_MSG_BAR_PADDING, UI_DEFAULT_MINI_ICONBAR_PADDING);
        pI.setCustomPluginValue(CUSTOM_ICON_SIZE, UI_DEFAULT_MINI_ICON_SIZE);
        psuedoParentItem = pI;
    }
    m_item = psuedoParentItem;
    m_innerPadding = m_item.getCustomValue(CUSTOM_MSG_PADDING, UI_DEFAULT_MINI_ICON_PADDING);
    m_outterPadding = m_item.getCustomValue(CUSTOM_MSG_BAR_PADDING, UI_DEFAULT_MINI_ICONBAR_PADDING);
    m_icon_size = m_item.getCustomValue(CUSTOM_ICON_SIZE, UI_ACTIVE_ICON_SIZE);
    m_ImageLabel=0;
    //m_hoverText =0;

    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    QRect r = par->geometry();
    this->setGeometry(r);
    m_view->setSceneRect(r);
    m_scene->setSceneRect(r);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //m_view->setViewport(par);


    //m_view->setStyleSheet("background: transparent");

    //setAutoFillBackground(true);

    qDebug() << "policy height" << m_icon_size + 2*m_outterPadding;
    QSize s(1000,m_icon_size + 2*m_outterPadding);
    setMinimumSize(s);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setEnabled(true);
    setVisible(true);

    m_view->show();
    m_view->raise();
    connect(this, SIGNAL(miniIconClicked(ListItem,bool)),
            (QObject*)gMainWidget, SLOT(miniIconClicked(ListItem,bool)));


}

void MessageIconWidget::itemClicked(ListItem it, bool selected){
    if(selected){
        m_activeItem = it;
    } else {
        m_activeItem = ListItem();
    }

//    QString selectedPath = it.getPath();
//    QList<QGraphicsItem *> itms = this->m_scene->items();
//    for(int i=0; i< itms.count(); i++){
//        MessageBarIcon* item = (MessageBarIcon*)itms[i];
//        QString itemPath = item->getItem().getPath();
//        if(!(itemPath == selectedPath)){
//            item->setActive(false);
//        } else {
//            item->setActive(true);
//        }
//    }
    emit miniIconClicked(it, selected);
    update();
}

void MessageIconWidget::addItem(ListItem li, int i){
    MessageBarIcon* icon = new MessageBarIcon(this,li);
    if(li == m_activeItem){
        icon->setActiveFilterIcon(true);;
    }
    icon->setPos(itemPos(i));
    m_scene->addItem(icon);
    m_view->show();
    m_view->raise();
}

void MessageIconWidget::addItem(QGraphicsItem* gItem){
    m_scene->addItem(gItem);
    m_view->show();
    m_view->raise();
}

void MessageIconWidget::setGeometry(QRect r){
    //Round-off to icon size
    if(m_ImageLabel){
        m_ImageLabel->move(r.topLeft());
        m_ImageLabel->setCurrentWidth(r.width());
        r = (m_ImageLabel->rect());
    }

    int mod_h = r.height();

    qDebug() << "Icon Widget set height" << mod_h;

    r.setHeight(mod_h);
    int w = r.width();

    w -= 2*m_outterPadding;
    w /= m_icon_size + 2*m_innerPadding;
    w *= m_icon_size + 2*m_innerPadding;
    w += 2*m_outterPadding;
    //w = MAX(w,100);
    r.setWidth(MAX(r.width(),w));
    QWidget::setGeometry(r);
    m_view->setGeometry(r);
    r.moveLeft(0);
    r.moveTop(0);
//        m_scene->setSceneRect(r);
//        m_view->setSceneRect(r);
    QRectF rr(r);

    m_view->setSceneRect(rr);
    m_scene->setSceneRect(rr);
    m_scene->update();
    m_view->show();


}
