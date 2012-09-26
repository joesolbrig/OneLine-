#include <QIcon>
#include "cat_store.h"
#include "cat_builder.h"
#include "itemarglist.h"

//First six chars digitized in order, ignoring anything but alpha differences
long codeStringStart(QString str){
    long res=1;
    for(int i=0; i<CHAR_ENCODE_LEN;i++){
        if(str.length()>i){
            res= CHAR_ENCODE_NUMBER*(res) + (TrieNode::encode(str[i])+1);
        } else {
            res= CHAR_ENCODE_NUMBER*(res);
        }
    }
    return res;
}



Cat_Store::Cat_Store(bool shouldRestore) :
    RELEVANTCE_BUCKET_COUNT(10),
    MAX_EXTENSION_ITEMS(10),
    HIGH_RELEVANCE_CUTOFF(HIGH_EXTERNAL_WEIGHT),
    VISIBILITY_CUTOFF(MEDIUM_EXTERNAL_WEIGHT+1),
    HISTORY_MEANING_THRESHHOLD(200),
    BASE_WEIGHT_PRIORITY(100),
    FCY_WEIGHT_SCALER (100),
    REWEIGH_INTERVALS(60*60),
    I_BY_FRCWEIGHT("Items By Frecency"),
    //I_BY_VERB_ARG("Verbs By Their Arguments"),
    I_BY_TIME("Items By Time"),
    I_BY_TYPE_TIME("Items By TypeTime"),
    I_BY_FINALWEIGHT("Items By Final Weight"),
    I_PINNED_BY_FINALWEIGHT("Pinned Items By Final Weight"),
    I_BY_SOURCEWEIGHT("Sources By Their Weight"),
    I_BY_SOURCEUPDATABILITY("Update Sources By Updatable Factor"),
    I_BY_SOURCEVISIBILITY("Sources By Their visibility"),
    I_BY_MESSAGETIME("Messages By Frecency"),
    I_BY_TYPE_NAME("Items by type then partial name"),
    I_BY_PIN_KEYS("Items by pinning keys"),
    I_BY_SOURCECHOSENNESS("Source Chosen frequencey By Source"),

    C_BY_PARENDID("Child Relations By Parents"),
    C_BY_CHILDID("Child Relations By Children"),
    C_BY_PARENTID_CHILDW("External Weights By Parent"),
    C_BY_PARENTID_RELW("Relation Weights By Parent - child ordering"),
    C_BY_PARENTID_RELT_W("Relation Weights By Parent - type-child ordering"),
    C_BY_PARENTID_UNSEEN_CHILDS("Relation Weights By Parent - unseen children ordering"),
    C_BY_PARENTID_SIBLING_CHILDS("Sibling ordered childrend By Parents by sibling order"),
    C_SOURCE_BY_PARENT_TYPE("Source Weight By Source Type"),
    C_VERB_BY_ACTIONPARENT("Verb Children By Parent Action Types"),
    C_SYNONYMS("List Of Synonyms Not Children Otherwise"),
    H_BY_TIME("History entries by time: Main Index"),
    H_BY_ITEMTIME("History entries by item then time"),
    V_BY_ITEMTIME("Visibility entries by item then time"),

    items_by_keys(this),
    item_index(shouldRestore, USER_APP_DIR),
    child_index(shouldRestore, USER_APP_DIR),
    history_index(shouldRestore, USER_APP_DIR)

{

    //This should be created before threads are running... so no locks here

    item_index.addIndex(I_BY_FRCWEIGHT);
    //item_index.addIndex(I_BY_VERB_ARG);
    item_index.addIndex(I_BY_TIME);
    item_index.addIndex(I_BY_TYPE_TIME);
    item_index.addIndex(I_BY_FINALWEIGHT);
    item_index.addIndex(I_PINNED_BY_FINALWEIGHT);
    item_index.addIndex(I_BY_SOURCEWEIGHT);
    item_index.addIndex(I_BY_SOURCEUPDATABILITY);
    item_index.addIndex(I_BY_SOURCEVISIBILITY);
    item_index.addIndex(I_BY_MESSAGETIME);
    item_index.addIndex(I_BY_TYPE_NAME);
    item_index.addIndex(I_BY_PIN_KEYS);
    item_index.addIndex(I_BY_SOURCECHOSENNESS);

    child_index.addIndex(C_BY_PARENDID);
    child_index.addIndex(C_BY_CHILDID);
    child_index.addIndex(C_BY_PARENTID_CHILDW);
    child_index.addIndex(C_BY_PARENTID_RELW);
    child_index.addIndex(C_BY_PARENTID_SIBLING_CHILDS);
    child_index.addIndex(C_BY_PARENTID_RELT_W);
    child_index.addIndex(C_BY_PARENTID_UNSEEN_CHILDS);
    child_index.addIndex(C_VERB_BY_ACTIONPARENT);
    child_index.addIndex(C_SYNONYMS);
    child_index.addIndex(C_SOURCE_BY_PARENT_TYPE);
    //child_index.addIndex(I_BY_SOURCECHOSENNESS);

    history_index.addIndex(H_BY_TIME);
    history_index.addIndex(H_BY_ITEMTIME);
    history_index.addIndex(V_BY_ITEMTIME);
    bucketCounter=1;
    metaCounters = 0;

    m_extendBucketCounter=1;
    m_extendMetaCounter=0;

    QList<CatItem> toRestore = item_index.get_list();
    for(int i=0; i< toRestore.count(); i++){
        QList<int> kl;
        items_by_keys.insert(toRestore[i],kl);
    }

    lastViewUpdate = appGlobalTime();
}

QMutex* Cat_Store::getMutext(){ return &catalogMutex;}


int Cat_Store::itemCount(){
    QMutexLocker locker(&catalogMutex);
    return item_index.count();
}
void Cat_Store::addSynonymChildren(CatItem& it){
    vector<DbChildRelation> crs = child_index.get_range(
            Tuple(it.getItemId()), Tuple(it.getItemId()+1), C_SYNONYMS);
    for(unsigned int i=0; i< crs.size();i++){
        QString synPath = (crs[i].getChildPath());
        CatItem synItem = this->getItemByPath(synPath,2);
        QList<CatItem> childs = synItem.getChildren();
        it.addChildren(childs);
    }
}

CatItem Cat_Store::getItemByPath(QString path,  int childDepth){
    QMutexLocker locker(&catalogMutex);
    m_returnRelationCount=0;
    CatItem it = getItemByPathPrivate(path, childDepth);
    return it;
}

QList<CatItem> Cat_Store::getCustomVerbByActionType(CatItem actionType){
    QMutexLocker locker(&catalogMutex);
    return getCustomVerbByActionTypeNamedPrivate(actionType);
}


QList<CatItem> Cat_Store::getItemChildren(ItemFilter* inputList, CatItem it){
    QMutexLocker locker(&catalogMutex);
    return getItemChildrenProtected(inputList,it);
}

QList<CatItem> Cat_Store::getItemActionChildren(CatItem it, CatItem::ItemType start_type,
                CatItem::ItemType end_type){
    QMutexLocker locker(&catalogMutex);
    vector<DbChildRelation> crs = child_index.get_range(
            Tuple(it.getItemId(),start_type), Tuple(it.getItemId(),end_type), C_BY_PARENTID_RELT_W);
    QList<CatItem> items;
    for(unsigned int i=0; i< crs.size();i++){
        CatItem child = item_index.getValue(crs.at(i).getChildPath());
        if(crs.at(i).getParentPath() == it.getPath())
            { items.append(child);}
    }
    return items;
}

QList<CatItem> Cat_Store::getItemsByKey(QString keystrokes, ItemFilter* filter,
                             int count, int* beginPos){
    Q_ASSERT(filter);
    QMutexLocker locker(&catalogMutex);//I_BY_TYPE_NAME
    QList<CatItem> doneItems;
    m_returnedPaths.clear();
    QList<CatItem> furtherItems;
    if(!keystrokes.contains(" ")){
        furtherItems = items_by_keys.find(keystrokes, count, filter, beginPos);
    }
    for(int i=0; i<furtherItems.size(); i++){
        CatItem it = furtherItems[i];
        it.setMatchType(CatItem::USER_KEYS);
        restoreRelations(it);
        setUnseenChildCount(it);
        doneItems.append(it);
        Q_ASSERT(doneItems.last().getMatchType() == CatItem::USER_KEYS);
    }
    int desired = beginPos? (*beginPos)*count : count;
    int offset = beginPos? (*beginPos-1)*count : 0;
    doneItems.append( getItemsByTypeKey(keystrokes,filter, desired, offset));
    m_returnedPaths.clear();
    return doneItems;
}

QList<ListItem> Cat_Store::getFileSources(){
    QList<ListItem> res;

    //Sub-Orderings first
    CatItem name(SIZE_SORT_CATEGORY_ITEM_PATH, NAME_NAME);
    name.setItemType(CatItem::ORGANIZING_TYPE);
    name.setOrganizingCharacteristic(NAME_KEY_STR);

    CatItem size(SIZE_SORT_CATEGORY_ITEM_PATH, SIZE_NAME);
    size.setItemType(CatItem::ORGANIZING_TYPE);
    size.setOrganizingCharacteristic(CHARACTERISTIC_CACHE_PREFIX + FILE_SIZE_CHARACTERISTIC_KEY);

    CatItem mimeType(MIMETYPE_SORT_CATEGORY_ITEM_PATH, MIME_TYPE_NAME);
    mimeType.setItemType(CatItem::ORGANIZING_TYPE);
    mimeType.setOrganizingCharacteristic(REAL_MIMETYPE_KEY);

    CatItem time(TIME_SORT_CATEGORY_ITEM_PATH, TIME_NAME);
    time.setItemType(CatItem::ORGANIZING_TYPE);
    time.setOrganizingCharacteristic(LAST_UPDATETIME_KEY);
    res.append(ListItem(name));
    res.append(ListItem(size));
    res.append(ListItem(mimeType));
    res.append(ListItem(time));
    for(int i=0; i< res.count(); i++){
        res[i].setFilterRole(CatItem::CATEGORY_FILTER);
    }
    return res;

}

QList<ListItem> Cat_Store::getBaseSources(){
    time_t now = ::appGlobalTime();
    QList<ListItem> results;

    QList<ListItem> fileChars = getFileSources();

    CatItem verbs_i(VERB_TYPE_PATH,VERB_NAME);
    ListItem verbs(verbs_i);
    verbs.setOrganizingType(CatItem::VERB);
    verbs.setItemType(CatItem::ORGANIZING_TYPE);
    verbs.setIcon(UI_NEXTACTION_PLACEHOLDER_ICON);
    results.append(verbs);

    CatItem files_i(FILE_TYPE_PATH,FILES_NAME);
    ListItem files(files_i);
    files.setOrganizingType(CatItem::LOCAL_DATA_DOCUMENT);
    files.setItemType(CatItem::ORGANIZING_TYPE);
    files.setIcon(FILE_TYPE_ICON);
    for(int i=0; i< fileChars.count(); i++){
        files.addChild(fileChars[i]);
    }
    results.append(files);


    CatItem sites_i(WEBADDRESS_PATH, WEBPAGE_NAME);
    ListItem sites(sites_i);
    sites.setOrganizingType(CatItem::PUBLIC_DOCUMENT);
    sites.setItemType(CatItem::ORGANIZING_TYPE);
    sites.setIcon(TAG_TYPE_ICON);
    results.append(sites);

    CatItem categories_i(CATEGORY_TYPE_PATH, CATEGORY_NAME);
    ListItem categories(categories_i);
    categories.setOrganizingType(CatItem::TAG);
    categories.setItemType(CatItem::ORGANIZING_TYPE);
    categories.setIcon(TAG_TYPE_ICON);

    CatItem messages_i(UNSEEN_MSG_ITEMS_PATH, MESSAGES_NAME);
    ListItem messages(messages_i);
    messages.setOrganizingType(CatItem::MESSAGE);
    messages.setItemType(CatItem::ORGANIZING_TYPE);
    messages.setIcon(MESSAGE_TYPE_ICON);
    results.append(messages);

    CatItem people_i(PEOPLE_PATH, PEOPLE_NAME);
    ListItem people(people_i);
    people.setOrganizingType(CatItem::PERSON);
    people.setItemType(CatItem::ORGANIZING_TYPE);
    people.setIcon(PERSON_TYPE_ICON);

    CatItem newsItems_i(UNSEEN_ITEMS_PATH, NEW_POSTS_NAME);
    ListItem newsItems(newsItems_i);
    newsItems.setOrganizingType(CatItem::PUBLIC_POST);
    newsItems.setItemType(CatItem::ORGANIZING_TYPE);
    newsItems.setIcon(FEED_TYPE_ICON);

    Tuple endMessageTuple((CatItem::MESSAGE),now);
    Tuple startMessageTuple((CatItem::MESSAGE),now-lastViewUpdate);
    int unseenMessages=item_index.get_range_count(
            startMessageTuple,endMessageTuple, I_BY_MESSAGETIME);
    Tuple endWebTuple((CatItem::PUBLIC_DOCUMENT),now);
    Tuple startWebTuple((CatItem::PUBLIC_DOCUMENT),now-lastViewUpdate);
    int unseenItems=item_index.get_range_count(
            startWebTuple,endWebTuple, I_BY_MESSAGETIME);;

//            vector<CatItem> scrs = item_index.get_range_by_pos(now-lastViewUpdate,now,I_BY_TIME);
//            for(unsigned int i=0;i<scrs.size();i++) {
//                if(scrs[i].getItemType()==CatItem::MESSAGE)
//                    { unseenMessages++;}
//                if(scrs[i].getItemType()==CatItem::PUBLIC_DOCUMENT)
//                    { unseenItems++; }
//            }

    messages.setCustomPluginValue(NEW_ITEM_OF_TYPE_KEY,unseenMessages);
    newsItems.setCustomPluginValue(NEW_ITEM_OF_TYPE_KEY,unseenItems);

    results.append(newsItems);
    results.append(people);
    results.append(categories);

//            CatItem typeParent(addPrefix(TYPE_PREFIX,QString::number(CatItem::ORGANIZING_TYPE)));
//            QList<ListItem> otherTypes = getTypeChildItems(typeParent,100);
//
//            for(int i=0; i<otherTypes.count(); i++){
//                if(otherTypes[i].hasLabel(TYPE_PREFIX)
//                    && !otherTypes[i].getName().isEmpty()
//                    && (otherTypes[i].hasLabel(ORGANIZING_CHARACTERISTIC_KEY))){
//                    res.append(otherTypes[i]);
//                }
//            }
    return results;
}

QList<ListItem> Cat_Store::getCondensedSources(){
    time_t now = ::appGlobalTime();
    QList<ListItem> results;

    ListItem locals(CatItem::createTypeParent(CatItem::LOCAL));
    results.append(locals);

    ListItem messages(CatItem::createTypeParent(CatItem::MESSAGE));
    Tuple startMessageTuple((CatItem::MESSAGE),now-lastViewUpdate);
    Tuple endMessageTuple((CatItem::MESSAGE),now);
    int unseenMessages=item_index.get_range_count(
            startMessageTuple,endMessageTuple, I_BY_MESSAGETIME);
    messages.setCustomPluginValue(NEW_ITEM_OF_TYPE_KEY,unseenMessages);
    results.append(messages);

    ListItem newsItems(CatItem::createTypeParent(CatItem::PUBLIC_FEED));
    Tuple endWebTuple((CatItem::PUBLIC_DOCUMENT),now);
    Tuple startWebTuple((CatItem::PUBLIC_DOCUMENT),now-lastViewUpdate);
    int unseenItems=item_index.get_range_count(
            startWebTuple,endWebTuple, I_BY_MESSAGETIME);
    newsItems.setCustomPluginValue(NEW_ITEM_OF_TYPE_KEY,unseenItems);
    results.append(newsItems);

    ListItem categories(CatItem::createTypeParent(CatItem::TAG));
    results.append(categories);

    for(int i=0; i< results.count(); i++){
        results[i].setFilterRole(CatItem::CATEGORY_FILTER);
    }

    return results;
}

QList<ListItem> Cat_Store::getOrganizingSources(ItemFilter* inputList){

    QMutexLocker locker(&catalogMutex);
    CatItem parentItem = inputList->getParentItem();
    CatItem filterItem = inputList->getFilterItem();
    QList<ListItem> res;
    if(parentItem.isEmpty() ){
        if(!filterItem.isEmpty()){
            if(filterItem.getOrganizeingType()!=CatItem::MIN_TYPE){
                QList<CatItem::ItemType>  types = filterItem.getOrganizingTypeList();
                for(int i=0; i< types.count();i++){
                    res.append(getSubSourcesFromType(types[i],UI_MINI_COUNT_LIMIT));
                    ListItem typeRep(CatItem::createTypeParent(types[i]));
                    typeRep.setFilterRole(CatItem::SUBCATEGORY_FILTER);
                    if(!(typeRep == filterItem)){
                        res.append(typeRep);
                    }
                }
            }
            res.append(coalatedSources(inputList));
            if(!res.isEmpty()){
                ListItem lfi(filterItem);
                lfi.setFilterRole(CatItem::ACTIVE_CATEGORY);
                res.push_front(lfi);
                return res;
            }
        }
        return getCondensedSources();
    } else if(parentItem.hasLabel(FILE_CATALOG_PLUGIN_STR)){
        return getFileSources();
    }
    return res;
    //Get a range of values...
}

QList<ListItem> Cat_Store::getSubSourcesFromType(CatItem::ItemType type, int limit){

    QList<ListItem> res;
    //CatItem typeParent(addPrefix(TYPE_PREFIX,QString::number(type)));
    CatItem typeParent = CatItem::createTypeParent(type);

    vector<DbChildRelation> crs = child_index.get_range(
            Tuple(typeParent.getItemId()), Tuple(typeParent.getItemId()+1), C_SOURCE_BY_PARENT_TYPE,limit);
    for(unsigned int i=0; i< MIN(crs.size(),(unsigned)limit);i++){
        QString path = (crs[i].getChildPath());
        CatItem item = this->getItemByPathPrivate(path,1);
        if(item.isEmpty()){ continue;}
        if(!item.hasSourceWeight()){ continue;}
        ListItem li(item);
        res.append(li);
    }

    return res;
}

QList<ListItem> Cat_Store::coalatedSources(ItemFilter* inputList){

    QList<ListItem> res;
    QList<CatItem> items = inputList->getListItems();

    QHash < QString, int > catOccurances;

    int itemCount = items.count();
    for(int i=0; i< itemCount; i++){
        QList<CatItem> cats = items[i].getSourceParents();
        for(int j=0; j< cats.count(); j++){
            CatItem cat = cats[j];
            if(!inputList->acceptItem(&cat)){
                continue;
            }
            if(catOccurances.contains(cat.getPath())){
                catOccurances[cat.getPath()]++;
            } else {
                catOccurances[cat.getPath()]=0;
            }
        }
    }

    QHash< QString, int >::iterator it = catOccurances.begin();
    for(;it!=catOccurances.end();it++){
        if(it.value() > USE_CATEGORY_FOR_FILTER_THRESH
           && (it.value() < CATEGORY_FILTER_PERCENTAGE* items.count())){
            CatItem cat = this->getItemByPath(it.key());
            ListItem li(cat);
            res.append(li);
        }
    }
    for(int i=0; i< res.count(); i++){
        res[i].setFilterRole(CatItem::SUBCATEGORY_FILTER);
    }
    CatItem filterItem = inputList->getFilterItem();
    ListItem lfi(filterItem);
    return res;
    //Get a range of values...
}




QList<CatItem> Cat_Store::getMainApps(){
    QMutexLocker locker(&catalogMutex);
    //CatItem typeParent(addPrefix(TYPE_PREFIX,QString::number(CatItem::VERB)));
    CatItem typeParent = CatItem::createTypeParent(CatItem::VERB);
    ItemFilter ifilt;
    return (getItemChildrenProtected(&ifilt,typeParent, VERB_PRELOAD_COUNT));
}

QList<CatItem> Cat_Store::getInitialItems(ItemFilter* filter, long limit , int* intialPos){
    QMutexLocker locker(&catalogMutex);
    Q_ASSERT(filter);
    m_returnedPaths.clear();

    QList<CatItem> res;
    QList<QList<CatItem> > priorityItems;
    QSet<QString> itemsUsedGuard;


    CatItem::ItemType organizingType = filter->getOrganizeingType();
    if(organizingType!=CatItem::MIN_TYPE){
        limit = limit*4;
    }

    //Highest weight items of a given type...
    if(organizingType!=CatItem::MIN_TYPE){
        QList<CatItem::ItemType> types = filter->getOrganizingTypeList();
        for(int i=0; i< types.count();i++){
            CatItem typeParent = CatItem::createTypeParent(types[i]);
            priorityItems.append(getItemChildrenProtected(filter,typeParent, limit, intialPos));
        }

    } else {
        CatItem::ItemType types[] = {CatItem::VERB,
                    CatItem::LOCAL_DATA_DOCUMENT,
                    CatItem::LOCAL_DATA_FOLDER,
                    CatItem::PUBLIC_DOCUMENT,
                    CatItem::TAG,
                    CatItem::MESSAGE
                };
        for(unsigned int i=0; i< sizeof(types); i++){
            CatItem typeParent = CatItem::createTypeParent(types[i]);
            priorityItems.append(getItemChildrenProtected(filter,typeParent, limit, intialPos));
        }
        //Messages, some ratio with their source...
        QList<CatItem> messageItems;
        messageItems = getInitialMessages(10, filter, limit, intialPos);
        priorityItems.append(messageItems);

        //Sources
        QList<CatItem> highTypeItems = getHighestTypeProtected(filter, limit,
                I_BY_SOURCEWEIGHT,true, intialPos);
        priorityItems.append(highTypeItems);

        //Pinned Items
//        QList<CatItem> pinnedItems = getHighestTypeProtected(filter, limit,
//                I_PINNED_BY_FINALWEIGHT,true, intialPos);
//        priorityItems.append(pinnedItems);

        //Misc high weight items
//        QList<CatItem>  highestRatedItems = getHighestTypeProtected(filter, limit,
//            I_BY_FRCWEIGHT,true, intialPos);
//        priorityItems.append(highestRatedItems);
    }

    QList<int> listIndex;
    for(int i=0;i<priorityItems.count();i++){listIndex.append(0);}

    bool go = true;
    while(go){
        go = false;
        for(int i=0;i<priorityItems.count();i++){
            if(priorityItems[i].count() > listIndex[i] &&  res.count() < limit){
                CatItem item = priorityItems[i][listIndex[i]];
                if(organizingType!=CatItem::MESSAGE){
                    if(item.getIsTimeDependant() ){
                        time_t curTime = appGlobalTime();
                        time_t creationTime = item.getCreationTime();
                        if(!item.isUnseenItem()
                            && creationTime < curTime - SEEN_MSG_CUTOFF_INTERVAL)
                            { continue; }
                        if(item.isUnseenItem()
                            && creationTime < curTime - UNSEEN_MSG_CUTOFF_INTERVAL)
                            { continue; }
                    }
                }
                setUnseenChildCount(item);
                listIndex[i]++;
                go = true;
                if(itemsUsedGuard.contains(item.getPath())){ continue;}
                itemsUsedGuard.insert(item.getPath());
                if(item.getItemType() != CatItem::PERSON || organizingType==CatItem::PERSON )
                    { res.append(item); }
            }
        }
    }
    if(intialPos)
        { *intialPos += limit; }
    return res;
}

qreal Cat_Store::getSourceRatio(CatItem source){
    QMutexLocker locker(&catalogMutex);
    QList<CatItem> organizingSources = getHighestTypeProtected(0,
            NUMBER_SOURCES_TO_BALANCE,
            I_BY_SOURCEWEIGHT,true);
    long weightBasis=0;
    for(int i=0; i<organizingSources.count();i++){
        weightBasis += organizingSources[i].getSourceWeight();
    }
    qreal oldWeightFactor = source.getSourceWeight();
    return oldWeightFactor/weightBasis;
}

void Cat_Store::setSourceToRatio(CatItem source, qreal ratio){
    QMutexLocker locker(&catalogMutex);
    QList<CatItem> organizingSources =
            getHighestTypeProtected(0, 10,
                            I_BY_SOURCEWEIGHT,true);

    long weightBasis=0;
    bool withinSet=false;
    for(int i=0; i<organizingSources.count();i++){
        if(i==organizingSources.count()-1 && !withinSet){
            //skip
        } else  if(!(organizingSources[i]==source)){
            weightBasis += organizingSources[i].getSourceWeight();
        } else {
            withinSet = true;
        }
    }

    qreal oldWeightFactor = source.getSourceWeight();
    qreal newWeight = weightBasis*ratio;
    qreal reweightFactor =
            (weightBasis -oldWeightFactor)/(weightBasis -newWeight);

    for(int i=0; i<organizingSources.count();i++){
        qreal realOldW = organizingSources[i].getSourceWeight();
        organizingSources[i].setSourceWeight(realOldW * reweightFactor);
        addItemEntryProtected(organizingSources[i]);
    }
    source.setSourceWeight(newWeight);
}


QList<CatItem> Cat_Store::coalateBySources(ItemFilter* filter, QList<CatItem> inList,
        int* intialPos, long limit){
    //Sources
    QList<CatItem> organizingSources = getHighestTypeProtected(filter, limit,
            I_BY_SOURCEWEIGHT,true, intialPos);
    QHash<QString, CatItem > SourceItems;

    qreal totalWeight =0;
    for(int i=0; i<organizingSources.count();i++){
        SourceItems[organizingSources[i].getPath()] = organizingSources[i];
        totalWeight += organizingSources[i].getSourceWeight();
    }

    QHash<QString, QList<CatItem> > priorityItems;
    QList<int> priorityPos;
    QList<qreal> relWeight;
    for(int i=0; i<organizingSources.count();i++){
        priorityPos.append(0);
        relWeight.append( organizingSources[i].getSourceWeight()/totalWeight);
    }
    QList<int> sourceIndices = priorityPos;

    for(int i=0; i<inList.count();i++){
        for(int j=0; j<organizingSources.count();j++){
            if(organizingSources[j].hasParent(organizingSources[j])){
                priorityItems[organizingSources[j].getPath()
                    ].append(inList[i]);
                break;
            }

        }
    }

    QList<CatItem> res;
    bool go = true;
    qreal adjustedWeight =0;

    //NO - sourceWeight should become FREQUENCY - DIRECTLY
    while(go){
        go = false;
        for(int i=0; i<organizingSources.count();i++){
            QList<CatItem>& priorItem = priorityItems[
                    organizingSources[i].getPath()
                    ];
            if(priorItem.length()>=sourceIndices[i]){
                continue;
            }
            go = true;
            CatItem item = priorItem[sourceIndices[i]];
            qreal newWeight = item.getRelevanceWeight()
                              * organizingSources[i].getSourceWeight();
            if(newWeight > adjustedWeight){
                sourceIndices[i]++;
                adjustedWeight = newWeight;
                res.append(item);
            }
        }
    }
//    while(go){
//        go = false;
//        for(int i=0; i<organizingSources.count();i++){
//            QList<CatItem>& priorItem = priorityItems[
//                    organizingSources[i]
//                ];
//            if(priorItem.length()>=sourceIndices[i]){
//                continue;
//            }
//            go = true;
//            CatItem item = priorItem[sourceIndices[i]];
//            qreal newWeight = item.getRelevanceWeight()
//                        * organizingSources[i].getSourceWeight();
//            if(newWeight > adjustedWeight){
//                sourceIndices[i]++;
//                adjustedWeight = newWeight;
//                res.append(item);
//            }
//        }
//    }
    return res;
}

QList<CatItem> Cat_Store::findEarlySiblings(CatItem item, int offset){
    QMutexLocker locker(&catalogMutex);
     return findEarlySiblingsProtected(item, offset);

}

void Cat_Store::setUnseenChildCount(CatItem& it){
    //C_BY_PARENTID_UNSEEN_CHILDS
    time_t eventTime = appGlobalTime();
    Tuple s(it.getItemId(),(eventTime - NEW_ITEM_CUTOFF_INTERVAL));
    Tuple e(it.getItemId(),eventTime);
    int count = child_index.get_range_count(s,e,C_BY_PARENTID_UNSEEN_CHILDS);
    it.setUnseenChildren(count);
}


QList<CatItem> Cat_Store::getOperations(ItemFilter* filter, int limit){
    CatItem typeParent = CatItem::createTypeParent(CatItem::OPERATION);
    QList<CatItem> res = getItemChildrenProtected(filter,typeParent, limit);
    return res;
}

QList<CatItem> Cat_Store::compressItems(int limit, QList<CatItem> il){
    int pLimit = limit*DB_ITEM_CATEGORY_RATIO;
    if(il.count() <= pLimit){
        return il;
    }

    QList<CatItem> res;
    for(int i = 0; i< pLimit; i++){ res.append(il[i]);}
    for(int i = pLimit; i <il.count(); i++){
        CatItem par = il[i].getTypeParent(BaseChildRelation::MESSAGE_SOURCE_PARENT);
        if(par.isEmpty()){continue;}
        QString parPath = par.getPath();
        if(m_returnedPaths.contains(parPath)){
            continue;
        }
        m_returnedPaths.insert(parPath);
        res.append(par);
        if(res.count()>=limit){
            break;
        }
    }
    return res;
}


QList<CatItem>  Cat_Store::buildListFromIndicesProtected(
        QStringList indices, ItemFilter* filter, long limit, int* initialPos){
    QList<CatItem> result;
    for(int i=0; i< indices.length(); i++){
        result.append(getHighestTypeProtected(filter, limit, indices[i], initialPos));
    }
    return result;
}

QList<CatItem> Cat_Store::getHighestRatedSources(ItemFilter* filter,
            long i,int* initialPos ){
    QMutexLocker locker(&catalogMutex);
    return getHighestTypeProtected(filter, i, I_BY_SOURCEWEIGHT, initialPos);
}

//This is only internally rates items...
QList<CatItem> Cat_Store::getHighestRatedItems(ItemFilter* filter, long i ,int* initialPos){
    QMutexLocker locker(&catalogMutex);
    return getHighestTypeProtected(filter, i, I_BY_FRCWEIGHT,initialPos);
}

QList<CatItem> Cat_Store::getHighestRatedMessages(ItemFilter* filter, long i, int* initialPos){
    QMutexLocker locker(&catalogMutex);
    return getHighestTypeProtected(filter, i, I_BY_MESSAGETIME,initialPos);
}


QList<CatItem> Cat_Store::getAssociatedDocuments(ItemFilter* , CatItem filterItem, int itemsDesired){
    QList<CatItem> res;
    m_insertedPaths.clear();
    if(filterItem.getSiblingOrder()>0){
        return findEarlySiblingsProtected(filterItem, itemsDesired);
    }

    QList<CatItem> types = getTypeParentItems(filterItem,true);
    int desired = itemsDesired;

    res.append(docListFromTypelist(types,desired));
    desired =  MAX(itemsDesired - res.count(),0);
    types = getTypeParentItems(filterItem, false);
    res.append(docListFromTypelist(types,desired));
    m_insertedPaths.clear();
    return res;
}

QList<CatItem> Cat_Store::docListFromTypelist(QList<CatItem> types, int desired){

    QList<CatItem> res;

    QList<int > groupP;
    QList< QList<CatItem> > groups;
    for(int i=0; i< types.count(); i++){
        if(m_insertedPaths.contains(types[i].getPath()))
            { continue;}
        m_insertedPaths.insert(types[i].getPath());
        QList<CatItem> byType = getTypeChildItems(types[i],desired);
        groups.append(byType);
        groupP.append(0);
    }
    int c = 0;

    //Intersperse assuming already sorted sublists...
    while(true){
        int bestWeight = 0;
        int bestPos=-1;
        for(int i=0; i< groups.count(); i++){
            if( groupP[i] < groups[i].length() ){
                if(m_insertedPaths.contains(groups[i][groupP[i]].getPath())){continue;}

                if(groups[i][groupP[i]].getFullWeight() > bestWeight) {
                    bestPos = i;
                    bestWeight = groups[i][groupP[i]].getFullWeight();
                }
            }
        }
        if(bestPos>-1){
            res.append(groups[bestPos][groupP[bestPos]]);
            groupP[bestPos]++;
            c++;
            if(c>=desired){
                return res;
            }
        } else { break;} //how we end...
    }
    return res;


}


QList<CatItem> Cat_Store::getTypeChildItems(CatItem filterItem, int count){
    QMutexLocker locker(&catalogMutex);
    QList <CatItem> res;
    Tuple s(filterItem.getItemId());
    Tuple e(filterItem.getItemId()+1);
    vector<DbChildRelation> scrs = child_index.get_range(
            s, e, C_BY_PARENTID_CHILDW,count);
    for(unsigned int i=0;i<scrs.size();i++) {
        QString childPath = scrs[i].getChildPath();
        if(m_insertedPaths.contains(childPath)){continue;}
        CatItem it = getItemByPathPrivate(childPath,0);
        if(it.getItemType() == CatItem::VERB){continue;}
        //if(!inputData->acceptItem(&it)){continue;}
        res.append(it);
    }
    return res;
}

QList<CatItem> Cat_Store::getTypeParentItems(CatItem childItem, bool onlyBest){
    QMutexLocker locker(&catalogMutex);
    QList <CatItem> res;
    vector<DbChildRelation> scrs = child_index.get_range(
            (childItem.getItemId()), childItem.getItemId()+1, C_BY_CHILDID);
    int c = scrs.size();
    for(int i=0;i<c;i++) {
         QString pathPath = scrs[i].getParentPath();
         CatItem it = getItemByPathPrivate(pathPath,0);
         if(scrs[i].getIsDefaultActionParent()){
             QList <CatItem> r;
             r.append(it);
             return r;
         }
         if(onlyBest){
             if(!(scrs[i].getChildRelType() == BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM)){
                 continue;
             }
         }
         if(it.getIsAction()){
             res.append(it);
         }
    }
    return res;
}


// Need to decide whether to do time by item or by overall batch
QList<CatItem> Cat_Store::getSourcesToExtend(){
    QMutexLocker locker(&catalogMutex);

    int allSourceC = item_index.count(I_BY_SOURCEUPDATABILITY);
    int relevanceBucketCount = MIN((allSourceC)/(MAX_EXTENSION_ITEMS/2),1);

    if(m_extendBucketCounter > m_extendMetaCounter
            || m_extendBucketCounter*MAX_EXTENSION_ITEMS > allSourceC){
        m_extendBucketCounter = 1;
        if(m_extendMetaCounter < relevanceBucketCount){
            m_extendMetaCounter++;
        } else { m_extendMetaCounter = 0;}
    }

    //LAST_UPDATETIME_KEY;

    //Get the highest and...
    vector<CatItem> scrs = item_index.get_range_by_pos(
            MAX(0, allSourceC - MAX_EXTENSION_ITEMS),  allSourceC, I_BY_SOURCEUPDATABILITY);

    //... a slice of the others
    vector<CatItem> scrs2 = item_index.get_range_by_pos(
            MAX(m_extendBucketCounter*(MAX_EXTENSION_ITEMS/2),0),
            MAX((m_extendBucketCounter+1)*(MAX_EXTENSION_ITEMS/2),0),I_BY_SOURCEUPDATABILITY);

    Q_ASSERT(scrs.size() <=(unsigned int)MAX_EXTENSION_ITEMS);
    Q_ASSERT(scrs2.size() <=(unsigned int)MAX_EXTENSION_ITEMS);

//            while(m_toUpdate.length() > MAX_EXTENSION_ITEMS*3){
//                m_toUpdate.pop_front();
//            }
    m_toUpdate.clear();

    for(unsigned int i=0;i<scrs.size();i++) {
        CatItem& srcItem = scrs[i];
        if(!m_toUpdate.contains(srcItem)) {
            restoreRelations(srcItem);
            m_toUpdate.append(srcItem);
        }
    }
    for(unsigned int i=0;i<scrs2.size();i++) {
        CatItem& srcItem = scrs2[i];
        if(!m_toUpdate.contains(srcItem)) {
            restoreRelations(srcItem);
            m_toUpdate.append(srcItem);
        }
    }
    m_extendBucketCounter++;
    return m_toUpdate;
}

QList<CatItem> Cat_Store::getAllItemForSave(){
    QMutexLocker locker(&catalogMutex);
    QList<CatItem> il = item_index.get_list();
    QList<CatItem> finalList;

    for(int i=0;i < il.count();i++){
        vector<DbChildRelation> crs = child_index.get_range(
            Tuple(il[i].getItemId()), Tuple(il[i].getItemId()+1), C_BY_PARENTID_CHILDW);
        CatItem res = il[i];
        res.clearRelations();
        for(unsigned int j=0;j< crs.size();j++){
            if(crs[j].getParentPath().isEmpty()){continue;}
            if(crs[j].getChildPath().isEmpty()){continue;}
            CatItem parent = item_index.getValue(crs[j].getParentPath());
            parent.clearRelations();
            CatItem child = item_index.getValue(crs[j].getChildPath());
            child.clearRelations();
            if(parent.isEmpty()){continue;}
            if(child.isEmpty()){continue;}
            res.addSavedRelations(DetachedChildRelation(parent, child, crs[j]));
        }
        finalList.append(res);
    }
    QList<HistoryEvent> hl = history_index.get_list();
    for(int i=0;i< hl.count();i++){
        finalList.append(hl[i].toCatItem());
    }
    return finalList;
}

//Now items should not have so many children that this operation is slow...
CatItem Cat_Store::addItem(CatItem it){
    QMutexLocker locker(&catalogMutex);
    m_insertedPaths.clear();
    if(it.isEmpty()){
        return it;
    }
    CatItem res = addItemProtected(it);
    setUnseenChildCount(res);
    m_insertedPaths.clear();
    return res;
}

void Cat_Store::addItemForWeigh(CatItem item){
    if(item.getIsTempItem())
        { return; }

    QMutexLocker locker(&catalogMutex);
    CatItem oldItem = item_index.getValue(item.getPath());
    if(!oldItem.isEmpty()){
        int w = oldItem.getExternalWeight();
        int oldWeight = oldItem.getFullWeight();
        if(w >HIGH_EXTERNAL_WEIGHT || w < MEDIUM_EXTERNAL_WEIGHT){ return; }
        CatItem weightParent = oldItem.getWeightParent();
        oldItem.setExternalWeight(w*ITEM_SEEN_REDUCTION_RATIO, weightParent);
        reweightItemProtected(oldItem);
        Q_ASSERT(oldWeight<=oldItem.getFullWeight());
    }

}

//Keeps track of group Addition

void Cat_Store::beginAddGroup(){
    QMutexLocker locker(&catalogMutex);
    m_insertedPaths.clear();
}

void Cat_Store::endAddGroup(){
    QMutexLocker locker(&catalogMutex);
    m_insertedPaths.clear();
}

CatItem Cat_Store::addItemInGroup(CatItem it){
    QMutexLocker locker(&catalogMutex);
    if(it.isEmpty()){
        return it;
    }
    CatItem res = addItemProtected(it);
    setUnseenChildCount(res);
    return res;
}

CatItem Cat_Store::setItemExecuted(InputList* inputList){
    QMutexLocker locker(&catalogMutex);

    //CatItem it(ItemFilter.asSerializedString());
    CatItem it = inputList->asStateItem();
    it.setLabel(SPECIFICALLY_CHOSEN_KEY);
    it.setSeen();

    time_t eventTime = appGlobalTime();
    CatItem outItem = addHistoryEvent(it, UserEvent(),eventTime);

    //QList<InputData> id = inputList.getInputData();
    CatItem& baseItem = inputList->at(0).getCurrentItem();
    for(int i=0;i<inputList->slotCount();i++){
        if(i>0){
            InputData id = inputList->at(i);
            DbChildRelation dbR(baseItem.getPath(), id.getCurrentItem().getPath());
            //Get saved instance
            dbR = child_index.find(dbR);
            dbR.setRelevanceWeight(dbR.getRelevanceWeight()+1);
            child_index.addEntry( (long)dbR.getParentId(), (long)(dbR.getTotalWeight()), dbR, C_BY_PARENTID_RELW);
            baseItem = id.getCurrentItem();
        }

        CatItem baseItem = inputList->itemAtSlot(i);
        if(!baseItem.isEmpty()){
            baseItem.setLabel(SPECIFICALLY_CHOSEN_KEY);
            addHistoryEvent(baseItem, UserEvent(),eventTime);
        }
    }

    QList<CatItem> simpleItems = inputList->nounList();
    for(int i=0; i < simpleItems.count(); i++){
        simpleItems[i].setLabel(SPECIFICALLY_CHOSEN_KEY);
        simpleItems[i].setLabel(EXECUTED_KEY);
        simpleItems[i].setSeen();
        if(simpleItems[i].isTimelyMessage()){
            simpleItems[i].clearExternalWeight();
        }
        reweightItemProtected(simpleItems[i]);
    }
    if(simpleItems.count()==1){
        QList<CatItem> orgParents = simpleItems[0].organizingCategoryParents();
        for(int i=0; i < orgParents.count(); i++){
            addHistoryEvent(baseItem, UserEvent(),eventTime);
        }
    }

    reweightItemsProtected();
    reweightItemProtected(it);
    pruneVisibilityList();
    return outItem;
}

void Cat_Store::reweightItems(QList<CatItem>* forExtension){
    QMutexLocker locker(&catalogMutex);
    reweightItemsProtected(forExtension);
}


//This could be called often,
//So perhaps we should do source reweighting elsewhere
void Cat_Store::addVisibilityEvent(CatItem item, UserEvent evt){
    QMutexLocker locker(&catalogMutex);

    time_t n = appGlobalTime();
    if(evt.event_type == UserEvent::NEW_MESSAGES_NOTED){
        lastViewUpdate = n;
    } else  if(evt.event_type == UserEvent::SHOWN_LIST || evt.event_type == UserEvent::SHOWN_EDITLINE){
        HistoryEvent he(item.getName(), QHash<QString, QString>(), item.getPath(), evt, n);
        Tuple(item.getItemId(),n);
        history_index.addEntry(n,he, V_BY_ITEMTIME);
        //history_index.addEntry(n,he, H_BY_TIME);

        CatItem oldItem = item_index.getValue(item.getPath());
        if(!oldItem.isEmpty()){
            int w = oldItem.getExternalWeight();
            int oldWeight = oldItem.getFullWeight();
            if(w >HIGH_EXTERNAL_WEIGHT || w < MEDIUM_EXTERNAL_WEIGHT){ return; }
            CatItem weightParent = oldItem.getWeightParent();
            if(weightParent.isEmpty()){
                weightParent = CatItem::createTypeParent(item.getItemType());
            }
            oldItem.setExternalWeight(w*ITEM_SEEN_REDUCTION_RATIO, weightParent);
            Q_ASSERT(oldWeight<=oldItem.getFullWeight());
            reweightItemProtected(oldItem);
            addItemEntryProtected(oldItem);
        } else {
            reweightItemProtected(item);
            addItemEntryProtected(item);
        }
    }
}

void Cat_Store::removeItem(CatItem it){
    QStringList parChildIndices = getParentChildInices();
    for(int i=0; i<parChildIndices.count();i++){
        vector<DbChildRelation> rels = child_index.get_range(
                Tuple(it.getItemId()), Tuple(it.getItemId()+1), parChildIndices[i]);
        for(unsigned int j=0;j<rels.size();j++){ child_index.removeObject(rels.at(j)); }
    }
//            vector<DbChildRelation> crs = child_index.get_range(
//                    Tuple(it.getItemId()), Tuple(it.getItemId()+1), C_BY_PARENTID_CHILDW);
//            for(unsigned int i=0;i<crs.size();i++){ child_index.removeObject(crs.at(i)); }
//
//            crs = child_index.get_range(
//                    Tuple(it.getItemId()), Tuple(it.getItemId()+1), C_BY_CHILDID);
//            for(unsigned int i=0;i<crs.size();i++){ child_index.removeObject(crs.at(i)); }
//
    item_index.removeObject(it);
}

void Cat_Store::testStruct(){
    cout << "\nOneLine(tm) db memory usage:";
    cout << "\n item index - size: " << item_index.size();
    item_index.testStruct();
    cout << "\n child index - size: " << child_index.size();
    child_index.testStruct();
    cout << "\n history index - size: " << history_index.size();
    history_index.testStruct();
    cout << "\n keydb ESTIMATED size: " << this->items_by_keys.allocatedSize();
    cout << "\n keydb Items referenced: " << this->items_by_keys.itemReferenceCount();
    cout << "\n keydb node count: " << this->items_by_keys.nodeCount() << "\n";
}

//This should be protected but can't be 'cause of circularity...
CatItem Cat_Store::getItemByPathPrivate(QString path, int depth){
    CatItem item = item_index.getValue(path);
    item.clearRelations();
    if(depth>=0){
        restoreRelations(item, depth);
    }
    
    if(false && item.getIsIntermediateSource()){
        vector<DbChildRelation> scrs = child_index.get_range(
                (item.getItemId()), item.getItemId()+1, C_SYNONYMS);
        for(unsigned int i=0; i< scrs.size(); i++){
            CatItem syn = getItemByPathPrivate(scrs.at(i).getParentPath(),0);
            if(!syn.getIsIntermediateSource()){
                return syn;
            }
        }
    }
    return item;
}

void Cat_Store::restoreRelations(CatItem& it, int depth){
    restoreRelationByIndex(it, C_BY_CHILDID, MIN(depth,1)); //parents
    restoreRelationByIndex(it, C_BY_PARENDID, depth-1); //children

}


Tuple Cat_Store::getInternalId(CatItem v){
    return item_index.getOrCreateId(v);
}

QList<CatItem> Cat_Store::findRecentUnseenRead(time_t cutOff){
    QMutexLocker locker(&catalogMutex);
    return findRecentUnseenReadProtected(cutOff);
}

void Cat_Store::reweightItem(CatItem& it){
    QMutexLocker locker(&catalogMutex);
    return reweightItemProtected(it);
}

//We want to remove the visibility of LOW priority items in the past...
//There shouldn't be that many at any time, so it's OK to just iterate over them..

//Unguarded but public 'cause creating friends is hard...

CatItem Cat_Store::getByInternalIdProtectMe(Tuple internalId){
    return item_index.getByInternalId(internalId);
}


QList<CatItem> Cat_Store::getInitialMessages(int initialSlots, ItemFilter* filter, long limit,
                                             int* initialPos){
    QList<CatItem> cl = getHighestTypeProtected(filter, limit, I_BY_MESSAGETIME, false, initialPos);
    cl = compressItems(limit, cl);

    if(subDivideByType(cl,MESSAGE_SOURCE_TYPE, initialSlots)){
        for(int i=0; i< cl.count();i++){
            QList<CatItem> gl = cl[i].getChildren();
            if( subDivideByType(gl,PERSON_POSTING_TYPE, initialSlots)){
                cl[i].clearRelations();
                cl[i].addChildren(gl);
            }
        }
    }
    return cl;
}

bool Cat_Store::subDivideByType(QList<CatItem>& items, QString type, int slotCount){
    if(items.count() < slotCount * SLOT_OVERLAP_RATIO) {
        return false;
    }
    QHash<QString, QList<CatItem> > labelList;
    QMap<QString, int> labelCounts;
    for(int i=0;i<items.count();i++){
        QString key = items[i].getParentChar(type);
        labelList[key].append(items[i]);
        if(labelCounts.contains(key)){
            labelCounts[key]++;
        } else{
            labelCounts[key] = 1;
        }
    }

    QMap<QString, int>::iterator lItter = labelCounts.begin();
    int c=0;
    int nestednessWeight=0;
    for(;lItter!=labelCounts.end() &&(c<slotCount);lItter++, c++){
        if(lItter.value() >= MIN_USEFUL_SUBCHILD){
            nestednessWeight++;
        }
    }
    if(nestednessWeight < slotCount*0.4){
        return false;
    }

    QHash<QString, QList<CatItem> >::iterator labItter = labelList.begin();
    QHash<QString, CatItem> resHash;
    for(;labItter!=labelList.end() &&(c<slotCount);labItter++, c++){
        QString path = labItter.key();
        if(resHash.contains(path)){
            CatItem parent = getItemByPath(path);
            QString pName = QString("from: ") + parent.getName();
            CatItem tempP = parent;
            tempP.clearRelations();
            tempP.setIsTempItem(true);
            tempP.setName(pName);



            if(!parent.isEmpty()){
                resHash[path] = parent;
            } else {
                resHash[path] = CatItem(path);
            }
        }
        resHash[path].addChildren(labItter.value());
   }
   QList<CatItem> res = resHash.values();
   items = res;
   return true;
}


QList<CatItem> Cat_Store::getCustomVerbByActionTypeNamedPrivate(CatItem argType){
    qint32 parentId = argType.getItemId();
    vector<DbChildRelation> crs = child_index.get_range(
            parentId, parentId+1, C_VERB_BY_ACTIONPARENT);
    QList<CatItem> items;
    for(unsigned int i=0; i< crs.size();i++){
        CatItem child = item_index.getValue(crs.at(i).getChildPath());
        if(!child.isCustomVerb()){continue;}
        child.clearRelations();
        child.setMatchType(CatItem::DATA_INFERENCE);
        if(crs.at(i).getParentPath() == argType.getPath())
            { items.append(child);}
    }
    return items;
}




QList<CatItem> Cat_Store::findEarlySiblingsProtected(CatItem item, int offset){
    QList<CatItem> res2;
    QList<DetachedChildRelation>  siblingRels = item.getSiblingOrderRels();
    for(int i=0; i< siblingRels.count();i++){
        int sibIndex = siblingRels[i].getSiblingOrder();
        Tuple start(siblingRels[i].getParentId(),sibIndex-offset);
        Tuple end(siblingRels[i].getParentId(),sibIndex);

        vector<DbChildRelation> childRs =
            child_index.get_range(start, end, C_BY_PARENTID_SIBLING_CHILDS);
        QList<CatItem> res1;
        for(unsigned int j=0; j<childRs.size();j++){
            CatItem item = getItemByPath(childRs[j].getChildPath());
            res1.append(item);
        }
        res2.append(res1);
    }
    return res2;

}

QList<CatItem> Cat_Store::getHighestTypeProtected(ItemFilter* filter, long i, QString index, bool takeLowest,
                                                  int* initialPos){
    long max_items = item_index.indexSize(index);
    vector<CatItem> scrs;
    int start = initialPos? *initialPos: 0;
    if(takeLowest){
        scrs = item_index.get_range_by_pos(start,i,index);
    } else {
        scrs = item_index.get_range_by_pos(max_items-i-(1+start),max_items-start,index);
    }
    QList<CatItem> res;
    for(unsigned int i=0;i<scrs.size();i++) {
        CatItem it = scrs[i];
        if(m_returnedPaths.contains(it.getPath())){continue;}
        m_returnedPaths.insert(it.getPath());
        if(filter && !filter->acceptItem(&it)){continue;}
        restoreRelations(it);
        res.append(it);
    }
    return res;
}

void Cat_Store::reweightItemProtected(CatItem& it){

    long newRelWeight = calcRelevanceWeight(it)*FCY_WEIGHT_SCALER;
    Q_ASSERT(newRelWeight>=0);
    it.setRelevanceWeight(newRelWeight);
    Q_ASSERT(newRelWeight==it.getRelevanceWeight());


    long tl = it.getCreationTime();
    CatItem clearedItem = it;
    //When I "clearRelations" matters!!!
    //Should be after weights calculated, before items added!!!
    clearedItem.clearRelations();

    item_index.addEntry(tl, clearedItem, I_BY_TIME);
    Tuple ttl((int)it.getItemType(), tl);
    item_index.addEntry(ttl, clearedItem, I_BY_TYPE_TIME);

    long newFullWeight = calcFullWeight(it);
    it.setTotalWeight(newFullWeight);

    Q_ASSERT(it.getFullWeight()== newFullWeight);

    it.clearRelations();
    item_index.addEntry(newFullWeight, it, I_BY_FINALWEIGHT);

    if(it.isUnseenItem()){
        Tuple t((int)it.getItemType(), tl);
        item_index.addEntry(t, it, I_BY_MESSAGETIME);
    } else {
        item_index.removeEntry(it, I_BY_MESSAGETIME);
    }

}

void Cat_Store::addItemEntryProtected(CatItem itemToAdd){

    qint64 fullWeight = itemToAdd.getFullWeight() ;
    qint64 relWeight = itemToAdd.getRelevanceWeight();
    Tuple internalId = item_index.addEntry(relWeight, itemToAdd, I_BY_FRCWEIGHT);
    itemToAdd.clearTempInformation();
    ItemRep rep(itemToAdd,internalId);
    items_by_keys.remove(rep);
    QList<int> kl;

    int charChanges = MIN((log(fullWeight)-1), KEY_SKIP_SWITCH_LIMIT-1);

    items_by_keys.insert(itemToAdd,kl, charChanges, internalId);

    long t = (long)itemToAdd.getItemType();
    long s = (codeStringStart(itemToAdd.getName()));
    Tuple typeNameT(t,s);
    CatItem existingTypeName = item_index.find(typeNameT,I_BY_TYPE_NAME);
    if(existingTypeName.isEmpty() || existingTypeName.getFullWeight() > itemToAdd.getFullWeight()){
        item_index.addEntry(typeNameT,itemToAdd,I_BY_TYPE_NAME);
    }

    if(itemToAdd.hasLabel(ITEM_PIN_KEY)){
        long s = (codeStringStart(itemToAdd.getPinnedString()));
        item_index.addEntry(s,itemToAdd,I_BY_PIN_KEYS);
        item_index.addEntry(fullWeight,itemToAdd,I_PINNED_BY_FINALWEIGHT);
        //I_BY_PIN_KEYS
    } else {
        item_index.removeEntry(itemToAdd, I_BY_PIN_KEYS);
        item_index.removeEntry(itemToAdd, I_PINNED_BY_FINALWEIGHT);
    }

    //Q_ASSERT(items_by_keys.find(toAdd.shortName,3,0).contains(it));
    if(itemToAdd.isUpdatableSource()){
        int totalSourceUpdateWeight(0);
        if(itemToAdd.hasSourceWeight()){
            totalSourceUpdateWeight = calcUpdatability(itemToAdd);
        }
        totalSourceUpdateWeight = MAX(totalSourceUpdateWeight, itemToAdd.getPositiveTotalWeight()/10);
        item_index.addEntry(totalSourceUpdateWeight, itemToAdd, I_BY_SOURCEUPDATABILITY);
    } else {
        item_index.removeEntry(itemToAdd, I_BY_SOURCEUPDATABILITY);
    }
    if(itemToAdd.isSource()) {
        item_index.addEntry(itemToAdd.getSourceWeight(), itemToAdd, I_BY_SOURCEWEIGHT);
    } else {
        item_index.removeEntry(itemToAdd, I_BY_SOURCEWEIGHT);
    }
}


QList<CatItem> Cat_Store::findRecentUnseenReadProtected(time_t cutOff){
    vector<CatItem> new_items = item_index.get_range(0, cutOff, I_BY_MESSAGETIME);
    QList<CatItem> res;
    for(unsigned int i=0; i<new_items.size(); i++){
        res.append(new_items[i]);
    //                int l = item_index.get_range_count(new_items[i].getItemId(), new_items[i].getItemId(),
    //                                                   H_BY_ITEMTIME);
    //                if(l == 0){
    //                    res.append(new_items[i]);
    //                }

    }
    return res;
}

QList<CatItem> Cat_Store::getItemChildrenProtected(ItemFilter* inputList,CatItem it,
        long limit, int* intialPos){
    int offset = intialPos? *intialPos: 0;
    vector<DbChildRelation> crs = child_index.get_range(
            Tuple(it.getItemId()), Tuple(it.getItemId()+1), C_BY_PARENTID_RELW, limit,offset);
    QList<CatItem> items;
    for(unsigned int i=0; i< crs.size();i++){
        CatItem child = item_index.getValue(crs.at(i).getChildPath());
        child.clearRelations();
        if(crs.at(i).getParentPath() != it.getPath()){continue;}
        if(inputList && !inputList->acceptItem(&child)){continue;}
        items.append(child);
    }
    return items;
}

int Cat_Store::restoreRelationByIndex(CatItem& it, QString index, int depth){
    //TODO: We want by CHILD AND PARENT ID or something...
    long itemId = it.getItemId();
    if(index == C_BY_PARENDID){
        if((depth<=0 && !it.isCustomVerb()) ||
            (depth < 0)){
            it.setChildStubbed();
            if(depth<=0){
                return 0;
            }
        } else {
            it.setChildStubbed(false);
        }
    }

    int relsAdded=0;
    if(depth <=0){
        return 0;
    }

    vector<DbChildRelation> childRs =
        child_index.get_range(itemId, it.getItemId()+1, index);

    for(unsigned int i=0;i<childRs.size();i++){
        //Type-parents only for db...
        if(hasPrefix(childRs[i].getParentPath(),TYPE_PREFIX)){
            continue;
        }

        if(childRs[i].getChildRelType() == BaseChildRelation::SUBSTITUTE
           && it.getPath()!=childRs[i].getParentPath()){
            QString substitute = childRs[i].getParentPath();
            CatItem newIt = getItemByPathPrivate(substitute, depth-1);
            it.mergeItem(newIt, true);
            break;
        } else {
            QString childPath = childRs[i].getChildPath();
            CatItem child = getItemByPathPrivate(childPath,depth-1);
            if(child.isEmpty()){
                child = CatItem(childRs[i].getChildPath());
            }

            QString parentPath = childRs[i].getParentPath();
            CatItem parent(parentPath);
            parent = getItemByPathPrivate(parentPath,depth-1);
            if(parent.isEmpty()){
                parent = CatItem(childRs[i].getParentPath());
            }
            if(parent.getItemType() == CatItem::ACTION_TYPE
               && index !=C_VERB_BY_ACTIONPARENT){
                int rels = restoreRelationByIndex(parent,C_VERB_BY_ACTIONPARENT,MAX(1,depth-1));
                if(rels==0){
                    parent.setStub(true);
                }
                // else { parent.setStub(false); }
                long parentId = parent.getItemId();
                int allChildren =
                    child_index.get_range_count(parentId, parentId+1, C_BY_PARENTID_CHILDW);
                int verbChildren =
                    child_index.get_range_count(parentId, parentId+1, C_VERB_BY_ACTIONPARENT);
                parent.setNounChildCount(MAX(allChildren - verbChildren,0));
            } else if(parent.getIsIntermediateSource()){
                restoreRelationByIndex(parent,C_BY_CHILDID,MAX(1,depth-1));
                QList<CatItem> pparents = parent.getParents();
                it.addParents(pparents);
                relsAdded+=pparents.count();
            }
            it.addSavedRelations(DetachedChildRelation(parent, child, childRs[i]));
            relsAdded++;
        }
    }

    return relsAdded;
}

CatItem Cat_Store::addItemProtected(CatItem itemToAdd, int recur){
    if(itemToAdd.hasSourceWeight()){
        qDebug() << "got source: " << itemToAdd.getPath();

    }
    if(m_insertedPaths.contains(itemToAdd.getPath())){
        CatItem re = item_index.getValue(itemToAdd.getPath());
        if(recur > 0 || itemToAdd.getName() ==re.getName()){
            re.clearRelations();
            return re;
        }
        if(!re.isStub()
                && re.hasRealName()
                && itemToAdd.getName() !=re.getName()) {
            itemToAdd.setPath(addPrefix(SYNONYM_PREFIX, itemToAdd.getName()));//
            itemToAdd.addParent(re, "", BaseChildRelation::SYNONYM);
        }
    }
    m_insertedPaths.insert(itemToAdd.getPath());
    if(recur >10){ return itemToAdd; }
    if(recur >1 && itemToAdd.isStub()) {return itemToAdd;}
    //qDebug() << QString(recur, ' ') << "CAT add Item:" << it.getPath();

    CatItem::MatchType matchType = itemToAdd.getMatchType();
    QList<int> matchIndex = itemToAdd.getMatchIndex();
    if(itemToAdd.isHistoryItem()){
        restoreHistoryEntry(itemToAdd);
        return itemToAdd;
    }
    if(itemToAdd.getPath().startsWith(PLUGIN_PATH_PREFIX))
        { pluginNames[itemToAdd.getPluginId()] = itemToAdd.getPath(); }

    if(itemToAdd.hasLabel(EXTERNAL_GEN_SOURCE_KEY)){
        CatItem argedSource(SOURCE_W_ARG_PATH);
        itemToAdd.addParent(argedSource);
    }

    bool sameWeight = false;
    CatItem oldItem;
    if(!item_index.getValue(itemToAdd.getPath()).isEmpty()){
        oldItem = item_index.getValue(itemToAdd.getPath());
        if(itemToAdd.getIsTempItem()){ //isStub()
            return oldItem;
        }
        if(oldItem.getWeightSection() == itemToAdd.getWeightSection())
            { sameWeight = true; }
        if(!itemToAdd.hasWeightInfo())
            { sameWeight = true; }
        if(itemToAdd.getCustomValue(REPLACE_CHILDREN_TEMP_KEY)){
            vector<DbChildRelation> crs = child_index.get_range(
                Tuple(oldItem.getItemId()), Tuple(oldItem.getItemId()+1),
                C_BY_PARENTID_CHILDW);
            for(unsigned int i=0;i<crs.size();i++)
                { child_index.removeObject(crs.at(i)); }
        }
        if(!oldItem.isEmpty()){
            oldItem.mergeItem(itemToAdd);
        }
        //it = oldItem;
    } else if(itemToAdd.getIsTempItem()){
        return itemToAdd;
    }

    addRelationsToDBProtected(itemToAdd, recur);//must be before reweigh to give access to external weight
    if(!oldItem.isEmpty()){ itemToAdd = oldItem; }
    reweightItemProtected(itemToAdd);
    addItemEntryProtected(itemToAdd);
    //Todo much could change to do below...

    if(m_toUpdate.contains(itemToAdd)
        && (appGlobalTime() - itemToAdd.getSourceUpdateTime()) < UPDATE_PERIOD*3){
        m_toUpdate.removeAll(itemToAdd);
    }
    Q_ASSERT(!item_index.getValue(itemToAdd.getPath()).isEmpty());
    itemToAdd.setMatchType(matchType);
    itemToAdd.setMatchIndex(matchIndex);;
    return itemToAdd;
}

void Cat_Store::addRelationsToDBProtected(CatItem& item, int recur){
    item.clearTempInformation();
    item.addArgumentsToRelations();
    addTypePseudoRelationsToItem(item);
    QList<DetachedChildRelation> relations = item.getRelations();


    //FIRST scan for the default action
    //Update what the default child is...
    int posDefaultChild=-1;
    int newDefaultChild=-1;
    for(int i=0; i < relations.count();i++){
        if(relations[i].getChildRelType() ==
           BaseChildRelation::SYNONYM){
            long parentId = (long)relations[i].getParentId();
            long childId = (long)relations[i].getChildId();
            DbChildRelation dbR(relations[i]);
            //SYNONYM is bi-directional
            Tuple t(parentId, childId);
            child_index.addEntry(t, dbR, C_SYNONYMS);
            Tuple t2(childId,parentId);
            DbChildRelation dbR2(relations[i].getChildPath(),relations[i].getParentPath());
            child_index.addEntry(t2, dbR2, C_SYNONYMS);
            continue;
        }
        if(relations[i].getIsDefault()){
            newDefaultChild = i;
        }
        if(newDefaultChild < 0){
            if(relations[i].getChildRelType() ==
                    BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM){
                newDefaultChild = i;
            }
            if(relations[i].getChildRelType() ==
               BaseChildRelation::MAYBE_DEFAULT_ACTION_CHILD){
                posDefaultChild= i;
            }
        }
    }
    if(newDefaultChild<0)
        {newDefaultChild = posDefaultChild;}
    for(int i=0; i < relations.count();i++) {
        if(newDefaultChild ==i){
            relations[i].setChildRelType(BaseChildRelation::CURRENT_DEFAULT_ACTION_ITEM);
        } else {
            relations[i].setNonDefaultAction();
        }
    }

    for(int i=0; i < relations.count();i++){
        Q_ASSERT((relations[i].getChildPath() == item.getPath()) ||
                 relations[i].getParentPath()== item.getPath() );
        if(relations[i].isForDelete()){
            removeRelation(relations[i]);
        } else if(i!=newDefaultChild){
            addRelation(relations[i], recur);
        }
    }
    if(newDefaultChild>-1){
        addRelation(relations[newDefaultChild], recur);
    }

    //Plugin is the parent
    if(recur >2){return;}
    QSet<QString> rawL = item.getLabels();
    QList<QString> labels = rawL.toList();
    for(int i=0; i < labels.count();i++){
        QString pluginName = pluginNames[stringHash(labels[i])];
        QString pluginPath = pluginName;
        if(pluginPath.isEmpty()){continue;}
        if(!pluginPath.contains("://")){
            pluginPath = addPrefix(QString(PLUGIN_PATH_PREFIX), pluginPath);
        }
        CatItem parPlugin(pluginPath, pluginName);
        if(item_index.contains(parPlugin)){
            parPlugin  = item_index.find(parPlugin);
        }
        DetachedChildRelation cr(parPlugin, item, pluginName);
        DbChildRelation testRel(pluginName,item.getPath());
        if(!child_index.contains(testRel)){
            addRelation(cr, recur);
        }
    }
}

void Cat_Store::addTypePseudoRelationsToItem(CatItem& item){
    CatItem::ItemType type = item.getItemType();
    if(type== CatItem::MIN_TYPE){ return; }
    if(item.hasLabel(TYPE_PARENT_KEY)){ return; }
    CatItem typeParent = CatItem::createTypeParent(type);
    if(typeParent.getPath() == item.getPath()){ return; }
    item.addParent(typeParent);
    if(item.getSortingType() !=CatItem::MIN_TYPE && item.getSortingType() !=item.getItemType()){
        CatItem par = CatItem::createTypeParent(item.getSortingType());
        item.addParent(par);
    }
}

void Cat_Store::removeRelation(DbChildRelation rel){
    QList<QString> pIndices = getParentChildInices();

    //hash Collisions remove relations at random, fixing would require work...
    for(int i=0; i< pIndices.count(); i++){
        child_index.removeObject(rel);
    }
}

void Cat_Store::addRelation(DetachedChildRelation cr, int recur){
    CatItem child = cr.getChild();
    CatItem parent = cr.getParent();
    Q_ASSERT(!parent.isEmpty() && !child.isEmpty());

    //We should be able to add CHILDREN to ANY DEPTH,
    //just not parent!!
    addItemProtected(child, recur+1);

    addItemProtected(parent, recur+1);
    long parentId = (long)cr.getParentId();
    long childId = (long)cr.getChildId();
    DbChildRelation dbR(cr);

    qint32 scaleW = MIN(cr.getExternalWeight()*((int)DOUBLE_SCALE_FACTOR),MAX_TOTAL_WEIGHT );
    Tuple ewTuple(parentId, scaleW);
    child_index.addEntry( ewTuple, dbR, C_BY_PARENTID_CHILDW);
    child_index.addEntry( parentId, (long)(child.getTotalWeight()), dbR, C_BY_PARENTID_RELW);
    child_index.addEntry( parentId, (long)(cr.getIsDefault()), dbR, C_BY_PARENTID_RELT_W);
    child_index.addEntry( parentId, dbR,C_BY_PARENDID);
    child_index.addEntry( childId, dbR, C_BY_CHILDID);

    long creationTime = child.getCreationTime();
    if(child.isUnseenItem()){
        Tuple t((int)parentId, creationTime);
        child_index.addEntry(t, dbR, C_BY_PARENTID_UNSEEN_CHILDS);
    } else {
        child_index.removeEntry(dbR, C_BY_PARENTID_UNSEEN_CHILDS);
    }
    //C_BY_PARENTID_SIBLING_CHILDS
    if(cr.getSiblingOrder()){
        Tuple sibTuple(cr.getParentId(),cr.getSiblingOrder());
        child_index.addEntry( sibTuple, dbR, C_BY_PARENTID_SIBLING_CHILDS);
    }

    if(cr.getChildType() == BaseChildRelation::WEIGHT_SOURCE
       ||( child.hasSourceWeight())){
        if(cr.getSourceWeight()>=0 ){
            Tuple swTuple(parentId, cr.getSourceWeight());
            child_index.addEntry( swTuple, dbR, C_SOURCE_BY_PARENT_TYPE);
        } else if(child.getSourceWeight()>0){
            Tuple swTuple(parentId, child.getSourceWeight());
            qDebug() << "added source parent: " << child.getPath();
            child_index.addEntry( swTuple, dbR, C_SOURCE_BY_PARENT_TYPE);
        }
    }

    if(dbR.getIsAction() && (child.getActionType() == CatItem::VERB_ACTION)){
        child_index.addEntry( parentId, dbR, C_VERB_BY_ACTIONPARENT);
    } else {
        child_index.removeEntry(dbR, C_VERB_BY_ACTIONPARENT);
    }
}

void  Cat_Store::restoreHistoryEntry(CatItem it){
    QString itemName = it.getPath().section("://",2);
    Q_ASSERT(!itemName.isEmpty());
    CatItem realIt = getItemByPathPrivate(itemName);
    Q_ASSERT(!realIt.isEmpty());
    if(it.isEmpty()){ return; }
    time_t n = it.getCustomValue(HISTORY_INTERNAL_TIME_KEY);
    addHistoryEvent(realIt, UserEvent(), n);
}

CatItem Cat_Store::addHistoryEvent(CatItem it, UserEvent evt, time_t n ){
    //We have to use the same 'now' for all the calculations
    //here but we don't worry about being otherwise exact...
    HistoryEvent he(it, evt, n);
    if(evt.getIsChosen()){
        history_index.addEntry(n,he, H_BY_TIME);
        Tuple t(it.getItemId(),n);
        history_index.addEntry(it.getItemId(),n ,he, H_BY_ITEMTIME);
    } else {
        history_index.addEntry(it.getItemId(),n,he, V_BY_ITEMTIME);
    }
    vector<DbChildRelation> parents = child_index.get_range(it.getItemId(), it.getItemId()+1, C_BY_CHILDID);
    for(unsigned int i=0;i < parents.size();i++){
        CatItem parent = CatItem(parents.at(i).getParentPath());
        if(evt.getIsChosen()){
            parent.setChosenness(parent.getChosenness()+1);
            item_index.addEntry(parent.getChosenness(), parent, I_BY_SOURCECHOSENNESS);
        } else {
            parent.setVisibility(parent.getVisibility()+1);
            item_index.addEntry(parent.getChosenness(), parent, I_BY_SOURCEVISIBILITY);
        }
    }
    return he.toCatItem();
}

double Cat_Store::calcRelevanceWeight(CatItem& it){
    //if(it.getRelevanceWeight() ==0){return 0;}//recalc items of relevance...
    if(it.getIsDepricated()){
        return DEPRICATED_DEFAULT_WEIGHT;
    }

    double weight = MEDIUM_EXTERNAL_WEIGHT + it.getExternalWeight()/10;
    const int timeFactor = 60*60;
    unsigned long f = 1;
    unsigned long now = appGlobalTime();

    //Q_ASSERT(history_index.get_range_count(Tuple(it.getItemId(),now+1),Tuple(it.getItemId(),LONG_MAX), H_BY_ITEMTIME));
    for(unsigned int i= 0; i < REWEIGH_INTERVALS; i++){
        unsigned int ff = (i==0)? 0 : f;
        if(now  <  ((ff)*timeFactor)){
            break;
        }
        //        if(history_index.count() > HISTORY_MEANING_THRESHHOLD
        //           && (par.getVisibility() < VISIBILITY_CUTOFF))
        //            {continue;}
        long int_begin = now - ((ff)*timeFactor);
        long int_end = (now > (f*2*timeFactor))?  now - (f*2*timeFactor) : 0;

        Tuple s(it.getItemId(),int_begin);
        Tuple e(it.getItemId(),int_end);
        //Start is *numerically* larger!
        double histWeight = history_index.get_range_count(e,s, H_BY_ITEMTIME);
        double visAntiWeight = history_index.get_range_count(e,s, V_BY_ITEMTIME);
        Q_ASSERT(histWeight>=0);
        if(histWeight>0){
            it.setLabel(SPECIFICALLY_CHOSEN_KEY);
            weight += histWeight*timeDiscount(i)*DOUBLE_SCALE_FACTOR;
        } else if(visAntiWeight>0){
            weight -= visAntiWeight*timeDiscount(i)*DOUBLE_SCALE_FACTOR*VISIBILITY_DISCOUNT_FACTOR;
        }
        f = 2*f;

    }
    weight = MAX(weight,0);
    return weight;
}

//Weight scale *DECREASES* as a item gains importance to keep "highest" a uniform value
//Weight is the max of overall frecency and frequency relative to each parent
//Scaled to make constant-sized database
long Cat_Store::calcFullWeight(CatItem it){
    qint32 base_w = it.getRelevanceWeight();
    long i_c = item_index.count();
    long i_o = (item_index.get_endOrder(base_w, I_BY_FRCWEIGHT)-1);
    if(!it.getPinnedString().isEmpty())
        { i_o = MAX(i_c, i_o - PINNED_MIN_ORDER); }

    long new_w = ((i_c - i_o+50)*DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR)/(i_c+100);
    new_w = new_w*RELATIVE_WEIGHT_ABSOLUTE_FRECENCY_FACTORE;


    Q_ASSERT(new_w >0);
    //new_w = item_index.count(I_BY_FRCWEIGHT) - new_w;
    vector<DbChildRelation> parentRelations = child_index.get_range(
            Tuple(it.getItemId(),0), Tuple(it.getItemId()+1,0), C_BY_CHILDID);
    for(unsigned int i=0;i < parentRelations.size();i++){
        CatItem par = item_index.getValue(parentRelations.at(i).getParentPath());
        if(!par.isSource()){ continue; }
        long sourceW = par.getSourceWeight();
        int sourceWeightOrder = item_index.get_order(Tuple(sourceW,0), I_BY_SOURCEWEIGHT);
        signed long externalWeight = parentRelations.at(i).getExternalWeight();
        if( externalWeight > MEDIUM_EXTERNAL_WEIGHT*2 && sourceWeightOrder <= DEFAULT_SOURCE_WEIGHT ){

            //OK, ordinal index of item relative to a given source, scaled to
            //the size of the source and it's weight
            Tuple t(parentRelations.at(i).getParentId(), externalWeight);
            Tuple boundT(parentRelations.at(i).getParentId()+1,0);
            long totalRange = child_index.get_range_count(t, boundT, C_BY_PARENTID_CHILDW);

            long wIndex = child_index.get_endOrder(t, C_BY_PARENTID_CHILDW);

            //The following condition probably should not happen at all
            //but I'm just fixing it on the fly
            if(wIndex >totalRange){ wIndex = totalRange;};

            long sourceWeight = (((1+totalRange) - wIndex)*DOUBLE_SCALE_FACTOR*DOUBLE_SCALE_FACTOR)
                             /
                             (totalRange*(log2(sourceWeightOrder)*100));
            Q_ASSERT(wIndex < i_c);

            if(sourceWeight >= 0){
                new_w = MIN(new_w, sourceWeight);
                new_w = MAX(1, new_w);
            }
         }
    }
    return new_w;
}

long Cat_Store::calcUpdatability(CatItem item){
    CatItem parent = item.getSourceParent();
    int parId = parent.getItemId();
    int sUpWeight = item.getUpdatingSourceWeight();
    long totalRange = child_index.get_order(Tuple(parId+1,0), C_SOURCE_BY_PARENT_TYPE);
    long pos = child_index.get_order(Tuple(parId,sUpWeight),C_SOURCE_BY_PARENT_TYPE);
    long orderFromTop = MAX(totalRange - pos,1);
    int order = log2(orderFromTop);
    long maxAllowValue = 2^(MAX(1,MAX_EXTERNAL_WEIGHT_BINARY_EXPONENT-order));
    return MIN(maxAllowValue, sUpWeight);
}


// We should do something to make it so that recent actions are what
// influence weight changes..
//
// This takes the inverse of a normalized vector, limited to
//        void reweighSources(){
//            float chosennessTotal=0;
//            //int sourceWeightTotal = 0;
//            //int visibilityTotal = 0;
//            vector<CatItem> cl = item_index.get_range(0, MAX_SOURCE_WEIGHT, I_BY_SOURCEVISIBILITY);
//            for(unsigned int i=0;i<cl.size();i++){
//                CatItem s = cl.at(i);
//                chosennessTotal +=(float)getWeightedChoseness(s)/((float)getWeightedVisibleness(s));
//            }
//            for(unsigned int i=0;i<cl.size();i++){
//                CatItem s = cl.at(i);
//                cl.at(i).setSourceWeight((s.getChosenness())/chosennessTotal);
//                item_index.addEntry(s.getSourceWeight(), cl.at(i), I_BY_SOURCEWEIGHT);
//            }
//        }

//Low weight items have visibility pruned so this only shows recent...
//But this function is just to slap high visibility items anyway...
double Cat_Store::getWeightedVisibleness(CatItem it){
    double weight = 0;
    const int timeFactor = 60*60;
    unsigned int f = 1;
    long now = appGlobalTime();
    for(unsigned int i= 0; i < REWEIGH_INTERVALS; i++){
        f = 2*f;
        long iid = it.getItemId();
        weight += history_index.get_range_count(
                Tuple(iid, now - f*timeFactor), Tuple(iid, now - f*2*timeFactor),
                V_BY_ITEMTIME)*timeDiscount(i);
    }
    Q_ASSERT(weight >=0);
    return weight;
}

//Seperate for emphesis
double Cat_Store::getWeightedChoseness(CatItem it){
    double weight = 0;
    const int timeFactor = 60*60;
    int f = 1;
    long now = appGlobalTime();
    for(unsigned int i= 0; i < REWEIGH_INTERVALS; i++){
        f = 2*f;
        if(now - f < 0){break;}
        long iid = it.getItemId();
        weight += history_index.get_range_count(
                Tuple(iid, now - f*timeFactor), Tuple(iid, now  - f*2*timeFactor),
                H_BY_ITEMTIME)*timeDiscount(i);
    }
    Q_ASSERT(weight >=0);
    return weight;
}

//Seperate for emphesis
//
//This formula "hyperbolically" discounts the past...
//I'm sure not if human hyperbolic discounting of future rewards
//justifies this but it's worth considering.
double Cat_Store::timeDiscount(qint32 t){
    double l = 1/((float)abs(t)+1);
    Q_ASSERT(l >0 );
    return l;
    }
    //This gets frecency
    //What matters most is reweighing the most recently rated items...
    //This is slanted toward the highest

void Cat_Store::reweightItemsProtected(QList<CatItem>* forExtension){
    int allItemC = history_index.count(H_BY_ITEMTIME);
    unsigned int c = MIN(allItemC, HIGH_RELEVANCE_CUTOFF);
    vector<HistoryEvent> items =
            history_index.get_range_by_pos( 0, c+1, H_BY_ITEMTIME);
    c = MIN(c, items.size());
    QSet<QString> weighedItems;
    for(unsigned int i=0;i < c;i++){
        QString itemPath = items.at(i).itemPath;
        CatItem itm = item_index.getValue(itemPath);
        if(!itm.isEmpty() && !weighedItems.contains(itm.getPath())){
            reweightItemProtected(itm);
            if(forExtension && itm.isSource()){
                forExtension->append(itm);
            }
            weighedItems.insert(itm.getPath());
        }
    }

    unsigned int relevanceBucketSize = allItemC/RELEVANTCE_BUCKET_COUNT;
    items = history_index.get_range_by_pos(
            bucketCounter*relevanceBucketSize,
            (bucketCounter+1)*relevanceBucketSize, H_BY_ITEMTIME);
    unsigned int r = MIN(relevanceBucketSize, items.size());
    for(unsigned int i=0;i < r;i++){
        QString itemPath = items.at(i).itemExpr;
        CatItem itm = item_index.getValue(itemPath);
        if(itm.isEmpty()){continue;}
        reweightItemProtected(itm);
        if(forExtension && itm.isSource()){
            forExtension->append(itm);
        }
    }
    //This sweeps items with O(1/weight) freqency
    //IE, weighting reweighing towards high weight items
    bucketCounter++;
    if(bucketCounter > metaCounters){
        bucketCounter = 0;
        if(metaCounters < RELEVANTCE_BUCKET_COUNT){
            metaCounters++;
        } else { metaCounters = 0;}
    }
    //reweighSources();
}

//Let's what happen if I do nothing..
void Cat_Store::pruneVisibilityList(){}

//For removing relations
QStringList Cat_Store::getParentChildInices(){
    QList<QString> pIndices;
    pIndices.append(C_BY_PARENDID);
    pIndices.append(C_BY_PARENTID_CHILDW);
    pIndices.append(C_BY_PARENTID_RELW);
    pIndices.append(C_BY_PARENTID_RELT_W);

    pIndices.append(C_BY_CHILDID);
    //pIndices.append(I_BY_SOURCECHOSENNESS);
    pIndices.append(C_VERB_BY_ACTIONPARENT);
    return pIndices;
}


QList<CatItem> Cat_Store::getItemsByTypeKey(QString keystrokes, ItemFilter* filter, int count,
    int offset){
    if(filter && filter->getOrganizeingType()!=CatItem::MIN_TYPE){
        return getItemsByTypePKey(keystrokes, filter, filter->getOrganizeingType(),
                count,offset);
    } else {
        QList<CatItem> res;
        for(int i=(int)CatItem::MIN_TYPE+10; i<(int)CatItem::MAX_TYPE; i+=10){
            res.append(getItemsByTypePKey(keystrokes, filter, (CatItem::ItemType)i,
                count,offset));
        }
        return res;
    }
}


QList<CatItem> Cat_Store::getItemsByTypePKey(QString keystrokes, ItemFilter* filter,
        CatItem::ItemType ItemType, int count,  int offset){
    long t = (long) ItemType;
    long s = codeStringStart(keystrokes);
    Tuple start(t,s);
    Tuple end(t,s+1);
    vector<CatItem> items = item_index.get_range(start, end, I_BY_TYPE_NAME, count, offset);
    int c = items.size();
    QList<CatItem> res;
    for(int i=0;i<c;i++) {
        CatItem it = items[i];
        if(!it.getName().startsWith(keystrokes)){
            continue; }
        if(m_returnedPaths.contains(it.getPath())){
            continue; }
        if(filter &&  !filter->acceptItem(&it)){
            continue; }
        m_returnedPaths.insert(it.getPath());
        it.setMatchType(CatItem::USER_KEYS);
        setUnseenChildCount(it);
        res.append(it);
    }
    return res;
}

QList<CatItem> Cat_Store::getPinnedItems(QString keystrokes, ItemFilter* filter,int count){
    vector<CatItem> items;
    if(keystrokes.length()>0){

    }
    long s = codeStringStart(keystrokes);
    QString endStrokes = keystrokes;
    int endL = endStrokes.length()-1;
    endStrokes[endL] = QChar((char)(endStrokes[endL].toAscii()+1));
    long e = codeStringStart(endStrokes);
    items = item_index.get_range(s, e, I_BY_PIN_KEYS,count);
    int c = items.size();
    QList<CatItem> res;
    for(int i=0;i<c;i++) {
        CatItem it = items[i];
        if(!it.getName().startsWith(keystrokes)){
            continue; }
        if(m_returnedPaths.contains(it.getPath())){
            continue; }
        if(filter && !filter->acceptItem(&it)){
            continue; }
        m_returnedPaths.insert(it.getPath());
        res.append(it);
    }
    return res;
}


