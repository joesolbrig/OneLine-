#ifndef INFORMATION_COALATOR_H
#define INFORMATION_COALATOR_H

#include <QList>
#include <QHash>
#include "item.h"
#include "list_item.h"

// This is hash which has ordering to it.
// We want to be able to get member by both order and itemId

//This isn't more more complex than just using a QList since
//QList requires some work to retrieve items by ID or path

bool timeLessThan(CatItem* it1, CatItem* it2);

QList<ListItem> sortListItems(QList<ListItem> il, QString label, bool asKey=false);
QList<CatItem> sortCatItems(QList<CatItem> il, QString label, bool asKey=false);

class OHash {
    protected:

        QList<QString> m_list;
        QHash<QString,ListItem> m_hash;
        bool m_formatted;
    public:

        OHash(){m_formatted=false;}

        OHash(OHash& o){
            m_list = o.m_list;
            m_hash = o.m_hash;
            m_formatted=false;
        }

        OHash& operator = (OHash& o){
            m_list = o.m_list;
            m_hash = o.m_hash;
            m_formatted = o.m_formatted;
            return *this;
        }

        virtual void clearListItems(){
            m_list.clear();
            m_hash.clear();
        }

        bool fullContains(QString path){
            return m_hash.contains(path);
        }

        void append(ListItem c){
            Q_ASSERT(!c.isEmpty());
            if(!m_hash.contains(c.getId())){
                m_list.append(c.getId());
                m_hash[c.getId()] = c;
            } else {
                ListItem li = m_hash[c.getId()];
                if(!li.isEmpty()){
                    li.merge(c);
                } else {
                    //foo todo, shouldn't happen, check why later
                    li = c;
                }
                m_hash[c.getId()] = li;
                if(!m_list.contains(c.getId())){
                    m_list.append(c.getId());
                }
            }
        }

        void prependWDup(ListItem c){
            Q_ASSERT(!c.isEmpty());
            if(m_hash.contains(c.getId())){
                m_hash[c.getId()].merge(c);
            }  else {
                m_hash[c.getId()] = c;
            }
            m_list.prepend(c.getId());
        }

        void addToStore(ListItem c){
            if(!m_hash.contains(c.getId())){
                m_hash[c.getId()] = c;
            } else {
                ListItem li = m_hash[c.getId()];
                if(li == c){
                    li.merge(c);
                    m_hash[c.getId()] = li;
                } else {
                    m_hash[c.getId()] = c;
                }
            }
        }

        void addList(QList<CatItem> c){
            for(int i=0;i<c.count();i++){
                ListItem li(c[i]);
                append(li);
           }
            m_formatted=false;
        }


        bool listContains(ListItem it){
            return m_list.contains(it.getPath());

        }

        void addList(QList<ListItem> c){
            for(int i=0;i<c.count();i++){
                append(c[i]);
            }
            m_formatted=false;
        }

        void intersperseList(QList<CatItem> c){
            if(c.count()<1){ return; }
            float sizeRatio = count()/c.count();

            for(int i=0;i<c.count();i++){
                ListItem li(c[i]);
                insertAtPos(li, (sizeRatio*i)+1);
            }
            m_formatted=false;
        }

        void remove(QString str){
            m_list.removeAll(str);
            m_hash.remove(str);
            m_formatted=false;
            Q_ASSERT(!m_hash.contains(str));
        }

        void remove(ListItem li){
            remove(li.getPath());
        }

        void hide(ListItem li){
            m_list.removeAll(li.getPath());
        }

        //We WANT to keep items in hash with no
        //references for later...
        QList<QString> reorderBy(QList<QString> l){
            QList<QString> temp = m_list;
            m_list = l;
            return temp;
        }

        QList<QString> reorderToList(QList<ListItem> l){
            QList<QString> res;
            for(int i=0;i<l.count();i++){
                res.append(l[i].getPath());
            }
            m_list = res;
            return m_list;
        }

        void showALL(){
            QList<QString> allItems = m_hash.keys();
            for(int i=0;i<allItems.count();i++){
                if(!m_list.contains(allItems[i])){
                    m_list.append(allItems[i]);
                }
            }
        }

        bool isEmpty(){
            return m_list.isEmpty();
        }

        int size(){
            return m_list.size();
        }

        void insertAtPos(ListItem c, int i){
            if(m_hash.contains(c.getId()))
                { m_list.removeAll(c.getId());}
            m_list.insert(i,c.getId());
            if(!m_hash.contains(c.getId()) || c.getIsTempItem()){
                m_hash[c.getId()] = c;
            } else if( m_hash[c.getId()] == c){
                m_hash[c.getId()].merge(c);
            } else {
                m_hash[c.getId()] = c;
            }
            m_formatted=false;
        }

        ListItem replaceAtPos(ListItem c, int i){
            ListItem oldItem = at(i);
            at(i) = c;
            m_formatted=false;
            return oldItem;
        }

        QList<ListItem> toList(){
            QList<ListItem> res;
            for(int i=0;i < count(); i++){
                ListItem it = at(i);
                res.append(it);
            }
            return res;
        }

        QList<CatItem> toCatList(){
            QList<CatItem> res;
            for(int i=0;i < count(); i++){
                res.append(at(i));
            }
            return res;
        }

        QList<QString> toStringList(){
            QList<QString> res;
            for(int i=0;i < count(); i++){
                res.append(at(i).getPath());
            }
            return res;
        }

        QSet<QString> keySet(){
            QSet<QString> res;
            for(int i=0;i < count(); i++){
                res.insert(at(i).getPath());
            }
            return res;
        }

        QList <ListItem> listFromOrder(QList<QString> l){
            QList<ListItem> res;
            for(int i=0;i < l.count(); i++){
                res.append(atStringRef(l[i]));
            }
            return res;
        }

        ListItem& atStringRef(QString str){
            return m_hash[str];
        }

        ListItem atString(QString str){
            return m_hash[str];
        }

        ListItem& at(int i){
            Q_ASSERT(i < m_list.count());
            return m_hash[m_list[i]];
        }

        int count(){
            Q_ASSERT(m_list.count() <= m_hash.count());
            return m_list.count();
        }

        int index(ListItem c){
            for(int i=0; i< m_list.count();i++){
                if(m_list[i] == c.getId()){
                    return i;
                }
            }
            return 0;
        }
};

class ItemListCondessor: public OHash {

protected:
    int m_charsAvail;
    QString m_fontStr;
    QList<QChar> m_hotKeys;
    QList<QString> m_itemsByUserDescription;
    QList<QString> m_organizingItems;
    QList<QString> m_organizingSubItems;
    QList<QString> m_sortedItems;
    QString m_organizingItemPath;
    QString m_organizingSubItemPath;

public:



    ItemListCondessor(){ }

    void relativeInformationFormatting(){

        QList<ListItem> oldList = this->toList();
        QList<ListItem> li = sortListItems(oldList, USER_DESCRIPTION_KEY_STR);
        m_itemsByUserDescription = reorderToList(li);
        for(int i=0;i< count();i++){
            setSplitPoint(i);
        }
        reorderToList(oldList);
    }

    void sortByCharacteristic(CatItem li){

        QList<ListItem> oldList = this->toList();

        QList<ListItem> items;
        if(li.isEmpty() || li.getOrganizingCharacteristic().isEmpty()){
            items = sortListItems(oldList, NAME_KEY_STR);
        } else {
            items = sortListItems(oldList, li.getOrganizingCharacteristic());
        }
    }

    QList<ListItem> getHighestValues(){

        int highestSection=0;
        QList<ListItem> highestItems;

        for(int i=0; i< count(); i++){
            if(at(i).getWeightSection() > highestSection){
                highestItems.clear();
                highestItems.append(at(i));
                highestSection = at(i).getWeightSection();
            } else if(at(i).getWeightSection() == highestSection){
                highestItems.append(at(i));
            }
        }
        return highestItems;
    }


    void addOrganizingItems(QList<ListItem> iconItems){
        m_organizingItems.clear();
        for(int i=0;i< iconItems.count();i++){
            //addOrganizingIcon(iconItems[i]);
            addToStore(iconItems[i]);
            m_organizingItems.append(iconItems[i].getPath());
        }
    }


//    void addOrganizingSubItems(ListItem iconItem){
//        m_organizingSubItems.clear();
//        if(iconItem.isEmpty()){ return;}
//        if(!m_organizingSubItems.contains(iconItem.getPath())){
//            m_organizingSubItems.append(iconItem.getPath());
//            addToStore(iconItem);
//            //m_organizingItems.append(iconItems[i].getPath());
//        }
//        addToStore(iconItem);
//    }

    void singleItemInfoFormat(ListItem item, int pos){
        insertAtPos(item, pos);
        int i=0;

        QList<ListItem> oldList = this->toList();
        QList<ListItem> li = sortListItems(oldList, USER_DESCRIPTION_KEY_STR);
        m_itemsByUserDescription = reorderToList(li);

        for(; i< m_itemsByUserDescription.count(); i++){
            if(item.getFormattedDescription() <
               m_itemsByUserDescription[i]){
                m_itemsByUserDescription.insert(i,item.getPath());
                i++;
                break;
            }
        }
        if(i<count()){ setSplitPoint(i); }
        if(i+1 < count()){ setSplitPoint(i+1); }

        //reorderBy(oldList);
        reorderToList(oldList);
        //Q_ASSERT(at(pos) == item);
    }


    void marqueInsert(ListItem item, int pos){
        int i=pos;
        for(; i< this->count(); i++){
            if(at(i).getItemType()== item.getItemType()){
                item = singleItemReplace(item,i);
            }
        }
        this->append(item);
    }

    ListItem singleItemReplace(ListItem item, int pos){
        ListItem oldItem = replaceAtPos(item, pos);
        int i=0;
        for(; i< m_itemsByUserDescription.count(); i++){
            if(item.getFormattedDescription() <
               m_itemsByUserDescription[i]){
                m_itemsByUserDescription.insert(i,item.getPath());
                i++;
                break;
            }
        }
        QList<QString> oldList = reorderBy(m_itemsByUserDescription);
        setSplitPoint(i);
        setSplitPoint(i+1);
        reorderBy(oldList);
        Q_ASSERT(at(pos) == item);
        return oldItem;
    }


    void setSplitPoint(int pos){
        //this->at(pos).setCharsAvailable(this->m_charsAvail);
        at(pos).setBaseSplitI();
        if(pos>0 && (pos < this->count())){
            int index = at(pos-1).dirDiff( at(pos));
            //at(pos-1).setCharsAvailable(this->m_charsAvail);
            at(pos-1).setSplitIndex(index);
            at(pos).setSplitIndex(index);
        }
    }

    QChar setSingleItemHotKey(ListItem& li, int charsAvail){
        li.setCharsAvailable(charsAvail);
        QList<int> kl = li.getMatchIndex();
        QChar k;
        int j = kl.length()>0 ? MIN(kl.last()+1, li.getName().length()): 0;

        //Skip to displayPath if we're using a long description
        if(li.getUseLongDescription()){ j = li.getName().length(); }

        //Otherwise, scan name
        for(;j< li.getName().length();j++){
            k = li.getName()[j];
            k = k.toLower();
            if(k.isLetter() && !m_hotKeys.contains(k)){
                li.setNextKeyIndex(j);
                li.setUseDisplayPathForNextKey( false);
                break;
            }
        }

        //At display path
        if(j >= li.getName().length()){
            QString rawDisplay = li.getRawDisplayPath(charsAvail);
            for(j=0;j< rawDisplay.length() ;j++){
                k = rawDisplay[j];
                k = k.toLower();
                if(!m_hotKeys.contains(k)){
                    li.setNextKeyIndex(li.getName().length() + j);
                    li.setUseDisplayPathForNextKey(true);
                    break;
                }
            }
        }
        return k;
    }

    QChar setExtraItemKey(ListItem& li, int charsAvail){
        li.setCharsAvailable(charsAvail);
        QList<int> kl = li.getMatchIndex();
        QChar k;
        int j = kl.length()>0 ? MIN(kl.last()+1, li.getName().length()): 0;
        for(;j< li.getName().length();j++){
            k = li.getName()[j];
            k = k.toLower();
            if(k.isLetter() && !m_hotKeys.contains(k)){
                li.setNextKeyIndex(j);
                li.setUseDisplayPathForNextKey( false);
                break;
            }
        }
        if(j >= li.getName().length()){
            QString rawDisplay = li.getRawDisplayPath(charsAvail);
            for(j=0;j< rawDisplay.length() ;j++){
                k = rawDisplay[j];
                k = k.toLower();
                if(!m_hotKeys.contains(k)){
                    li.setNextKeyIndex(li.getName().length() + j);
                    li.setUseDisplayPathForNextKey(true);
                    break;
                }
            }
        }
        return k;
    }

    void setNextChoiceKeys(int charsAvail){
        m_hotKeys.clear();
        int miniIconCount = m_organizingItems.count();
        for(int i=0;i< miniIconCount;i++){
            ListItem& li = atStringRef(m_organizingItems[i]);
            QChar k = setSingleItemHotKey(li, charsAvail);
            m_hotKeys.append(k);
        }
        for(int i=0;i< this->count();i++){
            ListItem& li = at(i);
            QChar k = setSingleItemHotKey(li, charsAvail);
            //m_hotKeys.insert(i,k);
            m_hotKeys.append(k);
        }
    }

    ListItem getItemByKey(QChar k){
        for(int i=0;i< m_organizingItems.count();i++){
            if(m_hash[m_organizingItems[i]].hotkeyChar() ==k){
                return m_hash[m_organizingItems[i]];
            }
        }
        for(int i=0;i< this->count();i++){
            if(m_hash[m_list[i]].hotkeyChar().toLower() ==k.toLower()){
                return m_hash[m_list[i]];
            }
        }


        return ListItem();
    }
};


class InformationCoalator : public ItemListCondessor {

private:

    ListItem m_filterItem;
    ItemListCondessor m_sortingItems;

public:

    InformationCoalator& operator = (InformationCoalator t){
        //OHash
        m_list = t.m_list;
        m_hash = t.m_hash;

        //Info formatter
        m_charsAvail = t.m_charsAvail;
        m_fontStr = t.m_fontStr;
        m_hotKeys = t.m_hotKeys;

        //Info coalator
        m_filterItem = t.m_filterItem;
        m_itemsByUserDescription = t.m_itemsByUserDescription;
        return *this;

    }

    InformationCoalator(const InformationCoalator& t) {
        //OHash
        m_list = t.m_list;
        m_hash = t.m_hash;

        //Info formatter
        m_charsAvail = t.m_charsAvail;
        m_fontStr = t.m_fontStr;
        m_hotKeys = t.m_hotKeys;

        //Info coalator
        m_filterItem = t.m_filterItem;
        m_itemsByUserDescription = t.m_itemsByUserDescription;

    }

    InformationCoalator(){ }

    InformationCoalator(QList<CatItem> ii){
        this->addList(ii);
    }

    InformationCoalator(QList<ListItem> ii){
        this->addList(ii);
    }

    ListItem atPath(QString itemPath){
        if(m_hash.contains(itemPath)){
            return m_hash[itemPath];
        } else  if(m_sortingItems.fullContains(itemPath)){
            return m_sortingItems.atStringRef(itemPath);
        } else {return ListItem();}
    }

    ListItem& atPathRef(QString path){
        return atStringRef(path);
    }

//    void clearListItems(){
//        ItemListCondessor::clearListItems();
//        //m_filterItem = ListItem();
//    }

    void sortByKeys(QString keys){

        QList<ListItem> oldList = this->toList();
        QList<ListItem> li = sortListItems(oldList, keys, true);
        reorderToList(li);
        return;
    }

    void sortFilterByItem(ListItem cItm){
        if(!cItm.isEmpty()){
            if(!cItm.getOrganizingCharacteristic().isEmpty()){
                QList<ListItem> oldList = this->toList();
                QList<ListItem> li = sortListItems(oldList, cItm.getOrganizingCharacteristic(), false);
                reorderToList(li);
            }

            m_organizingItems.append(cItm.getPath());
            addToStore(cItm);
            m_organizingItemPath = cItm.getId();
        } else {
            m_organizingItemPath.clear();
        }
    }

    void sortFilterBySubItem(ListItem cItm){
        if(!cItm.isEmpty()){
            if(!cItm.getOrganizingCharacteristic().isEmpty()){
                QList<ListItem> oldList = this->toList();
                QList<ListItem> li = sortListItems(oldList, cItm.getOrganizingCharacteristic(), false);
                reorderToList(li);
            }
            m_organizingItems.append(cItm.getPath());
            addToStore(cItm);
            m_organizingSubItemPath = cItm.getId();
        } else {
            m_organizingSubItemPath.clear();
        }

    }

    ListItem getFilterItem(){
        if(!m_organizingSubItemPath.isEmpty()){
            return atString(m_organizingSubItemPath);
        } else if(!m_organizingItemPath.isEmpty()){
            return atString(m_organizingItemPath);
        } else {
            return ListItem();
        }
    }


//    void sortFilterByItem(ListItem cItm, QString keys){
//
//        m_filterItem = cItm;
//        m_sortingItems.clear();
//        QList<ListItem> li;
//
//
//        if(!cItm.isEmpty() && !cItm.hasLabel(CHARACTERISTIC_SOURCE_LABEL)){
//            filterByItem(cItm);
//            return;
//        }
//
//        if(cItm.isEmpty() || cItm.getPath() == DEFAULT_SORT_CATEGORY_ITEM_PATH){
//            showALL();
//            li = toList();
//            li = sortListItems(li, keys, true);
//            m_filterItem = ListItem();
//        } else { li = sortListItems(li, cItm.getCustomString(FIELD_NAME_KEY)); }
//        reorderToList(li);
//
//        return;
//    }
//
//    void filterByItem(ListItem cItm){
//
//        for(int i=0; i <this->count(); i++){
//            ListItem li = this->at(i);
//            if(!cItm.hasChild(li)){
//                hide(li);
//                i--;//hide removes from list...
//            }
//        }
//    }

    ListItem getOrganizingItem(){
        ListItem li = atStringRef(m_organizingItemPath);
        return li;
    }

    void clearListItems(){
        QList<ListItem> filterItems = getFilterItems(100,100,100);

        OHash::clearListItems();
        for(int i=0;i< filterItems.count();i++){
            append(filterItems[i]);
        }
        m_hotKeys.clear();
        m_itemsByUserDescription.clear();
    }

    QList<ListItem> getFilterItems(int , int charsAvail, int ){
        QList<ListItem> res;
        for(int i=0; i< m_organizingItems.count(); i++){
            ListItem li = atStringRef(m_organizingItems[i]);
            if(li.isEmpty()){ continue;}
            QChar k = setExtraItemKey(li, charsAvail);
            m_hotKeys.insert(OHash::count() +i,k);
        }
        for(int i=0; i< m_organizingItems.count(); i++){
            ListItem li = atString(m_organizingItems[i]);
            if(li.isEmpty()){ continue;}
            res.append(li);
        }
        return res;
    }

    QList<ListItem> getFilterSubItems(int , int charsAvail, int ){
        for(int i=0; i< m_organizingSubItems.count(); i++){
            ListItem li = atStringRef(m_organizingSubItems[i]);
            QChar k = setExtraItemKey(li, charsAvail);
            m_hotKeys.insert(OHash::count() +i,k);
        }
        QList<ListItem> res;
        for(int i=0; i< m_organizingSubItems.count(); i++){
            ListItem li = atString(m_organizingSubItems[i]);
            res.append(li);
        }
        return res;
    }

    void expandNode(int treeSize){

        QList< QList<CatItem> > subChildren;
        QList< QList<CatItem> > visibleChild;
        int positionsAvailable = treeSize - treeSize;
        for(int i=0; i< count(); i++){
            subChildren[i] = at(i).getChildren();
            visibleChild[i].append(at(i));
        }

        QList<CatItem> newList;

        //adding a child from each till positions exhausted...
        int nthChild=0; int i=0;
        bool foundOne=false;
        while(positionsAvailable>0){
            if (i>=count()){
                if(!foundOne){
                    break;
                }
                i=0;
                nthChild++;
            }
            if(subChildren[i].count() >nthChild){
                CatItem formalChild(subChildren[i][nthChild]);
                formalChild.makePseudoChild(nthChild,subChildren[i].count());
                visibleChild[i].append(formalChild);
                positionsAvailable--;
            }
        }

        for(int j=0; j< visibleChild.count();j++){
            newList.append(visibleChild[i]);
        }
    }

    void condenseNodes(int treeSize){
        //We have to organize tags and single items seperately
        //at least as default

        QList<CatItem> tagged;
        QList<CatItem> unTagged;
        for(int i=0; i< count(); i++){
            if(this->at(i).getTagLevel()==CatItem::ATOMIC_ELEMENT){
                tagged.append(at(i));
            } else {
                unTagged.append(at(i));
            }
        }
        QList<CatItem> final;
        final.append(condenseFinal(tagged, treeSize));
        final.append(condenseFinal(unTagged, treeSize));

    }
    QList<CatItem> condenseFinal(QList<CatItem> items, int treeSize){
        float itemRatio = items.count()/treeSize;
        QList<CatItem> outItems;
        QString lastItemStr;
        int lastItem =-1;
        for(int i=0; i< count(); i++){
            if(lastItem < i*itemRatio){
                CatItem par = items[i];
                QString posStr = intersect(lastItemStr,par.getName());
                if(posStr.count() < par.getName().count()){
                    posStr = posStr + par.getName()[posStr.count()] + EXTENSION_STRING;
                }
                par.makePseudoParent(posStr);
                outItems.append(par);
                Q_ASSERT(outItems.count()==i+1);
                lastItem = i*itemRatio;
            } else {
                outItems[i].addChild(items[i]);
            }
        }
        return outItems;
    }


    void informativelyReorder(CatItem filterItem, int itemListSize ){
        if(itemListSize==0|| count()==0){
            return;
        }

        sortByCharacteristic(filterItem);
    }

    void distinguishItems(int itemsHigh, bool hasKeys){
        for(int i=0; i< count();i++){
            at(i).setDistinguished(CatItem::NOT_DISTINGUISHED);
        }
        if(hasKeys && this->count() > itemsHigh*MIN_NODE_RATIOS_FOR_CONDENSE){
            QList<ListItem> highest = getHighestValues();
            if(highest.count()*MIN_RATIO_FOR_SPOTLIGHT_HIGHEST < itemsHigh){
                for(int i=0; i< highest.count();i++){
                    highest[i].setDistinguished();
                    prependWDup(highest[i]);
                }
            }
        }
    }


    QList<ListItem> createTree(QList<ListItem> baseItems, int treeSize, qint32 minWeight=0){

        QList<ListItem> res;
        QSet<ListItem> coveredItems;
        while(((0 < baseItems.count()) && (res.count()< treeSize-1))
                || (baseItems.count() == 1 && (res.count()== treeSize-1) )){
            ListItem it = baseItems.takeFirst();
            qint32 w = it.getPositiveTotalWeight();
            Q_ASSERT(w>=0);
            if(w < minWeight){ continue; }
            QList<CatItem> childs = it.getChildren();
            for(int i=0; i < childs.count(); i++){
                ListItem li(childs[i]);
                coveredItems.insert(li);
            }
            res.append(it);
        }

        if(baseItems.count() >0){
            if(baseItems.count() ==1){
                res.append(baseItems.takeFirst());
            } else {
                CatItem o(QString(addPrefix(BUILTIN_PREFIX,"temp_others")), UI_MORE_STR);
                ListItem other(o);
                other.setIsTempItem(true);
                //QList<ListItem> subRes = createTree(allItems, treeSize);
                int added =0;
                for(int i=0;i <baseItems.count();i++) {
                    if(!coveredItems.contains(baseItems[i])){
                        other.addChild(baseItems[i]);
                        added++;
                    }
                }
                if(added > 0){
                    this->append(other);
                    res.append(other);
                }
            }
        } else if(res.count() < treeSize){
            QList<ListItem> childs = this->getChildItems();
            childs = sortListItems(childs,POSITIVE_WEIGHT_KEY_STR);
            while(childs.count() > 0 && res.count() < treeSize){
                this->append(childs.first());
                res.append(childs.first());
                childs.pop_front();
            }
        }

        return res;
    }

    void addCategories(){
        OHash res; //This merges inserted duplicate - important
        for(int i=0; i<this->size();i++){
            ListItem it = this->at(i);
            QList <CatItem> pars = it.getParents();
            for(int j=0; j<pars.size();j++){
                ListItem li(pars[j]);
                qDebug() << li.getPath() << i <<" - li child count: " << li.getChildren().count();
                li.setCustomPluginInfo(TEMP_CATERGORY_LABEL, TEMP_CATERGORY_LABEL);
                res.append(li);
            }
        }

        QList<ListItem> res2 = filterCategories(res.toList());
        this->addList(res2);
    }



protected:

    QList<ListItem> getChildItems(){
        QList<ListItem> res;
        for(int i=0; i<this->size();i++){
            ListItem it = this->at(i);
            QList <CatItem> childs = it.getChildren();
            for(int j=0; j<childs.size();j++){
                ListItem li(childs[j]);
                if(!listContains(li) && !res.contains(li)){
                    li.setMatchType(CatItem::CHILD_RELATION);
                    li.setCustomPluginInfo(PARENT_NAME_TO_CHILD_SEARCH,it.getName());
                    li.setCustomPluginInfo(PARENT_PATH_TO_CHILD_SEARCH,it.getPath());
                    res.append(li);
                }
            }
        }
        return res;
    }

    QList<ListItem> getCharacteristicCats(){
        QList<ListItem> res;
        for(int i=0; i<this->size();i++){
            ListItem it = this->at(i);
            if(it.hasLabel(TEMP_CATERGORY_LABEL)){
                continue;
            }
            QList <CatItem> pars = it.getCharacteristics();
            for(int j=0; j<pars.size();j++){
                if(!listContains(ListItem(pars[i])))
                    { res.append(ListItem(pars[i]));}
            }
        }
        res = filterCategories(res);
        return res;
    }


    QList<ListItem> filterCategories(QList<ListItem> cats){

        QList<ListItem> res;

        for(int i=0; i<cats.size();i++){
            if(!acceptCat(cats[i])){
                continue;
            }
            res.append(cats[i]);
        }
        return res;

    }

    bool acceptCat(ListItem c){
        int bigItemCoverage = largestSectionSize(c);
        int totalCoverage = c.getChildren().count();

        return ( totalCoverage > TOO_LOW_COALATE_RATIO*(this->count())
                &&
            bigItemCoverage < TOO_HIGH_COALATE_RATIO*(this->count()) );
    }

    int largestSectionSize(ListItem it){
        int secSize = 0;
        QList<CatItem> childs = it.getChildren();
        if(it.hasLabel(CHARACTERISTIC_SOURCE_LABEL)){
            for(int i=0;i < childs.count();i++){
                secSize = MAX(childs[i].getChildren().count(), secSize);
            }
        } else {
            secSize = childs.count();
        }
        return secSize;
    }

};


#endif // INFORMATION_COALATOR_H
