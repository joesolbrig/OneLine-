
#include "messageiconwidget.h"

#include <QList>
#include "item.h"
#include "appearance_window.h"
#include "textmessagewidget.h"
#include "list_item.h"


CloseToolItem::CloseToolItem(TextBarItem* parent): QGraphicsPixmapItem(parent){
    QIcon ic = QIcon::fromTheme(DEPRICATE_ICON_NAME);
    QPixmap pm = ic.pixmap(4,4);
    setPixmap(pm);
    connect(this, SIGNAL(closeToolClicked()),parent, SLOT(closeToolClicked()));

}

void CloseToolItem::mousePressEvent(QGraphicsSceneMouseEvent *){
    emit closeToolClicked();
}

TextBarItem::TextBarItem(TextMessageBar *parent, ListItem it) :
        QObject(parent), m_item(it)  {
    setAcceptsHoverEvents(true);

    connect(this, SIGNAL(itemClicked(ListItem, bool)),parent, SLOT(itemClicked(ListItem, bool)));
    connect(this, SIGNAL(closeToolClicked(ListItem)),parent, SLOT(closeToolClicked(ListItem)));
    connect(this, SIGNAL(miniIconRightClicked(QString, QPoint )),
            (QObject*)gMainWidget, SLOT(listMenuEvent(QString, QPoint )));
    prepareGeometryChange();
    m_animation = 0;
    m_animating = false;
    m_hovered = false;
    m_activated = m_item.hasLabel(ACTIVE_FILTER_ITEM_KEY);
    m_textItem=0;
    m_closerTool=0;

    m_textItem = new QGraphicsTextItem(this);


    m_font = m_item.getDisplayFont();
    m_font.setBold(true);
    if(m_item.getFilterRole() == CatItem::ACTIVE_CATEGORY){
        m_font.setPointSize(m_font.pointSize()+2);
        m_textItem->setDefaultTextColor(UI_ICONBAR_ITEM_COLOR);
    } else if(m_item.getFilterRole() == CatItem::CATEGORY_FILTER){
        m_font.setPointSize(m_font.pointSize()+1);
        if(it.getOrganizeingType() !=CatItem::MIN_TYPE){
            QColor typeColor = ListItem::colorFromType((CatItem::ItemType)
                                            it.getOrganizeingType());
            m_textItem->setDefaultTextColor(typeColor);
        } else {
            m_textItem->setDefaultTextColor(UI_ICONBAR_ITEM_COLOR);
        }
    } else if(m_item.getFilterRole() == CatItem::SUBCATEGORY_FILTER){
        m_font.setPointSize(m_font.pointSize()-2);
        m_textItem->setDefaultTextColor(UI_ICONBAR_ITEM_COLOR);
    } else {
        m_font.setPointSize(m_font.pointSize());
        m_textItem->setDefaultTextColor(UI_ICONBAR_MESSAGE_COLOR);
    }
    m_textItem->setFont(m_font);
    m_textItem->document()->setTextWidth((-1)); //never break

    m_textItem->document()->setDocumentMargin(0); //default is 4!
    m_textItem->document()->setIndentWidth(0);
    m_textItem->document()->setDefaultFont(m_font);
    m_textItem->document()->setTextWidth((-1)); //never break
    //qDebug() << "Text miniItem Width" << m_textItem->textWidth();
    m_textItem->setPos(QPointF(0,0));
    m_textItem->setZValue(zValue()+1);
    m_textItem->show();
    m_textItem->setAcceptHoverEvents(false);
    parent->addTextItem(m_textItem);
    updateText();
    m_aC = QColor(100,100,100);
    m_bC = QColor(225,225,220);
    //m_currentBackgroundColor = m_aC;
    setBigness(0);
    m_savedRectF = boundingRect();
    if(m_item.hasLabel(CLOSABLE_ORGANIZING_SOURCE_KEY) && m_item.getFilterRole() == CatItem::SUBCATEGORY_FILTER){
        m_closerTool = new CloseToolItem(this);
        m_closerTool->hide();
    }
}

void TextBarItem::paint( QPainter * painter,
             const QStyleOptionGraphicsItem * , QWidget * ){

    QRectF b = fixedRect();
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

    QFontMetrics fm(m_font);
    qreal w = boundingRect().width();
    qDebug() << "w: " << w;
    qreal acw = fm.averageCharWidth();
    qDebug() << "acw: " << acw;
    int charsAllowed = (w/acw);
    qDebug() << "charsAllowed: " << charsAllowed;
    bool altDn = gMainWidget->st_altKeyDown;
    QString text;
    if(altDn ){
        text = m_item.formattedName(false);
    } else {
        text = m_item.getName();
        text = text.left(charsAllowed);
    }
    text = (tagAs(QString(text),"center"));
//    if(m_item.getFilterRole() == CatItem::SUBCATEGORY_FILTER){
//        m_textItem->document()->setDefaultStyleSheet(UI_MINIBAR_DEFAULT_STYLE);
//    } else if(m_item.getFilterRole() == CatItem::CATEGORY_FILTER){
//        m_textItem->document()->setDefaultStyleSheet(UI_MINIBAR_DEFAULT_STYLE);
//    } else if(m_item.getItemType() == CatItem::LOCAL_DATA_FOLDER){
//        m_textItem->document()->setDefaultStyleSheet(UI_MINIBAR_DEFAULT_STYLE);
//    } else {
//        m_textItem->document()->setDefaultStyleSheet(UI_MINIBAR_DEFAULT_STYLE);
//    }
    m_textItem->setHtml(text);

    m_textItem->setZValue(zValue()+1);
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
    if(m_closerTool){
        QRectF closerRect = m_closerTool->boundingRect();
        closerRect.moveTopRight(
            QPointF(
                m_savedRectF.width() - (closerRect.width()/2),
                (closerRect.height()/2) - (m_savedRectF.height()/2)
            )
        );
        m_closerTool->setPos(closerRect.center());
        m_closerTool->setZValue(m_textItem->zValue()+1);
        m_closerTool->show();
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
    if(m_closerTool){
        m_closerTool->hide();
    }
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

void TextBarItem::mousePressEvent(QGraphicsSceneMouseEvent * evt) {
    Qt::MouseButtons bts = evt->buttons();
    Qt::MouseButton bt = evt->button();
    if(bts & Qt::RightButton || (bt ==Qt::RightButton)) {
        if(m_item.getFilterRole() == CatItem::MESSAGE_ELEMENT){ return;}
        setSelected(!m_activated);
        //QPoint mainWidgetPos = mapTo(gMainWidget,pos().toPoint());
        QPointF parPointF = mapToParent(pos());
        QPoint parPoint = parPointF.toPoint();
        QPoint globPoint = ((TextMessageBar*)parent())->mapToGlobal(parPoint);
        emit miniIconRightClicked(m_item.getPath(), globPoint);
    } else {
        if(m_item.getFilterRole() == CatItem::MESSAGE_ELEMENT){ return;}
        setSelected(!m_activated);
        if(m_item.getFilterRole() == CatItem::ACTIVE_CATEGORY){
            m_item.setFilterRole(CatItem::CATEGORY_FILTER);
        }
        emit itemClicked(m_item, m_activated);
    }
}

void TextBarItem::setSelected(bool sel){
    if(m_item.getFilterRole() == CatItem::MESSAGE_ELEMENT){ return;}
    m_activated = sel;
    if(m_activated || (m_item.getFilterRole() == CatItem::ACTIVE_CATEGORY)
        || (m_item.getFilterRole() == CatItem::CATEGORY_FILTER)){
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

    connect(this, SIGNAL(operateOnItem(QString, const CatItem )),
            gMainWidget, SLOT(operateOnItem(QString, const CatItem )));    addBackground();
}



int TextMessageBar::getMessageSpaceAvailable(){
    int length = m_view->geometry().width();
    int buttonWidth=0;
    for(int i=0; i < m_textButtons.length(); i++){
        buttonWidth += m_textButtons[i]->fixedRect().width() + (m_vertPadding/4);
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
    if(it.getFilterRole() == CatItem::SUBCATEGORY_FILTER){
        if(selected){
            m_subActiveItem = it;
        } else {
            m_subActiveItem = ListItem();
        }
    } else if(it.getFilterRole() == CatItem::CATEGORY_FILTER
              ||it.getFilterRole() == CatItem::ACTIVE_CATEGORY){
        m_subActiveItem = ListItem();
        if(selected){
            m_activeItem = it;
        } else {
            m_activeItem = ListItem();
        }
    } else if(it.getItemType() == CatItem::LOCAL_DATA_FOLDER){
        m_subActiveItem = ListItem();
        m_activeItem = ListItem();
    }

    for(int i=0; i < m_textButtons.count(); i++){
        TextBarItem* tb = m_textButtons[i];
        if((tb->getItemName()!= m_activeItem.getName()
            && tb->getItemName()!= m_subActiveItem.getName())){
            tb->setSelected(false);
        }
    }


    emit miniIconClicked(it, selected);
    update();
}

void TextMessageBar::closeToolClicked(ListItem it){

    if(m_subActiveItem == it){
        m_subActiveItem = ListItem();
    }

    int buttonIndex = -1;
    for(int i=0; i < m_textButtons.count(); i++){
        TextBarItem* tb = m_textButtons[i];
        if((tb->getItem().getPath()== it.getPath())){
            buttonIndex = i;
            m_scene->removeItem(tb->m_textItem);
            //it->m_textItem->deleteLater();
            tb->m_textItem = 0;
            m_scene->removeItem(tb);
        }
    }
    m_textButtons.removeAt(buttonIndex);

    CatItem setPrioritItem(addPrefix(OPERATION_PREFIX,SET_PRIORIT_OPERATION));
    setPrioritItem.setCustomValue(SET_PRIORITY_KEY_STR,0);

    emit miniIconClicked(it, false);
    emit operateOnItem(it.getPath(),setPrioritItem);
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

void TextMessageBar::setGeometry(QRect initialR){
    int mod_h = initialR.height();
//    qDebug() << "Icon Widget set height" << mod_h;
    initialR.setHeight(mod_h);
    int width = initialR.width();
    int leftSpace=m_vertPadding;
    if(m_navTextItem){
        m_navTextItem->setPos(QPointF(leftSpace,m_horzPadding));
        QRectF r = m_navTextItem->boundingRect();
        leftSpace = r.right();
    }
    width -= 2*m_horzPadding;
    Q_ASSERT((m_icon_size + leftSpace + m_vertPadding) >0);
    width /= m_icon_size + leftSpace + m_vertPadding;
    width *= m_icon_size + leftSpace + m_vertPadding;
    width += 2*m_horzPadding;
    initialR.setWidth(MAX(initialR.width(),width));
    QWidget::setGeometry(initialR);
    m_view->setGeometry(initialR);
    initialR.moveLeft(0);
    initialR.moveTop(0);
    int buttonWidth=0;
    for(int i=0; i < m_textButtons.length(); i++){
        buttonWidth += m_textButtons[i]->fixedRect().width();
    }
    if(m_textButtons.length()>0){
        qreal innerPadding = ((initialR.width() -(leftSpace + m_vertPadding)) - buttonWidth)/
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
    QRectF rr(initialR);
    m_view->setSceneRect(rr);
    m_scene->setSceneRect(rr);
    m_scene->update();
}

