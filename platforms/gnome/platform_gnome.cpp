/*
Oneline: Multibrowser
Copyright (C) 2012  Hans Joseph Solbrig

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <gtk/gtk.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnome/gnome-desktop-item.h>
#include <QtGui>
#include <QApplication>
#include <QX11Info>
#include "platform_gnome.h"
#include "platform_gnome_util.h"
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>

CatItem createActionChild(GAppInfo *app_info){

    const char *command_line_path = g_app_info_get_executable(app_info);
    if(!command_line_path ){
        qDebug() << "failed to get command line for extra app" ;
        return CatItem();
    }
    QString cmd = QString((command_line_path));
    if(cmd.isEmpty()){
        qDebug() << "empty command line for extra app ";
        return CatItem();
    }


    cmd = cmd.simplified();

    CatItem action(addPrefix(SHELL_VERB_PREFIX,command_line_path));
    action.setDescription(QString(g_app_info_get_description(app_info)));
    action.setUseDescription(true);
    action.setName(QString(g_app_info_get_name(app_info)));
    action.setItemType(CatItem::VERB);
    action.setCustomCommandLine(cmd);

    bool doesntTakeArgs = !g_app_info_supports_files(app_info)
                            && !g_app_info_supports_uris(app_info);
    if(doesntTakeArgs){
        action.setCustomPluginValue(VERB_TAKES_NO_ARGS_KEY,(int)doesntTakeArgs);
    }
    return action;

    //gtk_icon_theme_lookup_by_gicon

}

void addPixBuffSize(QIcon* ic, GAppInfo *app_info,
                  GtkIconTheme * curIconTheme, int size);

QIcon addPixBuff(CatItem* it, GAppInfo *app_info){
    QIcon ic;
    GtkIconTheme * curIconTheme = gtk_icon_theme_get_default();
    for(int i=16; i <= 52; i+=8){
        addPixBuffSize(&ic, app_info, curIconTheme, i);
    }
    QVariant icVar(ic);
    it->setCustomPluginVariant(REAL_ICON_KEY, icVar);
    return ic;
}

//Rummage through Gnome's trash to find the icon...
void addPixBuffSize(QIcon* ic, GAppInfo *app_info ,
                  GtkIconTheme * curIconTheme, int size){

    GError* err_ptr = NULL;
    GtkIconLookupFlags flags = GTK_ICON_LOOKUP_NO_SVG;
    GIcon * appGIcon = g_app_info_get_icon(app_info );
    if( !appGIcon){
        qDebug() << " error getting gicon";
        return; }
    GtkIconInfo * appGtkIconInfo = gtk_icon_theme_lookup_by_gicon(
                        curIconTheme,appGIcon,size, flags);
    if( !appGtkIconInfo){
        qDebug() << " error getting GtkIconInfo";
        return; }

    GdkPixbuf* buff = gtk_icon_info_load_icon(appGtkIconInfo,&err_ptr);

    if( err_ptr){
        qDebug() << " error loading app icon" << err_ptr->message;
        gtk_icon_info_free(appGtkIconInfo);
        return; }

    if( !buff){
        qDebug() << " error scaling buffer" ;
        return;  }

    buff = gdk_pixbuf_scale_simple(buff,size, size,GDK_INTERP_BILINEAR);
    if( !buff){
        qDebug() << " error scaling buffer" ;
        return; }
    gchar* pixmap;
    gsize buflen;
    gdk_pixbuf_save_to_buffer (buff, &pixmap,
                               &buflen, "png",
                               &err_ptr, NULL);

    if(err_ptr){
        qDebug() << " error setting buffer" << err_ptr->message;
        gdk_pixbuf_unref(buff);
        return; }

    QPixmap qp;
    bool success = qp.loadFromData((const uchar*) pixmap, buflen, "png");
    if(!success){
        qDebug() << " some error with qPixmap" ;
        if(buff){
            gdk_pixbuf_unref(buff);
        }
        return;  }

    ic->addPixmap(qp);
    free(pixmap);
    g_object_unref(buff);
    gtk_icon_info_free(appGtkIconInfo);

}



PlatformGnome::PlatformGnome()
{
    icons=0;
}

QApplication* PlatformGnome::init(int& argc, char** argv)
{
    // MUST CALL GTK BEFORE QAPP!  Otherwise things get confused
    g_thread_init(NULL); // necessary since gtk called from catalog thread
    gdk_threads_init();
    gtk_init(&argc, &argv);

    gnome_vfs_init();//This is suppose to be defunct but something demands it
        
    QApplication * app = new MyApp(argc, argv);
    QString styleName = "GTK";
    QStyle* gtkStyle = QStyleFactory::create(styleName);
    if(gtkStyle){
        app->setStyle(gtkStyle);
    } else {
        qDebug() << "style PlatformGnome::init" << styleName << " not found";
    }

    if(!icons){
        icons = (QFileIconProvider*) new GnomeIconProvider();
    }
    return app;

}

PlatformGnome::~PlatformGnome()
{ 
    GlobalShortcutManager::clear();
    //gnome_vfs_shutdown();
}

QList<Directory> PlatformGnome::GetInitialDirs() {
    QList<Directory> list;
    const char *dirs[] = { "/usr/share/applications/",
                           "/usr/local/share/applications/",
                           "/usr/share/gdm/applications/",
                           "/usr/share/applications/kde/",
                           "~/.local/share/applications/"};
    QStringList l;
    l << "*.desktop";

    for(int i = 0; i < 5; i++)
        list.append(Directory(dirs[i],l,false,false,100));

    list.append(Directory("~",QStringList(),true,false,DEFAULT_BASE_SEARCH_DEPTH));

    return list;
}

QHash<QString, QList<QString> > PlatformGnome::GetDirectories(QString base) {

    QHash<QString, QList<QString> > out;
    QDir d;
    d.mkdir(QDir::homePath() + base);

    QString shareBase = "/usr/share/oneline/";
    QString libBase = "/usr/lib/oneline/";


    out["skins"] += shareBase + "/skins";
    out["skins"] += qApp->applicationDirPath() + "/skins";
    out["skins"] += QDir::homePath() + base +"/skins";

    out["plugins"] += libBase + "/plugins";
    out["plugins"] += qApp->applicationDirPath() + "/plugins";
    out["plugins"] += QDir::homePath() + base +"/plugins";

    out["script"] += shareBase +"/script";
//    out["script"] += qApp->applicationDirPath() +"/script";;
//    out["script"] += QDir::homePath() + base +"/script";;

    out["config"] += QDir::homePath() + base + "oneline.ini";
    out[APP_HOME_KEY_DIRS] += QDir::homePath() + base + "";
    out["portConfig"] += qApp->applicationDirPath() + "/oneline.ini";
    out["db"] += QDir::homePath() + base + "oneline.db";
    out["portDB"] += qApp->applicationDirPath() + "/oneline.db";

    if (QFile::exists(out["skins"].last() + "/Default"))
        out["defSkin"] += out["skins"].last() + "/Default";
    else
      out["defSkin"] += out["skins"].first() + "/Default";

    out["platforms"] += qApp->applicationDirPath();

    return out;
}

CatItem PlatformGnome::alterItem(CatItem* item, bool addIcon) {
    GError *error=0;
    gdk_threads_enter();
    const char *content_type;
    const char *mime_type_chars;
    //const char *command_line_chars;

    if(item->getItemType() == CatItem::ACTION_TYPE) {
        return alterActionTypeItem(item, addIcon);
    }

    if(item->hasLabel(DESKTOP_ITEM_LABEL)) {
        QString dtName = item->getCustomString(DESKTOP_ITEM_LABEL);
        const char* desktop_item_name = dtName.toLatin1();
        GAppInfo* app_info = (GAppInfo*)g_desktop_app_info_new_from_filename(desktop_item_name);
        if(!app_info){
            qDebug() << "failed to get App Info for verb: " << desktop_item_name;
            gdk_threads_leave();
            return CatItem();
        }
        if(addIcon){
            addPixBuff(item, app_info);
        }
        bool doesntTakeArgs = !g_app_info_supports_files(app_info)
                                && !g_app_info_supports_uris(app_info);
        if(doesntTakeArgs){
            item->setCustomPluginValue(VERB_TAKES_NO_ARGS_KEY,(int)doesntTakeArgs);
        }
        bool takesURIs = g_app_info_supports_uris(app_info);
        if(takesURIs){
            item->setCustomPluginValue(VERB_TAKES_URIS,(int)doesntTakeArgs);
        }
        g_object_unref(app_info);
        gdk_threads_leave();
        return *item;
    }

    if(!item->getActionParentType().isEmpty()) {
        gdk_threads_leave();
        return CatItem();
    }
    QString mimeTypeString;
    if(!item->getCustomString(REAL_MIMETYPE_KEY).isEmpty()){
        mimeTypeString = item->getCustomString(REAL_MIMETYPE_KEY);
        mime_type_chars = mimeTypeString.toLatin1();
        content_type = g_content_type_from_mime_type(mime_type_chars);
    } else {
        mimeTypeString = gnome_vfs_get_mime_type_for_name(item->getPath().toLatin1());


        if(!mimeTypeString.isEmpty()){
            item->setCustomPluginInfo(REAL_MIMETYPE_KEY,mimeTypeString);
        }
        GFile *file = g_file_new_for_path(item->getPath().toLatin1());
        if(!file){
            qDebug() << "failed to create file info: " << item->getPath().toLatin1();
            gdk_threads_leave();
            return CatItem();
        }
        GFileQueryInfoFlags gf = G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS;
        GFileInfo *file_info = g_file_query_info( file, "standard::*", gf, NULL, &error);
        if(file_info && !error){
            content_type = g_file_info_get_content_type(file_info);
        } else {
            qDebug() << "failed to create file info: " << item->getPath().toLatin1();
            gdk_threads_leave();
            return CatItem();

        }
    }

    CatItem mimeType;
    QString description;
    if(!content_type){
        qDebug() << "failed to get content type for: " << item->getPath().toLatin1();
//        if (error) { g_error_free(error);}
//        gdk_threads_leave();
//        return CatItem();
    } else if(QString(content_type) == "mime/mime.cache"){
        qDebug() << "got some bogus generic value for: " << item->getPath().toLatin1();
//        if (error) { g_error_free(error);}
//        gdk_threads_leave();
//        return CatItem();
    } else {
        description = g_content_type_get_description(content_type);
        if(item->getCustomString(REAL_MIMETYPE_KEY).isEmpty()){
            QString mimeTypeTest = g_content_type_get_mime_type(content_type);
            if(!mimeTypeTest.isEmpty()) {
                mimeTypeString = mimeTypeTest;
                item->setCustomPluginInfo(REAL_MIMETYPE_KEY,mimeTypeString);
            }
        }
        if(!description.isEmpty() && !item->hasLabel(REAL_MIMEDESCRIPT_KEY)){
            item->setCustomPluginInfo(REAL_MIMEDESCRIPT_KEY,description);
        }
    }
    mimeType = CatItem::createActionItem(content_type, description);
    mimeType.setStub(true);
    if(!mimeType.isEmpty() && mimeType.getPath()!=item->getPath()){
        item->addParent(mimeType,FILE_CATALOG_PLUGIN_STR,
                        BaseChildRelation::DATA_CATEGORY_PARENT);
        //Q_ASSERT(item->getTypeParent(CatItem::ACTION_TYPE) ==mimeType);
    }

    item->setCustomPluginValue(ITEM_EXTENSION_LEVEL_KEY,0);
    gdk_threads_leave();
    return mimeType;
}

CatItem PlatformGnome::alterActionTypeItem(CatItem* item, bool addIcon){

    Q_ASSERT(item->getItemType() == CatItem::ACTION_TYPE);
    QString actionType = item->getName();
    QByteArray ba = actionType.toLocal8Bit();
    char* content_type = ba.data();
    GError *error=0;
    GAppInfo* app_info = g_app_info_get_default_for_type(
                                content_type, FALSE);
    if(!app_info){
        qDebug() << "failed to get App Info for: " << QString(content_type) << item->getPath();
        if(error){
            qDebug() << "error: " << error->message;
        }
        if (error) { g_error_free(error);}
        gdk_threads_leave();
        return CatItem();
    }
    CatItem action = createActionChild(app_info);


    GList* gl;
//#ifdef GIO28
//    gl = g_app_info_get_recommended_for_type(content_type);
//    for(int j=0;
//        j<MAX_CHILDTYPE_COUNT && gl;
//        gl = gl->next,j++){
//        Q_ASSERT(gl->data);
//        CatItem extraAction = createActionChild((GAppInfo*)gl->data);
//        if(extraAction==action){
//            continue;
//        }
//        if(addIcon){
//            addPixBuff(&extraAction,(GAppInfo*)gl->data);
//        }
//        item->addChild(extraAction,
//                       FILE_CATALOG_PLUGIN_STR,
//                       BaseChildRelation::PREFERED_ACTION_CHILD);
//    }
//#endif
    gl = g_app_info_get_all_for_type(content_type);
    for(int j=0;
            j<MAX_CHILDTYPE_COUNT && gl;
            gl = gl->next,j++){
        Q_ASSERT(gl->data);
        CatItem extraAction = createActionChild((GAppInfo*)gl->data);
        if(extraAction==action){
            continue;
        }
        if(addIcon){
            addPixBuff(&extraAction,(GAppInfo*)gl->data);
        }
        item->addChild(extraAction,
                          FILE_CATALOG_PLUGIN_STR,
                          BaseChildRelation::OPTIONAL_ACTION_CHILD);
    }



    if(addIcon){
        QIcon ic = addPixBuff(&action,app_info);
        QVariant icVar(ic);
        item->setCustomPluginVariant(REAL_ICON_KEY, icVar);
    }
    item->addChild(action,
                       FILE_CATALOG_PLUGIN_STR,
                       BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD);
    Q_ASSERT(item->getActionTypeChild() == action);

    if (error) { g_error_free(error); }
    gdk_threads_leave();

    item->setStub(false);
    return action;
}




//virtual QList<CatItem> getSubdesktopItems(CatItem it){
//
//}


bool PlatformGnome::Execute(QString path, QString args)
{
    if (!path.endsWith(".desktop", Qt::CaseInsensitive))
        {return false;}

    gdk_threads_enter();

    GError * error = NULL;
    GnomeDesktopItem* ditem = gnome_desktop_item_new_from_file(path.toLocal8Bit().data(),
							       (GnomeDesktopItemLoadFlags) 0,
							       &error);
    if (error) {
        g_error_free(error);
        gdk_threads_leave();
        return false;
    }
    if (!ditem) return false;
    
    // Get the args
    GList * list = NULL;
    args = args.trimmed();
    if (args.size() > 0) {
        foreach(QString s, args.split(" ")) {
            list = g_list_append(list, s.toLocal8Bit().data());
        }
    }
    
    gnome_desktop_item_launch(ditem, list, (GnomeDesktopItemLaunchFlags) 0, &error);

    g_list_free(list);

    if (error) {
        g_error_free(error);
        gnome_desktop_item_unref(ditem);
        gdk_threads_leave();
        return false;
    }
			      
    gnome_desktop_item_unref(ditem);
    gdk_threads_leave();
    return true;
}

//QList<CatItem> PlatformGnome::getActiveWindows(){
//    QHash<QString, CatItem>  testHash;
//    QList<CatItem>  res;
//
//    CatItem baseItem(BASE_TASKSRC_PATH);
//
//    WnckScreen * screen;
//    WnckWindow* window;
//    for(int i=0;i <10; i++){
//        screen = wnck_screen_get(i);
//        if(!screen){ break; }
//        //This isn't supposed to be called often...
//        wnck_screen_force_update(screen);
//
//        //Love the crude gnome structures...
//        GList* gl = wnck_screen_get_windows(screen);
//        for(int j=0;j<30 && gl;j++){
//            Q_ASSERT(gl->data);
//            window = (WnckWindow*)gl->data;
//            //wnck_screen_get_active_window(screen);
//            if(!wnck_window_is_skip_tasklist(window)){
//                CatItem it =  WindowToItem(window);
//                it.setExternalWeight(1,baseItem);
//                if(!testHash.contains(it.getPath())){
//                    res.append(it);
//                    testHash[it.getPath()] = it;
//                }
//            }
//            gl = gl->next;
//        }
//    }
//    return res;
//}

CatItem PlatformGnome::WindowToItem(WnckWindow* w){
    WnckApplication* a = wnck_window_get_application(w);
    QString name = wnck_application_get_name(a);

    QString pseudo_path = QString("wnck_id%1").arg(wnck_window_get_xid (w));
    CatItem it( addPrefix(TASK_PREFIX , pseudo_path), name);
    it.setDescription(wnck_window_get_name(w));
//    if(wnck_window_has_icon_name(w)){
//        it.icon = wnck_window_get_icon_name(w);
//    }
    it.setUseDescription(true);

    //Add parents for Application and gnome group that thing is in...
    //Then task are under applications...
    return it;
}


QIcon PlatformGnome::icon(const QFileInfo & info) {
    return ((GnomeIconProvider*)icons)->getGnomeicon(info);
}

QIcon PlatformGnome::iconFromMime(QString mimeType) {
    return ((GnomeIconProvider*)icons)->getGnomeiconFromMime(mimeType);
}
QString PlatformGnome::GetSettingsDirectory() {
    Q_ASSERT(false);
    return "";
};
void PlatformGnome::AddToNotificationArea() {

};
void PlatformGnome::RemoveFromNotificationArea() {};
bool PlatformGnome::isAlreadyRunning() { return false; }

// Set hotkey
bool PlatformGnome::SetHotkey(const QKeySequence& key, QObject* receiver, const char* slot) {
    GlobalShortcutManager::disconnect(oldKey, receiver, slot);
    GlobalShortcutManager::connect(key, receiver, slot);
    oldKey = key;
    return GlobalShortcutManager::isConnected(key);
}

// Need to alter an indexed item?  e.g. .desktop files
QList<CatItem> PlatformGnome::getSubdesktopItems(){return parseXdgMenus();};
CatItem PlatformGnome::associatedApp(QString ){return CatItem();}




Q_EXPORT_PLUGIN2(platform_gnome, PlatformGnome)
