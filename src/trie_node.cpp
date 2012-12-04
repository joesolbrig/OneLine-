#include <QIcon>
#include "cat_store.h"
#include "cat_builder.h"
const int Cat_Store::MAX_SOURCE_WEIGHT=60;

unsigned long TrieNode::m_node_count=0;

const int max_trie_leaves = 10;
const int n_to_keep = 20;
//const int extra_items = 5;


void TrieNode::insert(CatItem it, QList<int> kl ,
            int charChanges,Tuple tpl){
    insertFull(it, kl,charChanges,tpl  );
//    if(tpl.isEmpty()){
//        tpl = container->getInternalId(it);
//    }
//    ItemRep ir(it, tpl, charChanges);
//    if(kl.length() > 0)
//        {ir.setMatchKeyList(kl);}
//    insertRep(ir);

}

void TrieNode::insertFull(CatItem it, QList<int> kl,
                           int charChanges,Tuple tpl  )
{
    if(!it.hasRealName()){ return;}

    if(tpl.isEmpty())
        {tpl = mp_container->getInternalId(it); }

    ItemRep ir(it, tpl, charChanges);
    if(kl.length() > 0)
        {ir.setMatchKeyList(kl);}
    insertRep(ir);

    if(it.getName().contains(" ")){
        QList<QString> descriptionWords = it.getDescription().split(' ');
        for(int i=0; i<MIN(descriptionWords.length(),MAX_WORDS_IN_KEY_SEARCH); i++){
            if(!descriptionWords[i].isEmpty() && descriptionWords[i].count()>5){
                addItemPiece(it,tpl,descriptionWords[i],i, false);
            }
        }
    }
}

void TrieNode::addItemPiece(CatItem it, Tuple tpl, QString pieceName, int position, bool is_description){
    int charChanges = MIN((log(it.getFullWeight())-1), KEY_SKIP_SWITCH_LIMIT-1);
    ItemRep ir(it, tpl, charChanges, position, is_description);
    ir.setName(pieceName);
    insertRep(ir);

}


//Limiting the number of skips and length of areaw/ change_cn should limit
//the number of the nodes created
void TrieNode::insertRep(ItemRep ir, int length_limit){

    Q_ASSERT(mp_container);
    //ignore duplicates...
    QList<ItemRep> expelledItems;
    bool used;
    if(addItem(ir, &expelledItems, &used)){
        return;
    }
    m_child_total++;

    if(ir.at_end()){ return;}

    //This limits nodes to situations where they distinguish
    //between items with intersecting paths...
    QList<ItemRep> toAdd;
    
    for(int i=0; i < expelledItems.count(); i++){
        if(length_limit <= MAX_KEYS || expelledItems[i].getChangeCount() < KEY_SKIP_SWITCH_LIMIT-1){
            toAdd.append(expelledItems[i].nextItems());
        }
    }
    Q_ASSERT(m_bestItems.size() < n_to_keep*2);

    // ... we carefully treading between elimentating
    // entries and creating too many nodes....
    //
    if(length_limit > MAX_KEYS && ir.getChangeCount() >KEY_SKIP_SWITCH_LIMIT-2) {
        //The item is simply not added....
        return;
    }

    if(!used){ toAdd.append(ir.nextItems()); }

    for(int i=0;i<toAdd.length();i++){
        QChar c = toAdd[i].char_at();
        if(!m_entries[encode(c)]){
            TrieNode *tn = new TrieNode(mp_container);
            m_entries[encode(c)] = tn;
        }
        m_entries[encode(c)]->insertRep(toAdd[i], length_limit+1);
    }
}


//This routine needs to be fast!
//This ONLY return false IF the item has been USED!
bool TrieNode::addItem(ItemRep ir, QList<ItemRep>* expelledList, bool* used){
    Q_ASSERT(expelledList);
    Q_ASSERT(used);
    *used = false;
    Tuple itemId = ir.getInternalId();

    int l = m_bestItems.count();
    l = l;
    Q_ASSERT(l <= n_to_keep);

    QLinkedList<ItemRep>::iterator i = m_bestItems.begin();
    for(; i!=m_bestItems.end();i++){
        if(itemId == (*i).getInternalId()){

            if((*i).getNthPiece() == ir.getNthPiece() ){
                (*i) = ir;
                return true;
            } else {
                //We can't have multiple copies of the same thing
                //on the same node only distinguished by piece
                return false;
//                QString origName = (*i).getName();
//                (*i) = ir;
//                (*i).setName(origName);
            }
        }
    }

    i = m_bestItems.begin();
    const QLinkedList<ItemRep>::const_iterator theEnd = m_bestItems.end();
    for(int cnt=0; i!=theEnd;cnt++, i++){
        if(ir.get_weight() < (*i).get_weight() &&
           (cnt < (n_to_keep) || (ir.getItemType() == (*i).getItemType()))){
            i = m_bestItems.insert(i,ir);
            *used = true;
            while(m_bestItems.count() >= n_to_keep ){
                ItemRep extra = m_bestItems.last();
                m_bestItems.pop_back();
                expelledList->append(extra);
                return false;
            }
            return false;
        }
    }
    if(m_bestItems.count() < n_to_keep-1 ){
        m_bestItems.push_back(ir);
        *used = true;
        return true;
    }

    return false;

    //We always want to include the literal match and up (n_to_keep - 1) others
}



QList<CatItem> TrieNode::find(QString substr,int desired, ItemFilter* filter, int* beginPoint){
    QList<CatItem> res = findProtected(substr,desired, 0, filter, beginPoint);
    return res;
}

QList<CatItem> TrieNode::findProtected(QString substr, int desired,
                            int nthChar, ItemFilter* filter, int* beginPoint){
    //If there's only one choice, it's always best...
    //Let's us cull a lot of nodes...
    //Q_ASSERT(!substr.isEmpty());


    //We have to filter best item ALONG THE WAY!!!
    QChar c='a';
    QString& userKeys = substr;
    if(nthChar < userKeys.length()){ c= userKeys[nthChar].toLower(); }

    short int enCoded = encode(c);
    TrieNode* childEntry = m_entries[enCoded];

    if( nthChar >= substr.length() || (childEntry==0)){
        return filterChildNodes(userKeys, desired, filter, beginPoint);
    } else {
        QList <CatItem> res = filterCurrentNode(userKeys, desired, filter);
        res.append(childEntry->findProtected(userKeys, desired, nthChar+1,filter, beginPoint));
        return res;
    }
}

QList<CatItem> TrieNode::filterCurrentNode(QString substr, int numDesired, ItemFilter* filter){

    QHash<qint32,CatItem> alreadyFound;
    int depth = 0;
    int digDepth =0;
    breadthSearch(alreadyFound, substr, numDesired, depth, digDepth, 0, filter);
    return alreadyFound.values();
}

QList<CatItem> TrieNode::filterChildNodes(QString substr, int numDesired, ItemFilter* filter,
                                          int* beginPoint){
    QHash<qint32,CatItem> alreadyFound;
    int depth = DEFAULT_NODE_SEARCH_DEPTH;
    int digDepth =0;
//    int skipDepth =0;
//    if(beginPoint!=0){
//        skipDepth = *beginPoint;
//    }
    int skipDepth = beginPoint? *beginPoint: 0;
    breadthSearch(alreadyFound, substr, numDesired, depth, digDepth, skipDepth, filter);
    return alreadyFound.values();
}

bool TrieNode::breadthSearch(QHash<qint32,CatItem>& alreadyFound, QString matchString,
                    int desired, int depth, int digDepth, int skipDepth, ItemFilter* filter){


    bool worthLookingFurther = (digDepth >0);
    if(skipDepth<=0){
        worthLookingFurther = true;

        QLinkedList<ItemRep>::iterator i = m_bestItems.begin();
        for(;i != m_bestItems.end();i++){

            //This step optimizes - skip what you know is wrong before paying for a full check
            QList<int> outIndex;
            CatItem it_approx((*i).getName(),(*i).getName());
            if(!matchString.isEmpty() && !userkey_match(it_approx,matchString, &outIndex))
                { continue; }

            CatItem::ItemType mt = (*i).getItemType();
            if(!CatItem::matchOrganizingTypes(filter->getOrganizeingType(),mt))
                {continue;}

            //Now the "real" matching
            CatItem it = mp_container->getByInternalIdProtectMe((*i).getInternalId());
            outIndex.clear();

            //We want to allow names that are parts of the main name...
            if(!matchString.isEmpty() && !userkey_match(it,matchString, &outIndex)){
                if(matchString.length() < MIN_SUBSTR_MATCH_LEN)
                    { continue; }
                if(!it.getName().contains(matchString))
                    { continue; }
            }

            if (!filter || filter->acceptItem(&(it) )){
                worthLookingFurther =true;
                if(outIndex.length() > 0){
                    it.setMatchIndex(outIndex);
                }
                //it.setMatchIndex((*i).getMatchIndex());
                alreadyFound[it.getItemId()] = it;
            }
            if(alreadyFound.count() > desired){ return false; }
        }
    }


    if(depth<=0){ return true; }

    if(worthLookingFurther || matchString.length() <=BROAD_KEYSEARCH_MAXLEN ||
       (skipDepth) > 0) {
        for(int i=0;i < m_entries.count();i++){
            if(!m_entries[i]){continue;}
            m_entries[i]->breadthSearch(alreadyFound, matchString, desired, depth-1,
                                      digDepth-1,
                                      skipDepth-1, filter);
            if(alreadyFound.count() > desired){ return false; }
        }
    }

    return worthLookingFurther;
}
