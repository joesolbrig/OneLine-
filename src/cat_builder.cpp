#include "globals.h"
#include "main.h"
#include "options.h"

#include <QFile>
#include <QDataStream>
#include <qglobal.h>

#include "cat_builder.h"

#include "util/mmap_file/btree_impl.h"

Catalog* CatBuilder::cat=0;
CatBuilder::CatalogRefreshMode CatBuilder::refreshMode = REFRESH;

CatBuilder::CatBuilder(CatalogRefreshMode mode, QHash<QString, QList<QString> > dirs, bool forceDropCatalog)
{
    refreshMode = mode;
    m_extensionType = UserEvent::STANDARD_UPDATE;
    m_extendDefaultSources=true;
    if(forceDropCatalog || !cat){
        createCatalog(dirs);
    }
}

void CatBuilder::clear(CatalogRefreshMode mode, QHash<QString, QList<QString> > dirs, bool forceDropCatalog)
{
    refreshMode = mode;
    m_extensionType = UserEvent::STANDARD_UPDATE;
    m_extendDefaultSources=true;
    if(forceDropCatalog || !cat){
        createCatalog(dirs);
    }
}

QList<CatItem> CatBuilder::loadExtensionSourceForPlugins(){
    QList<CatItem > toExtend = cat->getGlobalSources();
    for(int i=0; i< toExtend.count();i++){
        CatItem it = toExtend[i];
        plugins_ptr->itemLoading(&it, UserEvent::STANDARD_UPDATE);
    }
    return toExtend;
}


CatItem CatBuilder::getItem(QString itemPath, int childDepth){
    return cat->getItem(itemPath, childDepth);
}

void CatBuilder::addItem(CatItem item){
    cat->addItem(item);
}

CatItem CatBuilder::updateItem(CatItem& it, int childDepth, UserEvent::LoadType lt){
    if(!it.shouldUpdate(childDepth, lt)){ return it;}


    CatItem realItem;
    QList<int> matches = it.getMatchIndex();

    if(!it.getIsTempItem()){
        QString path = it.getPath();
        realItem = getItem(path, childDepth);
        addSynonymChildren(realItem);

        //Our plugins can return items not in the database
        if(!realItem.isEmpty()){
            //realItem.merge(it);
            it.merge(realItem);
        }
        if(lt!=UserEvent::IGNORE){
            for(int i=0; i<childDepth;i++){
                //avoiding address of reference of smart-pointer itme
                CatItem holder = it;
                if(plugins_ptr->itemLoading(&holder,lt)){
                    cat->addItem(holder);
                    it = holder;
                }
            }
        }
    }
    it.setMatchIndex(matches);
    return it;
}

QList<CatItem> CatBuilder::indexItemList(QList<CatItem>* items){
    return cat->indexItemList(items);
}

long CatBuilder::totalItems(){
    return cat->count();
}

void CatBuilder::setShown(CatItem it, UserEvent etv){
    it.setBestMatchType(it.getMatchType());
    cat->setShown(it, etv);
}

void CatBuilder::getPluginURL(CustomFieldInfo* cfi){
    plugins_ptr->getCustomFieldInfo(cfi);
}


void CatBuilder::itemLoading(CatItem* it, UserEvent::LoadType lt){
    bool changed = plugins_ptr->itemLoading(it, lt);
    if(changed){
        cat->addItem(*it);
    }
}


Catalog* CatBuilder::createCatalog(QHash<QString, QList<QString> > dirs)
{
    if(cat){
        delete cat;
        cat = 0;
    }
    cat = new Catalog(dirs);
    cat->loadPlugins();
    if(!plugins_ptr){
        plugins_ptr = cat->plug_ptr();
    }
    return cat;
}

Catalog* CatBuilder::getOrCreateCatalog(QHash<QString, QList<QString> > dirs)
{
    if(cat){
        return cat;
    }

    cat = (Catalog*) new Catalog(dirs);
    cat->loadPlugins();
    if(!plugins_ptr){
        plugins_ptr = cat->plug_ptr();
    }
    return cat;
}

Catalog* CatBuilder::getCatalog()
{
    Q_ASSERT(cat);
    return cat;
}

void CatBuilder::destroyCatalog()
{
    if(cat){
        delete cat;
        cat = 0;
    }
}


//All the hard stuff should be done in the plugins...
void CatBuilder::run() {

    catalogTask(refreshMode);
    qDebug() << "CatBuilder task done: " << (int)this;

    if(refreshMode == CatBuilder::SEARCH){
        emit backgroundSearchDone(m_userKeys);
    } else {
        emit catalogFinished();
    }
}

void CatBuilder::catalogTask(CatalogRefreshMode refreshState) {
    Q_ASSERT(cat);
    Q_ASSERT(plugins_ptr);
    QList<CatItem> insertList;
    bool success=false;
    SearchInfo inf;
    inf.m_extensionType = m_extensionType;
    switch (refreshState){
        case LOAD:
            success = plugins_ptr->loadCatalogs(&(insertList));
            if(!success) {
                plugins_ptr->getCatalogs(&(insertList));
            }
            break;
        case REFRESH: plugins_ptr->getCatalogs(&(insertList));
            break;
        case EXTEND: doExtension(&insertList);
            break;
        case SEARCH:
            {
                inf.m_userKeys = this->m_userKeys;
                inf.m_keyWords = this->m_keywords;
                inf.itemListPtr = &(m_userItems);
                plugins_ptr->backgroundSearch(&inf, &(insertList));
            }
            break;
        case SAVE:
            //Eventually we'll just save additions...
//            QList<CatItem> toSave = cat->getItemsForSave();
//            plugins_ptr->saveCatalogs(&toSave);
            break;

    }
    QList<CatItem> mergedItems = cat->indexItemList(&insertList);
    m_extension_results.clear();
    for(int i=0;i<mergedItems.count();i++){
        CatItem it = mergedItems[i];
        if(it.getIsTimeDependant())
            { m_timelyItems.append(mergedItems[i]); }
        if(it.getMatchType() == CatItem::USER_KEYWORD){
            setShown(it, UserEvent::KEYS_TYPE);
            continue;
        }
        m_extension_results.append(it);

        //plugins_ptr->itemLoading(&it,UserEvent::JUST_FOUND);
    }
    QList<CatItem> res;
    inf.itemListPtr = &mergedItems;
    qDebug() << "give plugins another go at found items";
    plugins_ptr->itemsLoading(&inf,&res);
    res = cat->indexItemList(&res);
    mergedItems.append(res);
    m_extension_results = mergedItems;
}

//This let each plugin ask for background thread to
//to use to get items related to the current thread.
void CatBuilder::doExtension(QList<CatItem>* outList, QList<CatItem>* inList){
    //Send a message to any old threads to exit
    //They should exit a reasonable time later
    plugins_ptr->stopExtending();
    QList<CatItem> toExtend;
    if(m_extendDefaultSources){
        toExtend = cat->getSourcesForExtension();
        m_userItems.append(toExtend);
    }


    SearchInfo inf;
    if(inList){
        m_userItems.append(*inList);
    }
    for(int i=0; i< m_userItems.count(); i++){
        if(m_userItems[i].hasLabel(EXTENSION_COMMAND_KEY)){
            QString updateCommand = m_userItems[i].getCustomString(EXTENSION_COMMAND_KEY);
            updateCommand = updateCommand.simplified();
            QProcess proc;
            proc.startDetached(updateCommand);
        }
    }

    inf.m_userKeys = m_userKeys;
    inf.m_extensionType = m_extensionType;
    inf.m_keyWords = this->m_keywords;
    inf.itemListPtr = &m_userItems;
    inf.lastUpdate = gSettings->value("GenOps/lastUpdateTime", 0).toLongLong();
    if(!m_extendDefaultSources){
        plugins_ptr->extendCatalogs(&inf, outList);
    } else {
        plugins_ptr->doACatalogueExtension(&inf, outList);
    }
    gSettings->setValue("GenOps/lastUpdateTime", (long long)appGlobalTime());
    m_userItems.clear();
}



void CatBuilder::storeCatalog() {
    Q_CHECK_PTR(cat);
    QList<CatItem> saveList = cat->getItemsForSave();
    plugins_ptr->saveCatalogs(&(saveList));
}

int CatBuilder::launchObject(InputList& inputList, QList<CatItem> &outputList, QString* errString) {
    //up date catalogue with changes made to the existing item...

    CatItem baseItem = inputList.currentItem();
    if(!baseItem.isEmpty()){ cat->addItem(baseItem);}

    //cat->setExecuted(inputList);

    Q_ASSERT(plugins_ptr);
    plugins_ptr->preExecute(&inputList);
    QList<CatItem> output;
    int result = subexecute(inputList, output,errString);
    Q_ASSERT(cat);
    QList<CatItem> ci;
    ci.append( cat->setExecuted(&inputList));
    plugins_ptr->userItemsCreated(&ci);
    for(int i=0;i<output.count();i++){
        outputList.append(output[i]);
    }
    plugins_ptr->userItemsCreated(&ci);
    return result;
}

//This allow one plugin to call another by returning it's items...
int CatBuilder::subexecute(InputList& inputList, QList<CatItem> &outputList,
                           QString* errString,int /*recur*/) {

    QList<CatItem> output;
    int result = plugins_ptr->execute(&inputList, &output, errString);
    if(output.count() > 0) {
        for(int i=0; i <output.count();i++){
            outputList.append(output[i]);
            cat->addItem(output[i]);
        }
    }

    //QList<CatItem> reexecuteList = output;
    //if(result==MSG_EXECUTE_RESULT && recur < 20){
    //  for(int i=0; i <reexecuteList.count(); i++){
    //      InputList il;
    //      il.fromString(reexecuteList[i].getPath());
    //      result = MAX(result, subexecute(il, outputList, recur+1));
    //  }
    //}
    return result;
}
