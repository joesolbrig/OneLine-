#ifndef MULTI_TREE_H
#define MULTI_TREE_H

//WTF aren't these defined?...

///* Minimum of signed integral types.  */
//# define INT8_MIN		(-128)
//# define INT16_MIN		(-32767-1)
//# define INT32_MIN		(-2147483647-1)
//# define INT64_MIN		(2147483647)
///* Maximum of signed integral types.  */
//# define INT8_MAX		(127)
//# define INT16_MAX		(32767)
//# define INT32_MAX		(2147483647)
//# define INT64_MAX		(2147483647)
//
//# define UINT8_MAX		(255)
//# define UINT16_MAX		(65535)
//# define UINT32_MAX		(4294967295U)
//# define UINT64_MAX		(4294967295U)


#define FILEDB_IMPL

inline uint qHash(const Tuple&t ){
    return (uint)(t.getFirst() + t.getSecond()*1234);
}


#include <QHash>
#include <QList>
#include <QTextStream>
#include <QStringList>
#include "btree.h"

//Just for the QT creator syntax highligher...
//class Tuple;
//class LBTree;
//class Element;
#include "stdint.h"
#include <kchashdb.h>

#define SPLITCHAR "#"
#define SECOND_SPLITCHAR "$"
#define STRING_MODEL "%1" SPLITCHAR "%2"
#define hHash(x) (stringHash((x)))

using namespace std;
using namespace kyotocabinet;

class ItemLocation {
private:
    QHash<QString, Tuple > il;

    ItemLocation();


};

inline QDataStream &operator<<(QDataStream &out, const ItemLocation &index) {

    out << index.il;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, ItemLocation &index) {

    in >> index.il;
    return in;
}



template<class VALUE> class KCHash {
private:
    HashDB m_db;


public:
    static Tuple asTuple(QString s){
        QStringList strs = s.split(SPLITCHAR);
        if(strs.length() !=2){
            Q_ASSERT(false);
            return Tuple();
        }
        long i = strs[0].toLong();
        long j = strs[1].toLong();
        return Tuple(i,j);
    }

    static const char* asChars(Tuple t){
        QString a(STRING_MODEL);
        const char* c = a.arg(t.getFirst()).arg(t.getSecond()).toAscii().constData();
        return c;
    }

    static const char* asChars(VALUE v){
        QString s;
        QTextStream txs(&s);
        txs << v;
        return s.toAscii().constData();
    }

    static VALUE asValue(string* ssptr){
        VALUE v;
        QString s(ssptr->c_str());
        QTextStream txs(&s);
        txs >> v;
        return v;
    }

    KCHash(QString filePrefix): m_db(){
        if (!m_db.open(filePrefix+DEFAULT_KC_FILE, HashDB::OWRITER | HashDB::OCREATE)) {
                cerr << "open error: " << m_db.error().name() << endl;
                Q_ASSERT(false);
        }
    }
    ~KCHash(){
        if (!m_db.close()) {
                cerr << "close error: " << m_db.error().name() << endl;
        }
    }

    bool clear(){
        return m_db.clear();
    }

    void set(Tuple k, VALUE v, QString prefixe=""){
        m_db.set(prefixe + asChars(k),asChars(v));
    }

    void setValue(VALUE v){
        long i = m_db.count();
        Tuple k(i+1);
        m_db.set(asChars(k),asChars(v));
    }

    void setIndexValue(QString index, Tuple id, Tuple level){
        QString key = index + QString(SECOND_SPLITCHAR) + asString(id);
        QString value = asString(level);
        m_db.set(asChars(k),asChars(v));
    }

    VALUE get(Tuple k){
        string* ssptr = m_db.get(asChars(k));
        if(ssptr==0){
            return VALUE();
        } else {
            VALUE v;
            QString s(ssptr->c_str());
            QTextStream txs(&s);
            txs >> v;
            return v;
        }
    }

    bool contains(Tuple k){
        string* c = m_db.get(asString(k));
        return(c!=0);
    }


    bool remove(Tuple k){
        m_db.remove(asString(k));
    }


    //This follows Kyoto Cabinet example... deletes look weird but follow documentation
    QList<Tuple> keys(){
        QList<Tuple> res;
        DB::Cursor* cur = m_db.cursor();
        cur->jump();//we move cur to 1st record this way...

        pair<string, string>* rec;
        while ((rec = cur->get_pair(true)) != NULL) {
            res.append(asTuple(rec->first.c_str()));
            delete rec;
        }
        delete cur;
        return res;
    }

    //This follows Kyoto Cabinet example... deletes look weird but follow documentation
    QList<VALUE> values(){
        QList<VALUE> res;
        DB::Cursor* cur = m_db.cursor();
        cur->jump();//we move cur to 1st record this way...

        pair<string, string>* rec;
        while ((rec = cur->get_pair(true)) != NULL) {
            res.append(asValue(rec->second.c_str()));
            delete rec;
        }
        delete cur;
        return res;
    }

    int size(){
        return m_db.count();
    }
};

template <class INDEX_TYPE, class VALUE > class Bucket {
    private:
        KCHash<VALUE> m_Hb;

    public:
        Bucket(QString prefix): m_Hb(prefix){ }
	
	~Bucket(){
	}

        bool clear(){
            return m_Hb.clear();
        }
        qint32 getSize(){
            return m_Hb.size();
        }

        //Adding removing to bucket:
        Tuple insert(VALUE newV){
            Tuple tpl((long)hHash(newV.getId()));

            VALUE testV;
            Tuple  testTuple = tpl;

            //for hash collision case - we can assume we'll have a small value
            while(true){
                    if(!m_Hb.contains(testTuple)){break;}
                    testV = m_Hb.get(testTuple);
                    if(testV == newV){ return testTuple; }
                    if(testV.isEmpty()){break; }
                    ++testTuple;
            }
            m_Hb.set(testTuple,newV);
            return testTuple;
        }

        Tuple findTuple(VALUE existingV){
            Tuple tpl((long)hHash(existingV.getId()));

            VALUE tVal;
            Tuple  testTuple = tpl;

            //for hash collision case - we can assume we'll have a small value
            while(true){
                if(!m_Hb.contains(testTuple)){break;}
                tVal = m_Hb.get(testTuple);
                if(tVal == existingV){ return testTuple; }
                ++testTuple;
            }
            return Tuple();

        }

        bool contains(VALUE existingV){
            Tuple tpl((long)hHash(existingV.getId()));

            VALUE tVal;
            Tuple  testTuple = tpl;

            //for hash collision case - we can assume we'll have a small value
            while(true){
                if(!m_Hb.contains(testTuple)){return false;}
                tVal = m_Hb.get(testTuple);
                if(tVal == existingV){ return true; }
                ++testTuple;
            }
            return false;

        }

        bool contains(Tuple t){
            return m_Hb.contains(t);
        }

        VALUE get(Tuple tpl){
            return m_Hb.get(tpl);
        }

        bool remove(Tuple tpl){
            bool areaEmpty = true;
            m_Hb.set(tpl,VALUE());
            Tuple  testTuple = tpl;
            ++testTuple;
            while(true){
                if(!m_Hb.contains(testTuple)){break;}
                VALUE tVal = m_Hb.get(testTuple);
                if(!tVal.isEmpty()){
                    areaEmpty = false;
                }
                ++testTuple;
            }
            if(areaEmpty){
                testTuple = tpl;
                while(m_Hb.contains(testTuple)){
                    m_Hb.remove(testTuple);
                    ++testTuple;
                }
            }
        }
	
        QList<Tuple> keys(){
            return m_Hb.keys();
	}
	
	QList<VALUE> values(){
                return m_Hb.values();
	}

        int size(){
            m_Hb.size();
        }

};



//ID_TYPE defaults to string
template <class VALUE, class INDEX_TYPE>   class MultiTree {
    private:
        //Actual items...
        Bucket< INDEX_TYPE, VALUE> itemList;
        KCHash<IndexEntry> indexStore;

        //Indices of items
        typedef LBTree< Tuple, Tuple> ItemTree;
        QHash<INDEX_TYPE, ItemTree* >  subtrees;

        //house-keepin Indices showing item Locations...
        //typedef TreePosition<Tuple, INDEX_TYPE> TreePos;
        typedef QHash<QString, Tuple > ItemLocation;
        QHash<Tuple, ItemLocation* > itemLocations;

        typedef Node<Tuple, Tuple> refnode;
        typedef Element<Tuple, Tuple> Elm;

    public:
        MultiTree(): itemList(VALUE::typeName()){ }
        ~MultiTree(){
            typename QHash<Tuple, ItemLocation* >::iterator l_i;
            for(l_i=itemLocations.begin();l_i!=itemLocations.end();l_i++){
                ItemLocation* s =l_i.value();
                delete s;
            }

            typename QHash<INDEX_TYPE, ItemTree*>::const_iterator i = subtrees.begin();
            for(;i!=subtrees.end();i++){
                INDEX_TYPE s = i.key();
                delete subtrees[s];
            }
        }

        bool clear(){
            return itemList.clear();
        }

        void testStruct(){

            typename QHash<INDEX_TYPE, ItemTree*>::const_iterator i = subtrees.begin();
            for(;i!=subtrees.end();i++){
                INDEX_TYPE s = i.key();
                Q_ASSERT(subtrees[s]);
                subtrees[s]->get_root()->testStruct();

            }

            QList<Tuple> ids = itemList.keys();
            for( int i=0; i < ids.count(); i++){
                ItemLocation* il = itemLocations[ids[i]];
                QList<INDEX_TYPE> ks = il->keys();
                for(int j=0;j< ks.count();j++){
                    INDEX_TYPE k = ks[j];
                    Tuple s = ids[i];
                    Tuple v = (*il)[k];
                    Elm e(v,s);
                    int index;
                    refnode* e_res = subtrees[k]->get_root()->find_value_node(e, index);
                    Q_ASSERT(e_res);
                    Q_ASSERT((*e_res)[index].m_key == v);

                    Tuple r = (*e_res)[index].m_payload;
                    Q_ASSERT(r == s);
                    Tuple t = get_tuple(s,k);
                    Q_ASSERT(t==v);
                }
            }
        }

        bool addIndex(const INDEX_TYPE index_name){
            if(subtrees.find(index_name)!= subtrees.end()) {
                return false;
            }
            ItemTree* it = new ItemTree();
            refnode* root_ptr = new refnode(*it);
            it->set_root ((refnode*)(reinterpret_cast<refnode*> (0)), root_ptr);
            subtrees[index_name] = it;
            Q_ASSERT(subtrees[index_name]->get_root());
            return true;

        }

        long indexSize(const INDEX_TYPE index_name){
            Q_ASSERT((subtrees.find(index_name)!= subtrees.end()));
            return subtrees[index_name]->size();
        }

        bool contains(VALUE v){
            Tuple t = itemList.findTuple(v);
            if(!t.isEmpty()) {
                Q_ASSERT(itemLocations.contains(t));
                return true;
            } else {
                return false;
            }

        }

        bool removeIndex(const INDEX_TYPE index_name){
            typename QHash<INDEX_TYPE, ItemTree* >::iterator i =subtrees.get(index_name);
            if(i!= subtrees.end()){
                delete subtrees[index_name];
                return true;
            } else { return false;}
        }

        long max_size() {return itemList.size();}

        VALUE get_value(INDEX_TYPE id){
            if(itemList.contains(id)){
                return itemList.get(id);
            } else {
                return VALUE();
            }
        }


        long count(){
            return itemList.size();
        }

        long count(INDEX_TYPE index_name){
            Q_ASSERT(subtrees.find(index_name)!= subtrees.end());
            return subtrees[index_name]->size();
        }

        QList<VALUE> get_list(){
            return itemList.values();
        }

        qint64 get_range_count(long k_start, long k_end, const INDEX_TYPE subkey){
            Tuple s(k_start);
            Tuple e(k_end);
            return get_range_count(s,e, subkey);
        }

        qint64 get_range_count(Tuple k_start, Tuple k_end, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            return subtrees[subkey]->get_range_count(k_start, k_end);
        }

        vector<VALUE> get_range_by_pos(long start, long end, const INDEX_TYPE subkey){
            Q_ASSERT(end>=start);
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            ItemTree* tree =subtrees[subkey];
            std::vector<VALUE> res;

            if(end > start){
                refnode* start_n=0;
                unsigned int index=0;
                tree->get_root()->ordinalSearch(start, start_n, index);
                res = get_range_by_count(start_n, index, end-start,subkey);
                Q_ASSERT(res.size() == MIN(tree->get_root()->m_total_elements-start, (unsigned int)end- start));
            }
            return res;
        }

        vector<VALUE> get_range_by_count(refnode* start_n, int small_index, int count, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());

            vector<INDEX_TYPE> v;
            v=  subtrees[subkey]->get_root()->get_ordinal_range(start_n, small_index, count);
            vector<VALUE> res;
            for(unsigned int i=0;i < v.size();i++){
                res.push_back(itemList.get(v[i]));
            }
            return res;
        }




        VALUE find(VALUE v){
            INDEX_TYPE id = v.getId();
            if(itemList.contains(id)){
                return itemList.get(id);
            } else {
                return VALUE();
            }
        }


        VALUE find(long k,  const INDEX_TYPE subkey){
            Tuple t(k);
            return find(t, subkey);
        }

        VALUE find(Tuple k, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.contains(subkey));
            Elm e_res = subtrees[subkey]->get_root()->search_by_key(k);
            if (e_res.matches(refnode::m_failure)){
                VALUE v;
                return v;
            }
            Tuple id = e_res.m_payload;
            Q_ASSERT(itemList.contains(id));
            return itemList.get(id);
        }

        long get_order(Tuple k, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Elm e(k);
            return subtrees[subkey]->get_position(k);
        }

        vector<VALUE> get_range(long start, long end_bound, INDEX_TYPE subkey){
            return get_range(Tuple(start), Tuple(end_bound),subkey);
        }

        vector<VALUE> get_range(Tuple start, Tuple end_bound, INDEX_TYPE subkey){
            vector<VALUE> res;

            typename QHash<INDEX_TYPE,Tuple >::iterator i;

            if(subtrees.find(subkey)!= subtrees.end() ){
                refnode* root = subtrees[subkey]->get_root();
                vector<INDEX_TYPE> v =  root->get_range(start, end_bound);
                for(unsigned int i=0;i < v.size();i++){
                    res.push_back(itemList.get(v[i]));
                }
            }
            return res;
        }

        Tuple max(const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Elm e = subtrees[subkey]->get_root()->largest_key_in_subtree();
            return e.m_key;
        }

        //Return whether value previously existed...
        bool add(long k,  VALUE v, const INDEX_TYPE subkey){
            const Tuple t(k);
            return add(t, v, subkey);
        }

        bool add(long k,  long l, VALUE v, const INDEX_TYPE subkey){
            Tuple t(k, l);
            return add(t, v, subkey);
        }

        bool removeEntry(VALUE v, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Tuple old_k = get_tuple(v, subkey);
            bool remove_old = !old_k.isEmpty();
            if(remove_old){ remove_protected(old_k, v, subkey); }
            return remove_old ;
        }

        bool add(Tuple new_k, VALUE v, const INDEX_TYPE subkey){

            //INDEX_TYPE id = v.getId();
            Tuple id = this->itemList.findTuple(v);
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Tuple old_k = get_tuple(id, subkey);
            bool remove_old = true;
            Q_ASSERT(!new_k.isEmpty());
            if(old_k==new_k){
                VALUE old_v = itemList.get(id);
                old_v.merge(v);
                update_entry_protected(new_k, v, subkey);
                //skip this assert 'cause we want to avoid square bracket access
                //which add items if they don't exist...
                //Q_ASSERT(!((*itemLocations[id])[subkey]!=old_k));
            } else {
                remove_old = !old_k.isEmpty();
                if(remove_old){
                    remove_protected(old_k, v, subkey);
                }
                add_protected(new_k, v, subkey);
                ItemLocation* il2 =  itemLocations[id];
                Q_ASSERT((*il2)[subkey]!=old_k);
                Q_ASSERT((*il2)[subkey]==new_k);
            }
            return remove_old;
        }

        Tuple get_tuple(Tuple id, INDEX_TYPE subkey){
            if(!(subtrees.find(subkey)!= subtrees.end()))
                { return Tuple(); }
            if(!(itemLocations.find(id)!=itemLocations.end()))
                { return Tuple();}
            ItemLocation* il = itemLocations[id];
            Q_ASSERT(il);
            if(!(il->find(subkey)!=il->end()))
                { return Tuple();}
            Tuple existingKey =(*il)[subkey];
            Q_ASSERT(!existingKey.isEmpty());
            return existingKey;
        }

        bool removeObject(VALUE v){
            Tuple id = itemList.findTuple(v);
            if(!(itemList.contains(v))){ return false; }
            ItemLocation* il = itemLocations[id];
            //Our nodes are preserved in memory
            //BUT our elements are copied by value as the tree changes...
            //So we always need to search the present node for our element
            //
            typename QHash<QString,Tuple >::iterator i;
            for(i=il->begin(); i!=il->end(); ++i){
                Elm e((*i), id);
                bool s= subtrees[i.key()]->get_root()->delete_value(e);
                s = s;
                Q_ASSERT(s);
            }
//            typename QHash<INDEX_TYPE, ItemTree*>::const_iterator j = subtrees.begin();
//            for(;j!=subtrees.end();j++){
//                if(il->find(j.key()) !=il->end()){
//                    delete ((*il)[j.key()]);
//                }
//            }
            delete il;
            itemLocations.remove(id);
            itemList.remove(id);
            return true;
        }

    protected:

        void remove_protected(Tuple old_k, VALUE v, const INDEX_TYPE subkey){

            //INDEX_TYPE id = v.getId();
            Tuple id = itemList.findTuple(v);

            Q_ASSERT(itemLocations.find(id)!= itemLocations.end());
            ItemLocation* il = itemLocations[id];
            Q_ASSERT(il->find(subkey)!=il->end());
            Q_ASSERT((*il)[subkey] == old_k);
            il->remove(subkey);
            itemLocations.remove(id);
            itemLocations[id] = il;
            subtrees[subkey]->delete_object(old_k, id); //was bool s =
            Q_ASSERT((itemLocations[id]->find(subkey)==itemLocations[id]->end()));
        }

        void add_protected(Tuple k,  VALUE v, const INDEX_TYPE subkey){

            //INDEX_TYPE id = v.getId();
            Tuple id = itemList.findTuple(v);
            ItemTree* tree = subtrees[subkey];
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Q_ASSERT(!k.isEmpty());
            //Q_ASSERT(!(id == VALUE().getId()));
            Q_ASSERT(tree);

            tree->insert_at(k,id);
            update_entry_protected(k, v, subkey);
        }
        void update_entry_protected(Tuple k,  VALUE v, const INDEX_TYPE subkey){
            Tuple id = itemList.insert(v);
            ItemLocation* il = 0;
            if(itemLocations.find(id)!= itemLocations.end()){
                il =  itemLocations[id];
            } else {
                il =  new ItemLocation();
                itemLocations[id] = il;
            }
            Q_ASSERT(il);
            (*il)[subkey] = k;
            //Q_ASSERT(!(find(k,subkey).getId() == VALUE().getId()));
            Q_ASSERT((*itemLocations[id])[subkey]==k);

        }

};


#endif // MULTI_TREE_H
