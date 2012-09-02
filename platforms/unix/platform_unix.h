/*
  
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

#include "platform_unix_util.h"
#include "platform_base.h"
#include "platform_base_hotkey.h"
#include "platform_base_hottrigger.h"

#include <QX11Info>

#include "process_menu.h"


//#include <QDomNode>

/*
  This QAPP scans all x events for keypresses
  and sends them off to the hotkey manager
*/




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




class PlatformUnix : public QObject, public PlatformBase 
{
    Q_OBJECT
    Q_INTERFACES(PlatformBase)
private:
    AlphaBorder * alpha;
    QString alphaFile;

public:
    PlatformUnix();
    ~PlatformUnix();
    
    virtual QApplication* init(int & argc, char** argv);
    // Mandatory functions
    // Mandatory functions
    bool SetHotkey(const QKeySequence& key, QObject* receiver, const char* slot)
    {
	GlobalShortcutManager::disconnect(oldKey, receiver, slot);
	GlobalShortcutManager::connect(key, receiver, slot);
	oldKey = key;
	return GlobalShortcutManager::isConnected(key);
    }
    
    QString GetSettingsDirectory() { 
	return "";
    };
    
    
    virtual QList<Directory> GetInitialDirs();
    virtual bool Execute(QString , QString ){
        Q_ASSERT(false);
        return false;
    };
    
    void AddToNotificationArea() {};
    void RemoveFromNotificationArea() {};
    
    bool isAlreadyRunning() {
        return false;
    }
    

    QWidget* getAlphaWidget() {
        return alpha;
    }

    virtual QHash<QString, QList<QString> > GetDirectories(QString base);

    bool SupportsAlphaBorder();
    bool CreateAlphaBorder(QWidget* w, QString ImageName);
    void DestroyAlphaBorder() { delete alpha ; alpha = NULL; return; }
    void MoveAlphaBorder(QPoint pos) { if (alpha) alpha->move(pos); }
    void ShowAlphaBorder() { if (alpha) alpha->show(); }
    void HideAlphaBorder() { if (alpha) alpha->hide(); }
    void SetAlphaOpacity(double trans ) { if (alpha) alpha->SetAlphaOpacity(trans); }

    QIcon icon(const QFileInfo& info) {
	return ((UnixIconProvider*)icons)->getIcon(info); 
    }

    virtual CatItem alterItem(CatItem*, bool addIcon=false);
    virtual QList<CatItem> getSubdesktopItems(){
        return parseXdgMenus();
    };
    virtual QList<CatItem> getActiveWindows(){return QList<CatItem>();};
    virtual CatItem associatedApp(QString ){return CatItem();};


private:
    QKeySequence oldKey;
    QFileIconProvider* icons;

};




