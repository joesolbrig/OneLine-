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

#pragma once

//These defines are out of hand given this is the only way to access this stuff...
#define WNCK_I_KNOW_THIS_IS_UNSTABLE

#include <libwnck/libwnck.h>

//#include "platform_unix.h"
#include "platform_unix_util.h"
#include "platform_gnome_util.h"
#include "platform_base.h"
#include "platform_base_hotkey.h"
#include "platform_base_hottrigger.h"


#include "process_menu.h"
#include "gnomeSignalReceiver.h"

extern GnomeSignalReceiver* g_gsr;

class MyApp : public QApplication {
    Q_OBJECT
    public:
        MyApp(int & argc, char** & argv) : QApplication(argc,argv) {}
        bool x11EventFilter ( XEvent * event ) {
            if (event->type == KeyPress) {
                emit xkeyPressed(event);
            }
        return false;
    }
signals:
    void xkeyPressed(XEvent*);
};



class PlatformGnome : public QObject, public PlatformBase
{
 private:

    Q_OBJECT
    Q_INTERFACES(PlatformBase)

public:
    //V-table screwed up or something I don't understand
    //only functions in the same order as base
    PlatformGnome();
    ~PlatformGnome();
    QApplication* init(int& argc, char** argv);

    QIcon icon(const QFileInfo & info);
    virtual QIcon iconFromMime(QString mimeType);

    //not implemented..
    virtual QString mimeFromExtension(QString){return QString();}

    QString GetSettingsDirectory();
    QList<Directory> GetInitialDirs();
    void AddToNotificationArea();
    void RemoveFromNotificationArea();
    bool Execute(QString path, QString args);
    bool isAlreadyRunning();

    // Set hotkey
    bool SetHotkey(const QKeySequence& key, QObject* receiver, const char* slot);

    // Need to alter an indexed item?  e.g. .desktop files
    CatItem alterItem(CatItem*, bool addIcon=false);
    CatItem alterActionTypeItem(CatItem*, bool addIcon=false);
    QList<CatItem> getSubdesktopItems();
    //QList<CatItem> getActiveWindows();
    CatItem associatedApp(QString );

    QHash<QString, QList<QString> > GetDirectories(QString base);

    virtual void registerToGetActiveWindows(QWidget* mainWin){
        g_gsr = new GnomeSignalReceiver();
        connect(g_gsr, SIGNAL(itemsFound(QList<CatItem>)),mainWin,
                SLOT(itemsUpdatedFromBackground(QList<CatItem>)));
    };

    void scanActiveWindowsNow(){
        g_gsr->scanActiveWindows();
    };

    
private:
    CatItem WindowToItem(WnckWindow* w);

    QString alphaFile;
    QKeySequence oldKey;
    QFileIconProvider* icons;

};



