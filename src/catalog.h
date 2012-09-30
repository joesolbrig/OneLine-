#ifndef CATALOG_H
#define CATALOG_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "input_list.h"
#include "item.h"
#include "plugin_handler.h"

#include "cat_store.h"

#define GET_SOURCE_LIMIT 10000 //millisecs,

class CatItem;
class PluginHandler;
class InputList;
//class cat_store;

class Catalog {
protected:
    int filteredDepth;
    QHash<QString, QList<QString> > m_dirs;
    QSet<QString> m_pluginFilter;
public:
    Cat_Store cat_store; // public for testing only

    static PluginHandler plugins;
    static QString curFilterStr;
    Catalog(QHash<QString, QList<QString> > dirs, QSet<QString> pluginFilter=QSet<QString>());
    virtual ~Catalog();
    PluginHandler* plug_ptr(){return &plugins;}
    virtual void addItem(CatItem item);
    virtual void clearAll() { cat_store.clearAll();}

    //void addChildren(CatItem& item);
    //void addParents(CatItem& item);

    QMutex* getMutext(){ return cat_store.getMutext();}
    void loadPlugins();

    CatItem getItem(QString itemPath,  int childDepth=1){
        return cat_store.getItemByPath(itemPath, childDepth);
    }
    QList<CatItem> getItemsForSave(){
        return cat_store.getAllItemForSave();
    }

    QList<CatItem> indexItemList(QList<CatItem>* catListPtr=NULL);
    QList<CatItem> getMainApps(){ return cat_store.getMainApps();}
    void searchCatalogs(QString userStr, QList<CatItem> & , int count);
    CatItem setExecuted(InputList* inputList);
    void setShown(CatItem id, UserEvent etv=UserEvent::SHOWN_LIST);
    void checkHistory(QString txt, QList<CatItem> & list);
    static void showTheApp(){ plugins.showTheApp(); }
    static void hideTheApp(){ plugins.hideTheApp(); }
    void saveCatalogs(QList<CatItem>* items){ plugins.saveCatalogs(items); }
    void loadCatalogs(QList<CatItem>* items){
        plugins.loadCatalogs(items);
    }
    QList<CatItem> extractStubs(CatItem& it);
    QList<CatItem> expandStubs(QString userKeys, QList<CatItem>* inList);
    bool getItemsFromQuery(InputList &inputData, QList<CatItem>& outResults,
                           int itemsDesired, int* beginPos=0);
    QList<CatItem> parseRequest(InputList& inputData, int itemsDesired, int* beginPos);
    QList<CatItem> parseCustomVerbSelection(InputList & inputData);
    QList<CatItem> getOperations(InputList & inputData);
    QList<CatItem> getOperationChildren(InputList & inputData);

    void getMiniIcons(InputList & inputData);

    //Global sources take items as arguments...
    QList<CatItem> getGlobalSources(){
        return cat_store.getItemChildren(0, CatItem(SOURCE_W_ARG_PATH));
    }

    void addSynonymChildren(CatItem& toUpdate){
        return cat_store.addSynonymChildren(toUpdate);
    }

    QList<CatItem> getSourcesForExtension(){
        QList<CatItem> l = cat_store.getSourcesToExtend();
        cat_store.reweightItems(&l);
        return l;
    }
    void reweightItems(){
        cat_store.reweightItems();
    }

    void reweightItem(CatItem it){
        cat_store.reweightItem(it);
    }

    bool promoteChildItems(QList<CatItem>& results, InputList inputList);
    void scanChildren(QHash<qint32, CatItem>& scanH, QHash<qint32, CatItem>& outResults,
                           CatItem item, float lowestMatch, int recur=0);
    //static bool filter(CatItem& item, QString userStr, QList<int>* outIndex=0, int char_skips = 2);

    CatItem* filterByChar(CatItem &item, QChar c);
    bool GetFilteredResults(QChar c, QList<CatItem>* searchResults, int count);

    QString userString;
    QChar lastUserChar;
    int count() { return cat_store.itemCount();}
    //const CatItem & getItem(int i) { return itemHash[i]; }


private:
    //virtual QList<CatItem*> search(QString);
};






#endif
