#ifndef MULTI_TREE_H
#define MULTI_TREE_H

//WTF aren't these defined?...
#ifndef INT_CONSTANT_KLUGDE
#define INT_CONSTANT_KLUGDE

/* Minimum of signed integral types.  */
# define INT8_MIN		(-128)
# define INT16_MIN		(-32767-1)
# define INT32_MIN		(-2147483647-1)
# define INT64_MIN		(2147483647)
/* Maximum of signed integral types.  */
# define INT8_MAX		(127)
# define INT16_MAX		(32767)
# define INT32_MAX		(2147483647)
# define INT64_MAX		(2147483647)

# define UINT8_MAX		(255)
# define UINT16_MAX		(65535)
# define UINT32_MAX		(4294967295U)
# define UINT64_MAX		(4294967295U)
#endif

#define FILEDB_IMPL

inline uint qHash(const Tuple&t ){
    return (uint)(t.getFirst() + t.getSecond()*1234);
}


#include <QHash>
#include <QList>
//#include <QTextStream>
#include <QDataStream>
#include <QByteArray>
#include <QStringList>
#include "btree.h"

#include "stdint.h"

//Kyoto cabinet is chock of errors...
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wextra"
//#pragma GCC system_header //This "big gun", since there's one warning GCC just can't let go of, the #$#%!
#include <kchashdb.h>
#include <kccachedb.h>
#pragma GCC diagnostic warning "-Wunused-parameter"
#pragma GCC diagnostic warning "-Wextra"



#define hHash(x) (stringHash((x)))

using namespace std;
using namespace kyotocabinet;

class ItemLocation {
private:
    QHash<QString, Tuple > il;
public:

    ItemLocation(){}
    ItemLocation(ItemLocation& i){
        il = i.il;
    }

    ItemLocation(const ItemLocation& i){
        il = i.il;

    }

    void SetValue(QString index, Tuple l){
        il[index] = l;
    }

    ItemLocation& operator =(const ItemLocation& i){
        il = i.il;
        return *this;

    }

    QHash<QString, Tuple >::iterator begin(){
        return il.begin();
    }

    QHash<QString, Tuple >::iterator end(){
        return il.end();
    }

    Tuple level(QString index){return il[index];}

    bool contains(QString k){
        return il.contains(k);
    }

    void remove(QString n){
        il.remove(n);

    }

    QList<QString> keys() {
        return il.keys();
    }

    QHash<QString, QString > getHash() const{
        QHash<QString, QString > res;
        QList<QString> keys = il.keys();
        for(int i=0;(i < keys.count());i++){
            res[keys[i]]= il[keys[i]].asString();
        }
        return res;
    }

    void setHash(QHash<QString, QString> h) {
        QHash<QString, QString > res;
        QList<QString> keys = h.keys();
        for(int i=0;(i < keys.count());i++){
            il[keys[i]]= Tuple(h[keys[i]]);
        }

    }

    QString getId(){
        return asString();
    }

    QString asString() const{
        QString res;
        QList<QString> keys = il.keys();
        for(int i=0;(i < keys.count());i++){
            res += keys[i] + SECOND_SPLITCHAR + il[keys[i]].asString() + THIRD_SPLITCHAR;
        }
//        qDebug() << res;
        return res;
    }

    void fromString(QString s) {
        qDebug() << s;
        il.clear();
        QStringList l = s.split(THIRD_SPLITCHAR);

        for(int i=0;(i < l.count());i++){
            QString s2 = l[i];
            QStringList l2 = s2.split(SECOND_SPLITCHAR);
            if(l2.count()!=2 )
                break;
            Tuple nt(l2[1]);
            il[l2[0]] = nt;
//            qDebug() << "saving: " << nt.asString();
//            qDebug() << "result: " << il[l2[0]].asString();
            Q_ASSERT(!il[l2[0]].isEmpty());
        }
    }

};

inline QDataStream &operator<<(QDataStream &out, const ItemLocation &index) {

    out << index.getHash();
    return out;
}

inline QDataStream &operator>>(QDataStream &in, ItemLocation &index) {

    QHash<QString, QString > h;
    in >> h;
    index.setHash(h);
    return in;
}

//inline QTextStream &operator<<(QTextStream &out, const ItemLocation &index) {
//
//    out << index.asString();
//
//    return out;
//}
//
//inline QTextStream &operator>>(QTextStream &in, ItemLocation &index) {
//
//
//    QString s;
//    in >> s;
//    index.fromString(s);
//
//    return in;
//}


template<class VALUE> class KCHash {
private:
    HashDB m_db;
//    CacheDB m_db;


public:


    static VALUE asValue(string* ssptr){
        VALUE v;
        QByteArray ba(ssptr->c_str(),ssptr->size());
        QDataStream in(&ba, QIODevice::ReadOnly);
        in >> v;
        return v;
    }

    static QByteArray asString(VALUE v){
        QByteArray ba;
        QDataStream out(&ba, QIODevice::ReadWrite);
        out << v;
        return ba;
    }

    KCHash(QString filePrefix): m_db(){
        QString fn = filePrefix+DEFAULT_KC_FILE;
        qDebug() << "opening database" << fn;
        if (!m_db.open(fn.toAscii().constData(), HashDB::OWRITER | HashDB::OCREATE)) {
                qDebug() << "open error: " << m_db.error().name() << endl;
                Q_ASSERT(false);
        }
    }
    ~KCHash(){
        qDebug() << "closing database";
        if (!m_db.close()) {
            qDebug() << "close error: " << m_db.error().name() << endl;
            Q_ASSERT(false);
        }
    }

    bool clear(){
        return m_db.clear();
    }

    void set(Tuple k, VALUE v){

        QString keyStr = k.asString();
        QByteArray ba = asString(v);
        const char* c = ba.constData();

        int ksz = keyStr.size();
        int sz = ba.size();
        m_db.set(keyStr.toAscii().constData(), ksz,c, sz);
    }


    VALUE get(Tuple k){
        QString keyStr = k.asString();
        QByteArray keyAr = keyStr.toAscii();
        const char* kc = keyAr.constData();
        string* ssptr = m_db.get(kc);
        if(ssptr==0){
            return VALUE();
        } else {
            VALUE v = asValue(ssptr);
            qDebug() << "found: " << v.getId();
            delete ssptr;
            qDebug() << "still: " << v.getId();
            return v;
        }
    }

    bool contains(Tuple k){
        QString keyStr = k.asString();
        const char* c  = keyStr.toAscii().constData();
        string* ssptr = m_db.get(c);
        if(ssptr!=0){
            delete ssptr;
            return true;
        } else {return false;}

    }

    bool remove(Tuple k){
        const char* c = k.asString().toAscii().constData();
        return m_db.remove(c);
    }


    //This follows Kyoto Cabinet example... deletes look weird but follow documentation
    QList<Tuple> keys(){
        QList<Tuple> res;
        DB::Cursor* cur = m_db.cursor();
        cur->jump();//we move cur to 1st record this way...

        pair<string, string>* rec;
        while ((rec = cur->get_pair(true)) != NULL) {
            res.append(Tuple(rec->first.c_str()));
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
            string s = rec->second;
            res.append(asValue(&s));
            delete rec;
        }
        delete cur;
        return res;
    }

    long size(){
        long i = m_db.count();
        return i;
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
        INDEX_TYPE i = newV.getId();
        Tuple tpl((long)hHash(i));

        VALUE testV;
        Tuple  testTuple = tpl;

        //for hash collision case - we can assume we'll have a small value
        while(true){
                if(!m_Hb.contains(testTuple)){break;}
                testV = m_Hb.get(testTuple);
                if(testV.getId() == newV.getId()){ return testTuple; }
                if(testV.getId().isEmpty()){break; }
                ++testTuple;
        }
        m_Hb.set(testTuple,newV);
        return testTuple;
    }

    Tuple getExistingTupleId(VALUE existingV){
        Tuple tpl((long)hHash(existingV.getId()));

        VALUE tVal;
        Tuple  testTuple = tpl;

        //for hash collision case - we can assume we'll have a small value
        while(true){
            if(!m_Hb.contains(testTuple)){break;}
            tVal = m_Hb.get(testTuple);
            if(tVal.getId() == existingV.getId()){ return testTuple; }
            ++testTuple;
        }
        return Tuple();

    }

    //Either it's id or what it's id would be...
    Tuple getInternalId(VALUE existingV){
        return getInternalId(existingV.getId());
    }

    //Either it's id or what it's id would be...
    Tuple getInternalId(INDEX_TYPE id){
        Tuple tpl((long)hHash(id));

        VALUE tVal;
        Tuple  testTuple = tpl;

        //for hash collision case - we can assume we'll have a small value
        while(true){
            if(!m_Hb.contains(testTuple)){break;}
            tVal = m_Hb.get(testTuple);
            if(tVal.getId() == id){ break; }
            if(tVal.isEmpty() ){ break; }
            ++testTuple;
        }
        return tpl;

    }

    VALUE getValue(VALUE v){
        return getValue(v.getId());
    }

    VALUE getValue(INDEX_TYPE id){
        Tuple tpl((long)hHash(id));

        VALUE tVal;
        Tuple  testTuple = tpl;

        //for hash collision case - we can assume we'll have a small value
        while(true){
            if(!m_Hb.contains(testTuple)){break;}
            tVal = m_Hb.get(testTuple);
            if(tVal.getId() == id){

                return tVal;
            }
            ++testTuple;
        }
        return VALUE();
    }



    bool containsValue(VALUE existingV){
        VALUE v = getValue(existingV);
        return !(v.isEmpty());

    }

    bool contains(Tuple t){
        return m_Hb.contains(t);
    }


    VALUE get(Tuple tpl){
        return m_Hb.get(tpl);
    }

    VALUE getIndex(INDEX_TYPE t){
        long i = hHash(t);
        Tuple tpl(i);
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
        return true;
    }

    QList<Tuple> keys(){
        return m_Hb.keys();
    }

    QList<VALUE> values(){
            return m_Hb.values();
    }

    int size(){
        return m_Hb.size();
    }

};


//ID_TYPE defaults to string
template <class VALUE, class INDEX_TYPE>   class MultiTree {
    private:
        //Actual items...
        Bucket< INDEX_TYPE, VALUE> itemList;

        //Indices of items
        typedef LBTree< Tuple, Tuple> ItemTree;
        QHash<INDEX_TYPE, ItemTree* >  subtrees;

        //house-keepin Indices showing item Locations...
        KCHash<ItemLocation> itemLocations;

        typedef Node<Tuple, Tuple> refnode;
        typedef Element<Tuple, Tuple> Elm;

    public:
    MultiTree(): itemList(VALUE::typeName()), itemLocations(QString("IndexStore") + VALUE::typeName()){ }
        ~MultiTree(){
            typename QHash<INDEX_TYPE, ItemTree*>::const_iterator i = subtrees.begin();
            for(;i!=subtrees.end();i++){
                INDEX_TYPE s = i.key();
                delete subtrees[s];
            }
        }

        bool clear(){
            bool res = !(!itemLocations.clear() || !itemList.clear());
            Q_ASSERT(res);
            return res;

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
                Tuple itemInnerId = ids[i];
                ItemLocation il = itemLocations.get(itemInnerId);
                QList<QString> indexNames = il.keys();
                for(int j=0;j< indexNames.count();j++){
                    QString theIndexName = indexNames[j];
                    Tuple theLevel = il.level(theIndexName);
                    Elm e(theLevel,itemInnerId);
                    int index;
                    refnode* e_res = subtrees[theIndexName]->get_root()->find_value_node(e, index);
                    Q_ASSERT(e_res);
                    Q_ASSERT((*e_res)[index].m_key == theLevel);

                    Tuple r = (*e_res)[index].m_payload;
                    Q_ASSERT(r == itemInnerId);
                    Tuple t = getLevel(itemInnerId,theIndexName);
                    Q_ASSERT(t==theLevel);
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
            Tuple t = itemList.getExistingTupleId(v);
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



        long count(){
            long i = itemList.size();
            return i;
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

            vector<Tuple> v;
            v=  subtrees[subkey]->get_root()->get_ordinal_range(start_n, small_index, count);
            vector<VALUE> res;
            for(unsigned int i=0;i < v.size();i++){
                res.push_back(itemList.get(v[i]));
            }
            return res;
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

        VALUE find(VALUE v){
            return itemList.getValue(v.getId());
        }

        VALUE getValue(const INDEX_TYPE id){
            return itemList.getValue(id);

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
                vector<Tuple> v =  root->get_range(start, end_bound);
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
        bool addEntry(long k,  VALUE v, const INDEX_TYPE subkey){
            const Tuple t(k);
            return addEntry(t, v, subkey);
        }

        bool addEntry(long k,  long l, VALUE v, const INDEX_TYPE subkey){
            Tuple t(k, l);
            return addEntry(t, v, subkey);
        }

        bool removeEntry(VALUE v, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Tuple id = this->itemList.getExistingTupleId(v);
            if(id.isEmpty()){ return false; }
            Tuple old_k = getLevel(id, subkey);
            bool remove_old = !old_k.isEmpty();
            if(remove_old){ remove_protected(old_k, id, subkey); }
            return remove_old ;
        }

        bool addEntry(Tuple newLevel, VALUE v, const INDEX_TYPE subkey){

            Q_ASSERT(subtrees.contains(subkey));
            Q_ASSERT(!newLevel.isEmpty());

            Tuple tupleId = itemList.getInternalId(v);
            Tuple oldLevel = getLevel(tupleId, subkey);
            bool remove_old = true;
            if(oldLevel==newLevel){
                //VALUE old_v = itemList.get(id);
                //old_v.merge(v);
                tupleId = updateLocationLists(newLevel, v, subkey);
            } else {
                remove_old = !oldLevel.isEmpty();
                if(remove_old){
                    remove_protected(oldLevel, v, subkey);
                }
                addToTree(newLevel, tupleId, subkey);
                tupleId = updateLocationLists(newLevel, v, subkey);

                ItemLocation il2 =  itemLocations.get(tupleId);
//                qDebug() << "new level: " << newLevel.asString();
//                qDebug() << "stored level: " << (il2).level(subkey).asString();
                Q_ASSERT((il2).level(subkey)!=oldLevel);
                Q_ASSERT((il2).level(subkey)==newLevel);
            }
            return remove_old;
        }


        Tuple getLevel(Tuple tupleIndex, INDEX_TYPE subkey){
            if(!(subtrees.contains(subkey)))
                { return Tuple(); }
            if(!(itemLocations.contains(tupleIndex)))
                { return Tuple();}
            ItemLocation il = itemLocations.get(tupleIndex);
            if(!(il.contains(subkey)))
                { return Tuple();}
            Tuple existingLevel =il.level(subkey);
            Q_ASSERT(!existingLevel.isEmpty());
            return existingLevel;
        }

        bool removeObject(VALUE v){
            qDebug() << "value to remove" << v.getId();
            if(!(itemList.containsValue(v))){ return false; }
            Tuple id = itemList.getExistingTupleId(v);
            Q_ASSERT(!id.isEmpty());
            ItemLocation il = itemLocations.get(id);
            QList<QString> keys = il.keys();
            for(int i=0; i <keys.count(); i++){
                Elm e(il.level(keys[i]), id);
                bool s= subtrees[keys[i]]->get_root()->delete_value(e);
                s = s;
                Q_ASSERT(s);
            }
            itemLocations.remove(id);
            itemList.remove(id);
            return true;
        }

    protected:

        void remove_protected(Tuple id, VALUE v, const INDEX_TYPE subkey){

            //INDEX_TYPE id = v.getId();
            //Tuple id = itemList.findTuple(v);
            Q_ASSERT(!id.isEmpty());
            Q_ASSERT(itemLocations.contains(id));
            ItemLocation il = itemLocations.get(id);
            Q_ASSERT(il.contains(subkey));
            Q_ASSERT(il.level(subkey) == old_k);
            il.remove(subkey);
            itemLocations.remove(id);
            itemLocations.set(id, il);
            subtrees[subkey]->delete_object(old_k, id); //was bool s =
            Q_ASSERT(!(itemLocations.get(id).contains(subkey)));
        }

        void addToTree(Tuple level,  Tuple internalId, const INDEX_TYPE subkey){

            Q_ASSERT(!internalId.isEmpty());
            ItemTree* tree = subtrees[subkey];
            Q_ASSERT(subtrees.contains(subkey));
            Q_ASSERT(!level.isEmpty());
            //Q_ASSERT(!(id == VALUE().getId()));
            Q_ASSERT(tree);
            tree->insert_at(level,internalId);

        }

        Tuple updateLocationLists(Tuple level,  VALUE v, const INDEX_TYPE subkey){
            Tuple id = itemList.insert(v);
            Q_ASSERT(!id.isEmpty());
            ItemLocation il;
            if(itemLocations.contains(id)){ il =  itemLocations.get(id); }
            il.SetValue(subkey,level);
            itemLocations.set(id,il);
            Q_ASSERT(itemLocations.get(id).level(subkey)==level);
            return id;

        }

};


#endif // MULTI_TREE_H
