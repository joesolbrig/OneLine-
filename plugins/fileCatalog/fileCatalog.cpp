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
    QString endGroup = set->group();
    while(!endGroup.isEmpty()){
        set->endGroup();
        endGroup = set->group();
    }
    set->beginGroup(FILE_SETTINGS_KEY);
    bool getMenuItems = set->value(GET_MENU_ITEMS_KEY, true).toBool();
    if(getMenuItems){
        QList<CatItem> menuItems = thePlatform->getSubdesktopItems();
        items->append(menuItems);
    }
    bool addRecentItem = set->value(ADD_RECENT_ITEMS_SOURCE_KEY, true).toBool();
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
            qDebug() << "error base file: " << memDirs[i].name << " does not exist!";
            continue;
        }
        CatItem dirParent(fileInfo.absoluteFilePath());
        int depth = MIN(memDirs[i].depth, MAX_DIR_SEARCH_DEPTH);
        dirParent = CatItem::createFileItem(extendedTypes, fileInfo, dummy, depth,UserEvent::IGNORE,thePlatform,getPluginRep());
        dirParent.setCustomPluginInfo(FILE_DIRECTORY_PLUGIN_STR,memDirs[i].toString());
        indexDirectory(depth, extendedTypes, dirParent, items);
        if(memDirs[i].baseWeight>0){
            dirParent.setUpdatingSourceWeight(memDirs[i].baseWeight,getPluginRep());
        }

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
            indexDirectory(1, extendedTypes, it, outItems);
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
void FilecatalogPlugin::indexDirectory(int depth, QSet<QString> &extendedTypes,
    CatItem& parentItem, QList<CatItem>* output,UserEvent::LoadType lt,
    bool addChild, int overrideMaxPasses)
{
    //int depth=parentItem.getCustomValue(FILECAT_SEARCH_DEPTH);
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
    if(overrideMaxPasses>-1){
        maxPasses = MIN(maxPasses, overrideMaxPasses);
    }
    int passCount=0;
    QFileInfo fileInfo;
    QDateTime fileTime;
    fileTime.setTime_t(0);
    //For truly huge directories, this would suck but so would anything else...
    int i = 0;
    QList<CatItem> children;
    for (; i < dirsInfo.count() && passCount <maxPasses; ++i) {
        fileInfo = dirsInfo[i];
        //Ordering by directory is not quite strict but we'll just live with skipped files given this error
        //Q_ASSERT(fileTime.isNull() || fileTime <= fileInfo.created());
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
        if(fileInfo.isDir())
            { indexDirectory(depth-1, extendedTypes, item, output,lt);}
        output->append(item);
        if(addChild){
            children.append(item);
            qDebug() << "trying to add: " << item.getPath();
        }
    }
    parentItem.addChildrenBulk(children);
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
    qDebug() << "FilecatalogPlugin::modifyItem" << it->getPath() << " : " << lt;
    Q_ASSERT(it);
    time_t itemModTime = (it->getModificationTime());

    if(lt == UserEvent::JUST_FOUND){
        return false;
    }

    bool handled = false;
    QSet<QString> extendedTypes;

    if(it->isStub() &&  it->getItemType() == CatItem::ACTION_TYPE){
        Q_ASSERT(thePlatform);
        CatItem action = thePlatform->alterItem(it);
        return !action.isEmpty();
    }

    if(it->hasLabel(FILE_CATALOG_PLUGIN_STR)){
        QFileInfo fileInfo(it->getPath());
        time_t fileModTime = (time_t)fileInfo.lastModified().toTime_t();
        fileModTime = MAX(fileModTime,0);

        if(!fileInfo.exists()){
            it->setForDelete();
            qDebug() << "FilecatalogPlugin::modifyItem exiting 'cause file doesn't exist";
            return true;
        } else  if(it->hasLabel(IS_STUB_KEY_STR)
                && !it->isStub()
                && (fileModTime <= (itemModTime) )){
            //if((int)lt< (int)UserEvent::SELECTED ){ }
            qDebug() << "FilecatalogPlugin::modifyItem exit 'cause item should be up-to-date" << " : " << lt;
            qDebug() << "test: " << UserEvent::SELECTED;
            return false;
        }
        it->setLabel(IS_STUB_KEY_STR);
        it->setStub(false);
        it->setModificationTime(fileModTime);
        itemModTime = (it->getModificationTime());
        Q_ASSERT(it->hasLabel(IS_STUB_KEY_STR) &&
                 !it->isStub()
               && ( fileModTime <= (itemModTime)));

        CatItem par = thePlatform->alterItem(it);
        if(!par.isEmpty()){ handled = true; }


        if(fileInfo.isDir()){ //&& lt==UserEvent::SELECTED)
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
            QList<CatItem> itemsAdded;
            indexDirectory(1, extendedTypes,*it,
                &itemsAdded, lt, true, DEFAULT_SUBDIR_SCAN_LIMITED_PASSES);
            Q_ASSERT(!it->isStub());
            qDebug() << "FilecatalogPlugin::modifyItem got: " << it->getChildCount() << "children";
            return true;
        }
    }
    return true;
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
