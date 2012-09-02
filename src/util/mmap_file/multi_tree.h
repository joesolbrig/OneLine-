#ifndef MULTI_TREE_H
#define MULTI_TREE_H

#include <QHash>
#include <QList>
#include "mmap_pointer.h"

//Just for the QT creator syntax highligher...
//class Tuple;
//class LBTree;
//class Element;


typedef MPointer<ItemTree> ItemTreePtr;
typedef MPointer<ItemLocation> ItemLocationPtr;


//ID_TYPE defaults to string
template <class VALUE, class INDEX_TYPE>   class MultiTree {
    private:
        //Actual items...
        QHash< INDEX_TYPE, VALUE> itemList;

        //Indices of items
        typedef LBTree< Tuple, INDEX_TYPE> ItemTree;
        QHash<INDEX_TYPE, ItemTreePtr >  subtrees;

        //house-keepin Indices showing item Locations...
        //typedef TreePosition<Tuple, INDEX_TYPE> TreePos;
        typedef QHash<INDEX_TYPE, Tuple > ItemLocation;
        QHash<INDEX_TYPE, ItemLocationPtr > itemLocations;

        typedef Node<Tuple, VALUE> refnode;
        typedef MPointer<refnode> refnodePtr;
        typedef Element<Tuple, VALUE> Elm;

    public:
        MultiTree(){ }
        ~MultiTree(){
            typename QHash<INDEX_TYPE, ItemLocationPtr >::iterator l_i;
            for(l_i=itemLocations.begin();l_i!=itemLocations.end();l_i++){
                ItemLocationPtr s =l_i.value();
                delete s;
            }

            typename QHash<INDEX_TYPE, ItemTreePtr>::const_iterator i = subtrees.begin();
            for(;i!=subtrees.end();i++){
                INDEX_TYPE s = i.key();
                delete subtrees[s];
            }
        }

        void testStruct(){

            typename QHash<INDEX_TYPE, ItemTreePtr>::const_iterator i = subtrees.begin();
            for(;i!=subtrees.end();i++){
                INDEX_TYPE s = i.key();
                Q_ASSERT(subtrees[s]);
                subtrees[s]->get_base()->testStruct();

            }

            QList<INDEX_TYPE> ids = itemList.keys();
            for( int i=0; i < ids.count(); i++){
                ItemLocationPtr il = itemLocations[ids[i]];
                QList<INDEX_TYPE> ks = il->keys();
                for(int j=0;j< ks.count();j++){
                    INDEX_TYPE k = ks[j];
                    INDEX_TYPE s = ids[i];
                    Tuple v = (*il)[k];
                    Elm e(v,s);
                    int index;
                    refnode* e_res = subtrees[k]->get_base()->find_value_node(e, index);
                    Q_ASSERT(e_res);
                    Q_ASSERT((*e_res)[index].m_key == v);

                    INDEX_TYPE r = (*e_res)[index].m_payload;
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
            ItemTreePtr it = new ItemTree();
//            refnode* root_ptr = new refnode(*it);
//            it->set_root ((refnode*)(reinterpret_cast<refnode*> (0)), root_ptr);
            it->createBase();
            subtrees[index_name] = it;
            Q_ASSERT(subtrees[index_name]->get_base());
            return true;

        }

        long indexSize(const INDEX_TYPE index_name){
            Q_ASSERT((subtrees.find(index_name)!= subtrees.end()));
            return subtrees[index_name]->size();
        }

        bool contains(VALUE v){
            if(itemList.find(v.getId())!= itemList.end()) {
                Q_ASSERT(itemLocations.find(v.getId())!= itemLocations.end());
                return true;
            } else {
                Q_ASSERT(!(itemLocations.find(v.getId())!= itemLocations.end()));
                return false;
            }

        }

        bool removeIndex(const INDEX_TYPE index_name){
            typename QHash<INDEX_TYPE, ItemTreePtr >::iterator i =subtrees.find(index_name);
            if(i!= subtrees.end()){
                delete subtrees[index_name];
                return true;
            } else { return false;}
        }

        long max_size() {return itemList.size();}

        VALUE get_value(INDEX_TYPE id){
            if(itemList.find(id)!= itemList.end()){
                return itemList[id];
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

        qint32 get_range_count(long k_start, long k_end, const INDEX_TYPE subkey){
            Tuple s(k_start);
            Tuple e(k_end);
            return get_range_count(s,e, subkey);
        }

        qint32 get_range_count(Tuple k_start, Tuple k_end, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            return subtrees[subkey]->get_range_count(k_start, k_end);
        }

        vector<VALUE> get_range_by_pos(long start, long end, const INDEX_TYPE subkey){
            Q_ASSERT(end>=start);
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            ItemTreePtr tree =subtrees[subkey];
            std::vector<VALUE> res;

            if(end > start){
                refnode* start_n=0;
                unsigned int index=0;
                tree->get_base()->ordinalSearch(start, start_n, index);
                res = get_range_by_count(start_n, index, end-start,subkey);
                Q_ASSERT(res.size() == MIN(tree->get_base()->m_total_elements-start, (unsigned int)end- start));
            }
            return res;
        }

        vector<VALUE> get_range_by_count(refnode* start_n, int small_index, int count, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());

            vector<INDEX_TYPE> v;
            v=  subtrees[subkey]->get_base()->get_ordinal_range(start_n, small_index, count);
            vector<VALUE> res;
            for(unsigned int i=0;i < v.size();i++){
                res.push_back(itemList[v[i]]);
            }
            return res;
        }




        VALUE find(VALUE v){
            INDEX_TYPE id = v.getId();
            if(itemList.find(id)!= itemList.end()){
                return itemList[id];
            } else {
                return VALUE();
            }
        }


        VALUE find(long k,  const INDEX_TYPE subkey){
            Tuple t(k);
            return find(t, subkey);
        }

        VALUE find(Tuple k, const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Elm e_res = subtrees[subkey]->get_base()->search_by_key(k);
            if (e_res.matches(refnode::m_failure)){
                VALUE v;
                return v;
            }
            INDEX_TYPE id = e_res.m_payload;
            Q_ASSERT(itemList.find(id)!=itemList.end());
            return itemList[id];
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
                refnode* root = subtrees[subkey]->get_base();
                vector<INDEX_TYPE> v =  root->get_range(start, end_bound);
                for(unsigned int i=0;i < v.size();i++){
                    res.push_back(itemList[v[i]]);
                }
            }
            return res;
        }

        Tuple max(const INDEX_TYPE subkey){
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Elm e = subtrees[subkey]->get_base()->largest_key_in_subtree();
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
            Tuple old_k = get_tuple(v.getId(), subkey);
            bool remove_old = !old_k.isEmpty();
            if(remove_old){ remove_protected(old_k, v, subkey); }
            return remove_old ;
        }

        bool add(Tuple new_k, VALUE v, const INDEX_TYPE subkey){

            INDEX_TYPE id = v.getId();
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Tuple old_k = get_tuple(id, subkey);
            bool remove_old = true;
            Q_ASSERT(!new_k.isEmpty());
            if(old_k==new_k){
                VALUE old_v = itemList[id];
                old_v.merge(v);
                update_entry_protected(new_k, old_v, subkey);
                //skip this assert 'cause we want to avoid square bracket access
                //which add items if they don't exist...
                //Q_ASSERT(!((*itemLocations[id])[subkey]!=old_k));
            } else {
                remove_old = !old_k.isEmpty();
                if(remove_old){
                    remove_protected(old_k, v, subkey);
                }
                add_protected(new_k, v, subkey);
                ItemLocationPtr il2 =  itemLocations[id];
                Q_ASSERT((*il2)[subkey]!=old_k);
                Q_ASSERT((*il2)[subkey]==new_k);
            }
            return remove_old;
        }

        Tuple get_tuple(INDEX_TYPE id, INDEX_TYPE subkey){
            if(!(subtrees.find(subkey)!= subtrees.end()))
                { return Tuple(); }
            if(!(itemLocations.find(id)!=itemLocations.end()))
                { return Tuple();}
            ItemLocationPtr il = itemLocations[id];
            Q_ASSERT(il);
            if(!(il->find(subkey)!=il->end()))
                { return Tuple();}
            Tuple existingKey =(*il)[subkey];
            Q_ASSERT(!existingKey.isEmpty());
            return existingKey;
        }

        bool removeObject(VALUE v){
            INDEX_TYPE id = v.getId();
            if(!(itemList.find(id)!= itemList.end())){ return false; }
            Q_ASSERT(itemLocations.find(id)!= itemLocations.end());
            ItemLocationPtr il = itemLocations[id];
            //Our nodes are preserved in memory
            //BUT our elements are copied by value as the tree changes...
            //So we always need to search the present node for our element
            //
            typename QHash<INDEX_TYPE,Tuple >::iterator i;
            for(i=il->begin(); i!=il->end(); ++i){
                Elm e((*i), id);
                bool s= subtrees[i.key()]->get_base()->delete_value(e);
                s = s;
                Q_ASSERT(s);
            }
//            typename QHash<INDEX_TYPE, ItemTreePtr>::const_iterator j = subtrees.begin();
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

            INDEX_TYPE id = v.getId();
            Q_ASSERT(itemLocations.find(id)!= itemLocations.end());
            ItemLocationPtr il = itemLocations[id];
            Q_ASSERT(il->find(subkey)!=il->end());
            Q_ASSERT((*il)[subkey] == old_k);
            il->remove(subkey);
            itemLocations.remove(id);
            itemLocations[id] = il;
            subtrees[subkey]->delete_object(old_k, id);
            Q_ASSERT((itemLocations[id]->find(subkey)==itemLocations[id]->end()));
        }

        void add_protected(Tuple k,  VALUE v, const INDEX_TYPE subkey){

            INDEX_TYPE id = v.getId();
            ItemTreePtr tree = subtrees[subkey];
            Q_ASSERT(subtrees.find(subkey)!= subtrees.end());
            Q_ASSERT(!k.isEmpty());
            Q_ASSERT(!(id == VALUE().getId()));
            Q_ASSERT(tree);

            tree->insert_at(k,id);
            update_entry_protected(k, v, subkey);
        }
        void update_entry_protected(Tuple k,  VALUE v, const INDEX_TYPE subkey){
            INDEX_TYPE id = v.getId();
            itemList[id] = v;
            ItemLocationPtr il = 0;
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
