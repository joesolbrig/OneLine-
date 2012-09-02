#ifndef ITEM_REP_H
#define ITEM_REP_H

#include "item.h"

#include "util/mmap_file/btree.h"


/* Facade Class for CatItem -  It's used to store the keystrokes matching a given CatItem in TrieNode

Lots of these are created, should be as small as practical... clear unneed stuff in storage...

*/

class ItemRep {
    protected:
        Tuple m_internalId;
        QString m_name;
        QList<int> matchIndex;
        short change_cn;
        short l_to_match;
        bool skip_m;
        long totalWeight;

        //for filtering
        CatItem::ItemType itemType;

        bool m_name_from_description;
        short m_nthPiece;

    public:
        ItemRep(){
            matchType = CatItem::USER_KEYS;
            totalWeight = -1;
            l_to_match = -1; //Nothing matched yet...
            change_cn = 0;
            skip_m = false;
        }
        ItemRep(const ItemRep& i){
            matchIndex = i.matchIndex;
            matchType = i.matchType;
            //description = i.description;
            m_internalId = i.m_internalId;
            m_name = i.m_name;
            totalWeight = i.totalWeight;
            l_to_match = i.l_to_match;
            change_cn = i.change_cn;
            l_to_match = i.l_to_match;
            skip_m = i.skip_m;
            itemType = i.itemType;

        }

        ItemRep(ItemRep& i){
            matchIndex = i.matchIndex;
            matchType = i.matchType;
            m_internalId = i.m_internalId;
            m_name = i.m_name;
            //description = i.description;
            totalWeight = i.totalWeight;
            l_to_match = i.l_to_match;
            change_cn = i.change_cn;
            skip_m = i.skip_m;
            itemType = i.itemType;
        }

        ItemRep(QString , QString fp, Tuple id=Tuple(),
                CatItem::MatchType m= CatItem::NOT_MATCHED,
                short nthPiece=0, bool nameFromDescript=false){
            m_internalId = id;
            m_name = fp;
            //description = "";
            matchType = m;
            totalWeight = -1;
            l_to_match = -1;
            change_cn = 0;
            skip_m = false;
            m_nthPiece= nthPiece;
            m_name_from_description=nameFromDescript;
        }
        ItemRep(CatItem it, Tuple id=Tuple(), int change_count = 1,
                short nthPiece=0, bool nameFromDescript=false);
        ItemRep& operator=( const ItemRep &it ){
            m_internalId = it.m_internalId;
            m_name = it.m_name;
            matchIndex = it.matchIndex;
            matchType = it.matchType;
            totalWeight = it.totalWeight;
            change_cn = it.change_cn;
            l_to_match = it.l_to_match;
            skip_m = it.skip_m;
            return *this;
        }

        bool operator==( const ItemRep &it ){
            return m_internalId == it.m_internalId;
        }

        int getNthPiece(){
            return m_nthPiece;

        }

        //One or two items
        //l_to_match starts at -1 to let us return 1st item...
        QList<ItemRep> nextItems(){
            Q_ASSERT(!m_name.isEmpty());
            QList<ItemRep> res;
            ItemRep i = *this;
            i.l_to_match++;
            if(i.l_to_match < MAX_CHAR_LEN_FOR_KEY_MATCH){
                bool is_con = isConsonant(m_name[i.l_to_match]);
                if(skip_m && !is_con) {
                    if(change_cn < KEY_SKIP_SWITCH_LIMIT){
                        i.change_cn++;
                        i.skip_m = false;
                        res.append(i);
                    }
                    //else skip
                } else { res.append(i); }
            }

            if ( (change_cn >= KEY_SKIP_SWITCH_LIMIT) && !skip_m ){return res;}

            ItemRep i2 = i;
            i2.l_to_match++;
            if(!i2.next_consonant()){return res;}
            res.append(i2);
            return res;
        }

        //For removal...
        bool next_char(){
            Q_ASSERT(!m_name.isEmpty());
            l_to_match++;
            if(l_to_match == m_name.length()){
                return false;
            }
            bool is_con = isConsonant(m_name[l_to_match]);
            if(skip_m && !is_con) {
                if(change_cn < KEY_SKIP_SWITCH_LIMIT){
                    change_cn++;
                    skip_m = false;
                }
                return false;
            }
            matchIndex.append(l_to_match);
            return true;
        }
        void next_pos(){
            l_to_match++;
        }
        bool next_consonant(){
            Q_ASSERT(!m_name.isEmpty());
            if(!skip_m) { change_cn++;}
            skip_m = true;
            while(l_to_match < m_name.length()){
                if( isConsonant(m_name[l_to_match])){
                    matchIndex.append(l_to_match);
                    return true;
                }
                l_to_match++;
            }
            return false;
        }
        QChar char_at(){
            Q_ASSERT(!m_name.isEmpty());
            return m_name[l_to_match].toLower();
        }
        bool at_end(){
            Q_ASSERT(!m_name.isEmpty());
            return l_to_match >= m_name.length();
        }
        int get_position(){
            return l_to_match;
        }

        int getChangeCount(){
            return change_cn;

        }
        QString getName(){
            return m_name;
        }

//        QString getPath(){
//            return m_path;
//        }

        void setName(QString s){
            m_name = s;
        }

        void clearName(){
            m_name.clear();
        }



        void set_position(int pos){
            l_to_match = pos;
        }

        long get_weight(){
            Q_ASSERT(totalWeight > 0);
            return totalWeight;
        }

        CatItem::ItemType getItemType(){
            return itemType;
        }

        void setItemType(CatItem::ItemType t){
            itemType = t;
        }

        QList<int> getMatchIndex(){return matchIndex;}
        CatItem::MatchType matchType;


        void setMatchKey(int k){
            matchIndex.append(k);
        }

        void setMatchKeyList(QList<int>  kl){
            matchIndex = kl;
        }
        Tuple getInternalId(){return m_internalId;}

        //TODO - remove this duplicate logic somehow??
        CatItem::ActionType getActionType(){
            if( itemType ==CatItem::VERB){
                return CatItem::VERB_ACTION;
            }
            if( (int)itemType <=(int)CatItem::OPERATION){
                return CatItem::OPERATION_ACTION;
            }
            return CatItem::NOUN_ACTION;
        }


        long allocatedSize(){
            long allocSize = sizeof(ItemRep);
            allocSize += m_name.capacity();
            allocSize += matchIndex.length();
            return allocSize;
        }

};


#endif // ITEM_REP_H
