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
#include <QtSql/qsqlquery.h>
#include <QtSql/QSqlDatabase>
#include <QSqlError>

#ifdef Q_WS_WIN
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>

#endif

#include "firefox.h"

FirefoxPlugin* gmypluginInstance = NULL;

void FirefoxPlugin::getID(uint* id)
{
    *id = HASH_FIREFOX_PLUGIN;
}

void FirefoxPlugin::getName(QString* str) {
    *str = FIREFOX_PLUGIN_NAME;
}

void FirefoxPlugin::init()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setConnectOptions("QSQLITE_OPEN_READONLY=1");
}

void FirefoxPlugin::setPath(QString *path){
    if(!path) {return;}
    libPath = *path;
    checkRefreshTempFile();
}

void FirefoxPlugin::checkRefreshTempFile(){
//    QString firefoxPlacePath = getFirefoxPath();
//    m_db.setConnectOptions("QSQLITE_OPEN_READONLY");
//    m_db.setDatabaseName(firefoxPlacePath);
//    m_db.setConnectOptions("QSQLITE_OPEN_READONLY");

    bool refreshingPlaceInfo = false;
    QDir baseDir(QDir::homePath() + PUSER_APP_DIR);
    QString firefoxPlacePath = getFirefoxPath();
    if(firefoxPlacePath.length() == 0){return;}
    templatePlacePath = baseDir.absoluteFilePath("tempPlaces.sqlite");

    QFileInfo tempPlacesInfo(templatePlacePath);
    QFileInfo ffPlacesInfo(firefoxPlacePath);

    //Refresh if deleted or an hour old
    if(!tempPlacesInfo.exists() ||
       ( tempPlacesInfo.created().secsTo(
               ffPlacesInfo.lastModified() )> 60*60)){
        refreshingPlaceInfo = true;
        QFile ffp(firefoxPlacePath);
        if(tempPlacesInfo.exists()){
            QFile tpf(templatePlacePath);
            tpf.remove();
        }
        bool success = ffp.copy(templatePlacePath);
        if(!success){
            qDebug() << ffp.errorString();
        }
    }
    m_db.setDatabaseName(templatePlacePath);

    if(refreshingPlaceInfo){
        createIcons();
    }

}

void FirefoxPlugin::getLabels(InputList* )
{


}

void FirefoxPlugin::getResults(InputList*, QList<CatItem>* )
{

}


void FirefoxPlugin::getCatalog(QList<CatItem>* items)
{
    qDebug() << "Firefox plugin get catalog";
    createIcons();

    //Everything recent plus bookmarks...
    int frequencyMin = FIRE_FOX_FRECENCY;
    QString queryStr =
            " SELECT distinct url, moz_places.title, moz_places.frecency, moz_places.favicon_id  "
            " from moz_places, moz_bookmarks where (moz_bookmarks.fk=moz_places.id and frecency > 10) or "
            " (frecency > %1) order by frecency desc";
    queryStr = queryStr.arg(frequencyMin);
    itemsFromQuery(items, queryStr);

}

void FirefoxPlugin::extendCatalog(SearchInfo* existingItems, QList<CatItem>* items){

    qDebug() << "Firefox plugin EXtend catalog";
    createIcons(existingItems->lastUpdate);

    QString queryStr =
            " SELECT distinct url, moz_places.title, moz_places.frecency, moz_places.favicon_id "
            " from moz_places, moz_historyvisits where moz_historyvisits.id=moz_places.id and "
            " (visit_date > %1) ";

    qint64 lastUpdateTime = existingItems->lastUpdate*PRD_TIME_SCALE;
    queryStr = queryStr.arg(queryStr,  lastUpdateTime);
    itemsFromQuery(items, queryStr);

}

void FirefoxPlugin::itemsFromQuery(QList<CatItem>* items, QString queryStr)
{
    checkRefreshTempFile();
    //m_db.setConnectOptions("QSQLITE_OPEN_READONLY");

    //Sqlite Db shouldn't need a username or a password...
    if(!m_db.open()){
        qDebug() << m_db.lastError();
        return;//ERROR...
    }

    //In stages so we can be errors separately
    QString queryRes("");

    {//
        QSqlQuery query(queryRes, m_db );
        bool isSuccess = query.exec(queryStr);
        if(!isSuccess){
            qDebug() << "db error: " << queryStr << " : " << query.lastError();
            m_db.close();
            return;
        }

        CatItem me =getPluginRep();

        while (query.next()) {
            QString url= query.value(0).toString();
            if(url.isEmpty()){continue;}
            QString description= query.value(1).toString();
            CatItem newItem( url);
            newItem.setDescription(description);
            //newItem.setCustomPluginInfo(REAL_MIMETYPE_KEY,DEFAULT_HTML_MIME_TYPE);
            newItem.setActionType(DEFAULT_HTML_MIME_TYPE);
            Q_ASSERT(newItem.getActionParentType() == DEFAULT_HTML_MIME_TYPE);
            int rawFrecency = MIN(query.value(2).toInt(),MAX_EXTERNAL_WEIGHT) ;
            newItem.setExternalWeight(rawFrecency, me);
            newItem.setItemType(CatItem::PUBLIC_DOCUMENT);
            newItem.setIsUpdatableSource();
            //It's in Firefox ... ipso facto seen
            newItem.setCustomValue(ITEM_SEEN_KEY, (int)true);

            int id = query.value(3).toInt();

            QDir baseDir(QDir::homePath() + PUSER_APP_DIR);
            QString iconPath = baseDir.absoluteFilePath(ICON_SUBDIR_NAME);
            QString iconP = iconPath + PATH_SEP + QString("%1").arg(id);

            newItem.setIcon(iconP);
            items->append(newItem);
        }
        query.clear();
    }//Block so query is destroyed.

//    if(items->count()>0){
//        qDebug() << "Firefox plugin altering one item "
//                "and the other should get the benefits once inserted";
//        int l = items->count()-1;
//        CatItem last = items->at(l);
//        thePlatform->alterItem(&last);
//        items->insert(l,last);
//    }

    qDebug() << "Firefox plugin got items: " << items->count();

    m_db.close();
}

//Create files for all or recent favicons...
void FirefoxPlugin::createIcons(time_t time_from){

    QDir baseDir(QDir::homePath() + PUSER_APP_DIR);
    QString iconPath = baseDir.absoluteFilePath(ICON_SUBDIR_NAME);
    QDir d(iconPath);
    if(!d.exists()){
        d.mkdir(iconPath);
    }
    //We might want to erase earlier icons... or not

    m_db.setConnectOptions("QSQLITE_OPEN_READONLY");
    if(!m_db.open()){
        qDebug() << "error opening sqlite db:" << m_db.lastError();
        return;//ERROR...
    }

    QString queryStr;
    if(time_from==0){
        queryStr = ("SELECT distinct data, id from moz_favicons");
    } else {
        qint64 lastUpdateTime = time_from*PRD_TIME_SCALE;
        queryStr = ("SELECT distinct data, moz_favicons.id from moz_favicons, "
        "moz_historyvisits where moz_historyvisits.id=moz_favicons.id and "
                    " (visit_date > %1)");
        queryStr = queryStr.arg(queryStr,  lastUpdateTime);
    }

    QSqlQuery query("", m_db );
    bool isSuccess = query.exec(queryStr);
    if(!isSuccess){
        qDebug() << query.lastError();
        m_db.close();
        return;
    }
    while (query.next()) {
        QByteArray icon_data = query.value(0).toByteArray();
        int id = query.value(1).toInt();
        QString iconName = iconPath + PATH_SEP + QString("%1").arg(id);
        QFile file(iconName);
        file.open(QIODevice::WriteOnly);
        file.write(icon_data);
        file.close();
    }
    query.clear();
    m_db.close();

}

//bool FirefoxPlugin::modifyItem(CatItem* id , UserEvent::LoadType ) {
//    Q_ASSERT(id);
//    if(id->hasLabel(FIREFOX_PLUGIN_NAME)){
//        id->setActionType(DEFAULT_HTML_MIME_TYPE);
//        CatItem it= thePlatform->alterItem(id);
//        return !it.isEmpty();
//    }
//    return false;
//
//}

void FirefoxPlugin::launchItem(InputList*, CatItem* ) {

}

void FirefoxPlugin::doDialog(QWidget* , QWidget** )
{

}

void FirefoxPlugin::endDialog(bool)
{
}

QString FirefoxPlugin::getFirefoxPath()
{
	QString path;
	QString iniPath;
	QString appData;
	QString osPath;

#ifdef Q_WS_WIN
	GetShellDir(CSIDL_APPDATA, appData);
	osPath = appData + "/Mozilla/Firefox/";
#endif

#ifdef Q_WS_X11
	osPath = QDir::homePath() + "/.mozilla/firefox/";
#endif

	iniPath = osPath + "profiles.ini";

	QFile file(iniPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "couldn't open firefox ini file for reading";
        qDebug() << file.errorString();
        return "";
    }
	bool isRel = false;
	while (!file.atEnd()) {
		QString line = file.readLine();
		if (line.contains("IsRelative")) {
			QStringList spl = line.split("=");
			isRel = spl[1].toInt();
		}
		if (line.contains("Path")) {
			QStringList spl = line.split("=");
			if (isRel)
                path = osPath + spl[1].mid(0,spl[1].count()-1) ;
			else
				path = spl[1].mid(0,spl[1].count()-1);
			break;
		}
	} 	
    QDir ffDir(path);
    return ffDir.filePath("places.sqlite");

}

int FirefoxPlugin::msg(int msgId, void* wParam, void* lParam)
{
	bool handled = false;
	switch (msgId)
	{		
		case MSG_INIT:
			init();
			handled = true;
			break;
        case MSG_PATH:
            setPath((QString*)wParam);
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
//            case MSG_ITEM_LOADING:
//                    handled = modifyItem((CatItem* )wParam, (UserEvent::LoadType)(int)lParam);
//                    break;
            case MSG_LAUNCH_ITEM:
                    launchItem((InputList*) wParam, (CatItem*) lParam);
                    handled = true;
                    break;
            case MSG_HAS_DIALOG:
                    // Set to true if you provide a gui
                    handled = false;
                    break;
            case MSG_DO_DIALOG:
                    // This isn't called unless you return true to MSG_HAS_DIALOG
                    doDialog((QWidget*) wParam, (QWidget**) lParam);
                    break;
            case MSG_END_DIALOG:
                    // This isn't called unless you return true to MSG_HAS_DIALOG
                    endDialog((bool) wParam);
                    break;

		default:
			break;
	}
		
	return handled;
}

Q_EXPORT_PLUGIN2(firefoxP, FirefoxPlugin)
