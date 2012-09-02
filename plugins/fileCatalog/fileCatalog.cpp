/*
OneLine: Application Launcher
Copyright (C) 2010 Hans Solbrig

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

#include <QUrl>
#include <QFile>
#include <QRegExp>
#include <QTextCodec>

//#include <magic.h>
//#include <QMimeData>
//#include <kfileitem.h>

#ifdef Q_WS_WIN
#include <windows.h>
#include <shlobj.h>
#include <tchar.h>
#endif

#include "input_list.h"
#include "fileCatalog.h"
#include "itemarglist.h"

//#include "platform_base.h"

uint FilecatalogPlugin::HASH_FILE_CATALOG=(qHash(QString(FILE_CATALOG_PLUGIN_STR)));

void FilecatalogPlugin::init(){ }

void FilecatalogPlugin::getID(uint* id) { *id = HASH_FILE_CATALOG; }

void FilecatalogPlugin::getName(QString* str) { *str = FILE_CATALOG_PLUGIN_STR;}

void FilecatalogPlugin::getLabels(InputList* ){ }

void FilecatalogPlugin::getResults(InputList*, QList<CatItem>* ){
    //outItems->append(thePlatform->getActiveWindows());
}

void FilecatalogPlugin::getCatalog(QList<CatItem>* items){
    if(!items){ return; }
    CatItem me = getPluginRep();
    QSettings* set = *settings;

    //The system has the problem you can't be sure the state of the settings...
    set->endGroup();
    set->endGroup();
    set->beginGroup(FILE_SETTINGS_KEY);
    bool getMenuItems = set->value(GET_MENU_ITEMS_KEY, true).toBool();
    if(getMenuItems){
        QList<CatItem> menuItems = thePlatform->getSubdesktopItems();
        items->append(menuItems);
    }
    bool addRecentItem = set->value(ADD_RECENT_ITEMS_SOURCE_KEY, true).toBool();
//    if(addRecentItem){
//        CatItem recentItemSource1(XMEL_BOOKMARKS_PATH1);
//        QString filterFilePath = PUSER_APP_DIR + XLS_SUBDIR + BOOKMARK_XSLT;
//
//        recentItemSource1.setCustomPluginInfo(STREAM_FILTER_FILESOURCE,filterFilePath);
//        recentItemSource1.setLabel(STREAM_SOURCE_PATH);
//        recentItemSource1.setLabel(FILE_CATALOG_PLUGIN_STR);
//        recentItemSource1.setRequestUrl(QDir::homePath() + XBEL_HISTORY_NAME1);
//        recentItemSource1.setExternalWeight(MAX_MAX_EXTERNAL_WEIGHT,me);
//        recentItemSource1.setSourceWeight(MAX_MAX_EXTERNAL_WEIGHT,me);
//        recentItemSource1.setTagLevel(CatItem::INTERNAL_SOURCE);
//        recentItemSource1.setIsUpdatableSource(true);
//        items->append(recentItemSource1);
//
//        CatItem recentItemSource2(XMEL_BOOKMARKS_PATH2);
//        QString oldFilterFilePath = PUSER_APP_DIR + XLS_SUBDIR + OLD_STYLE_BOOKMARK_XSLT;
//        recentItemSource2.setCustomPluginInfo(STREAM_FILTER_FILESOURCE,oldFilterFilePath);
//        recentItemSource2.setLabel(STREAM_SOURCE_PATH);
//        recentItemSource2.setLabel(FILE_CATALOG_PLUGIN_STR);
//        recentItemSource2.setRequestUrl(QDir::homePath() + XBEL_HISTORY_NAME2);
//        recentItemSource2.setExternalWeight(MAX_EXTERNAL_WEIGHT,me);
//        recentItemSource2.setSourceWeight(MAX_EXTERNAL_WEIGHT, me);
//        recentItemSource2.setTagLevel(CatItem::INTERNAL_SOURCE);
//        recentItemSource2.setIsUpdatableSource(true);
//        items->append(recentItemSource2);
//    }
    set->setValue(GET_MENU_ITEMS_KEY, getMenuItems);
    set->setValue(ADD_RECENT_ITEMS_SOURCE_KEY, addRecentItem);
    set->endGroup();
    set->sync();
    CatItem dummy;

    QSet<QString> extendedTypes;
    QList<Directory> memDirs = getInitialDirs();
    for(int i = 0; i < memDirs.count(); ++i) {
        QString fp = adjustFilePath(memDirs[i].name);
        QFileInfo fileInfo(fp);
        if(!fileInfo.exists()){
            qDebug() << "base file: " << memDirs[i].name << " does not exist!";
            continue;
        }
        CatItem dirParent(fileInfo.absoluteFilePath());
        int depth = MIN(memDirs[i].depth, MAX_DIR_SEARCH_DEPTH);
        dirParent = CatItem::createFileItem(extendedTypes, fileInfo, dummy, depth,UserEvent::IGNORE,thePlatform,getPluginRep());
        dirParent.setCustomPluginValue(FILECAT_SEARCH_DEPTH, depth);
        dirParent.setCustomPluginInfo(FILE_DIRECTORY_PLUGIN_STR,memDirs[i].toString());
        indexDirectory(extendedTypes, dirParent, items);
        items->append(dirParent);
    }
}

void FilecatalogPlugin::extendCatalog(SearchInfo* existingItems, QList<CatItem>* outItems)
{
    Q_ASSERT(existingItems);
    Q_ASSERT(outItems);
    Q_ASSERT(existingItems->itemListPtr);

    qDebug() << "extending catalog";
    QSet<QString> extendedTypes;

    for(int i = 0; i < existingItems->itemListPtr->count(); ++i) {
        CatItem it = (*existingItems->itemListPtr)[i];

        if(it.hasLabel(EXTERNAL_EXTEND_COMMAND)){
            runProgram(it.getCustomString(EXTERNAL_EXTEND_COMMAND));
            continue;
        }
        if(!it.hasLabel(FILE_CATALOG_PLUGIN_STR)){ continue; }
        if(it.getTagLevel()==CatItem::ATOMIC_ELEMENT){ continue;}
//        QString codedDir = (*existingItems->itemListPtr)[i].getCustomString(FILE_DIRECTORY_PLUGIN_STR);
//        Directory d;
//        if(codedDir.length() != 0){
//            Directory dir(codedDir);
//            d = dir;
//        }
        QFileInfo fileInfo(it.getPath());
        if(!fileInfo.exists()){
            qDebug() << "file: " << fileInfo.path() << " does not exist!";
            it.setForDelete();
        } else if(!fileInfo.isDir()){
            qDebug() << "file: " << fileInfo.path() << " it not a dir but marked as one!";
            it.setTagLevel(CatItem::ATOMIC_ELEMENT);
        } else {
            it.setCustomPluginValue(FILECAT_SEARCH_DEPTH,1);
            indexDirectory(extendedTypes, it, outItems);
        }
        outItems->append(it);
    }
}


QList<Directory> FilecatalogPlugin::getInitialDirs(){
    QList<Directory> memDirs;
    Q_CHECK_PTR(thePlatform);
    QSettings* set = *settings;

    while( set->group()!=""){ set->endGroup(); }
    int size = set->beginReadArray(DIRECTORIES_GROUP);
    for(int i = 0; i < size; ++i) {
        set->setArrayIndex(i);
        Directory tmp(set);
        memDirs.append(tmp);
    }
    set->endArray();

    if (memDirs.count() == 0) {
        memDirs = thePlatform->GetInitialDirs();
    }
    return memDirs;
}


//One thing to explore is looking deeper into any directory with a higher rating...
void FilecatalogPlugin::indexDirectory(QSet<QString> &extendedTypes, CatItem& parentItem, QList<CatItem>* output,UserEvent::LoadType lt)
{
    int depth=parentItem.getCustomValue(FILECAT_SEARCH_DEPTH);
    if(depth ==0){
        parentItem.setStub(true);
        return;
    }
    qDebug() << "indexDirectory depth: " << depth;
    QDir dirObj(parentItem.getPath());
    QString dirString = dirObj.canonicalPath();

    if(!dirObj.exists()){
        qDebug() << "warning: Directory -" << parentItem.getPath() << "- does not exist";
        parentItem.setForDelete();
        return;
    }
    QFileInfoList dirsInfo = dirObj.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                                                  QDir::Time | QDir::Reversed);
    time_t scanTime = parentItem.getChildScanTime();
    int maxPasses = parentItem.getMaxUpdatePasses(DEFAULT_SUBDIR_SCAN_PASSES);
    int passCount=0;
    QFileInfo fileInfo;
    QDateTime fileTime;
    fileTime.setTime_t(0);
    //For truly huge directories, this would suck but so would anything else...
    int i = 0;
    for (; i < dirsInfo.count() && passCount <maxPasses; ++i) {
        fileInfo = dirsInfo[i];
        Q_ASSERT(fileTime.isNull() || fileTime <= fileInfo.created());
        fileTime = fileInfo.lastModified();
        if(scanTime > (int) fileTime.toTime_t()) { continue;}
        passCount++;
        if(fileInfo.canonicalFilePath().length() <  dirString.length() )
            { continue; }
        if(fileInfo.canonicalFilePath() == dirString)
            { continue; }
        CatItem item = CatItem::createFileItem(extendedTypes, fileInfo, parentItem,depth,UserEvent::IGNORE,thePlatform,getPluginRep());
        Q_ASSERT(parentItem.getPath() != item.getPath());
        item.setCustomPluginInfo(FILE_DIRECTORY_PLUGIN_STR,parentItem.getName());
        item.setCustomValue(FILECAT_SEARCH_DEPTH,depth-1);
        if(fileInfo.isDir())
            { indexDirectory(extendedTypes, item, output,lt);}
        output->append(item);
    }
    if(i < dirsInfo.count()){
        parentItem.setChildScanTime(fileTime.toTime_t());
        parentItem.setUpdatingSourceWeight(MAX_EXTERNAL_WEIGHT,getPluginRep());
    } else { parentItem.setChildScanTime(0); }
    parentItem.setStub(false);
    parentItem.setSourceUpdateTime(appGlobalTime());
}

CatItem FilecatalogPlugin::createFileItem(QSet<QString> &extendedTypes, QFileInfo initialInfo,
                                          CatItem& folderParentItem, int depth,
                                          UserEvent::LoadType lt) {

    QFileInfo fileInfo = initialInfo;
    bool isLink = false;
    if(initialInfo.symLinkTarget().length()!=0) {
        QFileInfo lkInfo(initialInfo.symLinkTarget());
        fileInfo = lkInfo;
        isLink = true;
    }

    //We'll get all the mime stuff with a different call...
    QString shortName;
    QString fullPath;
    QString suffix = fileInfo.suffix();
    fullPath = fileInfo.absoluteFilePath();
    if(fileInfo.isDir()){
        shortName = fileInfo.baseName();
        if(shortName.isEmpty()){
            QStringList pathParts = fullPath.split(QDir::separator());
            QDir d = fileInfo.absoluteDir();
            shortName = d.dirName();
            if(shortName.isEmpty()){
                if(!pathParts.last().isEmpty()){
                    shortName = pathParts.last();
                } else if(pathParts.length() > 1){
                    shortName = pathParts[pathParts.length()-1];
                }
            }
        }
    } else if(suffix == ONELINE_FILE_EXTENSION){
        return parseOnelineFile(fileInfo);
    } else {
        shortName = fileInfo.fileName();
    }

    CatItem typeParent;
    if(!fileInfo.suffix().isEmpty()){
        typeParent = CatItem(
                        addPrefix(ACTION_TYPE_PREFIX,
                                  QString(fileInfo.suffix())) );
    } else {
        typeParent = getPluginRep();
    }

    CatItem it(fullPath, shortName,HASH_FILE_CATALOG);
    if (fullPath.endsWith("~") ||
        suffix.endsWith("~") ||
        fullPath.endsWith("_files") ||
        fileInfo.isHidden() || folderParentItem.getIsHidden()) {
        it.setHidden();
    }

    it.addCharacteristic(FILE_SIZE_CHARACTERISTIC_KEY, fileInfo.size());

    if(fileInfo.isDir()){
        it.setItemType(CatItem::LOCAL_DATA_FOLDER);
        if(isLink){
            it.setCustomPluginValue(REPLACE_CHILDREN_TEMP_KEY,1);
        }
    }

    it.setCreationTime(fileInfo.created().toTime_t());
    it.setModificationTime(fileInfo.lastModified().toTime_t());

    long guessedRawTime = MAX(fileInfo.lastModified().toTime_t(),
                        abs((long long)(fileInfo.lastRead().toTime_t() - (ONE_DAY_IN_SECONDS))));

    long guessedRelativeTime = MAX( 0, appGlobalTime() - guessedRawTime);

    qint32 guessedWeight = MAX(1,(MEDIUM_EXTERNAL_WEIGHT/(depth)) -log2(depth+guessedRelativeTime))
                           *MEDIUM_EXTERNAL_WEIGHT;

    //We might only do this only for recent additions...
    CatItem mimeParent = thePlatform->alterItem(&it);

    if(!mimeParent.isEmpty()){
        if(mimeParent.getPath() == addPrefix(ACTION_TYPE_PREFIX,
                                             QString(MIME_EXECUTABLE))){
            it.setItemType(CatItem::VERB);
        }
        if(mimeParent.getPath() == addPrefix(
                                    ACTION_TYPE_PREFIX,QString(MIME_SCRIPT))){
            it.setItemType(CatItem::VERB);
        }
        typeParent = mimeParent;
        QString typeString = typeParent.getMimeType();
        if(typeString.isEmpty()){
            typeString = typeParent.getName();
        }

        if(!extendedTypes.contains(typeString) || ((int)lt) > (int)UserEvent::BECOMING_VISIBLE){
            thePlatform->alterItem(&typeParent);
            extendedTypes.insert(typeString);
        } else {
            typeParent.setStub(true);
        }
        it.addParent(typeParent);
    }

    if (guessedRelativeTime < 2*ONE_DAY_IN_SECONDS){
        modifyItem(&it,UserEvent::BECOMING_VISIBLE);
    }

    it.setExternalWeight(guessedWeight,typeParent);
    it.setLabel(FILE_CATALOG_PLUGIN_STR);

    if(!folderParentItem.isEmpty()){
        it.setExternalWeight(guessedWeight,folderParentItem);
        it.setChildStubbed(false);
        folderParentItem.setChildStubbed(false);

        if((int)lt < (int)UserEvent::SEEN){
            CatItem parentCopy = folderParentItem;
            parentCopy.setStub();
            parentCopy.clearRelations();
            Q_ASSERT(parentCopy.getChildren().count()==0);
            //parentCopy.addChild(item);
            it.addParent(parentCopy,FILE_CATALOG_PLUGIN_STR);
            Q_ASSERT(it.hasParentType(FILE_CATALOG_PLUGIN_STR));
        } else {
            it.addParent(folderParentItem,FILE_CATALOG_PLUGIN_STR);

        }
    }
    return it;
}

//These items are always verbs
CatItem FilecatalogPlugin::parseOnelineFile(QFileInfo fileInfo){
    QString settingsPath = fileInfo.absoluteFilePath();
    QSettings s(settingsPath, QSettings::IniFormat);
    CatItem i(&s,getPluginRep());
    return i;
}

bool FilecatalogPlugin::modifyItem(CatItem* it , UserEvent::LoadType lt) {
    Q_ASSERT(it);
    if(lt ==UserEvent::JUST_FOUND){ return false; }

    bool handled = false;
    QSet<QString> extendedTypes;

    if(it->isStub() &&  it->getItemType() == CatItem::ACTION_TYPE){
        Q_ASSERT(thePlatform);
        CatItem action = thePlatform->alterItem(it);
        return !action.isEmpty();
    }

    if(it->hasLabel(FILE_CATALOG_PLUGIN_STR)){
        QFileInfo fileInfo(it->getPath());
        if(!fileInfo.exists()){
            it->setForDelete();
            return true;
        } else if(it->hasLabel(IS_STUB_KEY_STR) &&
                  !it->isStub()
                && (time_t)fileInfo.lastModified().toTime_t() <= it->getModificationTime()){
            return false;
        }

        CatItem par = thePlatform->alterItem(it);
        if(!par.isEmpty()){ handled = true; }


        if((fileInfo.isDir() && lt==UserEvent::SELECTED)){
            QString pn = fileInfo.dir().absolutePath();
            if(!pn.isEmpty()){
                QFileInfo parInfo(pn);
                if(parInfo.exists()){
                    CatItem pParent(parInfo.dir().absolutePath());
                    pParent.setLabel(FILE_CATALOG_PLUGIN_STR);
                    CatItem pItem = CatItem::createFileItem(extendedTypes, parInfo, pParent,1,UserEvent::IGNORE,thePlatform,getPluginRep());
                    it->addParent(pItem);
                }
            }
            it->setCustomPluginValue(FILECAT_SEARCH_DEPTH, 1);
            QList<CatItem> itemsAdded;
            indexDirectory(extendedTypes,*it, &itemsAdded,lt);
            Q_ASSERT(!it->isStub());
            return true;
        }
    }
    return handled;
}


//This is only "actions"
//Tagging is done by the tag plugin-
void FilecatalogPlugin::launchItem(InputList* il, QList<CatItem>* output){

    CatItem newUserItem = il->userItem();
    if(!newUserItem.isEmpty()){
        QString fileName = newUserItem.getCustomString(ITEM_GEN_FILE_KEY);
        if(fileName.isEmpty()){
            QSettings set(fileName, QSettings::IniFormat);
            //newUserItem.toIni(set);
            set.sync();
        }
        output->append(newUserItem);
    }

    QString commandLine = il->getCommandLine();
    if(!commandLine.isEmpty()){
        runProgram(commandLine);
    }

}

void FilecatalogPlugin::doDialog(QWidget* , QWidget** ) {
//	if (gui != NULL) return;
//	gui = new Gui(parent);
//	*newDlg = gui;
}

void FilecatalogPlugin::endDialog(bool ) {
//	if (accept) {
//		gui->writeOptions();
//		init();
//	}
//	if (gui != NULL)
//		delete gui;
//
//	gui = NULL;
}

void FilecatalogPlugin::setPath(QString * path) {
	libPath = *path;
}

int FilecatalogPlugin::msg(int msgId, void* wParam, void* lParam)
{
	bool handled = false;
	switch (msgId)
	{		
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
	case MSG_GET_RESULTS:
                getResults((InputList*) wParam, (QList<CatItem>*) lParam);
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
        case MSG_ITEM_LOADING:
                handled = modifyItem((CatItem* )wParam, (UserEvent::LoadType)(int)lParam);
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


Q_EXPORT_PLUGIN2(fileCatalog, FilecatalogPlugin);
