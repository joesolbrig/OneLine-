#ifndef TEXTMESSAGEWIDGET_H
#define TEXTMESSAGEWIDGET_H

#include <QWidget>
#include <QBoxLayout>
#include <QApplication>
#include <QPainter>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QTextStream>
#include <QDataStream>
#include <QPropertyAnimation>
#include <QDesktopWidget>

#include "item.h"
#include "list_item.h"
#include "input_data.h"

#include "information_coalator.h"
#include "fancyHorizontallabel.h"
#include "globals.h"


//#include "GnomeSignalReceiver.h"
class TextItemAnimation;
class TextMessageBar;

//Just to remove some stupid warnings...
class MyQGraphicsItemBridge : public QGraphicsItem {

public:
    MyQGraphicsItemBridge(): QGraphicsItem(){}
private:
    Q_DISABLE_COPY(MyQGraphicsItemBridge)


};

class TextBarItem : public QObject, public MyQGraphicsItemBridge {
        Q_OBJECT
        Q_PROPERTY(qreal bigness READ bigness WRITE setBigness)
private:

    ListItem m_item;

    bool m_hovered;
    bool m_animating;
    QPropertyAnimation* m_animation;
    bool m_activated;
    QSize m_pixmap_sz;

    QRectF m_savedRectF;


    QColor m_aC;
    QColor m_bC;
    QColor m_currentBackgroundColor;

    QFont m_font;


public:
    QGraphicsTextItem* m_textItem;
    QString getItemName(){ return m_item.getName();}
    void setSelected(bool sel);

    TextBarItem(TextMessageBar* p=0, ListItem it=ListItem());
    ~TextBarItem();

    QRectF fixedRect() {
        QRectF b = m_savedRectF;
        b.setLeft(b.left() -UI_DEFAULT_TEXT_ITEM_H_PADDING);
        b.setRight(b.right() +UI_DEFAULT_TEXT_ITEM_H_PADDING);
        b.setTop(b.top() -UI_DEFAULT_TEXT_ITEM_V_PADDING);
        b.setBottom(b.right() +UI_DEFAULT_TEXT_ITEM_V_PADDING);
        return b;

    }

    void setActiveFilterIcon(bool s){m_activated = s;}
    void setText(QString text);

    ListItem getItem(){return m_item;}
    void updateText();
    virtual QRectF boundingRect() const;
    virtual QRectF baseBoundingRect() const;


    void setPos(QPointF p){
        QGraphicsItem::setPos(p);
        m_textItem->setPos(p);
    }

    qreal bigness() {return m_bigness;}
    void setBigness(qreal p);
    qreal m_bigness;


protected:

    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
    void mousePressEvent(QGraphicsSceneMouseEvent *);

    void paint( QPainter * painter,
                 const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    QPainterPath shape(){
        QPainterPath path;
        QRectF r = boundingRect();
        path.addEllipse(r);
        return path;
    }

    bool contains ( const QPointF & point ){
        return shape().contains(point);
    }

public slots:
    void animationDone();


signals:
    void painted();
    void itemClicked(ListItem li, bool selected);
    void miniIconRightClicked(ListItem it, QPoint p);

};

class TextItemAnimation : public QPropertyAnimation {

    QPainterPath m_path;
    TextBarItem* m_target;
    int m_deflection;

public:
    TextItemAnimation(QObject *target)
        : QPropertyAnimation(target, "bigness") {
        m_target = (TextBarItem*)target;
        m_path = QPainterPath();
        m_deflection = 0;
        connect(this, SIGNAL(finished()), m_target,SLOT(animationDone()));
    }

    void updateCurrentTime(int currentTime) {
        if (m_path.isEmpty()) {
            QPointF to = endValue().toPointF();
            QPointF from = startValue().toPointF();
            m_path.moveTo(from);
            m_path.lineTo(QPointF(from.x(), from.y() - m_deflection));
            m_path.lineTo(to);
        }
        int dura = duration();
        const qreal progress = ((dura == 0) ? 1 : ((((currentTime - 1) % dura) + 1) / qreal(dura)));

        //qreal easedProgress = easingCurve().valueForProgress(progress);
        //updateCurrentValue(easedProgress);
        //QPointF pt = m_path.pointAtPercent(easedProgress);
        updateCurrentValue(progress/2);
    }

    void startAnimation() {
        Q_ASSERT(m_target);
        m_deflection = m_target->boundingRect().height()* HEIGHT_DEFLECTION_RATIO;
        setStartValue(m_target->pos());
        setEndValue(m_target->pos());
        setDuration(UI_LIST_RESIZE_TIME*2);
        setLoopCount(1);
        start();
    }


};


class TextMessageBar : public QWidget
{
    Q_OBJECT

    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    TextBarItem* m_navTextItem;
    QList<TextBarItem*> m_textButtons;

    ListItem m_item;
    int m_vertPadding;
    int m_horzPadding;
    QSize m_childSize;
    int m_iconCount;

    int m_icon_size;

    ListItem m_activeItem;

    QGraphicsPixmapItem*  m_backgroundItem;

public:
    int addTextItem(ListItem li, int left);
    void addTextItem(QGraphicsItem* gItem);

    void update();
    
    void doUpdate();

    TextMessageBar(QWidget* par, ListItem psuedoParentItem=ListItem());

    int getInnerPadding(){
        return m_vertPadding;
    }

    void addNavText(QString navText);
    int getMessageSpaceAvailable();
    void addBackground();
    void addItems(QList<ListItem> ci);
    void clear();

    int getItemCount(){
        return m_scene->items().count();
    }

    int getTextButtonCount(){
        return m_textButtons.count();
    }

    int desiredHeight(){
        return m_icon_size + getTextHeight() + 2*getInnerPadding() + 2*m_horzPadding +30;
    }


    void setGeometry(QRect r);

    void setGeometry(int x, int y, int h, int w){
        QRect r(x, y, h,  w);
        setGeometry(r);
    }

    ~TextMessageBar(){
        m_scene->deleteLater();
        m_view->deleteLater();
    }

    int getTextHeight(){
        QFontMetrics fm(m_item.getDisplayFont());
        return fm.height();
    }

    int getTextCharacters(){
        QFontMetrics fm(m_item.getDisplayFont());
        return fm.averageCharWidth();
    }

    QSize getIconSize(){
        return QSize(m_icon_size,m_icon_size);
    }


    QSize getChildSize(){
        return QSize(m_icon_size + 2*getInnerPadding(),
                     m_icon_size + getTextHeight() + 2*getInnerPadding());
    }

    int getIconCount(){
        return m_scene->items().count();
    }

    QList<QGraphicsItem*>  getIcons(){
        return m_scene->items();
    }

    int itemPositions(){
        QRect r = this->geometry();
        int w = r.width();
        w -= 2*m_horzPadding;
        qreal deNom = (m_icon_size + 2*m_vertPadding);
        if(deNom==0){ return 0;}
        w = w/deNom;
        return w;
    }

signals:
    //void itemClick();
    void miniIconClicked(ListItem it, bool selected);

public slots:

    void itemClicked(ListItem it, bool selected);

    void itemsFound(QList<ListItem> ci){
        addItems(ci);
    }
};

#endif // TEXTMESSAGEWIDGET_H
