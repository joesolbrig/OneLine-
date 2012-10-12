#ifndef INPUT_DATA_H
#define INPUT_DATA_H

#include "item.h"
#include "list_item.h"
#include "inputLanguage.h"
#include "information_coalator.h"

/** InputData shows one segment (between tabs) of a user's query
    A user's entire query is represented by InputList
    and each element of the list represents a segment of the query.

    E.g.  query = "google <tab> this is my search" will have 2 InputData segments
    in the list.  One for "google" and one for "this is my search"
*/

class ListState {

public:
    CatItem m_currentItem;
    CatItem m_contextItem;
    InformationCoalator m_formattedItems;
    bool m_expanded;
    QString m_expandSearchStr;

    /** The user's entry */
    QString m_userKeys;
    QList<CatItem> m_matched_to_add;

    void clearAll(){
        m_currentItem = CatItem();
        m_formattedItems.clearListItems();
        m_matched_to_add.clear();
        m_userKeys.clear();
    }

    void clearItem(){
        m_currentItem = CatItem();
    }


    //List items...
    int getCurrentItemIndex(){
        if(!m_currentItem.isEmpty()){
            ListItem li(m_currentItem);
            return m_formattedItems.index(li);
        } else {return 0;}
    }

    void setCurrentItemByIndex(int i){
        m_currentItem = m_formattedItems.at(i);
    }

    ListItem& getListItem(int i ){
        return m_formattedItems.at(i);
    }


    int getListItemCount(){
        return m_formattedItems.size();
    }

    void setFilterItem(ListItem li){
        m_formattedItems.sortFilterByItem(li);
    }

    void setSubfilterItem(ListItem li){
        m_formattedItems.sortFilterBySubItem(li);
    }

    ListItem getFilterItem(){
        return m_formattedItems.getFilterItem();
    }

    void addOrganizingItems(QList<ListItem> li){
        //m_filterItem = li;
        m_formattedItems.addOrganizingItems(li);
    }


    void addSingleListItem(CatItem item, int pos){
        ListItem li(item);
        m_formattedItems.singleItemInfoFormat(li, pos);
    }

    void removeItem(CatItem item){
        m_formattedItems.remove(ListItem(item));

    }

    void cullItemsForInsert(){
        QString keys = getUserKeys();

        m_matched_to_add.clear();
        if(keys.length()>2 && !keys[keys.count()-1].isSpace()){
            for(int i=0;i< m_formattedItems.size();i++){
                ListItem& li = m_formattedItems.at(i);
                //li.hasKeyChar() &&  li.hotkeyChar() == keys[keys.length()-1]
                if( filterItemByKeys(li, keys) && !li.getIsTempItem()){
                    m_matched_to_add.append(li);
                }
            }
        }
        m_formattedItems.clearListItems();
    }

    void clearListItems(){
        m_formattedItems.clearListItems();
    }

    CatItem addListItemList(QList<CatItem> il){
        //Things previously labeled to match  by their highlight key... probably only one
        CatItem toReturn;
        m_formattedItems.addList(il);
        m_formattedItems.addList(m_matched_to_add);
        m_matched_to_add.clear();
        //m_formattedItems.sortFilterByItem(m_filterItem,getUserKeys());
        m_formattedItems.relativeInformationFormatting();

        if ( m_formattedItems.size() != 0 && (getUserKeys().length() >0)){
            for(int i=0;i< m_formattedItems.size(); i++){
                CatItem bestItem = m_formattedItems.at(i);
                QString keys = getUserKeys();
                if ( matches( &bestItem, keys)
                            &&
                        (bestItem.getMatchIndex().count() >= keys.length())){
                    toReturn = bestItem;
                    break;
                }
            }
        } else {
            toReturn = CatItem();
        }
        return toReturn;
    }


    CatItem intersperseListItemList(QList<CatItem> il){
        //Things previously labeled to match  by their highlight key... probably only one
        CatItem toReturn;
        m_formattedItems.intersperseList(il);
        m_formattedItems.intersperseList(m_matched_to_add);
        m_matched_to_add.clear();
        //m_formattedItems.sortFilterByItem(m_filterItem,getUserKeys());
        m_formattedItems.relativeInformationFormatting();

        if ( m_formattedItems.size() != 0 && (getUserKeys().length() >0)){
            for(int i=0;i< m_formattedItems.size(); i++){
                CatItem& bestItem = m_formattedItems.at(i);
                QString keys = getUserKeys();
                if ( matches( &bestItem, keys)
                    &&
                            (bestItem.getMatchIndex().count() >= keys.length())){
                    toReturn = bestItem;
                    break;
                }
            }
        } else {
            toReturn = CatItem();

        }
        return toReturn;
    }

    QList<ListItem> getFormattedListItems(VerbItem filterItem,int itemsHigh, int charsAvail, int treePos){

        QList<ListItem> res;
        if(this->getUserKeys().isEmpty()){
            if(treePos > 1){
                itemsHigh = MAX(itemsHigh, m_formattedItems.count());
            }
            m_formattedItems.informativelyReorder(filterItem, itemsHigh);
        } else {
            m_formattedItems.sortByKeys(m_userKeys);
        }
        m_formattedItems.distinguishItems(itemsHigh, !m_userKeys.isEmpty());
        m_formattedItems.setNextChoiceKeys(charsAvail);

        res = m_formattedItems.toList();

        return res;
    }

    QList<CatItem> getListItems(){
        return m_formattedItems.toCatList();
    }

    bool isFilterItem(CatItem item){
        return m_formattedItems.isFilterItem(item);
    }

    QList<ListItem> getOrganizingFiltersItems(int itemsHigh, int charsAvail, int minWeight){
        return  m_formattedItems.getFilterItems(itemsHigh, charsAvail, minWeight);
    }

    QList<ListItem> getOrganizingFilterSubItems(int itemsHigh, int charsAvail, int minWeight){
        return  m_formattedItems.getFilterSubItems(itemsHigh, charsAvail, minWeight);
    }


    ListItem getItemByPath(QString path){
        return m_formattedItems.atPath(path);
    }

    ListItem& getItemByPathRef(QString path){
        return m_formattedItems.atPathRef(path);
    }

    ListItem getItemByKey(QChar k){
        return m_formattedItems.getItemByKey(k);
    }

public:

    ListState()  { m_expanded=false;}
    ListState(QString str): m_userKeys(str) { m_expanded=false;}
    //ListState(CatItem item): KeyAssociation(item) {}
    ListState(const ListState& id) {
        m_userKeys = id.m_userKeys;
        m_formattedItems = id.m_formattedItems;
        m_currentItem = id.m_currentItem;
        m_expanded=id.m_expanded;

    }

    bool isExpanded(){return m_expanded;}
    void setExpanded(bool e=true){m_expanded=e;}

    QString getExpandedSearchStr(){return m_expandSearchStr;}
    void setExpandedSearchStr(QString str){m_expandSearchStr = str;}

    bool testIsPhrase(){ return m_userKeys.contains(WORD_DIVIDER); }

    /** Apply a label to this query segment */
    void setLabel(QString l) { m_currentItem.setLabel(l); }

    /** Returns the current owner id of the query */
    uint getID() { return m_currentItem.getItemId(); }

    /** Get the userKeys of the query segment */
    QString  getUserKeys() { return m_userKeys; }

    /** Set the userKeys of the query segment */
    void setUserKeys(QString t) { m_userKeys = t; }
    void appendUserText(QString k) {
        m_userKeys.append(k);
    }
    void popKey(int n=1){m_userKeys.chop(n);}

    CatItem& getCurrentItem() {
        return (m_currentItem);
    }

    void setCurrentResult(CatItem sr) {
        //Apps should always be args
        Q_ASSERT(sr.getItemType()!=CatItem::OPERATION);
        Q_ASSERT(!sr.isCustomVerb());
        m_currentItem = sr;
    }

    //Multi-Word stuff...
    QStringList getKeyWords(){
        if(!m_userKeys.contains(WORD_DIVIDER)){
            return QStringList();
        }
        return m_userKeys.split(WORD_DIVIDER, QString::SkipEmptyParts);
    }

    //TODO - actually keep track of context items...
    CatItem getContextItem(QString key){
        if(key[0]==CONTEXT_KEY){
            CatItem it(key.right(key.length()-1));
            return it;
        }
        return CatItem();
    }

    //
    void modifyUserText(QString newKeys, int ){
        this->m_userKeys = newKeys;
    }

};



class InputData {

private:
    QList<ListState> m_parentStack;
    VerbItem m_organizingArg;
    bool m_organizingArgChosen;

    //QList<ListItem> m_filterIcons;
    //ListItem m_filterItem;
    //ListItem m_filterItem;

public:

    ListState& curState(){
        Q_ASSERT(m_parentStack.count()>0);
        return m_parentStack.last();
    }

    bool argNeedsSettting(){
        if(!m_organizingArg.isEmpty()){
            return false;
        }

        return !m_organizingArgChosen;
    }

    bool isExpanded(){return curState().isExpanded();}
    void setExpanded(bool e=true){ curState().setExpanded(e);}

    QString getExpandedSearchStr(){return curState().getExpandedSearchStr();}
    void setExpandedSearchStr(QString str){curState().setExpandedSearchStr(str);}


    CatItem getFilterItem(){
        if(!curState().getFilterItem().isEmpty()){
            return curState().getFilterItem();
        }
//        if(m_parentStack.length()==1 && !m_filterItem.isEmpty()){
//            return m_filterItem;
//        }
        if(m_parentStack.length()==1 && !m_organizingArg.isEmpty()){
            return m_organizingArg;
        }
        return getParentItem();
    }


    void pushChildren(CatItem& parItem){
        QList<CatItem> childs = parItem.getRelatedItems();
        ListState ls;
        ls.addListItemList(childs);//m_contextItem
        if(m_parentStack.last().getCurrentItem().isEmpty()){
            m_parentStack.last().m_contextItem = parItem;
        }
        m_parentStack.append(ls);
    }


    CatItem contextItem(){
        if(m_parentStack.count()>1){
            if(!m_parentStack[m_parentStack.length()-2].getCurrentItem().isEmpty()){
                return m_parentStack[m_parentStack.length()-2].getCurrentItem();
            } else {
                return m_parentStack[m_parentStack.length()-2].m_contextItem;
            }
        } else {
            return CatItem();
        }
    }


    int popParent(){
        if(m_parentStack.length() >1){
            m_parentStack.pop_back();
        }
        return curState().getCurrentItemIndex();
    }

    CatItem navParent(){
        if(m_parentStack.count()>1){
            return m_parentStack[m_parentStack.length()-2].getCurrentItem();
        } else {
            return CatItem();
        }
    }

    bool isExploringChildren(){
        return (m_parentStack.count() >1);
    }

public:

    QString getExplorePath(int spaceAvailable){
        QString str;

        int count = m_parentStack.count();
        if(!curState().isExpanded()){
            count--;
        }
        for(int i=count-1; i >= 0; i--){
            //use full if we're at an object described by path
            if(!m_parentStack[i].getCurrentItem().isEmpty()){
                if(i){
                    if(str.length() > spaceAvailable -ELIPSIS_STRING_CHARS.length()){
                        str = ELIPSIS_STRING_CHARS + str;
                        break;
                    }
                }
                if((m_parentStack[i].getCurrentItem().hasLabel(FILE_CATALOG_PLUGIN_STR)
                    || m_parentStack[i].getCurrentItem().hasLabel(FILE_CATALOG_PLUGIN_STR))
                        &&(m_parentStack[i].getCurrentItem().getPath().length() < spaceAvailable)){
                    str = m_parentStack[i].getCurrentItem().getPath() + str;
                    break;
                } else {
                    if(!str.isEmpty()){
                        str = " "+ UI_SEPARATOR_STRING + " " + str;
                    }
                    str = m_parentStack[i].getCurrentItem().getName() + str;
                }
            }
        }
        return str;
    }

    //List items...
    int getCurrentItemIndex(){
        return curState().getCurrentItemIndex();
    }

    void setCurrentItemByIndex(int i){
        curState().setCurrentItemByIndex(i);
    }


    ListItem& getListItem(int i ){
        return curState().getListItem(i);
    }

    int getListItemCount(){
        return curState().getListItemCount();
    }

    void filterByItem(ListItem li){
        curState().setFilterItem(li);
    }

    void subFilterByItem(ListItem li){
        curState().setSubfilterItem(li);
    }


    CatItem getParentItem(){
        if(m_parentStack.length() > 1){
            return m_parentStack[m_parentStack.length()-2].getCurrentItem();
        } else {
            return CatItem();
        }

    }

    void addSingleListItem(CatItem item, int pos){
        curState().addSingleListItem(item, pos);
    }

    void removeItem(CatItem item){
        curState().removeItem(item);

    }

    void cullItemsForInsert(){
        curState().cullItemsForInsert();
    }

    void clearListItems(){
        curState().clearListItems();
    }

    CatItem addListItemList(QList<CatItem> il){
        return curState().addListItemList(il);
    }

    void intersperseListItemList(QList<CatItem> il){
        curState().intersperseListItemList(il);
    }

    QList<ListItem> getFormattedListItems(int itemsHigh, int charsAvail){
        return curState().getFormattedListItems(m_organizingArg, itemsHigh, charsAvail, m_parentStack.length());
    }

    QList<CatItem> getListItems(){
        return curState().getListItems();
    }

    bool isFilterItem(CatItem item){
        return curState().isFilterItem(item);

    }

    QList<ListItem> getFilterItems(int itemsHigh, int charsAvail, int minWeight){
        QList<ListItem> list = curState().getOrganizingFiltersItems(itemsHigh, charsAvail, minWeight);
        for(int i=0; i<list.count(); i++){
            if(list[i] == this->getFilterItem()){
                list[i].setLabel(ACTIVE_FILTER_ITEM_KEY);
            }
        }

        return list;
    }

    QList<ListItem> getFilterSubItems(int itemsHigh, int charsAvail, int minWeight){
        return curState().getOrganizingFilterSubItems(itemsHigh, charsAvail, minWeight);
    }

    void addOrganizingItems(QList<ListItem> baseFilterItems){
        curState().addOrganizingItems(baseFilterItems);
    }

    ListItem getItemByPath(QString path){
        return curState().getItemByPath(path);
    }

    ListItem& getItemByPathRef(QString path){
        return curState().getItemByPathRef(path);
    }

    ListItem findItemInParent(QString path){
        for(int i=this->m_parentStack.length()-2; i>=0; i-- ){
            ListItem li= m_parentStack[i].getItemByPath(path);
            if(!li.isEmpty()){
                return li;
            }
        }
        return ListItem();
    }

    void popToParentPosition(QString path){
        if(findItemInParent(path).isEmpty()){ return;}

        for(int i=this->m_parentStack.length()-1; i>=0; i-- ){
            ListItem li= m_parentStack[i].getItemByPath(path);
            if(li.isEmpty()){
                m_parentStack.pop_back();
            } else {
                break;
            }
        }
        Q_ASSERT(!getItemByPath(path).isEmpty());
    }

    ListItem getItemByKey(QChar k){
        return curState().getItemByKey(k);
    }


public:

    InputData(){
        ListState ls;
        m_parentStack.append(ls);
        m_organizingArgChosen = false;
    }

    InputData(QString str){
        ListState ls;
        ls.setUserKeys(str);
        m_parentStack.append(ls);
        m_organizingArgChosen = false;
    }

    InputData(const InputData& id){
        m_parentStack = id.m_parentStack;
        m_organizingArg = id.m_organizingArg;
        m_organizingArgChosen = id.m_organizingArgChosen;
        //m_filterItem = id.m_filterItem;
    }

    bool testIsPhrase(){
        return curState().testIsPhrase();
    }

    /** Apply a label to this query segment */
    void setLabel(QString l) { curState().setLabel(l); }

    /** Returns the current owner id of the query */
    uint getID() { return curState().getID(); }

    /** Get the userKeys of the query segment */
    QString  getUserKeys() { return curState().getUserKeys(); }

    /** Set the userKeys of the query segment */
    void setUserKeys(QString t) { curState().setUserKeys(t); }
    void appendUserText(QString k) { curState().appendUserText(k); }
    void popKey(int n=1){curState().popKey(n);}

    void setCurrentItem(CatItem sr) {
        curState().setCurrentResult(sr);
        if(!sr.isEmpty() && !m_organizingArgChosen ){/*|| (!m_organizingArg.getTagLevel() == CatItem::EXPLICIT_TAG
                                      &&(m_organizingArg.getItemType()!=CatItem::OPERATION))*/
            Q_ASSERT((m_organizingArg.getItemType()!=CatItem::OPERATION));
            qDebug() << "setCurrentItem clearing organizing arg";
            m_organizingArg = CatItem();
        }
    }

    CatItem& getCurrentItem() {
        return curState().getCurrentItem();
    }
    CatItem getCurrentValue() { return curState().getCurrentItem(); }
    CatItem getStandardVerb() {
        if(getParentItem().getItemType() == CatItem::VERB){
            return getParentItem();
        } else {
            return curState().getCurrentItem().getItemDefaultVerb();
        }
    }

//    bool showFormatted(){
//        return (!getCurrentItem().isEmpty()
//                 ||
//                 !getOrganizingArg().isEmpty()
//                 ||
//            (usingCustomVerb() && !inputData[m_itemPosition].getUserKeys().isEmpty()) );
//
//    }

    void clearUnmatchedItem(){
        CatItem it = curState().getCurrentItem();
        if(!it.isEmpty()){
            QString keys = getUserKeys();
            if (!matches( &it, keys)){
                curState().setCurrentResult(CatItem());
            }
        }
    }

    //Multi-Word stuff...
    QStringList getKeyWords(){
        return curState().getKeyWords();
    }

    //TODO - actually keep track of context items...
    CatItem getContextItem(QString key){
        return curState().getContextItem(key);
    }

    //
    void modifyUserText(QString newKeys, int i){
        curState().modifyUserText(newKeys, i);
    }

    VerbItem& getOrganizingArg(){
        return m_organizingArg;
    }

    void setOrganizingArg(VerbItem li){
        m_organizingArg = li;
    }

    bool isOrganizingArgChosen(){
        return m_organizingArgChosen;
    }

    void clearAll(){
        curState().clearAll();
        m_organizingArg = VerbItem();
        m_organizingArgChosen = false;
    }

    void clearItem(){
        curState().clearItem();
        m_organizingArg = VerbItem();
        m_organizingArgChosen = false;
    }

    //An verb organizing arg can be either "just visiting" or chosen
    void setOrganizingArgChosen(bool chosen=true){
        m_organizingArgChosen = chosen;
    }
};


#endif //INPUT_DATA_H
