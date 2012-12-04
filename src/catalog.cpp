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

#include "globals.h"
#include "main.h"
#include "options.h"

#include <QFile>
#include <QDataStream>
#include <qglobal.h>
#include "catalog.h"
#include "item_rep.h"
#include "cat_builder.h"
#include "util/mmap_file/btree_impl.h"

PluginHandler* CatBuilder::plugins_ptr;
PluginHandler Catalog::plugins;
QString Catalog::curFilterStr;


Catalog::Catalog(QHash<QString, QList<QString> > dirs, QSet<QString> pluginFilter):
        cat_store(true, (dirs[LOCAL_DIR])[0]) {
    m_dirs = dirs;
    m_pluginFilter = pluginFilter;
}

Catalog::~Catalog(){}

void Catalog::loadPlugins(){
    QList<CatItem> outList = plugins.loadPlugins(m_dirs, m_pluginFilter);
    indexItemList(&outList);
}

QList<CatItem> Catalog::indexItemList(QList<CatItem>* catListPtr){
    Q_ASSERT(catListPtr);
    QList<CatItem> outList;
    cat_store.beginAddGroup();
    for(int i=0;i< catListPtr->count();i++){
        CatItem it = (*catListPtr)[i];
        if(it.isEmpty()){
            continue;
        }
        if(it.isForDBInsertOnly()){
            cat_store.addItemInGroup(it);
            continue;
        } else if(it.getIsTempItem()){
            outList.append((it));
        } else {
            outList.append(cat_store.addItemInGroup(it));
        }
        //THREAD_SLEEP;
    }
    cat_store.endAddGroup();
    return outList;
}

void Catalog::addItem(CatItem item){
    cat_store.addItem(item);
}


CatItem Catalog::setExecuted(InputList* inputList){
    return cat_store.setItemExecuted(inputList);
}

void Catalog::setShown(CatItem it, UserEvent etv){
    cat_store.addVisibilityEvent(it,etv);
}

QList<CatItem> Catalog::extractStubs(CatItem& it){
    QList<CatItem> res;
    QList<CatItem> items = it.getParents();
    it.setChildStubbed(false);
    items.append(it.getChildren());
    for(int i=0; i<items.count();i++){
        if(items[i].isStub()){
            res.append(items[i]);
        }
    }
    return res;
}

QList<CatItem> Catalog::expandStubs(QString userKeys, QList<CatItem>* inList){

    QList<CatItem> stubs; QList<CatItem> oldStubs;
    QList<CatItem>* workingList = inList;
    QList<CatItem> res;
    bool refreshNeeded =false;
    int rep=0;

    QSet<QString> updated;
    while((workingList->length()>0)) {
        cat_store.beginAddGroup();
        for(int i=0; i<workingList->count();i++){
            CatItem toAdd = workingList->at(i);
            bool atStub = toAdd.isStub();
            if(plugins.itemLoading(&toAdd, UserEvent::BECOMING_VISIBLE)){
                if(toAdd.isForDelete()) {
                    cat_store.removeItem(workingList->at(i));
                    continue;
                }
                if(atStub) {
                    refreshNeeded = true;
                    //toAdd.setStub(false);
                    cat_store.addItemInGroup(toAdd);
                }
            }
            QList<CatItem> newStubs = extractStubs(toAdd);
            for(int j=0; j<newStubs.count(); j++){
                if(!updated.contains(newStubs[j].getPath())){
                    updated.insert(newStubs[j].getPath());
                    stubs.append(newStubs[j]);
                }
            }
        }
        oldStubs = stubs;
        workingList = &oldStubs;
        cat_store.endAddGroup();
        stubs.clear();
        Q_ASSERT(workingList->count() <1000);
        rep++;
        Q_ASSERT(rep<10);
    }

    cat_store.beginAddGroup();
    for(int i=0; i<inList->count();i++){
        CatItem toAdd = inList->at(i);
        if(toAdd.isForDBInsertOnly()){
            cat_store.addItemInGroup(toAdd);
            continue;
        }
        if(userKeys.isEmpty() || matches(&toAdd, userKeys) || toAdd.getTakesAnykeys()) {
            if(refreshNeeded){
                CatItem refreshed = cat_store.getItemByPath(toAdd.getPath());
                if(!refreshed.isEmpty()){
                    toAdd.mergeItem(refreshed,true);
                }
                toAdd.setMatchIndex(QList<int>());
            }
            if(!toAdd.hasLabel(BUILTIN))
                { res.append(toAdd);}
        }}
    cat_store.endAddGroup();
    return res;
}

bool Catalog::getItemsFromQuery(InputList &inputData, QList<CatItem>& outResults,
                                int itemsDesired,
                                int* beginPos){

    QString userKeys = inputData.getUserKeys();
    QList<CatItem> partialList = parseRequest(inputData,itemsDesired, beginPos);
    plugins.getLabels(&inputData);
    plugins.getResults(&inputData, &partialList);

    outResults = expandStubs(inputData.getUserKeys(), &partialList);

    //Skipping because it should happen in more detail later...
    //...
    //Sort by single keys ... even if we're on multiple words otherwise
    if(inputData.selectingByKeys()){
        Catalog::curFilterStr = userKeys;
        qSort(outResults.begin(), outResults.end(), CatLessNoPtr);

        while(outResults.count() > itemsDesired ){
            outResults.pop_back();
        }
    }

    return true;
}

void Catalog::getMiniIcons(InputList & inputData){
    QList <ListItem> miniIconItems = cat_store.getOrganizingSources(&inputData);
    inputData.setOrganzingItems(miniIconItems);
}

QList<CatItem> Catalog::parseRequest(InputList & inputData, int itemsDesired, int* beginPos){


    if(inputData.customVerbChosen() && !inputData.isOperationChosen()){
        return cat_store.getItemsByKey(inputData.getUserKeys(), &inputData, itemsDesired);
    }
    if(inputData.isOperationChosen()){
        return getOperationChildren(inputData);
    }

    if(!inputData.getParentItem().isEmpty() ){
        if(inputData.getParentItem().canFilterWith()){
            return cat_store.getAssociatedDocuments(&inputData, inputData.getParentItem(), itemsDesired);
        } else {
            CatItem par = cat_store.getItemByPath(inputData.getParentItem().getPath(),1);
            CatBuilder::updateItem(par,2,UserEvent::SELECTED);
            return par.getChildren();
        }
    }

    QList<CatItem> res;
    if(inputData.getUserKeys().length() >0){
        QString userKeys = inputData.getUserKeys();
        res.append(cat_store.getItemsByKey(userKeys, &inputData, itemsDesired, beginPos));
    } else if(inputData.slotCount() ==1 || inputData.isOperationChosen()){
        res.append(cat_store.getInitialItems(&inputData, itemsDesired, beginPos));
    }
    if(res.count()>0 && beginPos){ (*beginPos)++; }


    if(inputData.slotCount()>1 ){
        QList<CatItem> extra_actions;
        if(inputData.isCustomVerbChoosableByArgSig())
            { extra_actions = parseCustomVerbSelection(inputData); }

        QList<CatItem> operations = getOperations(inputData);

        if(extra_actions.count() > MAX_SEPERATE_VERBS){
            res.push_front(
                    CatItem::createTempfolder(EXTRA_ACTIONS_PSUEDO_FOLD_NAME, extra_actions));
        } else
            { res.append(extra_actions); }
        res.append(operations);


        //At "bare" verb - possibly w/ operation
        if(inputData.verbPosition() ==0){
            if(inputData.hasVerb() && !inputData.customVerbChosen() && inputData.nounList().length()==0){
                res.push_front(CatItem::createRunPseudoItem());
            }
            res.append(cat_store.getAssociatedDocuments(&inputData, inputData.getFilterItem(), itemsDesired));
        }
        if(inputData.atPossibleVerbSlot()){
            inputData.setDefaultVerbIfNeeded();
            CatItem defVerb = inputData.getItemDefaultVerb();
            if(!defVerb.isEmpty()) { res.append(defVerb); }

            QList<CatItem> associations = inputData.getNormalAssociation();
            CatItem defaultVerb = inputData.getItemDefaultVerb();
            if(!defaultVerb.isEmpty())
                { res.append(defaultVerb); }
            if(associations.count() > MAX_SEPERATE_VERBS){
                res.push_front(
                        CatItem::createTempfolder(ASSOCIATED_APPS_PSUEDO_FOLD_NAME, associations));
            } else
                { res.append(associations); }
        }
    }

    CatItem curArg = inputData.argChosenType();
    if(!curArg.isEmpty())
        { res.append(cat_store.getItemChildren(&inputData, curArg)); }
    qDebug() << res.count() << " items found";

    return res;
}

QList<CatItem> Catalog::getOperations(InputList & inputData){
    QList<CatItem> res;
    CatItem target = inputData.getStatementObject();
    CatBuilder::updateItem(target,2,UserEvent::SELECTED);
    QList<CatItem> tags = (target.getTypeParents(BaseChildRelation::TAG_PARENT));
    if(tags.count()>0){
        CatItem removeTagItem(OP_REMOVE_PATH,REMOVE_TAG_NAME);
        removeTagItem.setItemType(CatItem::OPERATION);
        removeTagItem.setOrganizingType(CatItem::TAG);
        removeTagItem.setTakesAnykeys(true);
        removeTagItem.setMatchType(CatItem::EXTERNAL_INFO);
        removeTagItem.setIcon(TAG_ICON_NAME);
        removeTagItem.setUseLongName(true);
        removeTagItem.setChildTypeToTake(BaseChildRelation::TAG_PARENT);
        removeTagItem.setIsTempItem(true);
        res.append(removeTagItem);
    }
    res.append(cat_store.getOperations(&inputData, 30));
    return res;
}

QList<CatItem> Catalog::getOperationChildren(InputList & inputData){
    CatItem target = inputData.getStatementObject();
    CatBuilder::updateItem(target,2,UserEvent::SELECTED);
    CatItem tag =inputData.getOperationTarget();
    if(tag.getchildTypeToTake()!=(BaseChildRelation::ChildRelType)0){
        return target.getTypeParents(tag.getchildTypeToTake());
    } else {
        return cat_store.getItemsByKey(inputData.getUserKeys(), &inputData, 30);
    }
}

QList<CatItem> Catalog::parseCustomVerbSelection(InputList & inputData){
    QList<CatItem> res;
    //Get any potential multi-field verbs/operations
//    QHash<qint32, CatItem> matchActions;
    QList<CatItem> rawActions;
    QList< QList < CatItem > > signatures = inputData.getCustomVerbSignature();

    //It can have types matched if it lacks an organizing verb
    for(int i=0;i<signatures.count();i++){
        rawActions = cat_store.getCustomVerbByActionType(signatures[i][0]);
        res.append(rawActions);
    }
//    for(int i=0;i<signatures.count();i++){
//        rawActions = cat_store.getCustomVerbByActionType(signatures[i][0]);
//        for(int j=0;j<rawActions.count();j++){
//            matchActions[rawActions[j].getItemId()] = rawActions[j];
//        }
//    }
//
//    QHash<qint32, CatItem>::iterator itter = matchActions.begin();
//    for(;itter != matchActions.end();itter++)
//        { res.append(itter.value()); }
    return res;

}





bool CatLessNoPtr (CatItem & a, CatItem & b) {
    return CatLess(&a, &b);
}

bool WeightLess (CatItem & a, CatItem & b) {
    return (a.getFullWeight() > b.getFullWeight());
}

bool CatLess (CatItem* a, CatItem* b)  {

    //Key-matching first
    QString matchStr = Catalog::curFilterStr;
    QList<int> aIndex;
    int match_a = consonant_match(*a,matchStr, &aIndex);
    QList<int> bIndex;
    int match_b = consonant_match(*b,matchStr, &bIndex);

    if(a->getMatchIndex().length()==0){
        a->setMatchIndex(aIndex);
    }
    if(b->getMatchIndex().length()==0){
        b->setMatchIndex(bIndex);
    }

    //Then various ordering factors...
    if(a->hasLabel(ITEM_PIN_KEY) || b->hasLabel(ITEM_PIN_KEY)){
        if(!a->hasLabel(ITEM_PIN_KEY)){
            return false;
        }
        if(!b->hasLabel(ITEM_PIN_KEY)){
            return true;
        }
    }

    if(a-> isOrderedBySibling() && b->isOrderedBySibling()){
        return (a->getSiblingOrder() < b->getSiblingOrder());
    }

    if(a->getMatchType() != CatItem::USER_KEYS || b->getMatchType() != CatItem::USER_KEYS){
        if(a->getMatchType() == CatItem::USER_KEYS){
            return true;
        }
        if(b->getMatchType() == CatItem::USER_KEYS){
            return false;
        }
        return (a->getFullWeight() < b->getFullWeight()); //Full weight works oppositely
    }



    Catalog::curFilterStr = Catalog::curFilterStr.toLower();

    bool localEqual = (a->getLowName()== matchStr);
    bool otherEqual = (b->getLowName() == matchStr);

    if (localEqual && !otherEqual) {return true;}
    if (!localEqual && otherEqual) {return false;}

    // Match Index should already be calculated
    // if this becomes costly...

    //First absolute match tested
    int localFind = a->getLowName().indexOf(matchStr);
    int otherFind = b->getLowName().indexOf(matchStr);

    //Both pos, return closest to front
    if (localFind != -1  && otherFind != -1) {return (localFind <= otherFind);}
    //Return the one which is negative
    if (localFind != -1  || otherFind != -1) {return (localFind >= otherFind);}

    //We need a function which increases at a decreasing rate
    // So use the sqrt - change if costly...
    int extra_a = a->getMatchIndex().count() > 0 ? sqrt( a->getMatchIndex()[0])/3 : 0;
    int extra_b = b->getMatchIndex().count() > 0 ? sqrt( b->getMatchIndex()[0])/3 : 0;

    localFind = match_a - extra_a;
    otherFind = match_b - extra_b;
    if (localFind > otherFind)
        return true;
    if (localFind < otherFind)
        return false;

    if(a->getFullWeight() < b->getFullWeight())
        return true;
    if (a->getFullWeight() > b->getFullWeight())
        return false;

    //Stuff to make sure unequal items return false...
    int localLen = a->getLowName().count();
    int otherLen = b->getLowName().count();

    if (localLen < otherLen)
        return true;
    if (localLen > otherLen)
        return false;

    // Absolute tiebreaker to prevent loops
    if (a->getPath() < b->getPath())
        return true;
    return false;

}

bool isVowell(QChar v){
    if ( v > 'A' || v < 'z')
        return false;
    v = v.toLower();
    switch (v.toAscii())
    {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
        case 'y':
            return true;
        default:
            break;
    }
    return false;
}
