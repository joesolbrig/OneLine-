#ifndef TRIE_NODE_H
#define TRIE_NODE_H

#include <QHash>
#include <QList>
#include <QVector>
#include <QLinkedList>
#include "cat_store.h"
#include "item_rep.h"
#include "input_list.h"

typedef int RelationType;
class Cat_Store;
class TrieNode;
class ItemFilter;

const int KEY_POSITIONS=30;


class TrieNode {
private:
    Cat_Store* mp_container;
    QVector<TrieNode*> m_entries;
    QLinkedList<ItemRep> m_bestItems;
    int m_child_total;
    static unsigned long m_node_count;
public:
    //Can't assert container here since it's sometimes created automatically
    //But any time I do something with a trinode, I need a container...
    TrieNode(Cat_Store* cs=0):
        mp_container(cs), m_entries(KEY_POSITIONS, 0) {
        //for(int i=0;i<KEY_POSITIONS;i++){ m_entries[i]=0;}
        m_child_total = 0;
    }

    TrieNode& operator=(TrieNode& s){
        m_entries=s.m_entries;
        m_bestItems = s.m_bestItems;
        mp_container = s.mp_container;
        m_child_total = s.m_child_total;
        Q_ASSERT(mp_container);
        return *this;
    }

    ~TrieNode(){
        for(int i=0; i < m_entries.count();i++){
            if(m_entries[i]){
                delete m_entries[i];
                m_entries[i] = 0;
            }
        }
        m_bestItems.clear();
    }

    static short int encode(QChar v){
        if(v.isPunct()){ return 0; }
        if (!v.isLetter()) {return 1;}
        v = v.toLower();
        short c = (short)v.toAscii();

        if (c > (short)'z'){
            c =  ((short)'z')+1;
        }
        if (c < (short)'a'){
            c =  ((short)'z')+1;
        }
        short r = (c - (short)'a');
        Q_ASSERT(r < KEY_POSITIONS);
        Q_ASSERT(r >=0);
        return r;

    }

    void insertFull(CatItem it, QList<int> kl = QList<int>(),int charChanges=-1,Tuple tpl=Tuple());
    void addItemPiece(CatItem it, Tuple tpl, QString piece, int position, bool is_description);

    void insert(CatItem it, QList<int> kl,
                int charChanges=1, Tuple tpl=Tuple());

//    void insert(CatItem it, Tuple tpl, QList<int> kl = QList<int>(),
//                int charChanges=1){ //, int l_to_match=0
//
//        ItemRep ir(it, tpl, charChanges);
//        if(kl.length() > 0)
//            {ir.setMatchKeyList(kl);}
//        insertRep(ir);
//    }

    void insertRep(ItemRep ir, int length_limit=0);


    TrieNode* getLookAheadNode();

    //This is less selective than insert
    //It changes ir as it goes and the different
    //versions as copied to sub-functions
    void remove(ItemRep ir, int l=0){
        //QString name = ir.getName();
        m_bestItems.removeAll(ir);
        ItemRep ir1 = ir;
        if( ir1.next_char()){
            QChar c = ir1.char_at();
            if(m_entries[encode(c)]){
                m_entries[encode(c)]->remove(ir1, l+1);
                if ((m_entries[encode(c)]->m_bestItems.size()==0)
                        && (m_entries[encode(c)]->m_entries.count()==0)){
                    delete m_entries[encode(c)];
                    m_entries[encode(c)] = 0;
                }
            }
        }
        ItemRep ir2 = ir;
        ir2.next_pos();
        if(ir2.next_consonant()){
            QChar c = ir2.char_at();
            if(m_entries[encode(c)]) {
                m_entries[encode(c)]->remove(ir2, l+1);
                if ((m_entries[encode(c)]->m_bestItems.size()==0) && (m_entries[encode(c)]->m_entries.count()==0)) {
                    delete m_entries[encode(c)];
                    m_entries[encode(c)] = 0;
                }
            }
        }
        m_child_total--;
    }

    QList<CatItem> find(QString substr, int desired, ItemFilter* filter, int* beginPoint=0);

    //Diagnostics...
    long allocatedSize(){
        long allocSize = sizeof(TrieNode);
        for(int i=0; i < m_entries.count();i++){
            if(m_entries[i]){
                allocSize += m_entries[i]->allocatedSize();
            }
        }
        QLinkedList<ItemRep>::iterator i = m_bestItems.begin();
        for(; i!=m_bestItems.end();i++){
            allocSize += (*i).allocatedSize();
        }
        return allocSize;
    }

    long itemReferenceCount(){
        long r =m_bestItems.size();
        for(int i=0; i < m_entries.count();i++){
            if(m_entries[i]){
                r+= m_entries[i]->itemReferenceCount();
            }
        }
        return r;
    }

    long nodeCount(){
        long c = 1;
        for(int i=0; i < m_entries.count();i++){
            if(m_entries[i]){
                c+= m_entries[i]->nodeCount();
            }
        }
        return c;
    }

    //-- IE, except for testing...
    QList<CatItem> findProtected(QString substr, int desired, int l, ItemFilter* filter, int* beginPoint=0);
protected:

    //What happens if there just aren't any
    //more children
    QList<CatItem> filterCurrentNode(QString substr, int numDesired, ItemFilter* filter);
    QList<CatItem> filterChildNodes(QString substr, int numDesired, ItemFilter* filter, int* beginPoint=0);

    bool breadthSearch(QHash<int,CatItem>& alreadyFound, QString substr, int numDesired,
                       int depth, int digDepth, int skipDepth, ItemFilter* filter);

    bool addItem(ItemRep ir, QList<ItemRep>* expelledList, bool* used);

};

#endif // TRIE_NODE_H
