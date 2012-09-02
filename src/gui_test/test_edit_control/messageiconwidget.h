#ifndef MESSAGEICONWINDOW_H
#define MESSAGEICONWINDOW_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include <gtk/gtk.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnome/gnome-desktop-item.h>
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

class MessageIconWidget;

#include "GnomeSignalReceiver.h"
class IconAnimation;


//Just to write icons in an icon bar
class MessageBarIcon : public QObject, public QGraphicsPixmapItem {
        Q_OBJECT

        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
private:

    ListItem m_item;

    bool m_hover;
    bool m_animating;
    int m_magnification;
    IconAnimation* m_animation;



public:

    MessageBarIcon(QObject *p, ListItem it);

    QRectF boundingRect();

protected:

    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

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

public slots:
    void animationDone(){
        m_animating = false;
        update();
    }

signals:
    void painted();
    void itemSelected(ListItem li);


};


class GnomeSignalReceiver : public QObject {
        Q_OBJECT
        QApplication* theApp;
        WnckScreen* mp_workingScreen;

        QList<CatItem> activeWindows;
        MessageIconWidget* targetWindow;

    public:

        GnomeSignalReceiver() : mp_workingScreen(0){}

        void initializeList(){
            mp_workingScreen = wnck_screen_get_default();
            wnck_screen_force_update(mp_workingScreen);
            scanActiveWindows();
        }

        void setTargetWindow(MessageIconWidget* t);

        void setScreen(WnckScreen* s){ mp_workingScreen = s; }

        void scanActiveWindows(){
            QHash<QString, CatItem>  testHash;
            QList<CatItem>  res;

            if(!mp_workingScreen){
                mp_workingScreen = wnck_screen_get_default();
                Q_ASSERT(mp_workingScreen);
            }

            CatItem baseItem("gnome_task://tester");
            GList* gl = wnck_screen_get_windows(mp_workingScreen);
            for(int j=0;j<30 && gl;j++){
                Q_ASSERT(gl->data);
                WnckWindow* window = (WnckWindow*)gl->data;
                //wnck_screen_get_active_window(screen);
                if(!wnck_window_is_skip_tasklist(window) &&
                   !wnck_window_is_skip_pager(window)){
                    CatItem it =  WindowToItem(window);
                    it.setExternalWeight(1,baseItem);
                    if(!testHash.contains(it.getPath())){
                        res.append(it);
                        testHash[it.getPath()] = it;
                    }
                }
                gl = gl->next;
            }
            emit itemsFound(res);
        }

        CatItem WindowToItem(WnckWindow* w){
            //QString name = wnck_application_get_name(a);

            QString pseudo_path = QString("wnck_id%1").arg(wnck_window_get_xid (w));
            QString itemName = addPrefix(TASK_PREFIX , pseudo_path);
            CatItem it( itemName);
            it.setDescription(wnck_window_get_name(w));
            it.setUseDescription(true);
            it.setName(it.getDescription());
            if(wnck_window_needs_attention(w)){
                it.setIsTimeDependant();
            }

            WnckApplication* a = wnck_window_get_application(w);
            const char* nameChars=0;
            if(a){
                //pAppIt = this->getApplicationParent(a);
                nameChars = wnck_application_get_name(a);
                QString name(nameChars);
                CatItem pAppIt(name);

                it.addParent(pAppIt);
            }

            if(wnck_window_has_icon_name(w)){
                it.setIcon(wnck_window_get_icon_name(w));
            }

            QIcon ic;
            GdkPixbuf * pb;
            pb = wnck_window_get_icon(w);
            if(pb){
//                gdk_pixbuf_get_bits_per_sample(pb);
//                bool hasAlpha = gdk_pixbuf_get_has_alpha(pb);
//                int nChan = gdk_pixbuf_get_n_channels (pb);
                QImage im(gdk_pixbuf_get_pixels(pb),
                                gdk_pixbuf_get_width(pb),
                                gdk_pixbuf_get_height(pb),
                                gdk_pixbuf_get_rowstride(pb),
                                QImage::Format_ARGB32);
                                //No documents make clear that ARGB is it but it works
                                //all the hazy docs I can find point that way...

                QRect ir = im.rect();
                QPixmap pm(QPixmap::fromImage(im));
                QRect ir2 = pm.rect();
                //ic.addPixmap(pm);

                //Exploratory...
//                QImage im2 = im.copy(QRect(0,0,36,36));
                QImage im2 = im;
                QPixmap pm2 = QPixmap::fromImage(im2);

                pm2 = pm2.scaledToHeight(UI_DEFAULT_MINI_ICON_SIZE,
                                                Qt::SmoothTransformation);

                ic.addPixmap(pm2);
            }
            pb = wnck_window_get_mini_icon(w);
            if(pb){
//                int bps = gdk_pixbuf_get_bits_per_sample(pb);
//                bool hasAlpha = gdk_pixbuf_get_has_alpha(pb);
//                int nChan = gdk_pixbuf_get_n_channels (pb);


                QImage image2(gdk_pixbuf_get_pixels(pb),
                                gdk_pixbuf_get_width(pb),
                                gdk_pixbuf_get_height(pb),
                                gdk_pixbuf_get_rowstride(pb),
                                QImage::Format_ARGB32);
                QPixmap pixmap2(QPixmap::fromImage(image2));
                ic.addPixmap(pixmap2);
            }
            QVariant icVar(ic);

            it.setCustomPluginVariant(REAL_ICON_KEY, icVar);

            //Add parents for Application and gnome group that thing is in...
            //Then task are under applications...
            return it;
        }

        CatItem getApplicationParent(WnckApplication* a){
            const char* nameChars = wnck_application_get_name(a);
            QString name(nameChars);
            int pid = wnck_application_get_pid(a);

            QString pseudo_path = QString("wnck_app_id%1").arg(pid);
            //QChar* tt = (pseudo_path.data());
            QString fp = addPrefix(TASK_PREFIX , pseudo_path);
            CatItem it( fp, name);
            QString t = it.getPath();
            it.getDescription() = name;
            it.setUseDescription(true);

            const char* icon_name =  wnck_application_get_icon_name(a);
            if(icon_name){
                it.setIcon(QString(icon_name));
            }

            QIcon ic;
            GdkPixbuf * pb;
            pb = wnck_application_get_icon(a);
            if(pb){
//                int bps = gdk_pixbuf_get_bits_per_sample(pb);
//                bool hasAlpha = gdk_pixbuf_get_has_alpha(pb);
//                int nChan = gdk_pixbuf_get_n_channels (pb);
                QImage im(gdk_pixbuf_get_pixels(pb),
                                gdk_pixbuf_get_width(pb),
                                gdk_pixbuf_get_height(pb),
                                gdk_pixbuf_get_rowstride(pb),
                                QImage::Format_ARGB32);
                                //No documents make clear that ARGB is it but it works and hazy
                                //I can find point that way...

                QRect ir = im.rect();
                QPixmap pm(QPixmap::fromImage(im));
                QRect ir2 = pm.rect();
                //ic.addPixmap(pm);

                QImage im2 = im;
                QPixmap pm2 = QPixmap::fromImage(im2);

                pm2 = pm2.scaledToHeight(UI_DEFAULT_MINI_ICON_SIZE,
                                                Qt::SmoothTransformation);

                ic.addPixmap(pm2);

                QVariant icVar(ic);
                it.setCustomPluginVariant(REAL_ICON_KEY, icVar);
            }
            return it;
        }



    signals:
        void itemsFound(QList<CatItem> ci);

};

class IconAnimation;

class IconAnimation : public QPropertyAnimation {

    QPainterPath m_path;
    MessageBarIcon* m_target;
    int m_deflection;

public:
    IconAnimation(QObject *target)
        : QPropertyAnimation(target, "pos") {
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

        qreal easedProgress = easingCurve().valueForProgress(progress);
        QPointF pt = m_path.pointAtPercent(easedProgress);
        updateCurrentValue(pt);
        //emit valueChanged(pt);
    }

    void startAnimation() {
        
        setStartValue(m_target->pos());
        setEndValue(m_target->pos());
        setDuration(STANDARD_MOMENT/SIGNAL_MOMENT_RATIO);
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
    int m_icon_size;
    int m_innerPadding;
    int m_outterPadding;
    QSize m_childSize;
    int icon_count;

protected:

    QPointF itemPos(int i){
        float x = m_outterPadding
            + (m_icon_size + 2*m_innerPadding)*(i);
        float y = m_outterPadding;
        return QPointF(x,y);
    }

public:
    void addItem(ListItem li, int i){
        MessageBarIcon* ii = new MessageBarIcon(this,li);
        ii->setPos(itemPos(i));
        m_scene->addItem(ii);
        m_view->show();
    }

    int getInnerPadding(){
        return m_innerPadding;
    }

    MessageIconWidget(QWidget* par): QWidget(par) {
        m_innerPadding = m_item.getCustomValue(CUSTOM_MSG_PADDING, UI_DEFAULT_MINI_ICON_PADDING);
        m_outterPadding = m_item.getCustomValue(CUSTOM_MSG_PADDING, UI_DEFAULT_MINI_ICONBAR_PADDING);
        m_icon_size = m_item.getCustomValue(CUSTOM_ICON_SIZE, UI_DEFAULT_MINI_ICON_SIZE);

        m_scene = new QGraphicsScene(this);
        m_view = new QGraphicsView(m_scene, this);
        setEnabled(true);
        setVisible(true);
        Q_ASSERT(par);

    }

    void setGeometry(QRect r){
        //Round-off to icon size

        int mod_h = m_icon_size + getTextHeight() + 2*getInnerPadding() + 2*m_outterPadding;

        r.setHeight(mod_h);
        int w = r.width();

        w -= 2*m_outterPadding;
        w /= m_icon_size + 2*m_innerPadding;
        w *= m_icon_size + 2*m_innerPadding;
        w += 2*m_outterPadding;
        r.setWidth(w);
        QWidget::setGeometry(r);
        r.setLeft(0);
        r.setTop(0);
        m_scene->setSceneRect(r);
        m_view->setSceneRect(r);

    }

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
        w -= 2*m_outterPadding;
        w /= (m_icon_size + 2*m_innerPadding);
        return w;
    }

    void addItems(QList<CatItem> ci){
//        InformationCoalator iCol(ci, itemPositions());
//        ci = iCol.coalate();


        //ci = InputData::coalateFoundItems(ci, itemPositions());

        QList<QGraphicsItem*> its = m_scene->items();
        qDebug() << " ---- clearing ----- ";
        for(int i=0; i < its.count(); i++){
            m_scene->removeItem(its[i]);
        }

        for(int i=0; i < MIN(ci.count(),6); i++){
            ListItem li(ci[i]);
            qDebug() << "adding: " << li.getPath();
            qDebug() << "(formatted) name: " << li.formattedName();
            addItem(li, i);
        }
    }

public slots:

    void itemClicked(ListItem it){

        QMessageBox msgBox;
        QString msg= it.getPath() + "executed";
        msgBox.setText(msg);
        msgBox.exec();

    }
    void itemsFound(QList<CatItem> ci){
        addItems(ci);

    }
};






#endif // MESSAGEICONWINDOW_H
