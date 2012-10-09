/*
OneLine: Application Launcher
Copyright (C) 2011 Hans Solbrig

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

#include "item.h"
#include "../fileCatalog/fileCatalog.h"

#include "fullTextPlugin.h"
//#include "platform_base.h"


// These following functions
// Need to be defined for any recoll client...

bool PluginUpdater::stopindexing=false;
RclConfig *rclconfig=0;

// Global stop request flag. This is checked in a number of place in the
// indexing routines.
int stopindexing=0;

Recoll_Interface ri;

RclConfig* RclConfig::getMainConfig()
{
    return rclconfig;
}

void FullTextPlugin::init()
{
    m_recoll_interface = &ri;
    QString path = (*settings)->value("FullTextPlugin/recol_db_dir", QDir::homePath() + "/.recoll").toString();
    QString error;
    m_recoll_interface->createDB(path, error);
    if(!error.isEmpty()){
        qDebug() << "createDB path" << path << " error:" << error;
    }

    QDir baseDir(PUSER_APP_DIR);
    QString tempPath = baseDir.absoluteFilePath(RECOL_PREVIEW_SUBDIR);
    QDir d(tempPath);
    if(!d.exists()){
        baseDir.mkdir(tempPath);
    }
    m_recoll_interface->setTempDir(tempPath);
}

FullTextPlugin::~FullTextPlugin(){
    delete m_recoll_interface;

}

//void FullTextPlugin::afterLoad(QList<CatItem>* results)

void FullTextPlugin::getID(uint* id)
{
    *id = HASH_FULLTEXT_CATALOG;
}

void FullTextPlugin::getName(QString* str)
{
    *str = "Recoll Full Text Catalog";
}

void FullTextPlugin::getLabels(InputList*  ) { }

void FullTextPlugin::extendCatalog(SearchInfo* info, QList<CatItem>* r){
    //QMutexLocker locker(&m_fullTextMutex);
    Q_ASSERT(gMutexPtr);
    QMutexLocker locker(&m_fullTextMutex); //this->gMutexPtr
    if(info->m_subthread){
        recoll_threadinit();
    }
    if(!m_recoll_interface->openDB(true)){
        return;
    }
    CatItem me = getPluginRep();

    QList<CatItem>& l = *info->itemListPtr;

    if((int)info->m_extensionType >= (int)UserEvent::SELECTED){
       QList<CatItem>* items = info->itemListPtr;
       for(int i=0; i< items->count(); i++){
            CatItem item = items->at(i);
            m_recoll_interface->getDocForPreview(item);
            QString path = item.getPath().simplified();
            while(!path.isEmpty() && path.endsWith(ARG_SEPERATOR)){
                path.chop(1);
                path = path.simplified();
            }

            item.setPath(path);
            qDebug() << "extendCatalog appending: " << item.getPath();
            r->append(item);
        }
       m_recoll_interface->closeDB();
       return;
    }

    for(int i=0;i <l.count();i++){
        CatItem& id = l[i];
        QString path = id.getPath();
        if(id.hasLabel(EXTENSION_SUBSTITUTE_PATH_KEY)){
            path = id.getSubstitutionIndexingPath();
        }
        QFile file(path); //= QDir::absoluteFilePath(fn);
        QFileInfo fi(file);
        if(fi.isDir()){
            QDir dirObj(path);
            QFileInfoList dirsInfo = dirObj.entryInfoList(QDir::Files, QDir::Time);
            for (int i = 0; i < dirsInfo.count(); ++i) {
                QFileInfo fileInfo = dirsInfo[i];
                indexAFileItem(id, fileInfo, r, false);
            }
        } else if(file.exists()) {
            unsigned int sourceUpdateTime = (unsigned int)id.getSourceUpdateTime();
            if( !id.isUpdatableSource() && sourceUpdateTime == fi.lastModified().toTime_t()
                && (sourceUpdateTime !=0)){
                continue;
            }
            indexAFileItem(id, fi, r);
            r->append(id);
        }
    }
    m_recoll_interface->closeDB();
}


//
void FullTextPlugin::indexAFileItem(CatItem& id, QFileInfo fi, QList<CatItem>* r, bool compound){
    if(!compound){
        if(m_recoll_interface->addNonCompoundFile(id,fi, r)){
            id.setSourceUpdateTime(fi.lastModified().toTime_t());
        }
    } else if(m_recoll_interface->addAPossiblyCompoundFile(id,r, FULLTEXT_EXTENDWITHIN_CYCLES)){
        id.setIsUpdatableSource(true);
        id.setSourceWeight(MAX_EXTERNAL_WEIGHT, getPluginRep());
        id.setLabel(IS_FULL_TEXT_INDEXED);
        //Set so it will always try to update
        id.setSourceUpdateTime((fi.lastModified().toTime_t()-100));
    } else {
        id.setIsUpdatableSource(false);
        if(id.hasSourceWeight() &&
                id.getSourceParents().contains(getPluginRep())){
            id.setSourceWeight(MEDIUM_EXTERNAL_WEIGHT, getPluginRep());
            id.setSourceUpdateTime(fi.lastModified().toTime_t());
        }

    }
}

void FullTextPlugin::itemsLoaded(SearchInfo* inf, QList<CatItem>* res){
    QList<CatItem>* items = inf->itemListPtr;
    Q_ASSERT(gMutexPtr);
    QMutexLocker locker(&m_fullTextMutex);
    recoll_threadinit();
    if(!m_recoll_interface->openDB(true)){
        return;
    }
    CatItem me = getPluginRep();

    for(int i=0; i< items->count(); i++){
        CatItem it = (*items)[i];
        if(it.getIsTempItem()){ continue; }
        if(it.hasLabel(FILE_CATALOG_PLUGIN_STR)){
            if((int)inf->m_extensionType >= (int)UserEvent::SELECTED){
                m_recoll_interface->getDocForPreview(it);
                QString path = it.getPath().simplified();
                while(!path.isEmpty() && path.endsWith(ARG_SEPERATOR)){
                    path.chop(1);
                    //path = path.left(path.length()-2);
                }
                it.setPath(path);
                qDebug() << "itemsLoaded appending: " << it.getPath();
                res->append(it);
            } else {
                QString fn = it.getPath();
                QFile f(fn); //= QDir::absoluteFilePath(fn);
                QFileInfo fi(fn);
                if(f.exists() && !fi.isDir()) {
                    if(it.hasLabel(FULL_TEXT_PLUGIN_KEY)
                        && (time_t)fi.lastModified().toTime_t() <= it.getModificationTime()){
                        continue;
                    }
                    if(m_recoll_interface->addAPossiblyCompoundFile(it,res,it.getMaxUpdatePasses())){
                        it.setUpdatingSourceWeight(MAX_EXTERNAL_WEIGHT,me);
                    } else {
                        it.setIsUpdatableSource(false);

                    }
                    it.setLabel(FULL_TEXT_PLUGIN_KEY);
                }
            }

        } else if(it.hasLabel(SHOULD_CACHE_KEY)){
            QByteArray itemText;
            if(it.hasLabel(LONG_TEXT_KEY_STR)){
                itemText = it.getCustomString(LONG_TEXT_KEY_STR).toAscii();
            } else {
                itemText = it.getDescription().toAscii();
            }

            qint64 hashId= stringHash(it.getPath());

            QString cacheFileName = cachePath() + PATH_SEP + QString("%1").arg(hashId);
            QFile cacheFile(cacheFileName);
            cacheFile.open(QIODevice::WriteOnly);
            cacheFile.write(itemText);
            cacheFile.close();

            CatItem cacheItem(cacheFileName);
            it.addChild(cacheItem,"",BaseChildRelation::SUBSTITUTE);
            m_recoll_interface->addAPossiblyCompoundFile(cacheItem,res,cacheItem.getMaxUpdatePasses());
        }
        //(*items)[i] = it;
    }
    m_recoll_interface->closeDB();
}


//The only extras we might add on files on the "edge" of our index
QString FullTextPlugin::cachePath(){
    QDir baseDir(PUSER_APP_DIR);
    QString cacheItemPath = baseDir.absoluteFilePath(PUSER_APP_DIR +"/"+ITEM_CACHE_NAME);
    QDir d(cacheItemPath);
    if(!d.exists()){ d.mkdir(cacheItemPath); }
    return cacheItemPath;


}
void FullTextPlugin::search(SearchInfo* search_info, QList<CatItem>* results)
{
    Q_ASSERT(gMutexPtr);
    QMutexLocker locker(&m_fullTextMutex);
    qDebug() << "FullTextPlugin::search: " ;
    if(search_info->m_subthread){
        recoll_threadinit();
    }
    if(!m_recoll_interface->openDB(true)){
        return;
    }
    QList<CatItem> res;

    QStringList kw = search_info->m_keyWords;
    qDebug() << "words " << kw.join(":");
    int kwCount = kw.count();
    if(kwCount < 1){
        m_recoll_interface->closeDB();
        return;
    }

    CatItem dirItem;
    if(search_info->itemListPtr &&  search_info->itemListPtr->length()>0){
        dirItem = search_info->itemListPtr->first();
    }

    //Create one semi-tag for each key word,
    //These is will be filtered, reworked by the main app
    QString query;
    QString dirPath;
    for(int i=0;i<kwCount;i++){
        if(kw[i].isEmpty()){continue;}
        if(kw[i].length() <4){continue;}
        query += kw[i] + " ";
    }
    if(dirPath.isEmpty() && !dirItem.isEmpty()){
        dirPath = dirItem.getPath();
    }

    QString error;
    QList<RecollQueryItem> resItems;
    qDebug() << query;
    m_recoll_interface->getMatches(query, dirPath, resItems, MAX_SEARCH_ITEMS, error);
    if(resItems.count()==0){
        //If we get nothing, don't create key-word!
        m_recoll_interface->closeDB();
        return;
    }
    QHash<QString, CatItem> items;
    for(int i=0;i<resItems.count();i++){
        CatItem finalItem = CreateFullTextItem(resItems[i], results);
        qDebug() << "FullTextPlugin::search: got " << finalItem.getPath();

        if(finalItem.isEmpty()){
            continue;
        }
        //finalItem.setIsTempItem(true);
        if(!items.contains(finalItem.getPath())){
            items[finalItem.getPath()] = finalItem;
        } else {
            qDebug() << "FullTextPlugin::search: merging" << finalItem.getPath();
            items[finalItem.getPath()].merge(finalItem,true);
        }
    }
    qDebug() << "FullTextPlugin::search: get " << items.count();
    res.append(items.values());
    results->append(res);
    m_recoll_interface->closeDB();

}

//      TODO - more specific parsing... search subdirectory
//        CatItem co = id.getContextItem(kw[i]);
//        if(co.isEmpty()){
//        } else {
//            if(co.hasLabel(FILE_DIRECTORY_PLUGIN_STR)){
//                QDir testD(co.getPath());
//                if(testD.exists()){
//                    //later overwrites earlier directories
//                    dirPath = testD.absolutePath();
//                }
//            }
//            if(dirPath.isEmpty() && !dirItem.isEmpty()){
//                dirPath = dirItem.getPath();
//            }
//        }

//CatItem FullTextPlugin::createKeywordItem(QString keyword, QList<CatItem>* ){
//
//    QString kwText = keyword.toLower().simplified();
//    CatItem kwItem((KEYWORD_PREFIX + kwText), kwText);
//    kwItem.setTagLevel(CatItem::KEY_WORD);
//    kwItem.setItemType(CatItem::TAG);
//    kwItem.setIcon(TAG_ICON_NAME);
//    kwItem.setLabel(FULL_TEXT_PLUGIN_NAME);
//    return kwItem;
//}

//We could be very elaborate in our formatting but
//that can wait...
CatItem FullTextPlugin::CreateFullTextItem(RecollQueryItem recolItem, QList<CatItem>* items) {

    QString path = recolItem.filePath;
    if(path.simplified().endsWith(ARG_SEPERATOR)){
        path = path.simplified().left(path.length()-2);
    }


    QFileInfo fi(path);
    CatItem res;
    qDebug() << "CreateFullTextItem with path:" << path;
    if(fi.exists()){
        QString containingDir = fi.dir().path();
        if(containingDir == CatItem::itemCacheDir()){
            QFile cacheFile(path);
            if(cacheFile.open(QIODevice::WriteOnly |QIODevice::Text)){
                char buf[MAX_PATH_LENGTH];
                int res = cacheFile.readLine(buf, sizeof(buf));
                QString text;
                if(res!=-1){
                    while(res>0 && (buf[res-1]=='\n'||(buf[res-1]==0)) ){ buf[res-1]=0;}
                    if(buf[0]!=0){
                        QString realPath(buf);
                        CatItem realItem(realPath);
                        while(cacheFile.readLine(buf, sizeof(buf)) !=-1){
                            text += QString(buf);
                        }
                        realItem.setTemporaryLongHtml(htmlizeStringLines(text));
                        realItem.setStub(true);
                        return realItem;
                    }
                }
            }
        }

        CatItem parent(containingDir);
        QSet<QString> dummySet;
        res = CatItem::createFileItem(dummySet,
                                      fi,
                                      parent,
                                      2,
                                      UserEvent::IGNORE,thePlatform,
                                      getPluginRep());
        if(!recolItem.longText.isEmpty()){
            qDebug() << "setTemporaryLongHtml" << recolItem.longText;
            res.setTemporaryLongHtml(recolItem.longText);
        }
    } else {
        CatItem pathIt(path);
        if(m_recoll_interface->getDocForPreview(pathIt)){
            res = pathIt;
        } else {
            qDebug() << "WARNING file: " << recolItem.filePath << "gone (possibly deleted, for example)";
            return CatItem();
        }
    }

    QString abstractStr = recolItem.abstractText.simplified();
    res.setName(fi.baseName());
    if(!abstractStr.isEmpty()){
        res.setName(abstractStr);
        res.setTemporaryDescription(abstractStr);
        //res.setUseDescription(true);
    }
    res.setMatchType(CatItem::USER_WORDS);
    //res.setIsTempItem(true);

    for(int i=0;i<recolItem.resultTerms.count();i++){
        CatItem kwParent = CatItem::createKeywordItem(recolItem.resultTerms[i]);
        res.setExternalWeight(HIGH_EXTERNAL_WEIGHT, kwParent);
        kwParent.addChild(res,TAG_LABEL,BaseChildRelation::TAG_PARENT);
        items->append(kwParent);
    }

    return res;

}

void FullTextPlugin::getCatalog(QList<CatItem>* ){

}

//TODO, to-do, To Do
//This is duplicated from fileItem...
CatItem FullTextPlugin::createFileItem(QString path, bool ) {

    //We should get all the mime stuff with file catalog
    QString fullPath = path;
    QFileInfo fi(fullPath);
    CatItem it(path, fi.baseName(),HASH_FILE_CATALOG);
    it.setStub(true);

    return it;
}


void FullTextPlugin::launchItem(InputList* , QList<CatItem>* )
{


}

void FullTextPlugin::doDialog(QWidget* , QWidget** ) {
//	if (gui != NULL) return;
//	gui = new Gui(parent);
//	*newDlg = gui;
}

void FullTextPlugin::endDialog(bool ) {
//	if (accept) {
//		gui->writeOptions();
//		init();
//	}
//	if (gui != NULL)
//		delete gui;
//
//	gui = NULL;
}

void FullTextPlugin::setPath(QString * path) {
	libPath = *path;
}

int FullTextPlugin::msg(int msgId, void* wParam, void* lParam)
{
    bool handled = false;
    switch (msgId){
            case MSG_INIT:
                init();
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
            case MSG_BACKGROUND_SEARCH:
                search((SearchInfo*) wParam, (QList<CatItem>*) lParam);
                handled = true;
                break;
            case MSG_GET_CATALOG:
                getCatalog((QList<CatItem>*) wParam);
                handled = true;
                break;
            case MSG_EXTEND_CATALOG:
                extendCatalog((SearchInfo*) wParam, (QList<CatItem>*)lParam);
                handled = true;
                break;
            case MSG_ITEMS_LOADING:
                itemsLoaded((SearchInfo*) wParam, (QList<CatItem>*)lParam);
                handled = true;
                break;
            case MSG_LAUNCH_ITEM:
                launchItem((InputList*) wParam, (QList<CatItem>*) lParam);
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
            case MSG_PATH:
                    setPath((QString *) wParam);
            default:
                    break;
        }

	return handled;
}

Q_EXPORT_PLUGIN2(FullTextPlugin, FullTextPlugin);
