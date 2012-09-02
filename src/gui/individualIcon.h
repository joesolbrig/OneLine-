#ifndef INDIVIDUALICON_H
#define INDIVIDUALICON_H
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
//#include <libwnck/libwnck.h>
//#include <gtk/gtk.h>
//#include <libgnomevfs/gnome-vfs.h>
//#include <libgnome/gnome-desktop-item.h>
#include <gio/gio.h>

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

#include "item.h"
#include "list_item.h"
#include "input_data.h"

#include "information_coalator.h"

#include "messageiconwidget.h"



class IndividIconAnimation;

class MessageBarIcon;

class IndividualIcon : public QObject, public QGraphicsItem {
        Q_OBJECT

//        Q_PROPERTY(QPointF spreadPos READ getSpreadPos WRITE setSpreadPos) //
        Q_INTERFACES(QGraphicsItem)
private:

    ListItem m_item;

    bool m_hover;
    bool m_animating;

    bool m_followingMouse;
    bool m_captured;

    float m_delay_factor;
    float m_magnification;
    MessageBarIcon* m_parent;

    QPointF m_desiredLocation;
    IndividIconAnimation* m_animationRep;


    QRectF m_boundingRect;
    
//    QPointF getSpreadPos(){
//        return m_boundingRect.center();
//
//    }




public:

    IndividualIcon(MessageBarIcon *p, ListItem it);

    QRectF boundingRect(){
        return m_boundingRect;
    }
    void setDesiredLocation(QPointF p){
        m_desiredLocation = p;
    }

    QPointF getDesiredPosition(){
        return m_desiredLocation;
    }

    ListItem getItem(){
        return m_item;
    }


    void setBoundingRect(QRectF r){
        m_boundingRect = r;
    }

    QRectF boundingRect() const {
        return m_boundingRect;
    }


protected:

    void hoverEnterEvent(QGraphicsSceneHoverEvent *){
        m_hover = true;
        m_delay_factor = DEFAULT_DELAY_FACTOR;
    }

    void hoverMoveEvent(QGraphicsSceneHoverEvent *){

    }


    void hoverLeaveEvent(QGraphicsSceneHoverEvent *){
        m_hover = false;

    }

    void mousePressEvent(QGraphicsSceneMouseEvent *){
        emit itemSelected(m_item);
    }

    void paint( QPainter * painter,
                 const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    QPainterPath shape(){
        QPainterPath path;
        path.addEllipse(boundingRect());
        return path;

    }

    bool contains ( const QPointF & point ){
        return shape().contains(point);
    }


    //Between 0 and 1
//    void setSpreadPos(QPointF pt);


public slots:
    void animationDone(){
        m_animating = false;
        update();

    }

signals:
    void painted();
    void itemSelected(ListItem li);


};


#endif // INDIVIDUALICON_H
