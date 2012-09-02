#ifndef MULTI_TREE_H
#define MULTI_TREE_H

#include <QHash>
#include <QList>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>

#define DEFAULT_KC_FILE "casket.kch"

template<class VALUE> class ShortKVList  class KCHash {
	private:
		 HashDB m_db;

	public:
	KCHash(): m_db(){
		if (!m_db.open(DEFAULT_KC_FILE, HashDB::OWRITER | HashDB::OCREATE)) {
			cerr << "open error: " << db.error().name() << endl;
			Q_ASSERT(false);
		}
	}
	~KCHash(){
		if (!db.close()) {
			cerr << "close error: " << db.error().name() << endl;
		}
	}
	
	void set(QString k, VALUE v){
		QString s;
		QTextStream txs(&s);
		txs << v;
		m_db.set(k.toAscii(),s.toAscii());
	}
	
	QString get(QString k){
		VALUE v;
		char* c_str = m_db.get(k.toAscii)
		QString s(c);
		QTextStream txs(&s);
		txs >> v;
		return v;
	}
	
	bool contains(QStrking k){
		m_db.contain(k.toAscii());
		
	}
}


#define hHash(x) stringHash(x)

inline Hash_id qHash(BucketPos x ){ return x.bigIndex*1000 + x.smallIndex; }

template <class VALUE, class INDEX_TYPE> class Bucket {
	KCHash<VALUE> m_bucket;
	
    public:
        Bucket(){
            size = 0;
        }

        qint32 getSize(){
            return size;
        }

        //Adding removing to bucket:
        int add(VALUE v){
		Hash_id i = hHash(v.getId());
		int t=0;
		
		VALUE tVal;
		QString a("%1%2");
		QString f;
		//for hash collision case
		while(true){
			f = a.arg(i).arg(t)
			tVal = m_bucket.get(f);
			if(tVal.isEmpty()){ break; }
			if(tVal == v){ return t; }
			t++;
		}
		m_bucket.set(f, v);
		size++;
		return t;
        }

        VALUE get_value(Hash_id i, Sub_id s){
		QString a("%1%2");
		QString f;
		f = a.arg(i).arg(t)
		return m_bucket.get(f);
        }

        VALUE get_value(BucketPos bp){
            return itemBucket[bp.bigIndex][bp.smallIndex];
        }

        VALUE get_value(Hash_id id, INDEX_TYPE t){
            if(!itemBucket.contains(id))
                { return VALUE(); }

            for(int i=0; i < itemBucket[id]; i++){
                if(itemBucket[id][i].getId() == t)
                    { return itemBucket[id][i]; }
            }
            return false;
        }

        VALUE get_value(INDEX_TYPE t){
            Hash_id id = hHash(t);
            if(!itemBucket.contains(id))
                { return VALUE(); }

            for(int i=0; i < itemBucket[id].size(); i++){
                if(itemBucket[id][i].getId() == t)
                    { return itemBucket[id][i]; }
            }
            return VALUE();
        }

        bool remove(Hash_id id, Sub_id s){
            itemBucket[id][s] = VALUE();
            itemBucketLength[id]= itemBucketLength[id]-1;
            if(itemBucketLength[id]==0){
                itemBucketLength.remove(id);
            }
            if(!itemBucket[id][s].isEmpty())
                { size--;}
            return true;
        }

        QList<Hash_id> keys(){
            return itemBucket.keys();
        }

        QList<VALUE> get_all_values(){
            QList<VALUE> res;
            for(int i=0; i < itemBucket.size(); i++){
                for(int j=0; j < itemBucket[i].size(); j++){
                    res.append(itemBucket[i][j]);
                }
            }
            return res;
        }

};



//Just for the QT creator syntax highligher...
class Tuple;
template<class KEY, class VALUE> class LBTree;
template<class KEY, class VALUE> class Element;


#define Hash_id qint32
#define Sub_id short


//Crude key-value map intended to be really small... not fast

template<class ID_TYPE, class VALUE> class ShortKVList {
    struct ShortKVList_N {
            typedef ShortKVList_N N;

            N* nextN;
            VALUE v;
            ID_TYPE ref;

            ShortKVList_N(ID_TYPE id): nextN(0){ ref = id; }
            ~ShortKVList_N(){ if(nextN){ delete(nextN); } }

            //allocates stuff if empty like
            VALUE& operator[] (ID_TYPE id){
                N* counter = this;
                if(!counter->ref){
                    counter->ref = id;
                    return counter->v;
                }
                while(counter->nextN && !(counter->ref==id)){ counter = counter->nextN; }
                if(counter->ref==id){ return counter->v; }
                Q_ASSERT(!counter->nextN);
                counter->nextN = new ShortKVList_N(id);
                return counter->nextN->v;
            }

            bool contains(ID_TYPE id){
                N* counter = this;
                while(counter->nextN && !(counter->ref==id)){ counter = counter->nextN; }
                return counter->ref==id;
            }

            ShortKVList_N* remove(ID_TYPE id){
                N* counter = this;
                N* prev = 0;
                while(counter->nextN && !(counter->ref==id)){
                    prev = counter;
                    counter = counter->nextN;
                }
                if(!(counter->ref == id)){ return 0; }
                if(prev){ prev->nextN = counter->nextN; }
                return counter; //to be deleted in parent
            }
            QList<ID_TYPE> keys(){
                QList<ID_TYPE> res;
                N* counter = this;
                while(counter){
                    res.append(counter->ref);
                    counter = counter->nextN;
                }
                return res;

            }

            int size(){
                int sz=0;
                N* counter = this;
                while(counter){
                    sz++;
                }
                return sz;

            }
    };

private:
    ShortKVList_N* d;
public:
    ShortKVList():  d(0){}
    ~ShortKVList() { if(d) {delete d;} }

    VALUE& operator[] (ID_TYPE id){
        if(!d){
            d = new ShortKVList_N( id);
            return d->v;
        } else { return (*d)[id]; }
    }
    void  remove(ID_TYPE id){
        ShortKVList_N* rm = d->remove(id);
        if(rm){
            if(rm == d){ d = d->nextN; }
            delete rm;
        }
    }

    bool contains(ID_TYPE id){ return d? d->contains(id): false;}
    QList<ID_TYPE> keys(){return d? d->keys(): QList<ID_TYPE>();}
    int size(){return d? d->size(): 0;}



};







struct BucketPos {
    Hash_id bigIndex;
    Sub_id smallIndex;
    BucketPos(){
        bigIndex = -1; smallIndex = -1;

    }
    BucketPos(Hash_id b, Sub_id s){ bigIndex = b; smallIndex = s; }

    bool lessThan(BucketPos& o) const {
        if(bigIndex < o.bigIndex){ return true; }
        if(bigIndex > o.bigIndex){ return false;}

        if(smallIndex < o.smallIndex){ return true; }
        if(smallIndex > o.smallIndex){ return false;}
        return false;
    }

    bool eq(BucketPos& o) const{
        return (bigIndex == o.bigIndex) && (smallIndex == o.smallIndex);
    }

    bool operator < (BucketPos& o) const{
        return lessThan(o);
    }

    bool operator > (BucketPos& o) const{
        return (!lessThan(o) && !eq(o));
    }

    bool operator == (BucketPos& o) const{
        return eq(o);
    }

    bool operator != (BucketPos& o) const{
        return !eq(o);
    }

    bool operator <= (BucketPos& o) const{
        return lessThan(o) || eq(o);
    }

    bool operator >= (BucketPos& o) const{
        return (!lessThan(o) );
    }

};




//INDEX_TYPE defaults to string
template <class VALUE, class INDEX_TYPE>   class MultiTree {
    private:
        //items psuedo-hash-buckets
        Bucket<VALUE, INDEX_TYPE> itemBucket;

        //Indices of items
        typedef LBTree< Tuple, BucketPos> ItemTree;
        QHash<INDEX_TYPE, ItemTree* >  subtrees;

        //house-keepin Indices showing item Locations...
        QHash< INDEX_TYPE, QHash< Hash_id, ShortKVList< Sub_id, Tuple > > > itemValues;

        typedef Node<Tuple, BucketPos> refnode;
        typedef Element<Tuple, BucketPos> Elm;

    public:

        MultiTree(){ }
        ~MultiTree(){

            typename QHash<INDEX_TYPE, ItemTree*>::const_iterator i = subtrees.begin();
            for(;i!=subtrees.end();i++){
                INDEX_TYPE s = i.key();
                delete subtrees[s];
            }
        }

        void testStruct(){

            typename QHash<INDEX_TYPE, ItemTree*>::const_iterator i = subtrees.begin();
            for(;i!=subtrees.end();i++){
                INDEX_TYPE s = i.key();
                Q_ASSERT(subtrees[s]);
                subtrees[s]->get_root()->testStruct();

            }

            //Subscript MADNESS but what the heck...
            QList<INDEX_TYPE> ks = itemValues.keys();
            for( int i=0; i < ks.count(); i++){
                QList<Hash_id> hiks= itemValues[ks[i]].keys();
                for(Hash_id j=0; j < hiks.size();j++){
                    QList<Sub_id> siks= itemValues[ks[i]][hiks[j]].keys();
                    for(Sub_id s=0; s < siks.size();s++){
                        Tuple v = itemValues[ks[i]][hiks[j]][siks[s]];
                        BucketPos bp(hiks[j],siks[s]);
                                //itemValues[ks[i]][hiks[j]][siks[s]];
                        Elm e(v,bp);
                        int index;
                        refnode* e_res = subtrees[ks[i]]->get_root()->find_value_node(e, index);
                        Q_ASSERT(e_res);
                        Q_ASSERT((*e_res)[index].m_key == v);
                        bp = (*e_res)[index].m_payload;
                        //Q_ASSERT(bp == BucketPos(hiks[j],siks[s]));
//                        Tuple t = get_tuple(s,k);
//                        Q_ASSERT(t==v);
                    }
                }
            }

        }

        bool addIndex(const INDEX_TYPE index_name){
            if(subtrees.contains(index_name)) { return false; }
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
            Hash_id id = hHash(v.getId());
            if(itemBucket.is_at(v, id)) {
                return true;
            } else {
                return false;
            }
        }

        bool removeIndex(const INDEX_TYPE index_name){
            typename QHash<INDEX_TYPE, ItemTree* >::iterator i =subtrees.find(index_name);
            if(i!= subtrees.end()){
                delete subtrees[index_name];
                return true;
            } else { return false;}
        }

        long max_size() {return itemBucket.getSize();}

        VALUE get_value(INDEX_TYPE id){
            return itemBucket.get_value(id);
        }

        long count(){
            return itemBucket.getSize();
        }

        long count(INDEX_TYPE index_name){
            Q_ASSERT(subtrees.find(index_name)!= subtrees.end());
            return subtrees[index_name]->size();
        }

        QList<VALUE> get_list(){
            return itemBucket.get_all_values();
        }

        qint64 get_range_count(long k_start, long k_end, const INDEX_TYPE treekey){
            Tuple s(k_start);
            Tuple e(k_end);
            return get_range_count(s,e, treekey);
        }

        qint64 get_range_count(Tuple k_start, Tuple k_end, const INDEX_TYPE treekey){
            Q_ASSERT(subtrees.find(treekey)!= subtrees.end());
            return subtrees[treekey]->get_range_count(k_start, k_end);
        }

        vector<VALUE> get_range_by_pos(long start, long end, const INDEX_TYPE treekey){
            Q_ASSERT(end>=start);
            Q_ASSERT(subtrees.find(treekey)!= subtrees.end());
            ItemTree* tree =subtrees[treekey];
            std::vector<VALUE> res;

            if(end > start){
                refnode* start_n=0;
                unsigned int index=0;
                tree->get_root()->ordinalSearch(start, start_n, index);
                res = get_range_by_count(start_n, index, end-start,treekey);
                Q_ASSERT(res.size() == MIN(tree->get_root()->m_total_elements-start, (unsigned int)end- start));
            }
            return res;
        }

        vector<VALUE> get_range_by_count(refnode* start_n, int small_index, int count, const INDEX_TYPE treekey){
            Q_ASSERT(subtrees.find(treekey)!= subtrees.end());

            vector<BucketPos> v;
            v=  subtrees[treekey]->get_root()->get_ordinal_range(start_n, small_index, count);
            vector<VALUE> res;
            for(unsigned int i=0;i < v.size();i++){
                res.push_back(itemBucket.get_value(v[i]));
            }
            return res;
        }


        //Find the value itself? Yes, the gives the saved version...
        VALUE find(VALUE v){
            INDEX_TYPE id = v.getId();
            return itemBucket.get_value(id);
        }


        VALUE find(long k,  const INDEX_TYPE treekey){
            Tuple t(k);
            return find(t, treekey);
        }

        VALUE find(Tuple k, const INDEX_TYPE treekey){
            Q_ASSERT(subtrees.find(treekey)!= subtrees.end());
            BucketPos bp;
            Elm e(k,bp);
            refnode* node;
            int index;
            Elm e_res = subtrees[treekey]->get_root()->search(e, node, index);
            if (e_res.matches(refnode::m_failure)){
                VALUE v;
                return v;
            }
            bp = e_res.m_payload;
            Q_ASSERT(!(itemBucket.get_value(bp)==VALUE()));
            return itemBucket.get_value(bp);
        }

        long get_order(Tuple k, const INDEX_TYPE treekey){
            Q_ASSERT(subtrees.contains(treekey));
            Elm e(k);
            return subtrees[treekey]->get_position(k);
        }

        vector<VALUE> get_range(long start, long end_bound, INDEX_TYPE treekey){
            return get_range(Tuple(start), Tuple(end_bound),treekey);
        }

        vector<VALUE> get_range(Tuple start, Tuple end_bound, INDEX_TYPE treekey){
            vector<VALUE> res;

            typename QHash<INDEX_TYPE,Tuple >::iterator i;

            if(subtrees.find(treekey)!= subtrees.end() ){
                refnode* root = subtrees[treekey]->get_root();
                vector<BucketPos> v =  root->get_range(start, end_bound);
                for(unsigned int i=0;i < v.size();i++){
                    res.push_back(itemBucket.get_value(v[i]));
                }
            }
            return res;
        }

        Tuple max(const INDEX_TYPE treekey){
            Q_ASSERT(subtrees.contains(treekey));
            Elm e = subtrees[treekey]->get_root()->largest_key_in_subtree();
            return e.m_key;
        }

        //Return whether value previously existed...
        bool add(long k,  VALUE v, const INDEX_TYPE treekey){
            const Tuple t(k);
            return add(t, v, treekey);
        }

        bool add(long k,  long l, VALUE v, const INDEX_TYPE treekey){
            Tuple t(k, l);
            return add(t, v, treekey);
        }

        bool removeEntry(VALUE v, const INDEX_TYPE treekey){
            Q_ASSERT(subtrees.contains(treekey));
            Tuple old_k = get_tuple(v, treekey);
            bool remove_old = !old_k.isEmpty();
            if(remove_old){ remove_protected(old_k, v, treekey); }
            return remove_old ;
        }

        bool add(Tuple new_k, VALUE v, const INDEX_TYPE treekey){
            Q_ASSERT(!new_k.isEmpty());
            Q_ASSERT(subtrees.contains(treekey));

            INDEX_TYPE ind = v.getId();
            Hash_id id = hHash(ind);
            Sub_id s = itemBucket.subkey(v);
            qDebug() << "subindex s: " << s;
            if(s ==-1){
                s = itemBucket.add(v);
            }

            Tuple old_k = get_tuple(ind, treekey);
            if(!old_k.isEmpty()){
                VALUE old_v = itemBucket.get_value(id,s);
                old_v.merge(v);
                v = old_v;
                remove_from_index(id,s,treekey);
            }
            BucketPos bp(id, s);
            subtrees[treekey]->insert_at(new_k,bp);
            itemValues[treekey][id][s] = new_k;
            return true;
        }

        Tuple get_tuple(INDEX_TYPE ind, INDEX_TYPE treekey){
            Q_ASSERT((subtrees.contains(treekey)));
            Hash_id id = hHash(ind);

            if(!(itemValues[treekey].contains(id)))
                { return Tuple();}

            Sub_id s = itemBucket.subkey(ind);
            if(!(itemValues[treekey][id].contains(s)))
                { return Tuple();}

            return itemValues[treekey][id][s];

        }



        void remove_from_index(Hash_id id, Sub_id s, INDEX_TYPE treekey){

            if(!(itemValues[treekey].contains(id)))
                { return;}
            if(!(itemValues[treekey][id].contains(s)))
                { return;}

            Tuple t = itemValues[treekey][id][s];
            BucketPos bp(id, s);
            Elm e(t, bp);

            bool suc= subtrees[treekey]->get_root()->delete_value(e);
            suc = suc;
            Q_ASSERT(suc);

            itemValues[treekey][id].remove(s);
            if(itemValues[treekey][id].size()==0){
                itemValues[treekey].remove(id);
                //always keep treekey entry, though...
            }

        }

        //Our nodes are preserved in memory
        //BUT our elements are copied by value as the tree changes...
        //So we always need to search the present node for our element
        //
        bool removeObject(VALUE v){
            INDEX_TYPE ind = v.getId();
            Hash_id id = hHash(ind);
            Sub_id s = itemBucket.subkey(ind);
            if(s==-1){return false;}

            QList<INDEX_TYPE> ks = itemValues.keys();
            for( int i=0; i < ks.count(); i++){
                remove_from_index( id,s,ks[i]);
            }
            itemBucket.remove(id,s);
            return true;

        }

    protected:

        void remove_protected(Tuple t, VALUE v, const INDEX_TYPE treekey){
            Hash_id id = hHash(v.getId());
            Sub_id s = itemBucket.subkey(v.getId());

            BucketPos bp(id, s);
            Elm e(t, bp);

            bool suc= subtrees[treekey]->get_root()->delete_value(e);
            suc = suc;
            Q_ASSERT(suc);

            itemValues[treekey][id].remove(s);
            if(itemValues[treekey][id].size()==0){
                itemValues[treekey].remove(id);
                //always keep treekey entry, though...
            }

        }


};


#endif // MULTI_TREE_H
