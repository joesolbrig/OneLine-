
#include "messageiconwidget.h"

#include <QList>
#include "item.h"
#include "appearance_window.h"
#include "textmessagewidget.h"
#include "list_item.h"


TextBarItem::TextBarItem(TextMessageBar *p, ListItem it) :
        QObject(p), m_item(it)  {
    setAcceptsHoverEvents(true);

    connect(this, SIGNAL(itemClicked(ListItem, bool)),p, SLOT(itemClicked(ListItem, bool)));
    prepareGeometryChange();
    m_animation = 0;
    m_animating = false;
    m_hovered = false;
    m_activated = m_item.hasLabel(ACTIVE_FILTER_ITEM_KEY);
    m_textItem=0;

    m_textItem = new QGraphicsTextItem();
    QFont fnt = m_item.getDisplayFont();
    //QFont fnt(UI_MINI_BAR_FONT,FONT_SIZE_DEFAULT);
    fnt.setBold(true);
    if(m_item.getFilterRole() == CatItem::ACTIVE_CATEGORY){
        fnt.setPointSize(fnt.pointSize()+2);
        m_textItem->setDefaultTextColor(UI_ICONBAR_ITEM_COLOR);
    } else if(m_item.getFilterRole() == CatItem::CATEGORY_FILTER){
        fnt.setPointSize(fnt.pointSize()+1);
        m_textItem->setDefaultTextColor(UI_ICONBAR_ITEM_COLOR);
    } else if(m_item.getFilterRole() == CatItem::SUBCATEGORY_FILTER){
        fnt.setPointSize(fnt.pointSize()-1);
        m_textItem->setDefaultTextColor(UI_ICONBAR_ITEM_COLOR);
    } else {
        fnt.setPointSize(fnt.pointSize());
        m_textItem->setDefaultTextColor(UI_ICONBAR_MESSAGE_COLOR);
    }
    m_textItem->setFont(fnt);
    m_textItem->document()->setTextWidth((-1)); //never break
    QString htmlText = tagAs(QString(m_item.getNameForEditLine()),"center");
    m_textItem->setHtml(htmlText);
    m_textItem->document()->setDocumentMargin(0); //default is 4!
    m_textItem->document()->setIndentWidth(0);
    m_textItem->document()->setDefaultFont(fnt);
    if(m_item.getFilterRole() == CatItem::SUBCATEGORY_FILTER){
        m_textItem->document()->setDefaultStyleSheet(UI_MINIBAR_DEFAULT_STYLE);
    } else {
        m_textItem->document()->setDefaultStyleSheet(UI_MINIBAR_DEFAULT_STYLE);
    }
    m_textItem->document()->setTextWidth((-1)); //never break
    //qDebug() << "Text miniItem Width" << m_textItem->textWidth();
    m_textItem->setPos(QPointF(0,0));
    m_textItem->setZValue(zValue()+1);
    m_textItem->show();
    m_textItem->setAcceptHoverEvents(false);
    p->addTextItem(m_textItem);
    m_aC = QColor(100,100,100);
    m_bC = QColor(225,225,220);
    //m_currentBackgroundColor = m_aC;
    setBigness(0);
    m_savedRectF = boundingRect();
}

void TextBarItem::paint( QPainter * painter,
             const QStyleOptionGraphicsItem * , QWidget * ){

    QRectF b = fixedRect();
//    QRectF textR = m_textItem->boundingRect();
//    b.moveCenter(textR.center());  text.document()->size().width();

    //boundingRect();
    //b.setHeight(m_pixmap_sz.height());
//    painter->save();
//    QPen pn;
//    pn.setColor(UI_SELECTED_MINI_ICON_BACKGROUND);
//    pn.setWidth(UI_SELECTED_MINI_ICON_PENWIDTH);
//    painter->setPen(pn);
//    painter->drawRect(b.toRect());
//    painter->restore();

    painter->save();
    if(m_activated || (m_item.getFilterRole() == CatItem::ACTIVE_CATEGORY)){
        painter->fillRect(b, m_bC);
    }
    painter->restore();
}


TextBarItem::~TextBarItem(){
    if(m_animation){
        m_animation->deleteLater();
    }
//    if(m_textItem){
//        m_textItem->deleteLater();
//    }
}

void TextBarItem::setText(QString text){
    m_item.setName(text);
    updateText();
    update();
}

void TextBarItem::updateText(){
    if(!m_textItem){ return;}

    bool altDn = gMainWidget->st_altKeyDown;
    if(altDn ){
        m_item.getNameForEditLine();
        m_textItem->setHtml(tagAs(QString(m_item.formattedName(false)),"center"));
    } else {
        m_textItem->setHtml(tagAs(QString(m_item.getName()),"center"));
    }
    m_textItem->setZValue(zValue()+1);
    m_textItem->document()->setDefaultStyleSheet(UI_MINIBAR_DEFAULT_STYLE);
    m_textItem->update();
}

QRectF TextBarItem::boundingRect() const{
    QRectF b = baseBoundingRect();
    b.setLeft(b.left() -UI_DEFAULT_TEXT_ITEM_H_PADDING);
    b.setRight(b.right() +UI_DEFAULT_TEXT_ITEM_H_PADDING);
    b.setTop(b.top() -UI_DEFAULT_TEXT_ITEM_V_PADDING);
    b.setRight(b.right() +UI_DEFAULT_TEXT_ITEM_V_PADDING);
    return b;
}

QRectF TextBarItem::baseBoundingRect() const{
    QRectF b;
    if(!m_textItem){ return b;}

    b = m_textItem->boundingRect();
    //b.setWidth(m_textItem->textWidth());
    QFontMetrics fm(this->m_textItem->font());
    qreal w = fm.width(m_item.getName());
    b.setWidth(w);
    QSize s = ((TextMessageBar*)parent())->getChildSize();
    b.setWidth(MAX(b.width(),s.width()));
    b.setHeight(MAX(b.height(),s.height()));
    return b;
}

void TextBarItem::hoverEnterEvent(QGraphicsSceneHoverEvent *){
    if(m_item.getFilterRole() == CatItem::MESSAGE_ELEMENT){ return;}
    if(!m_textItem){ return;}
    if(!m_hovered){
        m_hovered=true;
        m_textItem->setHtml(tagAs(m_item.formattedName(false),"center"));
        if(!m_animating){
            m_savedRectF = baseBoundingRect();
            int textWidth = m_textItem->document()->textWidth();
            setTransformOriginPoint(textWidth/2, m_savedRectF.center().y());
            QRectF textRect = m_textItem->boundingRect();
            QFontMetrics fm(m_textItem->document()->defaultFont());
            qreal extraHeight = fm.ascent();
            m_textItem->setTransformOriginPoint(textRect.center().x(),textRect.center().y() - extraHeight);
            if(m_animation){delete m_animation;}
            m_animation = new QPropertyAnimation(this,"bigness",parent());
            //m_animation->setDuration(UI_FADEIN_TIME/4);
            QEasingCurve ec(QEasingCurve::OutCubic);
            m_animation->setEasingCurve(ec);
            m_animation->setStartValue(0);
            m_animation->setEndValue(1);
            //m_animation->setDuration(5000);
            connect(m_animation, SIGNAL(finished()), this,SLOT(animationDone()));
            m_animation->start();
        }
    }
    update();
}

void TextBarItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *){
    if(m_item.getFilterRole() == CatItem::MESSAGE_ELEMENT){ return;}
    m_hovered =false;
    if(!m_animating){
        setBigness(0);
    }
    updateText();
    update();
}

void TextBarItem::animationDone(){
    m_animating = false;
    if(!m_hovered){
        setBigness(0);
    }
}

void TextBarItem::setBigness(qreal b){
    m_bigness = b;
    if(m_textItem){
        m_textItem->setScale(1+m_bigness/8);
        qreal q = m_bigness*2;
        qreal p = MIN(MAX(1-q,0),1);
        q = MIN(MAX(1-p,0),1);
        Q_ASSERT(p + q ==1);
        m_currentBackgroundColor = QColor((m_aC.red()*p+m_bC.red()*q),
                                          (m_aC.green()*p+m_bC.green()*q),
                                          (m_aC.blue()*p+m_bC.blue()*q),
                                          255);
    }

//    qreal h =(m_aC.hue()*p+m_bC.hue()*q)/2;
//    qreal s = (m_aC.saturation()*p+m_bC.saturation()*q)/2;
//    qreal v = (m_aC.value()*p+m_bC.value()*q)/2;
//    m_currentBackgroundColor.setHsv(h,s,v,255);
    update();
}


void TextBarItem::mousePressEvent(QGraphicsSceneMouseEvent *){
    if(m_item.getFilterRole() == CatItem::MESSAGE_ELEMENT){ return;}
    setSelected(!m_activated);
    emit itemClicked(m_item, m_activated);
}

void TextBarItem::setSelected(bool sel){
    if(m_item.getFilterRole() == CatItem::MESSAGE_ELEMENT){ return;}
    m_activated = sel;
    if(m_activated || (m_item.getFilterRole() == CatItem::ACTIVE_CATEGORY)){
        m_textItem->setDefaultTextColor(UI_ICONBAR_ITEM_COLOR);
    } else {
        m_textItem->setDefaultTextColor(UI_ICONBAR_ACTIVE_ITEM_COLOR);
    }
}



TextMessageBar::TextMessageBar(QWidget* par, ListItem psuedoParentItem): QWidget(par) {
    m_navTextItem=0;
    m_backgroundItem = 0;
    m_scene = 0;
    m_view = 0;

    if(psuedoParentItem.isEmpty()){
        ListItem pI;
        pI.setCustomPluginValue(CUSTOM_MSG_PADDING, UI_DEFAULT_MINI_ICON_PADDING);
        pI.setCustomPluginValue(CUSTOM_MSG_PADDING, UI_DEFAULT_MINI_ICONBAR_PADDING);
        pI.setCustomPluginValue(CUSTOM_ICON_SIZE, UI_DEFAULT_MINI_ICON_SIZE);
        psuedoParentItem = pI;
    }
    m_item = psuedoParentItem;
    m_vertPadding = m_item.getCustomValue(CUSTOM_MSG_PADDING, UI_DEFAULT_ICONBAR_SIDE_PADDING);
    m_horzPadding = m_item.getCustomValue(CUSTOM_MSG_PADDING, UI_DEFAULT_ICONBAR_TOP_PADDING);
    m_icon_size = m_item.getCustomValue(CUSTOM_ICON_SIZE, UI_ACTIVE_ICON_SIZE);

    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    QRect r = par->geometry();
//    m_view->setSceneRect(r);
//    m_scene->setSceneRect(r);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    qDebug() << "policy height" << m_icon_size + 2*m_horzPadding;
    QSize s(1000,m_icon_size + 2*m_horzPadding);
    setMinimumSize(s);
    setContentsMargins(0,0,0,0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setEnabled(true);
    setVisible(true);
    m_view->raise();
    connect(this, SIGNAL(miniIconClicked(ListItem,bool)),
            (QObject*)gMainWidget, SLOT(miniIconClicked(ListItem,bool)));

    addBackground();
}



int TextMessageBar::getMessageSpaceAvailable(){
    int length = m_view->geometry().width();
    int buttonWidth=0;
    for(int i=0; i < m_textButtons.length(); i++){
        buttonWidth += m_textButtons[i]->fixedRect().width();
    }
    length -=buttonWidth;
    QFont fnt;
    if(m_textButtons.count()>0){
        fnt = m_textButtons[0]->m_textItem->document()->defaultFont();
    } else {
        ListItem li;
        fnt = li.getDisplayFont();
    }
    QFontMetrics fm(fnt);
    return length/fm.averageCharWidth();

}

void TextMessageBar::addBackground(){
    QString backgroundPath(UI_MINI_BAR_BACKGROUND);
    qDebug() << backgroundPath;
    QPixmap pm(backgroundPath);
    Q_ASSERT(!pm.isNull());
    Q_ASSERT(m_backgroundItem == 0);
    m_backgroundItem = new QGraphicsPixmapItem(pm);
    m_backgroundItem->setOffset(0,0);
    m_backgroundItem->setZValue(-1);
    m_backgroundItem->setPos(0,0);
    m_scene->addItem(m_backgroundItem);
}

void TextMessageBar::addNavText(QString navText){
    if(m_navTextItem){
        m_scene->removeItem(m_navTextItem->m_textItem);
        //m_navTextItem->m_textItem->deleteLater();
        m_navTextItem->m_textItem = 0;
        m_scene->removeItem(m_navTextItem);
        //m_navTextItem->deleteLater();
        m_navTextItem = 0;
    }

    if(!navText.isEmpty()){
        CatItem d("dummy://text", navText);
        ListItem navItem(d);
        navItem.setFilterRole(CatItem::MESSAGE_ELEMENT);
        m_navTextItem = new TextBarItem(this,navItem);
        m_navTextItem->setPos(QPointF(1,m_horzPadding));
        m_scene->addItem(m_navTextItem);
    }
}

void TextMessageBar::clear(){
    for(int i=0; i < m_textButtons.count(); i++){
        TextBarItem* it = m_textButtons[i];
        m_scene->removeItem(it->m_textItem);
        //it->m_textItem->deleteLater();
        it->m_textItem = 0;
        m_scene->removeItem(it);
        //it->deleteLater();
    }
    m_activeItem = ListItem();
    m_textButtons.clear();
    m_scene->update();
}
void TextMessageBar::addItems(QList<ListItem> ci){
    clear();
    int left=m_vertPadding;
    if(m_navTextItem!=0){
        QRectF leftRect = m_navTextItem->boundingRect();
        left = leftRect.right();
    }

    int count = MIN(ci.count(),itemPositions());
    QSet<QString> dupGuard;
    for(int i=0; i < count; i++){
        ListItem modItem = ci[i];
        if(dupGuard.contains(modItem.getName())){ continue;}
        dupGuard.insert(modItem.getName());
        left = addTextItem(modItem, left);
    }

    QRect r = this->geometry();
    r.moveTopLeft(QPoint(0,0));
    m_view->setSceneRect(r);
    m_scene->setSceneRect(r);
    m_scene->update();

}

void TextMessageBar::update(){
    m_scene->update();
    QWidget::update();
}


int TextMessageBar::addTextItem(ListItem li, int left){
    TextBarItem* textItem = new TextBarItem(this,li);
    if(li == m_activeItem){
        textItem->setActiveFilterIcon(true);;
    }
    QRectF r = textItem->boundingRect();

    textItem->setPos(QPointF(left,m_horzPadding));
    m_textButtons.append(textItem);
    m_scene->addItem(textItem);
    m_view->show();
    m_view->raise();
    textItem->update();
    return left + r.width();
}

void TextMessageBar::itemClicked(ListItem it, bool selected){
    for(int i=0; i < m_textButtons.count(); i++){
        TextBarItem* tb = m_textButtons[i];
        if((tb->getItemName()!= it.getName() )){
            tb->setSelected(false);
        }
    }
    if(selected){
        m_activeItem = it;
    } else {
        m_activeItem = ListItem();
    }
    emit miniIconClicked(it, selected);
    update();
}

void TextMessageBar::doUpdate(){
    if(m_navTextItem!=0){ m_navTextItem->updateText();}
    for(int i=0; i < m_textButtons.count(); i++){
        TextBarItem* tb = m_textButtons[i];
        if(tb){
            (tb)->updateText();
        }
    }
}


void TextMessageBar::addTextItem(QGraphicsItem* gItem){
    m_scene->addItem(gItem);
}

void TextMessageBar::setGeometry(QRect r){
//    if(m_backgroundItem){
//        m_backgroundItem->setPos(0,0);
//    }
    //Round-off to icon size

    int mod_h = r.height();
    qDebug() << "Icon Widget set height" << mod_h;
    r.setHeight(mod_h);
    int w = r.width();
    int leftSpace=m_vertPadding;
    if(m_navTextItem){
        m_navTextItem->setPos(QPointF(leftSpace,m_horzPadding));
        QRectF r = m_navTextItem->boundingRect();
        leftSpace = r.right();
    }

    w -= 2*m_horzPadding;
    Q_ASSERT((m_icon_size + leftSpace + m_vertPadding) >0);
    w /= m_icon_size + leftSpace + m_vertPadding;
    w *= m_icon_size + leftSpace + m_vertPadding;
    w += 2*m_horzPadding;
    //w = MAX(w,100);
    r.setWidth(MAX(r.width(),w));
    QWidget::setGeometry(r);
    m_view->setGeometry(r);
    r.moveLeft(0);
    r.moveTop(0);

    //
    int buttonWidth=0;
    for(int i=0; i < m_textButtons.length(); i++){
        buttonWidth += m_textButtons[i]->fixedRect().width();
    }


    if(m_textButtons.length()>0){
        qreal innerPadding = ((r.width() -(leftSpace + m_vertPadding)) - buttonWidth)/
                             (m_textButtons.length());
        qreal buttonSide = innerPadding/2 + leftSpace;

        for(int i=0; i < m_textButtons.length(); i++){
            m_textButtons[i]->setPos(
                    QPointF(buttonSide,
                            m_textButtons[i]->pos().y()));
            buttonSide+=m_textButtons[i]->fixedRect().width();
            buttonSide+=innerPadding;
        }
    }

//        m_scene->setSceneRect(r);
//        m_view->setSceneRect(r);
    QRectF rr(r);

    m_view->setSceneRect(rr);
    m_scene->setSceneRect(rr);
    m_scene->update();
    //m_scene->update();
    //m_view->show();
    //update();

}
