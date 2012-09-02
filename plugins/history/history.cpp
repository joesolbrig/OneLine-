/*
oneline: Multibrowser and App Launcher
Copyright (C) 2012 Hans Solbrig

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
#include <QUrl>
#include <QFile>
#include <QRegExp>
#include <QTextCodec>

#ifdef Q_WS_WIN
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>

#endif

#include "history.h"


// This plugin currently reads all items from and writes all items to a file stream
// It conceivably could write to a socket or other stream...


void HistoryPlugin::init()
{

}

void HistoryPlugin::getID(uint* id)
{
	*id = HASH_HISTORY_PLUGIN;
}

void HistoryPlugin::getName(QString* str)
{
	*str = "History";
}

void HistoryPlugin::getLabels(InputList* )
{
}

bool HistoryPlugin::load(QList<CatItem>* cat){
    loadFile("userCreatedItems.db",cat);

    if(loadFile("oneline.db",cat)){
        return true;
    }
    //Attempt some recovery but rescan db
    qDebug() << "file not loaded, attempting recovery..";
    loadFile("oneline.db.backup",cat);
    return false;

}



bool HistoryPlugin::loadFile(QString fileName,QList<CatItem>* cat){
    QString dest = (*settings)->fileName();
    int lastSlash = dest.lastIndexOf(QLatin1Char('/'));
    if (lastSlash == -1) {return false;}

        dest = dest.mid(0, lastSlash) + "/";
        dest += fileName;

    QFile inFile(dest);
    if (!inFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray ba = inFile.readAll();
    QByteArray unzipped = qUncompress(ba);
    QDataStream in(&unzipped, QIODevice::ReadOnly);
//    qint32 version;
//    in >> version;
//    if (version !=QDataStream::Qt_4_2){
//        return;
//    }
    while(!in.atEnd()) {
            CatItem item;
            in >> item;
        cat->append(item);
    }
    return true;
}


void HistoryPlugin::save(QList<CatItem>* cat){

    QString dest = (*settings)->fileName();
    int lastSlash = dest.lastIndexOf(QLatin1Char('/'));
    if (lastSlash == -1){
        return;
    }
    dest = dest.mid(0, lastSlash);
    dest += "/oneline.db";

    QByteArray ba;
    QDataStream out(&ba, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_4_2);
    for(int i = 0; i < cat->count(); i++) {
        CatItem it = cat->value(i);
        out << it;
    }

    QFile file(dest);
    if(file.exists()){
        QFile backupFile(dest +".backup");
        if(backupFile.exists()){
            backupFile.remove();
        }
        file.copy(dest,dest +".backup");
    }
    if (!file.open(QIODevice::WriteOnly)) {
        Q_ASSERT(FALSE);
        qDebug() << "Could not open database for writing";
    }
    // Zip the archive
    file.write(qCompress(ba));
}

void HistoryPlugin::saveUserActions(QList<CatItem>* actions){

    QString dest = (*settings)->fileName();
    int lastSlash = dest.lastIndexOf(QLatin1Char('/'));
    if (lastSlash == -1){
        return;
    }
    dest = dest.mid(0, lastSlash);
    dest += "/userCreatedItems.db";

    QByteArray ba;
    QDataStream out(&ba, QIODevice::ReadWrite);
    out.setVersion(QDataStream::Qt_4_2);
    for(int i = 0; i < actions->count(); i++) {
        CatItem it = actions->value(i);
        out << it;
    }

    // Don't zip the archive 'cause it's smaller
    // and we want to append
    QFile file(dest);
    if (!file.open(QIODevice::Append)) {
        Q_ASSERT(FALSE);
        //		qDebug() << "Could not open database for writing";
    }
    file.write(ba);
}

void HistoryPlugin::getCatalog(QList<CatItem>* items)
{
    items = items;
}

void HistoryPlugin::getResults(InputList*, QList<CatItem>* )
{
//	// Check for history matches
//	QString location = "History/" + txt;
//	QStringList hist;
//	hist = gSettings->value(location, hist).toStringList();
//	if(hist.count() == 2) {
//		for(int i = 0; i < catMatches.count(); i++) {
//			if (catMatches[i]->lowName == hist[0] &&
//				catMatches[i]->fullPath == hist[1]) {
//				CatItem* tmp = catMatches[i];
//				catMatches.removeAt(i);
//				catMatches.push_front(tmp);
//			}
//		}
//	}
}


void HistoryPlugin::launchItem(InputList* , CatItem* )
{
	//Should never be called this stores other things items..
}

void HistoryPlugin::doDialog(QWidget* , QWidget** ) {
//	if (gui != NULL) return;
//	gui = new Gui(parent);
//	*newDlg = gui;
}

void HistoryPlugin::endDialog(bool ) {
//	if (accept) {
//		gui->writeOptions();
//		init();
//	}
//	if (gui != NULL)
//		delete gui;
//
//	gui = NULL;
}


int HistoryPlugin::msg(int msgId, void* wParam, void* lParam)
{
	bool handled = false;
	switch (msgId)
	{		
	case MSG_INIT:
		init();
		handled = true;
		break;
    case MSG_LOAD_ITEMS:
        handled = load((QList<CatItem>*) wParam);
        break;
    case MSG_SAVE_ITEMS:
        save((QList<CatItem>*) wParam);
        handled = true;
        break;
	case MSG_GET_LABELS:
                getLabels((InputList*) wParam);
		handled = true;
		break;
	case MSG_GET_ID:
		getID((uint*) wParam);
		handled = true;
		break;
	case MSG_GET_NAME:
		getName((QString*) wParam);
		handled = true;
		break;
	case MSG_GET_RESULTS:
                getResults((InputList*) wParam, (QList<CatItem>*) lParam);
		handled = true;
		break;
	case MSG_GET_CATALOG:
		getCatalog((QList<CatItem>*) wParam);
		handled = true;
		break;
	case MSG_LAUNCH_ITEM:
                launchItem((InputList*) wParam, (CatItem*) lParam);
		handled = true;
		break;
        case MSG_USER_CREATED_ITEM:
                saveUserActions((QList<CatItem>*) wParam);
                handled = true;
                break;
	case MSG_HAS_DIALOG:
		handled = true;
		break;
	case MSG_DO_DIALOG:
		doDialog((QWidget*) wParam, (QWidget**) lParam);
		break;
	case MSG_END_DIALOG:
		endDialog((bool) wParam);
		break;
	default:
		break;
	}

	return handled;
}

Q_EXPORT_PLUGIN2(filecatalog, HistoryPlugin) 
