#ifndef CAT_STORE_H
#define CAT_STORE_H


#include "item.h"
#include "item_rep.h"
#include "input_list.h"
#include <QHash>

#define HASH_TYPE map

#include "util/mmap_file/btree.h"
#include <QTime>
#include <QMutex>

#include "history_item.h"
#include "trie_node.h"
#include "globals.h"

//This is an "object database" layer living on top of Kyoto cabinet
//and a series of custom b-tree indices

//Functions with "protected" in their names SHOULD not be used outside this file even though some
//are public for sharing with other classes in this file...

//First six chars digitized in order, ignoring anything but alpha differences
long codeStringStart(QString str);

//Lots of algorithm thought has gone into this...
//MultiTree makes things seem relatively simple

//This stores relations persistantly using the MultiTree class on top of Kyoto Cabinet
class Cat_Store {
    private:
        QMutex catalogMutex;
        const int RELEVANTCE_BUCKET_COUNT;
        const int MAX_EXTENSION_ITEMS;
        const int HIGH_RELEVANCE_CUTOFF;
        const int VISIBILITY_CUTOFF;
        const int HISTORY_MEANING_THRESHHOLD;
        const int BASE_WEIGHT_PRIORITY;
        const unsigned int FCY_WEIGHT_SCALER;
        const unsigned int REWEIGH_INTERVALS;
        static const int MAX_SOURCE_WEIGHT;

        const QString I_BY_FRCWEIGHT;
        //const QString I_BY_VERB_ARG;
        const QString I_BY_TIME;
        const QString I_BY_TYPE_TIME;
        const QString I_BY_FINALWEIGHT;
        const QString I_PINNED_BY_FINALWEIGHT;
        const QString I_BY_SOURCEWEIGHT;
        const QString I_BY_SOURCEUPDATABILITY;
        const QString I_BY_SOURCEVISIBILITY;
        const QString I_BY_MESSAGETIME;
        const QString I_BY_TYPE_NAME;
        const QString I_BY_PIN_KEYS;
        const QString I_BY_SOURCECHOSENNESS;

        const QString C_BY_PARENDID;
        const QString C_BY_CHILDID;
        const QString C_BY_PARENTID_CHILDW;
        const QString C_BY_PARENTID_CHILDAW;
        const QString C_BY_PARENTID_RELW;
        const QString C_BY_PARENTID_RELT_W;
        const QString C_BY_PARENTID_UNSEEN_CHILDS;
        const QString C_BY_PARENTID_SIBLING_CHILDS;
        const QString C_SOURCE_BY_PARENT_TYPE;
        const QString C_VERB_BY_ACTIONPARENT;
        const QString C_SYNONYMS;
        const QString H_BY_TIME;
        const QString H_BY_ITEMTIME;
        const QString V_BY_ITEMTIME;

        TrieNode items_by_keys;
        MultiTree<CatItem, QString> item_index;
        MultiTree<DbChildRelation, QString> child_index;
        MultiTree<HistoryEvent, QString> history_index;
        int bucketCounter;
        int metaCounters;
        QHash<quint64, QString> pluginNames;

        int m_extendBucketCounter;
        int m_extendMetaCounter;

        time_t m_lastViewUpdate;
        QList<CatItem> m_toUpdate;
        QSet<QString> m_insertedPaths;
        QSet<QString> m_returnedPaths;
        int m_returnRelationCount;


    public:
        //Everything public should be mutex-protected, show not call anything else public
        //

        Cat_Store(bool shouldRestore=true, QString overideDir="");
        void clearAll();

        QMutex* getMutext();
        QList<ListItem> getFileSources();

        int itemCount();
        void addSynonymChildren(CatItem& it);
        CatItem getItemByPath(QString path,  int childDepth=0);

        QList<CatItem> getCustomVerbByActionType(CatItem actionType);
        QList<CatItem> getItemChildren(ItemFilter* inputList, CatItem it);
        QList<CatItem> getItemActionChildren(CatItem it, CatItem::ItemType start_type,
                        CatItem::ItemType end_type=(CatItem::ItemType)(((int)CatItem::MAX_TYPE)));
        QList<CatItem> getItemsByKey(QString keystrokes, ItemFilter* filter,
                                     int count=-1, int* beginPos=0);
        QList<ListItem> getBaseSources();
        QList<ListItem> getCondensedSources();
        QList<ListItem> getOrganizingSources(ItemFilter* inputList);
        QList<ListItem> getSubSourcesFromType(CatItem::ItemType type, int limit=UI_MINI_COUNT_LIMIT);
        QList<ListItem> coalatedSources(ItemFilter* inputList);
        QList<CatItem> getMainApps();
        QList<CatItem> getInitialItems(ItemFilter* filter, long limit = MAX_SOURCE_COUNT, int* initialPos=0);

        QList<CatItem> getHighestSourceParents(ItemFilter* filter);

    public:
        void setUnseenChildCount(CatItem& it);
        QList<CatItem> getOperations(ItemFilter* filter, int limit);
        QList<CatItem> compressItems(int limit, QList<CatItem> il);
        QList<CatItem>  buildListFromIndicesProtected(
                QStringList indices, ItemFilter* filter, long limit,int* initialPos=0);
        QList<CatItem> getHighestRatedSources(ItemFilter* filter, long i = MAX_SOURCE_COUNT,int* initialPos=0);

        //This is only internally rates items...
        QList<CatItem> getHighestRatedItems(ItemFilter* filter, long i = MAX_SOURCE_COUNT,int* initialPos=0);
        QList<CatItem> getHighestRatedMessages(ItemFilter* filter, long i = MAX_SOURCE_COUNT,int* initialPos=0);
        QList<CatItem> getAssociatedDocuments(ItemFilter* , CatItem filterItem, int itemsDesired);
        QList<CatItem> getTypeChildItems(CatItem filterItem, int count);


    public:

        // Need to decide whether to do time by item or by overall batch
        QList<CatItem> getSourcesToExtend();
        QList<CatItem> getAllItemForSave();

        //Now items should not have so many children that this operation is slow...
        CatItem addItem(CatItem it);
        void addItemForWeigh(CatItem it);

        //Keeps track of group Addition

        void beginAddGroup();
        void endAddGroup();
        CatItem addItemInGroup(CatItem it);
        CatItem setItemExecuted(InputList* inputList);
        void reweightItems(QList<CatItem>* forExtension=0);

        QList<CatItem> findEarlySiblings(CatItem item, int offset);

        qreal getSourceRatio(CatItem source);
        void setSourceToRatio(CatItem source, qreal ratio);

        //This could be called often,
        //So perhaps we should do source reweighting elsewhere
        void addVisibilityEvent(CatItem it, UserEvent evt);
        void removeItem(CatItem it);
        void testStruct();
        //This should be protected but can't be 'cause of circularity...
        CatItem getItemByPathPrivate(QString path, int depth=0);

        void restoreRelations(CatItem& it, int depth=DEFAULT_RESTORE_DEPTH);

        Tuple getInternalId(CatItem v);
        QList<CatItem> findRecentUnseenRead(time_t cutOff = NEW_ITEM_CUFFOFF);
        void reweightItem(CatItem& it);

        //We want to remove the visibility of LOW priority items in the past...
        //There shouldn't be that many at any time, so it's OK to just iterate over them..

        //Unguarded but public 'cause creating friends is hard...

        CatItem getByInternalIdProtectMe(Tuple internalId);

        QList<CatItem> getCustomVerbByActionTypeNamedPrivate(CatItem argType);
    protected:
        QList<CatItem> docListFromTypelist(QList<CatItem> types, int desired);
        QList<CatItem> getActionTypeParents(CatItem childItem, bool onlyBest);
        QList<CatItem> getInitialMessages(int initialSlots, ItemFilter* filter, long limit = MAX_SOURCE_COUNT,
                                          int* initialPos=0);

        bool subDivideByType(QList<CatItem>& items, QString type, int slotCount);
        QList<CatItem> findEarlySiblingsProtected(CatItem item, int offset);

        QList<CatItem> getHighestTypeProtected(ItemFilter* filter, long i, QString index, bool takeLowest=false,
                                               int* initialPos=0, bool filterBySeen=true);

        void reweightItemProtected(CatItem& it);
        void addItemEntryProtected(CatItem itemToAdd);

        QList<CatItem> findRecentUnseenReadProtected(time_t cutOff);

        QList<CatItem> getItemChildrenProtected(ItemFilter* inputList,CatItem it, long limit=-1, int* intialPos=0);

        int restoreRelationByIndex(CatItem& it, QString index, int depth);

        CatItem addItemProtected(CatItem itemToAdd, int recur = 0);
        void addRelationsToDBProtected(CatItem& item, int recur);
        void addTypePseudoRelationsToItem(CatItem& item);

        void removeRelation(DbChildRelation rel);

        void addRelation(DetachedChildRelation cr, int recur);
        void  restoreHistoryEntry(CatItem it);
        CatItem addHistoryEvent(CatItem it, UserEvent evt, time_t n );

        double calcRelevanceWeight(CatItem& it);

        //Weight scale *decreases* as a item gains importance to keep "highest" a uniform value
        //Weight is the max of overall frecency and frequency relative to each parent
        //Scaled to make constant-sized database
        long calcFullWeight(CatItem it);
        long calcUpdatability(CatItem item);


        // We should do something to make it so that recent actions are what
        // influence weight changes..
        //

        //Low weight items have visibility pruned so this only shows recent...
        //But this function is just to slap high visibility items anyway...
        double getWeightedVisibleness(CatItem it);

        //Seperate for emphesis
        double getWeightedChoseness(CatItem it);
        //Seperate for emphesis
        //
        //This formula "hyperbolically" discounts the past...
        //I'm sure not if human hyperbolic discounting of future rewards
        //justifies this but it's worth considering.
        double timeDiscount(qint32 t);

        //This gets frecency
        //What matters most is reweighing the most recently rated items...
        //This is slanted toward the highest
        void reweightItemsProtected(QList<CatItem>* forExtension=0);
        void reweightHightweightItems(const QString, bool takeLowest );
        //Let's what happen if I do nothing..
        void pruneVisibilityList();

        //For removing relations
        QStringList getParentChildInices();


        QList<CatItem> getItemsByTypeKey(QString keystrokes, ItemFilter* filter, int count,
                int offset=0);

        QList<CatItem> getItemsByTypePKey(QString keystrokes, ItemFilter* filter,
                    CatItem::ItemType ItemType, int count,  int offset=0);

        QList<CatItem> getPinnedItems(QString keystrokes, ItemFilter* filter,int count);

};
#include "util/mmap_file/btree_impl.h"


#endif // CAT_STORE_H
