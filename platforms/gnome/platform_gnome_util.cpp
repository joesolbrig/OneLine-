
#include <libgnomeui/libgnomeui.h>
#include "platform_gnome_util.h"
#include <gtk/gtk.h>


#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QPainter>

#include <QX11Info>
#include "constants.h"

void loadIconSize(QIcon* ic, gchar* fileType,
                  GtkIconTheme * curIconTheme, int size){

    GError* err_ptr = NULL;

    GdkPixbuf* buff = gtk_icon_theme_load_icon(curIconTheme,
                                               fileType,
                                               size,
                                               GTK_ICON_LOOKUP_NO_SVG,
                                               &err_ptr);
    if( err_ptr){
        qDebug() << " error loading theme" << err_ptr->message;
        return; }

    buff = gdk_pixbuf_scale_simple(buff,size, size,GDK_INTERP_BILINEAR);
    if( !buff){
        qDebug() << " error scaling buffer" ;
        return; }

    gchar* pixmap=0;
    gsize buflen;
    gdk_pixbuf_save_to_buffer (buff,
                               &pixmap,
                               &buflen,
                               "png",
                               &err_ptr, NULL);

    if(err_ptr){
        qDebug() << " error setting buffer" << err_ptr->message;
        if(pixmap){
            free(pixmap);
        }
        return;
    }


    QPixmap qp;
    bool success = qp.loadFromData((const uchar*) pixmap, buflen, "png");
    if(!success){
        qDebug() << " some error with qPixmap" ;
        if(pixmap){
            free(pixmap);
        }
        return;
    }

    ic->addPixmap(qp);

    free(pixmap);
    g_object_unref(buff);

}

QIcon GnomeIconProvider::getGnomeicon(const QFileInfo& info) const
{
    QPixmap pm;
    if (info.fileName().endsWith(".png", Qt::CaseInsensitive))
        { pm = QPixmap(info.absoluteFilePath());}
    if (info.fileName().endsWith(".ico", Qt::CaseInsensitive))
        pm = QPixmap(info.absoluteFilePath());
    if (info.fileName().endsWith(".jpg", Qt::CaseInsensitive))
        pm = QPixmap(info.absoluteFilePath());
    if (info.fileName().endsWith(".tif", Qt::CaseInsensitive))
        pm = QPixmap(info.absoluteFilePath());
    if (info.fileName().endsWith(".gif", Qt::CaseInsensitive))
        pm = QPixmap(info.absoluteFilePath());

    if(!pm.isNull()){
        QIcon ic(pm);
        ic.addPixmap(pm.scaledToHeight(16));
        ic.addPixmap(pm.scaledToHeight(22));
        ic.addPixmap(pm.scaledToHeight(24));
        ic.addPixmap(pm.scaledToHeight(32));
        ic.addPixmap(pm.scaledToHeight(48));
        return ic;
    }

    gdk_threads_enter();
    GnomeIconLookupResultFlags resultFlags;

    GtkIconTheme * curIconTheme = gtk_icon_theme_get_default();

    char* fileName = info.absoluteFilePath().toLocal8Bit().data();

    if(!fileName){
        qDebug() << " error: null file name" ;
        gdk_threads_leave();
        return QIcon();
    }

    if(strlen(fileName) ==0){
        qDebug() << " error: zero-length file" ;
        gdk_threads_leave();
        return QIcon();
    }
    //END GTK so leave???
    gdk_threads_leave();


    //Call the function that accesses files, we do
    //our own caching so don't bother w/ cached version
    GError* err_ptr = NULL;

    //needs to be re-worked as something like http://svn.gna.org/svn/graveman/trunk/src/thumbnails.c
    gchar* fileType = gnome_icon_lookup_sync(curIconTheme,
                                        NULL,
                                        fileName,
                                        NULL,
                                        GNOME_ICON_LOOKUP_FLAGS_NONE,
                                        &resultFlags);
    if(err_ptr){
        qDebug() << " error loading theme" << err_ptr->message;
        //gdk_threads_leave();
        return QIcon();
    }

    Q_ASSERT(fileType);
    if(QString(fileType) == "gnome-fs-regular" || QString(fileType) == "gnome-fs-directory"){
        qDebug() << "got some bullshit type, returning null ";
        //gdk_threads_leave();
        return QIcon();
    }

    qDebug() << "got file type: " << fileType;

    QIcon ic;

    //Annoyingly, we need multiple pixmaps...
    //try gdk_pixbuf_scale_simple later
    for(int i=8; i < 70; i+=8){
        loadIconSize(&ic, fileType,curIconTheme, i);
    }
    //gdk_threads_leave();
    return ic;
}


QIcon GnomeIconProvider::getGnomeiconFromMime(QString mimeType) const
{
    //qDebug() << "at mime type: " << mimeType;

    char* fileType = mimeType.toLocal8Bit().data();

    gdk_threads_enter();
    GtkIconInfo* icinfo = gtk_icon_theme_lookup_icon(gtk_icon_theme_get_default(),
                                                     fileType,
                                                     UI_STANDARD_ICON_WIDTH,
                                                     GTK_ICON_LOOKUP_NO_SVG);
    if(!icinfo){
        //qDebug() << " icinfo null" ;
        gdk_threads_leave();
        return QIcon();
    }

    GdkPixbuf* buff = gtk_icon_info_load_icon(icinfo, NULL);
    if(!buff){
        qDebug() << " buff null, what's up with that?" ;
        gdk_threads_leave();
        return QIcon();
    }

    gchar* pixmap;
    gsize buflen;

    GError err;
    GError* err_ptr = &err;
    bool success = gdk_pixbuf_save_to_buffer(buff,
                                              &pixmap,
                                              &buflen,
                                              "png",
                                              &err_ptr, NULL);
    if(!success){
        if(abs((int)err_ptr->message) < 100){
            qDebug() << " error setting buffer and error, the fuckers" ;
        } else {
            qDebug() << " error setting buffer" << err_ptr->message;
        }
        gdk_threads_leave();
        return QIcon();
    }


    QPixmap qp;
    success = qp.loadFromData((const uchar*) pixmap, buflen, "png");
    if(!success){
        qDebug() << " some error with qPixmap" ;
        return QIcon();
    }

    QIcon qico(qp);
    free(pixmap);
    g_object_unref(buff);
    gdk_threads_leave();

    return qp;
}



