#ifndef MULTI_TREE_H
#define MULTI_TREE_H

#define ItemTreePtr ItemTree*
#define ItemLocationPtr ItemLocation*

#define NPointer MPointer< Node<Tuple, Tuple, VALUE> >
#define refnodePtr MPointer< Node<Tuple, Tuple, VALUE> >
#define createRefNodePtr(arg) MPointer<Node <Tuple, Tuple, VALUE> >((new(MPointer<Node<Tuple, Tuple, VALUE> >::allocate()) Node<Tuple, Tuple, VALUE>(arg)))
#define createIndexRefNodePtr(arg) MPointer<Node <Tuple, Tuple, PARAM_VALUE> >((new(MPointer<Node<Tuple, Tuple, PARAM_VALUE> >::allocate()) Node<Tuple, Tuple, PARAM_VALUE>(arg)))

inline uint qHash(const Tuple&t ){
    return (uint)(t.getFirst() + t.getSecond()*1234);
}


#include <QHash>
#include <QList>
//#include <QTextStream>
#include <QDataStream>
#include <QByteArray>
#include <QStringList>
#include <QDir>
#include "btree.h"
#include "keyvaluedb.h"



class ItemLocation {
private:
    QHash<QString, Tuple > m_il;
public:
    friend QDataStream &operator>>(QDataStream &in, ItemLocation &index);
    friend QDataStream &operator<<(QDataStream &out, const ItemLocation &index);

    ItemLocation(){}
    ItemLocation(ItemLocation& i){
        m_il = i.m_il;
    }

    ItemLocation(const ItemLocation& i){
        m_il = i.m_il;

    }

    void SetValue(QString index, Tuple l){
        m_il[index] = l;
    }

    ItemLocation& operator =(const ItemLocation& i){
        m_il = i.m_il;
        return *this;

    }

    QHash<QString, Tuple >::iterator begin(){
        return m_il.begin();
    }

    QHash<QString, Tuple >::iterator end(){
        return m_il.end();
    }

    Tuple level(QString index){return m_il[index];}

    bool contains(QString k){
        return m_il.contains(k);
    }

    void remove(QString n){
        m_il.remove(n);

    }

    QList<QString> keys() {
        return m_il.keys();
    }

    QHash<QString, QString > getHash() const{
        QHash<QString, QString > res;
        QList<QString> keys = m_il.keys();
        for(int i=0;(i < keys.count());i++){
            res[keys[i]]= m_il[keys[i]].asString();
        }
        return res;
    }

    void setHash(QHash<QString, QString> h) {
        QList<QString> keys = h.keys();
        for(int i=0;(i < keys.count());i++){
            m_il[keys[i]]= Tuple(h[keys[i]]);
        }

    }

    QString getId(){
        return asString();
    }

    QString asString() const{
        QString res;
        QList<QString> keys = m_il.keys();
        for(int i=0;(i < keys.count());i++){
            res += keys[i] + SECOND_SPLITCHAR + m_il[keys[i]].asString() + THIRD_SPLITCHAR;
        }
        return res;
    }

    void fromString(QString s) {
        m_il.clear();
        QStringList l = s.split(THIRD_SPLITCHAR);

        for(int i=0;(i < l.count());i++){
            QString s2 = l[i];
            QStringList l2 = s2.split(SECOND_SPLITCHAR);
            if(l2.count()!=2 )
                break;
            Tuple nt(l2[1]);
            m_il[l2[0]] = nt;
            Q_ASSERT(!m_il[l2[0]].isEmpty());
        }
    }

};

inline QDataStream &operator<<(QDataStream &out, const ItemLocation &index) {
    out << index.m_il;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, ItemLocation &index) {

    in >> index.m_il;
    return in;
}



template<class VALUE> class KCHash {
private:
    HashDB m_db;

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

    KCHash(QString filePrefix, QString path=""): m_db(){
        QString fn = filePrefix+DEFAULT_KC_FILE;
        if(!path.isEmpty()){
            QDir d(path);
            if(!d.exists()){
                QFileInfo f(path);
                if(f.dir().exists()){
                    d = f.dir();
                }
            }
            if(!d.exists()){
                d = QDir(QDir::homePath() + PATH_SEP + path);
            }
            if(!d.exists()){
                qDebug() << "db path " << path << " does not exist";
                Q_ASSERT(false);
                exit(1);
            }
            fn = d.filePath(fn);
        }
        qDebug() << "opening database" << fn;
        if (!m_db.open(fn.toAscii().constData(), HashDB::OWRITER | HashDB::OCREATE)) {
                qDebug() << "open error: " << m_db.error().name() << endl;
                Q_ASSERT(false);
                exit(1);
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
            //Trying to get error but it's hidden in infinite namespace munge...
//            kyotocabinet::Cursor::Error r = m_db.error();
//            if(QString(r.message()) != QString("no record")){
//                qDebug() << "failure message" << r.message();
//            }

            return VALUE();
        } else {
            VALUE v = asValue(ssptr);
            delete ssptr;
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
    Bucket(QString prefix, QString path=""): m_Hb(prefix, path){ }

    ~Bucket(){
    }

    bool clear(){
        return m_Hb.clear();
    }
    qint32 getSize(){
        return m_Hb.size();
    }

    //Adding removing to bucket:
    Tuple insertOrReplace(VALUE newV){
        Q_ASSERT(!newV.getId().isEmpty());
        INDEX_TYPE i = newV.getId();
        Tuple tpl((long)hHash(i));
        VALUE testV;
        Tuple  testTuple = tpl;

        //loop for hash collision case - we can expect that we'll only go a short ways in this loop
        while(true){
            if(!m_Hb.contains(testTuple)){break;}
            testV = m_Hb.get(testTuple);
            Q_ASSERT(!testV.getId().isEmpty());
            if(testV.getId() == newV.getId()){ break; }
            ++testTuple;
        }
        m_Hb.set(testTuple,newV);

        //Q_ASSERT(m_Hb.get(testTuple).getId() == newV.getId());
        //Q_ASSERT(m_Hb.get(testTuple).getId() == newV.getId()); //for testing weirdness
        return testTuple;
    }

    Tuple getExistingTupleId(VALUE existingV){
        Tuple tpl((long)hHash(existingV.getId()));
        VALUE tVal;
        Tuple  testTuple = tpl;

        while(true){
            if(!m_Hb.contains(testTuple)){break;}
            tVal = m_Hb.get(testTuple);
            if(tVal.getId() == existingV.getId()){ return testTuple; }
            ++testTuple;
        }
        return Tuple();

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
        typedef LBTree< Tuple, Tuple, VALUE> ItemTree;
        typedef Node<Tuple, Tuple, VALUE> refnode;
        typedef Element<Tuple, Tuple, VALUE> Elm;

        //Actual items...
        Bucket< INDEX_TYPE, VALUE> m_itemList;
        Bucket< INDEX_TYPE, Tuple> m_baseIndices;

        //Indices of item levels
        QHash<INDEX_TYPE, ItemTreePtr >  m_subtrees;

        //house-keeping indices showing item values on the indices...
        //this allows us to move and change the items positions
        KCHash<ItemLocation> itemLocations;
        KCHash<ItemLocation> baseLocations;

    public:

        MultiTree(bool restoreTree=true, QString path=""):
                m_itemList(QString("values_") +VALUE::typeName(),path),
                m_baseIndices(QString("BaseIndices") + VALUE::typeName(),path),
                itemLocations(QString("IndexStore_") + VALUE::typeName(),path),
                baseLocations(QString("BaseLocations") + VALUE::typeName(),path){
            MPointer<refnode>::setPath(path);
            if(restoreTree){
                restore();
            } else {
                MPointer<refnode>::createAllocator();
                clearAll();
            }
        }

        ~MultiTree(){
            save();
            typename QHash<INDEX_TYPE, ItemTreePtr>::const_iterator i = m_subtrees.begin();
            for(;i!=m_subtrees.end();i++){
                INDEX_TYPE s = i.key();
                delete m_subtrees[s];
            }
            MPointer<refnode>::destroyAllocator();
        }


        void sync(){
            save();
            MPointer<refnode>::destroyAllocator();

        }

        void restore(){
            QList<Tuple> keys;
            if( MPointer<refnode>::restoreAllocator()){
                qDebug() << "Multi-tree saved mmap index loaded";
                keys = m_baseIndices.keys();
                for(int i=0; i< keys.count(); i++){
                    ItemLocation il = baseLocations.get(keys[i]);
                    QList<QString> subkeys = il.keys();
                    for(int j=0; j < subkeys.count(); j++){
                        if (!m_subtrees.contains(subkeys[j])){
                            addIndex(subkeys[j]);
                        }
                        Tuple value = il.level(subkeys[j]);
                        ItemTree* tree = m_subtrees[subkeys[j]];
                        qint64 offset = value.getFirst();
                        NPointer p(offset,true);
                        refnode* pReal = p.valuePtr();
                        tree->set_root(pReal);
                    }
                }
            } else {
                qDebug() << "Multitree rebuilding index";
                QList<Tuple> keys = m_itemList.keys();
                for(int i=0; i< keys.count(); i++){
                    ItemLocation il = itemLocations.get(keys[i]);
                    QList<QString> subkeys = il.keys();
                    for(int j=0; j < subkeys.count(); j++){
                        if ( !m_subtrees.contains(subkeys[j])) {
                            this->addIndex(subkeys[j]);
                        }
                        Tuple v = il.level(subkeys[j]);
                        Tuple itemInternalId = keys[i];
                        this->addToTree(v, itemInternalId, subkeys[j]);
                    }

                }

            }

        }

        void save(){
            m_baseIndices.clear();
            baseLocations.clear();
            if(!MPointer<refnode>::isLoaded()){return;}
            typename QHash<INDEX_TYPE, ItemTreePtr>::const_iterator i = m_subtrees.begin();
            for(;i!=m_subtrees.end();i++){
                INDEX_TYPE subkey = i.key();
                qint64 rootMemOffset = m_subtrees[subkey]->get_root().getOffset();
                Tuple offsetTuple(rootMemOffset,0);

                ItemLocation il;
                Tuple internalId = m_baseIndices.insertOrReplace(offsetTuple);
                if(baseLocations.contains(offsetTuple)){
                    il =  itemLocations.get(internalId);
                }
                il.SetValue(subkey,offsetTuple);
                baseLocations.set(internalId,il);
            }
        }


        bool clearAll(){
            bool res = !(!itemLocations.clear() || !m_itemList.clear()
                         || !m_baseIndices.clear() || !baseLocations.clear());
            //MPointer<refnode>::clear();

            Q_ASSERT(res);
            return res;

        }

        void testStruct(){

            typename QHash<INDEX_TYPE, ItemTreePtr>::const_iterator i = m_subtrees.begin();
            long totalFilledElements =0;
            for(;i!=m_subtrees.end();i++){
                INDEX_TYPE s = i.key();
                Q_ASSERT(m_subtrees[s]);
                m_subtrees[s]->get_base()->testStruct();
                totalFilledElements += m_subtrees[s]->get_base()->testAllocatedSize();
            }

            cout << "\n Explicitly allocated memory, supposedly: " <<  totalFilledElements ;

            QList<Tuple> ids = m_itemList.keys();
            for( int i=0; i < ids.count(); i++){
                Tuple itemInnerId = ids[i];
                ItemLocation il = itemLocations.get(itemInnerId);
                QList<QString> indexNames = il.keys();
                for(int j=0;j< indexNames.count();j++){
                    QString theIndexName = indexNames[j];
                    Tuple theLevel = il.level(theIndexName);
                    Elm e(theLevel,itemInnerId);
                    int index;
                    Q_ASSERT(m_subtrees.contains(theIndexName));
                    ItemTreePtr itp = m_subtrees[theIndexName];
                    MPointer< refnode > base = itp->get_base();
                    Q_ASSERT(base);
                    refnodePtr e_res = base->find_value_node(e, index);
                    Q_ASSERT(e_res);
                    Q_ASSERT(e_res->at(index).m_key == theLevel);

                    Tuple r = e_res->at(index).m_payload;
                    Q_ASSERT(r == itemInnerId);
                    Tuple t = getLevel(itemInnerId,theIndexName);
                    Q_ASSERT(t==theLevel);
                }
            }
        }

        bool addIndex(const INDEX_TYPE index_name){
            if(m_subtrees.find(index_name)!= m_subtrees.end()) {
                return false;
            }
            ItemTreePtr it = new ItemTree();
//            MPointer< Node<Tuple, Tuple, VALUE> > root_ptr = createRefNodePtr(*it);
//            it->set_root(root_ptr);
            it->createBase();
            m_subtrees[index_name] = it;
            Q_ASSERT(m_subtrees[index_name]->get_base());
            return true;

        }

        long indexSize(const INDEX_TYPE index_name){
            Q_ASSERT((m_subtrees.find(index_name)!= m_subtrees.end()));
            return m_subtrees[index_name]->size();
        }

        bool contains(VALUE v){
            Tuple t = m_itemList.getExistingTupleId(v);
            if(!t.isEmpty()) {
                Q_ASSERT(itemLocations.contains(t));
                return true;
            } else {
                return false;
            }

        }

        bool removeIndex(const INDEX_TYPE index_name){
            typename QHash<INDEX_TYPE, ItemTreePtr >::iterator i =m_subtrees.get(index_name);
            if(i!= m_subtrees.end()){
                delete m_subtrees[index_name];
                return true;
            } else { return false;}
        }

        long size() {return m_itemList.size();}

        long count(){
            long i = m_itemList.size();
            return i;
        }

        long count(INDEX_TYPE index_name){
            Q_ASSERT(m_subtrees.find(index_name)!= m_subtrees.end());
            return m_subtrees[index_name]->size();
        }

        QList<VALUE> get_list(){
            return m_itemList.values();
        }

        qint32 get_range_count(long k_start, long k_end, const INDEX_TYPE subkey){
            Tuple s(k_start);
            Tuple e(k_end);
            return get_range_count(s,e, subkey);
        }

        qint32 get_range_count(Tuple k_start, Tuple k_end, const INDEX_TYPE subkey){
            Q_ASSERT(m_subtrees.find(subkey)!= m_subtrees.end());
            return m_subtrees[subkey]->get_range_count(k_start, k_end);
        }

        vector<VALUE> get_range_by_pos(long start, long end, const INDEX_TYPE subkey){
            std::vector<VALUE> res;
            if(start<0){  start = 0; }
            if (end < start){ return res; }
            Q_ASSERT(m_subtrees.find(subkey)!= m_subtrees.end());
            ItemTreePtr tree =m_subtrees[subkey];
            if( end > start && ((unsigned int)start <= tree->get_base()->node_size()) ){
                refnodePtr start_n;
                unsigned int index=(long)0;
                tree->get_base()->ordinalSearch(start, start_n, index);
                res = get_range_by_count(start_n, index, end-start,subkey);
                Q_ASSERT(res.size() == MIN(tree->get_base()->totalElements()-start, (unsigned int)end- start));
            }
            return res;
        }

        vector<VALUE> get_range_by_count(refnodePtr start_n, int small_index, int count, const INDEX_TYPE subkey){
            Q_ASSERT(m_subtrees.find(subkey)!= m_subtrees.end());

            vector<Tuple> v;
            v=  m_subtrees[subkey]->get_base()->get_ordinal_range(start_n, small_index, count);
            vector<VALUE> res;
            for(unsigned int i=0;i < v.size();i++){
                res.push_back(m_itemList.get(v[i]));
            }
            return res;
        }

        VALUE find(long k,  const INDEX_TYPE subkey){
            Tuple t(k);
            return find(t, subkey);
        }

        VALUE find(Tuple k, const INDEX_TYPE subkey){
            Q_ASSERT(m_subtrees.contains(subkey));
            MPointer<refnode> base = m_subtrees[subkey]->get_base();
            refnode* basePtr = base.valuePtr();
            Elm e_res = basePtr->search_by_key(k);
            if (e_res.matches(refnode::m_failure)){
                VALUE v;
                return v;
            }
            Tuple id = e_res.m_payload;
            if(!m_itemList.contains(id)){
                VALUE v;
                return v;
            }
            return m_itemList.get(id);
        }

        VALUE find(VALUE v){
            return m_itemList.getValue(v.getId());
        }

        VALUE getValue(const INDEX_TYPE id){
            return m_itemList.getValue(id);

        }

        VALUE getByInternalId(const Tuple t){
            return m_itemList.get(t);

        }
        void insertValue(VALUE v){
            Q_ASSERT(!m_itemList.getExistingTupleId(v).isEmpty());
            m_itemList.insertOrReplace(v);
        }

        //Get first position among value
        long get_order(Tuple k, const INDEX_TYPE subkey){
            Q_ASSERT(m_subtrees.find(subkey)!= m_subtrees.end());
            Elm e(k);
            return m_subtrees[subkey]->get_position(k);
        }

        //This gets last position among equal values
        long get_endOrder(Tuple k, const INDEX_TYPE subkey){
            Q_ASSERT(m_subtrees.find(subkey)!= m_subtrees.end());
            ++k;
            Elm e(k);
            return m_subtrees[subkey]->get_position(k)-1;
        }

        vector<VALUE> get_range(long start, long end_bound, INDEX_TYPE subkey){
            return get_range(Tuple(start), Tuple(end_bound),subkey);
        }

        vector<VALUE> get_range(Tuple start, Tuple end_bound, INDEX_TYPE subkey,
                                long limit=-1, long offset=-1){
            vector<VALUE> res;

            Q_ASSERT(m_subtrees.find(subkey)!= m_subtrees.end());

            if(m_subtrees.find(subkey)!= m_subtrees.end() ){
                refnodePtr base_node = m_subtrees[subkey]->get_base();
                vector<Tuple> v =  base_node->get_range(start, end_bound,limit, offset);
                for(unsigned int i=0;i < v.size();i++){
                    res.push_back(m_itemList.get(v[i]));
                }
            }
            return res;
        }

        vector<VALUE> get_range_by_limit(Tuple start, int limit, INDEX_TYPE subkey){
            vector<VALUE> res;
            if(m_subtrees.find(subkey)!= m_subtrees.end() ){
                refnodePtr base_node = m_subtrees[subkey]->get_base();
                int localIndex=-1;
                Elm e(start);
                refnodePtr startNode = base_node->searchGL(e,localIndex);
                vector<Tuple> v =  base_node->get_ordinal_range(startNode, localIndex,limit);
                for(unsigned int i=0;i < v.size();i++){
                    res.push_back(m_itemList.get(v[i]));
                }
            }
            return res;
        }

        Tuple max(const INDEX_TYPE subkey){
            Q_ASSERT(m_subtrees.contains(subkey));
            Elm e = m_subtrees[subkey]->get_base()->largest_key_in_subtree();
            return e.m_key;
        }

        Tuple getOrCreateId(VALUE v){
            Tuple id = this->m_itemList.getExistingTupleId(v);
            if(!id.isEmpty()){ return id; }
            return m_itemList.insertOrReplace(v);
        }

        //Return whether value previously existed...
        Tuple addEntry(long k,  VALUE v, const INDEX_TYPE subkey){
            const Tuple t(k);
            return addEntry(t, v, subkey);
        }

        Tuple addEntry(long k,  long l, VALUE v, const INDEX_TYPE subkey){
            Tuple t(k, l);
            return addEntry(t, v, subkey);
        }

        bool removeEntry(VALUE v, const INDEX_TYPE subkey){
            Q_ASSERT(m_subtrees.find(subkey)!= m_subtrees.end());
            Tuple id = this->m_itemList.getExistingTupleId(v);
            if(id.isEmpty()){ return false; }
            Tuple old_k = getLevel(id, subkey);
            if(old_k.isEmpty()){return false;}
            removeEntry_protected(old_k, id, subkey);
            return true;
        }

        Tuple addEntry(Tuple newLevel, VALUE v, const INDEX_TYPE subkey){

            Q_ASSERT(m_subtrees.contains(subkey));
            Q_ASSERT(!newLevel.isEmpty());

            //Tuple itemInternalId = itemList.getInternalId(v);
            Tuple itemInternalId = m_itemList.insertOrReplace(v);
            Tuple oldLevel = getLevel(itemInternalId, subkey);
            bool sizedIncreases = false;
            if((oldLevel!=newLevel)){
                if(!oldLevel.isEmpty()){
                    sizedIncreases = false;
                    removeEntry_protected(oldLevel, itemInternalId, subkey);
                } else
                    {sizedIncreases = true;}
                addToTree(newLevel, itemInternalId, subkey);
            }
            updateLocationLists(newLevel, itemInternalId, subkey);
            {
                //Tested - but now takes too much time
//                ItemLocation il2 =  itemLocations.get(itemInternalId);
//                Q_ASSERT(!sizedIncreases || (il2).level(subkey)!=oldLevel);
//                Q_ASSERT((il2).level(subkey)==newLevel);
            }
            return itemInternalId;
        }


        Tuple getLevel(Tuple tupleIndex, INDEX_TYPE subkey){
            if(!(m_subtrees.contains(subkey)))
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
            if(!(m_itemList.containsValue(v))){ return false; }
            Tuple id = m_itemList.getExistingTupleId(v);
            Q_ASSERT(!id.isEmpty());
            ItemLocation il = itemLocations.get(id);
            QList<QString> keys = il.keys();
            for(int i=0; i <keys.count(); i++){
                Elm e(il.level(keys[i]), id);
                bool s= m_subtrees[keys[i]]->get_base()->delete_value(e);
                s = s;
                Q_ASSERT(s);
            }
            itemLocations.remove(id);
            m_itemList.remove(id);
            return true;
        }

    protected:

        void removeEntry_protected(Tuple level, Tuple internalId, const INDEX_TYPE subkey){

            Q_ASSERT(!internalId.isEmpty());
            Q_ASSERT(itemLocations.contains(internalId));
            ItemLocation il = itemLocations.get(internalId);
            Q_ASSERT(il.contains(subkey));
            Q_ASSERT(il.level(subkey) == level);
            il.remove(subkey);
            itemLocations.remove(internalId);
            itemLocations.set(internalId, il);
            m_subtrees[subkey]->delete_object(level, internalId); //was bool s =
            Q_ASSERT(!(itemLocations.get(internalId).contains(subkey)));
        }

        void addToTree(Tuple level,  Tuple internalId, const INDEX_TYPE subkey){

            Q_ASSERT(!internalId.isEmpty());
            ItemTreePtr tree = m_subtrees[subkey];
            Q_ASSERT(m_subtrees.contains(subkey));
            Q_ASSERT(!level.isEmpty());
            Q_ASSERT(tree);
            tree->insert_at(level,internalId);

        }

        void updateLocationLists(Tuple level,  Tuple internalId, const INDEX_TYPE subkey){
            Q_ASSERT(!internalId.isEmpty());
            ItemLocation il;
            if(itemLocations.contains(internalId)){ il =  itemLocations.get(internalId); }
            il.SetValue(subkey,level);
            itemLocations.set(internalId,il);
            Q_ASSERT(itemLocations.get(internalId).level(subkey)==level);
            //return internalId;
        }

        void debugOutIndexSize(){


        }

};


#endif // MULTI_TREE_H
