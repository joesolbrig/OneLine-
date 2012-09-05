#include <QPluginLoader>
#include <QDebug>
#include "plugin_interface.h"
#include "platform_util.h"
//#include "globals.h"
//#include "main.h"

QObject* loadFromPlatformCandidates(QStringList files){
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
    QStringList subfiles;
    Q_ASSERT(true);
    #ifdef Q_WS_X11
    #ifndef QT_NO_DEBUG
        subfiles += "./debug/libplatform_gnome.so";
        subfiles += QString(PLATFORMS_PATH) + "libplatform_gnome.so";
    #endif
    #ifdef QT_NO_DEBUG
    subfiles += "./release/libplatform_gnome.so";
    subfiles += QString(PLATFORMS_PATH) + "libplatform_gnome.so";
    #endif
    #endif
    QObject * library = loadFromPlatformCandidates(subfiles);
    return qobject_cast<PlatformBase*>(library);
}
