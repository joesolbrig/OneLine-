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

#include "plugin_handler.h"

#include <QLibrary>
#include <QDir>
#include <QDebug>
#include "globals.h"
#include "main.h"
#include "is_test.h"

#include "fillInItem.h"

PluginHandler::PluginHandler() {

}


PluginHandler::~PluginHandler() {

}

QList<CatItem> PluginHandler::loadPlugins(QHash<QString, QList<QString> > dirs, QSet<QString> ) {
    //Lets us call this multiple times for different test cases...
    plugins.clear();

    // Get the list of plugins to load
    //QHash<QString,ExtendConfigInfo> ecInfo;

    int size = gSettings->beginReadArray(PLUGIN_SETTINGS_ARRAY_KEY);
    for(int i = 0; i < size; ++i) {
        gSettings->setArrayIndex(i);
        QString name = gSettings->value(PLUGIN_SETTINGS_NAME_KEY).toString();
        PluginInfo& info = plugins[name];
        info.name = name;

        bool toLoad = gSettings->value(PLUGIN_SETTINGS_LOAD_KEY, true).toBool();
        info.loaded = toLoad;

        info.baseLoadInterval = gSettings->value(PLUGIN_SETTING_LOAD_INTERVAL_KEY
                ,PLUGIN_SETTING_DEFAULT_LOAD_INTERVAL).toInt();
        info.workingloadInterval = info.baseLoadInterval;
        info.lastLoadTime=0;

        info.baseExtendInterval = gSettings->value(PLUGIN_SETTING_EXTEND_INTERVAL_KEY
                ,PLUGIN_SETTING_DEFAULT_EXTEND_INTERVAL).toInt();
        info.workingExtendInterval = info.baseExtendInterval;
        info.lastExtendTime =0;
    }
    gSettings->endArray();


    // Load up the plugins in the plugins/ directory
    gSettings->beginWriteArray(PLUGIN_SETTINGS_ARRAY_KEY);
    int arrayCounter =0;

    foreach(QString szDir, dirs["plugins"]) {
        QDir pluginsDir(szDir);
        foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
            gSettings->setArrayIndex(arrayCounter);
            gSettings->setValue(PLUGIN_SETTINGS_NAME_KEY,fileName);
            if(!pluginsDir.exists(fileName)){
                plugins[fileName].loaded =false;
            }
            if (plugins.contains(fileName)
                    && plugins[fileName].loaded) {
                qDebug() << "loading plugin: " << fileName;
                LoadPluginFromFile(pluginsDir, fileName);
                gSettings->setValue(PLUGIN_SETTINGS_LOAD_KEY,true);
            } else {
                gSettings->setValue(PLUGIN_SETTINGS_LOAD_KEY,false);
            }
            arrayCounter++;
        }
    }
    gSettings->endArray();
    gSettings->sync();



    QList<CatItem> basePluginItems;
    CatItem absoluteBase(BASE_SRCOBJECT_PATH);
    QString itPath;
    QHash<QString, PluginInfo>::iterator i =  plugins.begin();
    for(; i!= plugins.end();i++){
        if(!i.value().loaded){continue; }
        QString pName = i.value().name;
        if(pName.isEmpty()){ continue;}
        Q_ASSERT(itPath != addPrefix(PLUGIN_PATH_PREFIX,pName));
        itPath = addPrefix(PLUGIN_PATH_PREFIX,pName);
        qDebug() << "plugin base item path: " << itPath;
        CatItem plugItem(itPath);
        plugItem.setTagLevel(CatItem::BASE_PLUGIN);
        plugItem.setPluginId(SYSTEM_ITEM_ID);
        plugItem.setSourceWeight(DEFAULT_SOURCE_WEIGHT,absoluteBase);
        plugItem.setItemType(CatItem::PLUGIN_TYPE);
        plugItem.setExternalWeight(1, absoluteBase);//Minimum weight
        basePluginItems.append(plugItem);

    }
    return basePluginItems;

}

void PluginHandler::LoadPluginFromFile(QDir pluginsDir,QString fileName) {
    bool handled=false;
    uint id=0;
    if(!QLibrary::isLibrary(fileName)){
        qDebug() << "error file is not library" << fileName;
        return;
    }

    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    //loader.setLoadHints(QLibrary::ResolveAllSymbolsHint);
    bool success = loader.load();
    if(!success){
        qDebug() << "error loading: "
        << pluginsDir.absoluteFilePath(fileName) << " er str: " << loader.errorString();
        return;
    }

    QObject *plugin = loader.instance();
    if(!plugin){
        qDebug() << "error creating"
                << pluginsDir.absoluteFilePath(fileName) << " er str: " << loader.errorString();
        return;
    }

    PluginInterface *plug = qobject_cast<PluginInterface *>(plugin);

    //If the previous cast fails it is probably a build/version problem
    Q_ASSERT(plug);

    //We gotta give each plugin a pointer to the global objects because of the
    //limits of shared libraries...
    plug->settings = &gSettings;
    plug->gMutexPtr = CatBuilder::getMutext();
    {
        QMutexLocker locker(plug->gMutexPtr);
        QString appDirStr = USER_APP_DIR;
        qDebug() << "adding dir str:" << appDirStr << "to: " << fileName;
        plug->addHomeDir(appDirStr);
    }
    plug->thePlatform = gPlatform;
    //Q_CHECK_PTR(CatBuilder::cat);
    //plug->setCatalogIndex(&CatBuilder::cat->itemHash);
    PluginInfo& info = plugins[fileName];

    handled = plug->msg(MSG_GET_ID, (void*) &id);
    info.id = id;
    QString name;
    plug->msg(MSG_GET_NAME, (void*) &name);
    info.name = fileName;
    info.internalName = name;
    info.obj = plug;
    info.path = pluginsDir.absoluteFilePath(fileName);

    //The keys should yield varying values 'cause we're are on a different section

    if (handled) {
        info.loaded = true;
        plug->msg(MSG_INIT);
        QString cdir = pluginsDir.canonicalPath();
        plug->msg(MSG_PATH, &cdir);

        // Load any of the plugin's plugins of its own
        QList<PluginInfo> additionalPlugins;
        plug->msg(MSG_LOAD_PLUGINS, &additionalPlugins);

    } else {
        info.loaded = false;
        loader.unload();
    }

}

void PluginHandler::showTheApp()
{
    foreach(PluginInfo info, plugins) {
        if (info.loaded)
                info.obj->msg(MSG_APPLICATION_SHOWING);
    }
}

void PluginHandler::hideTheApp()
{
    foreach(PluginInfo info, plugins) {
        if (info.loaded)
            info.obj->msg(MSG_APPLICATION_HIDING);
    }
}

void PluginHandler::getLabels(InputList* id)
{
    Q_ASSERT(id->slotCount()>0);
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            Q_ASSERT(info.obj);
            info.obj->msg(MSG_GET_LABELS, (void*) id);
        }
    }
}

void PluginHandler::getResults(InputList* id, QList<CatItem>* results)
{
    Q_ASSERT(id->slotCount()>0);
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            info.obj->msg(MSG_GET_RESULTS, (void*) id, (void*) results);
            SetPluginForChildren(info.name, results);
        }
    }
}


bool PluginHandler::loadCatalogs(QList<CatItem>* outItems) {
    bool isLoaded = true;
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            if(!info.obj->msg(MSG_LOAD_ITEMS, (void*) outItems)){
                isLoaded = false;
            }
        }
        //Need to add connection of children to each plugin...
    }
    return isLoaded;
}
void PluginHandler::saveCatalogs(QList<CatItem>* saveItems) {
    foreach(PluginInfo info, plugins) {
        if (info.loaded)
            info.obj->msg(MSG_SAVE_ITEMS, (void*) saveItems);
    }
}

void PluginHandler::getCatalogs(QList<CatItem>* outItems) {
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            info.obj->msg(MSG_GET_CATALOG, (void*) outItems);
            //This might need optimizing, we'll see...
            SetPluginForChildren(info.name, outItems);
            info.lastLoadTime = appGlobalTime();
        }
    }
}

void PluginHandler::getSourceIds(qint32 id, QSet<qint32>* sourceIdList){
    foreach(PluginInfo info, plugins) {
        if (info.loaded)
            info.obj->msg(MSG_GET_SOURCE_ID, (void*) id, (void*)sourceIdList);
    }
}


void PluginHandler::extendCatalogs(SearchInfo* search_info, QList<CatItem>* outItems) {
    Q_ASSERT(search_info);
    Q_ASSERT(outItems);
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
                info.obj->msg(MSG_EXTEND_CATALOG, (void*) search_info, (void*)outItems);
                SetPluginForChildren(info.name, outItems);
        }
    }
}

void PluginHandler::doACatalogueExtension(SearchInfo* search_info, QList<CatItem>* outItems) {
    Q_ASSERT(search_info);
    Q_ASSERT(outItems);

    QString toExtend;
    int earliestTimeOfItemExtension = 0;
    int now = appGlobalTime();
    foreach(PluginInfo info, plugins) {
        if (!info.loaded){continue;}
        if(earliestTimeOfItemExtension ==0 || info.lastExtendTime < earliestTimeOfItemExtension){
            if(info.lastExtendTime + info.workingExtendInterval < now ){
                earliestTimeOfItemExtension = info.lastExtendTime;
                toExtend = info.name;

                PluginInfo& plugin = plugins[toExtend];
                plugin.obj->msg(MSG_EXTEND_CATALOG, (void*) search_info, (void*)outItems);
                SetPluginForChildren(plugin.name, outItems);
                int dur =  now - plugin.lastExtendTime;
                plugin.workingExtendInterval = adjustForSpeed(
                      plugin.baseExtendInterval,
                      plugin.workingExtendInterval,
                      dur);
                plugin.lastExtendTime = now;
            }
        }

    }
//    if(!toExtend.isEmpty()){
//    }

}

void PluginHandler::doTestCatalogueExtension(QString pluginName, SearchInfo* search_info, QList<CatItem>* outItems) {
    Q_ASSERT(search_info);
    Q_ASSERT(outItems);
    Q_ASSERT(plugins.contains(pluginName));
    PluginInfo& plugin = plugins[pluginName];
    Q_ASSERT(plugin.obj);
    Q_ASSERT(plugin.loaded);
    plugin.obj->msg(MSG_EXTEND_CATALOG, (void*) search_info, (void*)outItems);

}

void PluginHandler::backgroundSearch(SearchInfo* search_info, QList<CatItem>* outItems) {
    Q_ASSERT(search_info);
    Q_ASSERT(outItems);
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            info.obj->msg(MSG_BACKGROUND_SEARCH, (void*) search_info, (void*)outItems);
            SetPluginForChildren(info.name, outItems);
        }
    }
}

void PluginHandler::stopExtending(SearchInfo* items) {
    foreach(PluginInfo info, plugins) {
        if (info.loaded)
            info.obj->msg(MSG_STOP_EXTENDING_CATALOG, (void*) items, 0);
    }
}

bool PluginHandler::itemLoading(CatItem* item, UserEvent::LoadType lt) {
    bool modified= false;
    foreach(PluginInfo info, plugins) {
        if (info.loaded) {
            modified = (info.obj->msg(MSG_ITEM_LOADING, (void*) item, (void*) lt));
        }
    }
    return modified;
}

void PluginHandler::itemsLoading(SearchInfo* items , QList<CatItem>* res) {
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            (info.obj->msg(MSG_ITEMS_LOADING, (void*) items, (void*) res));
        }
    }
}

void PluginHandler::getCustomFieldInfo(CustomFieldInfo* cfi) {
    Q_ASSERT(cfi);

    foreach(PluginInfo info, plugins) { //and info.name == cfi->pluginSource
        if (info.loaded )
            info.obj->msg(MSG_CUSTOM_FIELD_EDITING, (void*) cfi, (void*) 0);
    }
}


void PluginHandler::preExecute(InputList* id) {
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            info.obj->msg(MSG_ITEM_LAUNCHED, (void*)id);
        }
    }
    return;
}

void PluginHandler::saveCreatedItem(CatItem item) {
    QString fileName = item.getName() + QString::number(item.getItemId());
    fileName += QString(".") + ONELINE_FILE_EXTENSION;
    QString filePath =  USER_APP_DIR + GENERATED_ITEM_DIR + fileName;
    QSettings set(filePath,QSettings::IniFormat);
    item.saveToSettings(&set);
    set.sync();
}


int PluginHandler::execute(InputList* inputInfo, QList<CatItem>* results,
                           QString* errString ) {

    CatItem actionItem = inputInfo->getOperationPart();
    if(!actionItem.isEmpty() ){
        actionItem = CatBuilder::updateItem(actionItem);
        inputInfo->refreshOperation(actionItem);
        CatItem toTag = inputInfo->getStatementObject();
        CatItem tag =inputInfo->getOperationTarget();
        if(actionItem.getPath()==addPrefix(OPERATION_PREFIX,TAG_OPERATION_NAME)){
            if(tag.isEmpty()){
                QString tagName = inputInfo->getUserKeys();
                Q_ASSERT(!tagName.isEmpty());
                tag = CatItem::createKeywordItem(tagName);

            }
            toTag.setStub();
            toTag.clearRelations();
            tag.addChild(toTag,TAG_LABEL,BaseChildRelation::TAG_PARENT);
            CatBuilder::addItem(tag);
            Q_ASSERT(tag.canNavigateTo());
            return MSG_NO_ACTION;
        }
        if(actionItem.getPath()==OP_REMOVE_PATH){
            Q_ASSERT(!tag.isEmpty());
            tag.setChildForRemove(toTag);
            CatBuilder::addItem(tag);
            return MSG_NO_ACTION;
        }
    }

    actionItem = inputInfo->getActionPart();
    if(!actionItem.getFillinGenerator().isEmpty()){
        CatItem resultItem = FillinItem::createChildren(actionItem,inputInfo->asFieldValueHash(false));
        resultItem.addParent(actionItem, QString(""), BaseChildRelation::ACCOUNT_SOURCE_PARENT);
        saveCreatedItem(resultItem);
        QList<CatItem> childs = resultItem.getChildren();
        for(int i=0;i<childs.length();i++){
            saveCreatedItem(childs[i]);
        }
        results->append(resultItem);
        return MSG_TAKE_OUTPUT;
    }

    if(actionItem.getIsSourceRefresh()){
        SearchInfo inf;
        inf.m_extensionType = UserEvent::SEEN;
        QList<CatItem> outList;
        QList<CatItem> inList = inputInfo->nounList();
        if(inList.count()>0){

            //inList.append(actionItem);
            inf.itemListPtr = &inList;
            extendCatalogs(&inf, &outList);
            for(int i=0; i<outList.count(); i++){
                CatBuilder::addItem(outList[i]);
            }
        }
        if(outList.count()>0){
            CatItem sourceParent = inList[0];
            sourceParent.addChildren(outList);
            inputInfo->pushChildren(sourceParent);


        }



        return MSG_TAKE_OUTPUT;
    }

    int id = actionItem.getPluginId();

    QString name;
    foreach(PluginInfo info, plugins) {
        if (info.id ==id){ name = info.name;}}

    if (name.isEmpty()) {
        (*errString) = PLUGIN_NOT_FOUND_ERR;
        return MSG_SHOW_ERR;
    }
    if (!plugins[name].loaded) {
        (*errString) = PLUGIN_NOT_LOADED_ERR;
        return MSG_SHOW_ERR;
    }
    return plugins[name].obj->msg(
            MSG_LAUNCH_ITEM, (void*) inputInfo, (void*) results);
}



//Anything the user creates should be saved immediately and seperately
//since it isn't always reconstitued by a getCatalog call
void PluginHandler::userItemsCreated(QList<CatItem>* inList) {
    foreach(PluginInfo info, plugins) {
        if (info.loaded){
            info.obj->msg(MSG_USER_CREATED_ITEM, (void*)inList);
        }
    }
    return;
}

QWidget* PluginHandler::doDialog(QWidget* parent, uint id) {
    QString name;
    foreach(PluginInfo info, plugins) {
        if (info.id ==(int)id){ name = info.name;}}

    if (!plugins.contains(name)) return NULL;
    if (!plugins[name].loaded) return NULL;
    QWidget* newBox = NULL;
    plugins[name].obj->msg(MSG_DO_DIALOG, (void*) parent, (void*) &newBox);
    return newBox;
}

void PluginHandler::endDialog(uint id, bool accept) {
    QString name;
    foreach(PluginInfo info, plugins) {
        if (info.id ==(int)id){ name = info.name;}}

    if (!plugins.contains(name)) return;
    if (!plugins[name].loaded) return;
    plugins[name].obj->msg(MSG_END_DIALOG, (void*) accept);
}

void PluginHandler::SetPluginForChildren(QString pluginName, QList<CatItem>* items){
    Q_ASSERT(items);
    for(int i=0;i<items->count();i++){
        (*items)[i].setPluginSourceForChildren(pluginName);
    }

}
