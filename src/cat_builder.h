#ifndef CAT_BUILDER_H
#define CAT_BUILDER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "input_list.h"
#include "item.h"
#include "plugin_handler.h"

#include "catalog.h"

class Catalog;



class CatBuilder : public QThread
{
    Q_OBJECT
public:
    enum CatalogRefreshMode {
        LOAD,
        REFRESH,
        EXTEND,
        SEARCH,
        SAVE
    };

private:

    static PluginHandler* plugins_ptr;
    static CatalogRefreshMode refreshMode;
    static Catalog* cat;
    int runningSubthreads;

public:
    QList<CatItem> m_userItems;
    QString m_userKeys;
    QStringList m_keywords;
    QList<CatItem> m_extension_results;
    QList<CatItem> m_timelyItems;
    UserEvent::LoadType m_extensionType;
    bool m_extendDefaultSources;

    CatBuilder(CatalogRefreshMode mode, QHash<QString, QList<QString> > dirs, bool forceDropCatalog=false);
    void clear(CatalogRefreshMode mode, QHash<QString, QList<QString> > dirs, bool forceDropCatalog=false);
    static Catalog* createCatalog(QHash<QString, QList<QString> > dirs);
    static Catalog* getOrCreateCatalog(QHash<QString, QList<QString> > dirs);
    static Catalog* getCatalog();
    static void destroyCatalog();
    //Just for testing...
    static PluginHandler* getPluginHandle() { return plugins_ptr;}

    static QMutex* getMutext(){
        Q_ASSERT(cat);
        return cat->getMutext();
    }


    static bool getItemsFromQuery(InputList &inputList,
            QList<CatItem>& searchResults, int requestedNumber, int* beginPos=0){
        Q_ASSERT(cat);
        return cat->getItemsFromQuery(inputList, searchResults, requestedNumber, beginPos);
    }
    static void getMiniIcons(InputList &inputList){
        cat->getMiniIcons(inputList);
    }

    static CatItem getItem(QString itemPath, int childDepth=1);
    static void addItem(CatItem item);
    static void addSynonymChildren(CatItem& toUpdate) {
        cat->addSynonymChildren(toUpdate);
    }
    static CatItem updateItem(CatItem& it, int childDepth=1, UserEvent::LoadType lt= UserEvent::IGNORE);
    static long totalItems();
    //We need the new framework for this...
    static void setShown(CatItem it, UserEvent etv=UserEvent::SHOWN_LIST);
    static void getPluginURL(CustomFieldInfo* );
    static QList<CatItem> loadExtensionSourceForPlugins();
    static void itemLoading(CatItem* it, UserEvent::LoadType lt);
    static int launchObject(InputList& inputList, QList<CatItem> &outputList, QString* errString);
    bool addEndingVerb(InputList &fullData);
    static int subexecute(InputList& inputList, QList<CatItem> &outputList,QString* errString,int recur=0);
    static CatItem getItemById(int itemId);
    static void storeCatalog();
    static QList<CatItem> getMainApps(){ return cat->getMainApps();}
    void catalogTask(CatalogRefreshMode );
    void run();
    //Subthread method(s)
    void doExtension(QList<CatItem>* outList, QList<CatItem>* inList=0);
    void subThreadEndRoutine();

public slots:
    static QList<CatItem> indexItemList(QList<CatItem>* items);

signals:
    void catalogFinished();
    void catalogExtended();
    void catalogIncrement(float);
    void backgroundSearchDone(QString );
};



#endif // CAT_BUILDER_H
