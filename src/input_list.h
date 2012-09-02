#ifndef INPUT_LIST_H
#define INPUT_LIST_H

#include "item.h"
#include "input_data.h"

//#include "item_rep.h"
//
//#include "itemarglist.h"

#include "itemWSHash.h"

class ItemRep;

class ItemFilter {

    public:
        virtual bool acceptItem(CatItem* ){ return true; }
        virtual CatItem::ItemType getOrganizeingType(){ return CatItem::MIN_TYPE; }
        virtual void setSearchLevel(int ){ }
        virtual int getSearchLevel(){ return 0;}

        virtual CatItem getOrganizingParent(){ return CatItem();}
        virtual CatItem getFilterItem(){ return CatItem();}
        virtual CatItem getParentItem(){ return CatItem();}
        virtual QList<CatItem> getListItems(){ return QList<CatItem>();}
        virtual QList<CatItem::ItemType> getOrganizingTypeList(){ return QList<CatItem::ItemType>();}

};


struct SearchInfo {
    //InputList_old* userInfoPtr;
    SearchInfo(){
        m_extensionType=UserEvent::STANDARD_UPDATE;
        lastUpdate=0;
        pluginId=0;
        m_subthread=true;

    }

    QList<CatItem>* itemListPtr;
    QString m_userKeys;
    QStringList m_keyWords;
    qint32 pluginId;
    time_t lastUpdate;
    QString attributeToUpdate;
    UserEvent::LoadType m_extensionType;
    bool m_subthread;
};

struct CustomFieldInfo {

    CustomFieldInfo(){
        fieldTransition = UNDETERMINED;
        showUrl = true;
        canTab = false;
        mustTab = false;
        windowTakesKeys = true;
    }


    enum FieldTransition {
        UNDETERMINED,
        JUST_APPEARING,
        URL_CHANGED,
        FIELD_CHANGED,
        FIELD_ACCEPTED,
        TABBING_PAST
    } fieldTransition;

    InputList* inputList;
    QString newKeys;
    QString fieldExpression;
    QString pluginSource;
    QString existingUrl;
    QString newUrl;
    bool showUrl;
    QString errorMessage;
    bool canTab;
    bool mustTab;
    bool windowTakesKeys;

    QRect windowSize;

};



#endif // INPUT_LIST_H
