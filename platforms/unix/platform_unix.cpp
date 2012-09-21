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

#include <QtGui>
#include <QApplication>
#include <QX11Info>
#include "platform_unix.h"

#include <X11/Xlib.h>


#define GNOME_PLUGIN_DEFINE 1

PlatformUnix::PlatformUnix() : PlatformBase() 		
{
    alpha = NULL;
    icons = NULL;
}

QApplication* PlatformUnix::init(int & argc, char** argv)
{        
    //    QApplication * app = new QApplication(*argc, argv);
    QApplication * app = new MyApp(argc, argv);
    icons = (QFileIconProvider *) new UnixIconProvider();
    return app;
}

PlatformUnix::~PlatformUnix()
{ 
    GlobalShortcutManager::clear();
    delete icons;
}

QList<Directory> PlatformUnix::GetInitialDirs() {
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

    list.append(Directory("~",QStringList(),true,false,0));
    
    return list;
}


QHash<QString, QList<QString> > PlatformUnix::GetDirectories(QString ){
    QHash<QString, QList<QString> > out;
    QDir d;
    d.mkdir(QDir::homePath() + "/.oneline");
    
    out["skins"] += qApp->applicationDirPath() + "/skins";
    out["skins"] += QDir::homePath() + "/.oneline/skins";
#ifdef SKINS_PATH_Foo
    out["skins"] += SKINS_PATH;
#endif

    out["plugins"] += qApp->applicationDirPath() + "/plugins";
    out["plugins"] += QDir::homePath() + "/.oneline/plugins";
//#ifdef PLUGINS_PATH_foo
//    out["plugins"] += PLUGINS_PATH;
//#endif

    out["config"] += QDir::homePath() + "/.oneline/oneline.ini";
    out["portConfig"] += qApp->applicationDirPath() + "/oneline.ini";
    out["db"] += QDir::homePath() + "/.oneline/oneline.db";
    out["portDB"] += qApp->applicationDirPath() + "/oneline.db";
    
    if (QFile::exists(out["skins"].last() + "/Default"))
	out["defSkin"] += out["skins"].last() + "/Default";
    else
      out["defSkin"] += out["skins"].first() + "/Default";

    out["platforms"] += qApp->applicationDirPath();
//#ifdef PLATFORMS_PATH_foo
//    out["platforms"] += PLATFORMS_PATH;
//#endif

    return out;
}



bool PlatformUnix::CreateAlphaBorder(QWidget* w, QString ImageName)
{
    if (alpha)
	delete alpha;
  
    if (ImageName == "")
	ImageName = alphaFile;
    alphaFile = ImageName;
    alpha = new AlphaBorder(w, ImageName);
    return true;
}

bool PlatformUnix::SupportsAlphaBorder()
{
    //    return QX11Info::isCompositingManagerRunning();
    
    QProcess qp;
    QString program = "/bin/sh";
    QStringList args;
    args << "-c" << "ps ax | grep 'compiz\\|beryl' | grep -v 'grep'";
    qp.start(program, args);   
    qp.waitForFinished();
    QByteArray result = qp.readAll();
    //    qDebug() << result;
    if (result.length() > 0)
	return true;
    return false;
    
}

//Q_EXPORT_PLUGIN2(platform_unix, PlatformUnix)


CatItem PlatformUnix::alterItem(CatItem* item, bool ) {
    if (!item->getPath().endsWith(".desktop", Qt::CaseInsensitive))
        return CatItem();

    QString locale = QLocale::system().name();

    QFile file(item->getPath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return CatItem();

    QString name = "";
    QString icon = "";
    QString exe = "";
    while(!file.atEnd()) {
	QString line = file.readLine();
	
	if (line.startsWith("Name[" + locale, Qt::CaseInsensitive)) 
	    name = line.split("=")[1].trimmed();
	

	else if (line.startsWith("Name=", Qt::CaseInsensitive)) 
	    name = line.split("=")[1].trimmed();

	else if (line.startsWith("Icon", Qt::CaseInsensitive))
	    icon = line.split("=")[1].trimmed();
	else if (line.startsWith("Exec", Qt::CaseInsensitive))
	    exe = line.split("=")[1].trimmed();	
    }
    if (name.size() >= item->getName().size() - 8) {
        item->setName(name);
    }

        
    QStringList allExe = exe.trimmed().split(" ",QString::SkipEmptyParts);
    if (allExe.size() == 0)
        return CatItem();
    exe = allExe[0];
    allExe.removeFirst();
    //    exe = exe.trimmed().split(" ")[0];

    
    // Look for the executable in the path
    if (!QFile::exists(exe) && exe != "") {
        foreach(QString line, QProcess::systemEnvironment()) {
            if (!line.startsWith("Path", Qt::CaseInsensitive))
            continue;

            QStringList spl = line.split("=");
            QStringList spl2 = spl[1].split(":");
            foreach(QString dir, spl2) {
                QString tmp = dir + "/" + exe;
                if (QFile::exists(tmp)) {
                    exe = tmp;
                    break;
                }
            }
            break;
        }
    }
    
    
    item->getPath() = exe + " " + allExe.join(" ");

    // Cache the icon for this desktop file
    UnixIconProvider* u = (UnixIconProvider*) icons;
    
    icon = u->getDesktopIcon(file.fileName(), icon);

    item->setIcon(icon);

    file.close();
    return CatItem();
}

#ifndef GNOME_PLUGIN_DEFINE
Q_EXPORT_PLUGIN2(platform_unix, PlatformUnix)
#endif
