#include <QPluginLoader>
#include <QDebug>
#include "plugin_interface.h"
#include "platform_util.h"
//#include "globals.h"
//#include "main.h"

QObject* loadFromPathList(QStringList files){
    QObject * library = NULL;
    int c = files.count();
    for(int i=0; i < c && !library;i++){
        QString file = files[i];
        qDebug() << "trying to load: " << file;
        QPluginLoader loader(file);
        qDebug() << "error str1: " << loader.errorString();
        library = loader.instance();
        qDebug() << "error str2: " << loader.errorString();
        if (library) {
            qDebug() << " success";
            break;
        }
    }

    if (!library){
        qDebug() << "Could not load platform file!";
        exit(1);
    }
    return library;

}


PlatformBase * loadPlatform()
{
    #ifdef Q_WS_WIN
    files += "platform_win.dll";
    #endif
//    #ifdef Q_WS_X11
//    files += "libplatform_unix.so";
//    files += QString(PLATFORMS_PATH) + "/libplatform_unix.so";
//    #endif
//    qDebug() << "Library file pathes: " << files;
//    loadFromPathList(files);
    QStringList subfiles;
    Q_ASSERT(true);
    #ifdef Q_WS_X11
    #ifndef QT_NO_DEBUG
        subfiles += "./debug/libplatform_gnome.so";
        subfiles += QString(PLATFORMS_PATH) + "libplatform_gnome.so";
    #endif
    #endif
    QObject * library = loadFromPathList(subfiles);
    /*
    int desktop = getDesktop();
    if (desktop == DESKTOP_WINDOWS)
        files += "/platform_win.dll";
    else if (desktop == DESKTOP_GNOME) {
        files += "libplatform_unix.so";
        //	files += "libplatform_gnome.so";
        //files += QString(PLATFORMS_PATH) + QString("/libplatform_gnome.so");
    }
    else if (desktop == DESKTOP_KDE) {
        files += "libplatform_kde.so";
        files += QString(PLATFORMS_PATH) + "/libplatform_kde.so";
    }
    */

    return qobject_cast<PlatformBase*>(library);
}
