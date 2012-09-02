#ifndef GNOMESIGNALRECEIVER_H
#define GNOMESIGNALRECEIVER_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include <gtk/gtk.h>

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


        void setScreen(WnckScreen* s){ mp_workingScreen = s; }

        void scanActiveWindows(){
            QHash<QString, CatItem>  testHash;
            QList<CatItem>  res;

            if(!mp_workingScreen){
                mp_workingScreen = wnck_screen_get_default();
                Q_ASSERT(mp_workingScreen);
            }

            CatItem baseItem("gnome_task://all_task");
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
            if(a){
                CatItem pAppIt = this->getApplicationParent(a);
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
                                //No documents maked clear that ARGB is appropriate
                                // but it works and all the hazy docs I can
                                //find point that way...

                QRect ir = im.rect();
                QPixmap pm(QPixmap::fromImage(im));
                QRect ir2 = pm.rect();
                //ic.addPixmap(pm);

                //Exploratory...
//                QImage im2 = im.copy(QRect(0,0,36,36));
                QImage im2 = im;
                QPixmap pm2 = QPixmap::fromImage(im2);

                pm2 = pm2.scaledToHeight(UI_MAIN_ICON_SIZE,
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

                pm2 = pm2.scaledToHeight(UI_MAIN_ICON_SIZE,
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


#endif // GNOMESIGNALRECEIVER_H
