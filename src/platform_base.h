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

#ifndef PLATFORM_BASE_H
#define PLATFORM_BASE_H

#include <QtGui>
#include <QWidget>
#include <QPoint>
#include <QStringList>
#include <QList>
#include <QFileIconProvider>
#include <QObject>
#include <QProcess>

#include "platform_util.h"
#include "directory.h"
#include "item.h"

//Pure virtual interface, darn it
class PlatformBase {
public:
    PlatformBase(){ };
    virtual ~PlatformBase(){};

    virtual QApplication* init(int & argc, char** argv)=0;
    virtual QIcon icon(const QFileInfo & info)=0;
    virtual QIcon iconFromMime(QString ){return QIcon();};
    virtual QString mimeFromExtension(QString){return QString();}
    virtual QString GetSettingsDirectory() = 0;
    virtual QList<Directory> GetInitialDirs() = 0;
    virtual void AddToNotificationArea() = 0;
    virtual void RemoveFromNotificationArea() = 0;
    virtual bool Execute(QString path, QString args)=0;
    virtual bool isAlreadyRunning() = 0;

    // Set hotkey
    virtual bool SetHotkey(const QKeySequence& key, QObject* receiver, const char* slot) = 0;

    // Need to alter an indexed item?  e.g. .desktop files
    virtual CatItem alterItem(CatItem*, bool addIcon=false) {
        addIcon = addIcon;//dumb compiler spoofing...
        return CatItem();}
    virtual QList<CatItem> getSubdesktopItems()= 0;

    virtual CatItem associatedApp(QString )= 0;


    virtual QHash<QString, QList<QString> > GetDirectories(QString ,
                                                           QSettings* set=0) {
        set=set;
        return QHash<QString, QList<QString> >();
    };
    virtual void registerToGetActiveWindows(QWidget*){Q_ASSERT(false);};
    virtual void scanActiveWindowsNow(){Q_ASSERT(false);};


};

Q_DECLARE_INTERFACE(PlatformBase, "net.oneline.PlatformBase/1.0")

#endif
