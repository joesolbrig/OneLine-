#ifndef MESSAGEICONWINDOW_H
#define MESSAGEICONWINDOW_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
//#include <libwnck/libwnck.h>
//#include <gtk/gtk.h>
//#include <libgnomevfs/gnome-vfs.h>
//#include <libgnome/gnome-desktop-item.h>
//#include <gio/gio.h>

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
#include "main.h"

class MessageIconWidget;

//#include "GnomeSignalReceiver.h"
class IconAnimation;

//class RichTextGraphicItem : public QGraphicsItem {
//public:
//
//    RichTextGraphicItem(QString itemText, QString keyText, QFont font){
//        m_itemText = itemText;
//        m_keyText = keyText;
//        m_font = font;
//        m_isActive = false;
//    }
//
//    void setActive(bool a=true){
//        m_isActive = a;
//    }
//
//    void paint( QPainter * painter,
//                const QStyleOptionGraphicsItem *, QWidget * widget = 0 ){
//        widget = widget;//warning...
//        QRect r(QGraphicsItem::boundingRect().toRect());
//        if(m_isActive){
//            drawHtmlLine(painter, fnt, r, m_itemText);
//        } else if(gMainWidget->st_altKeyDown) {
//            drawHtmlLine(painter, fnt, r, m_keyText);
//        }
//    }
//
//private:
//    QString m_itemText;
//    QString m_keyText;
//    QFont m_font;
//    bool m_isActive;
//
//};


//Just to write icons in an icon bar
class MessageBarIcon : public QObject, public QGraphicsPixmapItem {
        Q_OBJECT
        //Q_PROPERTY(QPointF pos READ pos WRITE setPos)
        Q_PROPERTY(qreal bigness READ bigness WRITE setBigness)

private:

    ListItem m_item;

    bool m_hovered;
    bool m_animating;
    qreal m_magnification;
    IconAnimation* m_animation;
    bool m_activated;
    QSize m_pixmap_sz;

    QRectF m_savedRectF;

    QGraphicsTextItem* m_textItem;


public:

    MessageBarIcon(QObject *p, ListItem it);
    ~MessageBarIcon();

    void setActiveFilterIcon(bool s){m_activated = s;}

    ListItem getItem(){return m_item;}

    QRectF boundingRect();
    void setPos(QPointF p){
        QGraphicsPixmapItem::setPos(p);
        QRectF r = boundingRect();
        m_textItem->setPos(p.x() - r.width(), p.y() + 3);
    }

    qreal bigness() {return m_bigness;}
    void setBigness(qreal p){
        m_magnification = p;
        this->setScale(1+m_magnification/2);

    }
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
    void animationDone(){
        m_animating = false;
        setBigness(0);
        //update();
    }

signals:
    void painted();
    void itemClicked(ListItem li, bool selected);

};


class IconAnimation;

class IconAnimation : public QPropertyAnimation {

    QPainterPath m_path;
    MessageBarIcon* m_target;
    int m_deflection;

public:
    IconAnimation(QObject *target)
        : QPropertyAnimation(target, "bigness") {
        m_target = (MessageBarIcon*)target;
        m_path = QPainterPath();
        m_deflection = m_target->boundingRect().height()* HEIGHT_DEFLECTION_RATIO;
        connect(this, SIGNAL(finished()), m_target,SLOT(animationDone()));
    }

    //Assure circular movement...
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
        
        setStartValue(m_target->pos());
        setEndValue(m_target->pos());
        setDuration(UI_LIST_RESIZE_TIME*2);
        setLoopCount(1);
        start();
    }


};


class MessageIconWidget : public QWidget
{
    Q_OBJECT

    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    ListItem m_item;
    int m_innerPadding;
    int m_outterPadding;
    QSize m_childSize;
    int m_iconCount;

    int m_icon_size;

    ListItem m_activeItem;

    FancyHorizontalLabel* m_ImageLabel;
    
    

public:


    void addItem(ListItem li, int i);
    void addItem(QGraphicsItem* gItem);
    void addTextItem(){

    }

    void update(){
        m_scene->update();
        QWidget::update();

    }


    QPointF itemPos(int i){
        float x = m_outterPadding
            + (m_icon_size + 2*m_innerPadding)*(i);
        float y = m_outterPadding;
        return QPointF(x,y);
    }

    MessageIconWidget(QWidget* par, ListItem psuedoParentItem=ListItem());

    void setBackgroundLabel(){
//        setAttribute(Qt::WA_TranslucentBackground);
//        m_view->setStyleSheet("background: transparent");
//        if(!m_ImageLabel){
//            m_ImageLabel = new FancyHorizontalLabel();
//
//            QImage image(UI_TASKBAR_LABEL);
//            m_ImageLabel->setupImage(image,
//                UI_DEFAULT_TASKBAR_CUT, UI_DEFAULT_SECTION_WIDTH);
//            QRect r = this->geometry();
//            r.setHeight(image.height());
//            r.setWidth(m_ImageLabel->baseWidth());
//            m_ImageLabel->setGeometry(r);
//            this->setGeometry(r);
//        }
//        m_ImageLabel->show();
//        m_ImageLabel->lower();
    }

    int getInnerPadding(){
        return m_innerPadding;
    }


    void addItems(QList<ListItem> ci){

        QList<QGraphicsItem*> its = m_scene->items();
        for(int i=0; i < its.count(); i++){
            m_scene->clear();;
            //delete gitm;
        }

        for(int i=0; i < MIN(ci.count(),itemPositions()); i++){
            ListItem modItem = ci[i];
            addItem(modItem, i);
        }
        QRect r = this->geometry();
        //m_view->setViewport(this);
        r.moveTopLeft(QPoint(0,0));
        m_view->setSceneRect(r);
        m_scene->setSceneRect(r);
        m_scene->update();

    }
    int desiredHeight(){
        return m_icon_size + getTextHeight() + 2*getInnerPadding() + 2*m_outterPadding +30;
    }


    void setGeometry(QRect r);

    void setGeometry(int , int , int , int ){
        Q_ASSERT(false);
    }

    ~MessageIconWidget(){
        m_scene->deleteLater();
        m_view->deleteLater();
    }

    int getTextHeight(){
        QFontMetrics fm(m_item.getDisplayFont());
        return fm.height();
    }

    int getTextCharacters(){
        QFontMetrics fm(m_item.getDisplayFont());
        return fm.width("/mmmm/")/6;
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
        QRect r;
        int w;
        if(!m_ImageLabel){
            r = gAppArea();
            w = r.width()*UI_TASKBAR_SCREEN_RATIO;

        } else {
            r = this->geometry();
            w = r.width();
        }
        w -= 2*m_outterPadding;
        w /= (m_icon_size + 2*m_innerPadding);
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






#endif // MESSAGEICONWINDOW_H
