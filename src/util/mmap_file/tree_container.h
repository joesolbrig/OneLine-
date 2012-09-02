#ifndef TREE_CONTAINER_H
#define TREE_CONTAINER_H

#include "btree.h"

template<class KEY, class PAYLOAD> class LTree {
private:
    QSharedPointer< LBTree<KEY, PAYLOAD> > root_tracker;
    bool detached;

public:
    LTree(){
        root_tracker = new LBTree<KEY, PAYLOAD>();
        Node* root_ptr = new Node(tracker);
        tracker.set_root((Node*)null_ptr, root_ptr);
        detached = true;
    }
    LTree(const BTree<Key, T> &other) {
        root_tracker = other.root_tracker;
        detached = false;

    }
    ~LTree(){
        //tracker automatically deleted when out of scope
    }
    void operator =[](KEY k, PAYLOAD p){
        if(!detached){
            root_tracker = root_tracker->duplicate();
        }
        Element<KEY, PAYLOAD> el;
        el.m_key = k;
        el.m_payload = p;
        root_tracker->get_root()->tree_insert(el);
    }
    PAYLOAD& operator [](KEY k){
        Element<KEY, PAYLOAD> el;
        el.m_key = k;
        el.m_payload = p;

        Element<KEY, PAYLOAD> desired;
        desired.m_key = k;
        NODE* last;
        Element<KEY, PAYLOAD>& result = tracker.get_root()->search (desired, last);
        return result->m_payload;
    }
    BTree<KEY, PAYLOAD> &operator=(const BTree<KEY, PAYLOAD> &other){
        root_tracker = other.root_tracker;
        detached = false;
        return *this;

    }

    //There is no internal similarity here so it's just a copy...
    explicit BTree(const typename std::map<KEY, PAYLOAD> &other){
        std::map<KEY, PAYLOAD>::iterator i = other.begin();
        for(i = other.begin(); i!=other.end();i++){
            if(!detached){
                root_tracker = root_tracker->duplicate();
            }
            Element<KEY, PAYLOAD> el;
            el.m_key = i.key();
            el.m_payload = i.value();
            root_tracker->get_root()->tree_insert(el);
        }
    }
    class iterator
    {
        friend class const_iterator;
        Node *base_node;
        BTree *parent;
        unsigned int index;
        unsigned int global_index;
        unsigned long write_index;
        bool detached;
        bool at_end;

        check_refresh(){
            if(root_tracker){
                if(parent->get_insert_index() != write_index){
                    refresh();
                }

            }

        }

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;
        typedef PAYLOAD value_type;
        typedef PAYLOAD *pointer;
        typedef PAYLOAD &reference;

        iterator(){
            at_end= false;
        }
        iterator(Node *node, unsigned int i){
            at_end= false;
            base_node = node;
            index=i;
        }

        const KEY &key() const {node.m_vector[index].m_key;}
       PAYLOAD &value() const{node.m_vector[index].m_value;}
       PAYLOAD &operator*() const{value();}
       PAYLOAD *operator->() const;
        bool operator==(const iterator &o) const;
        bool operator!=(const iterator &o) const;

        //This is only O(log n), it is O(1) in the opposite direction
        iterator &operator++(){
            int above_index = 0;
            check_refresh();
            if(index <base_node->m_count){
                if(base_node->m_vector[index].mp_subtree){
                    base_node = base_node->m_vector[index].mp_subtree;
                } else {
                    index++;
                    return;
                }
            } else while( base_node->mp_parent  && //This climbs the tree finding next items...
                base_node->mp_parent->m_vector[above_index] < base_node->m_vector[index]){
                if(above_index > base_node->mp_parent->m_count-1){
                    if(!base_node->mp_parent->mp_parent){
                        base_node = 0;
                        base_index=0;
                        at_end= true;
                        return;
                    }
                    base_node = base_node->mp_parent;
                    above_index = 0;
                    base_index = base_node->mp_parent->m_count-1;
                }
                above_index++;
            }
            base_node = base_node->mp_parent;
            base_index = above_index;
        }
        iterator operator++(int);
        iterator &operator--();
        iterator operator--(int) ;
        iterator operator+(int j) const;
        iterator operator-(int j) const;
        iterator &operator+=(int j);
        iterator &operator-=(int j);

        bool operator==(const const_iterator &o) const;
        bool operator!=(const const_iterator &o) const;

    };
    friend class iterator;

    class const_iterator
    {

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;
        typedef PAYLOAD value_type;
        typedef const PAYLOAD *pointer;
        typedef const PAYLOAD &reference;

        const_iterator();
        const_iterator(BTreeData::Node *node);
        const_iterator(const iterator &o);

        const KEY &key() const;
        const PAYLOAD &value() const;

        const PAYLOAD &operator*() const;
        const PAYLOAD *operator->() const;
        bool operator==(const const_iterator &o);
        bool operator!=(const const_iterator &o);

        const_iterator &operator++();
        const_iterator operator++(int);
        const_iterator &operator--() ;
        const_iterator operator--(int);
        const_iterator operator+(int j) const;
        const_iterator operator-(int j) const ;
        const_iterator &operator+=(int j);
        const_iterator &operator-=(int j);

    };
    friend class const_iterator;

    std::map<KEY, PAYLOAD> toStdMap() const{
        iterator i;
        std::map<KEY, PAYLOAD> m;
        for(i = begin(); i!=end();i++){
            m[i.key()] = i.value();
        }
        return m;
    }

    bool operator==(const BTree<KEY, PAYLOAD> &other) const{
        return root_tracker == other.root_tracker;
    }
    bool operator!=(const BTree<KEY, PAYLOAD> &other) const{
        return !(root_tracker == other.root_tracker);
    }

    int size(){
        return root_tracker->size();
    }
    bool isEmpty(){
        return (root_tracker->size() !=0);
    }

    void detach(){
        root_tracker = root_tracker->duplicate();
        detached = true;
    }
    bool isDetached(){
        return detached;
    }
//    void setSharable(bool sharable);
//    void setInsertInOrder(bool ordered);
    void clear();

    int remove(const KEY &key);
    PAYLOAD take(const KEY &key);

    bool contains(const KEY &key) const;
    const Key key(const PAYLOAD &value) const;
    const Key key(const PAYLOAD &value, const Key &defaultKey) const;
    const PAYLOAD value(const KEY &key) const;
    const PAYLOAD value(const KEY &key, const PAYLOAD &defaultValue) const;
   PAYLOAD &operator[](const KEY &key);
    const PAYLOAD operator[](const KEY &key) const;

    QList<Key> uniqueKeys() const;
    QList<Key> keys() const;
    QList<Key> keys(const PAYLOAD &value) const;
    QList<T> values() const;
    QList<T> values(const KEY &key) const;
    int count(const KEY &key) const;

};


template <class Key, class PAYLOAD> class BTree
{
private:
    bool detached;//is this our original or a copy
    LBTree* root_tracker;

public:
    BTree();
    BTree(const BTree<KEY, PAYLOAD> &other) ;
    ~BTree();

    BTree<KEY, PAYLOAD> &operator=(const BTree<KEY, PAYLOAD> &other);
    explicit BTree(const typename std::map<KEY, PAYLOAD> &other);
    std::map<KEY, PAYLOAD> toStdMap() const;

    bool operator==(const BTree<KEY, PAYLOAD> &other) const;
    bool operator!=(const BTree<KEY, PAYLOAD> &other) const;

    int size();
    bool isEmpty();

    void detach();
    bool isDetached();
    void setSharable(bool sharable);
    void setInsertInOrder(bool ordered);
    void clear();

    int remove(const KEY &key);
    PAYLOAD take(const KEY &key);

    bool contains(const KEY &key) const;
    const Key key(const PAYLOAD &value) const;
    const Key key(const PAYLOAD &value, const Key &defaultKey) const;
    const PAYLOAD value(const KEY &key) const;
    const PAYLOAD value(const KEY &key, const PAYLOAD &defaultValue) const;
   PAYLOAD &operator[](const KEY &key);
    const PAYLOAD operator[](const KEY &key) const;

    QList<Key> uniqueKeys() const;
    QList<Key> keys() const;
    QList<Key> keys(const PAYLOAD &value) const;
    QList<T> values() const;
    QList<T> values(const KEY &key) const;
    int count(const KEY &key) const;

    class const_iterator;

    class iterator
    {
        friend class const_iterator;
        Node *base_node;
        BTree *parent;
        unsigned int index;
        unsigned int global_index;
        unsigned long write_index;
        bool detached;
        bool at_end;

        check_refresh(){
            if(root_tracker){
                if(parent->get_insert_index() != write_index){
                    refresh();
                }

            }

        }

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;
        typedef PAYLOAD value_type;
        typedef PAYLOAD *pointer;
        typedef PAYLOAD &reference;

        iterator(){
            at_end= false;
        }
        iterator(Node *node, unsigned int i){
            at_end= false;
            base_node = node;
            index=i;
        }

        const KEY &key() const {node.m_vector[index].m_key;}
       PAYLOAD &value() const{node.m_vector[index].m_value;}
       PAYLOAD &operator*() const{value();}
       PAYLOAD *operator->() const;
        bool operator==(const iterator &o) const;
        bool operator!=(const iterator &o) const;

        //This is only O(log n), it is O(1) in the opposite direction
        iterator &operator++(){
            int above_index = 0;
            check_refresh();
            if(index <base_node->m_count){
                if(base_node->m_vector[index].mp_subtree){
                    base_node = base_node->m_vector[index].mp_subtree;
                } else {
                    index++;
                    return;
                }
            } else while( base_node->mp_parent  && //This climbs the tree finding next items...
                base_node->mp_parent->m_vector[above_index] < base_node->m_vector[index]){
                if(above_index > base_node->mp_parent->m_count-1){
                    if(!base_node->mp_parent->mp_parent){
                        base_node = 0;
                        base_index=0;
                        at_end= true;
                        return;
                    }
                    base_node = base_node->mp_parent;
                    above_index = 0;
                    base_index = base_node->mp_parent->m_count-1;
                }
                above_index++;
            }
            base_node = base_node->mp_parent;
            base_index = above_index;
        }
        iterator operator++(int);
        iterator &operator--();
        iterator operator--(int) ;
        iterator operator+(int j) const;
        iterator operator-(int j) const;
        iterator &operator+=(int j);
        iterator &operator-=(int j);

        bool operator==(const const_iterator &o) const;
        bool operator!=(const const_iterator &o) const;

    };
    friend class iterator;

    class const_iterator
    {

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef ptrdiff_t difference_type;
        typedef PAYLOAD value_type;
        typedef const PAYLOAD *pointer;
        typedef const PAYLOAD &reference;

        const_iterator();
        const_iterator(BTreeData::Node *node);
        const_iterator(const iterator &o);

        const KEY &key() const;
        const PAYLOAD &value() const;

        const PAYLOAD &operator*() const;
        const PAYLOAD *operator->() const;
        bool operator==(const const_iterator &o);
        bool operator!=(const const_iterator &o);

        const_iterator &operator++();
        const_iterator operator++(int);
        const_iterator &operator--() ;
        const_iterator operator--(int);
        const_iterator operator+(int j) const;
        const_iterator operator-(int j) const ;
        const_iterator &operator+=(int j);
        const_iterator &operator-=(int j);

    };
    friend class const_iterator;

    // STL style
    iterator begin();
    const_iterator begin() const;
    const_iterator constBegin() const;
    iterator end() ;
    const_iterator end() const;
    const_iterator constEnd() const;
    iterator erase(iterator it);

    // more Qt
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;
    int count() const { return d->size; }
    iterator find(const KEY &key);
    const_iterator find(const KEY &key) const;
    const_iterator constFind(const KEY &key) const;
    iterator lowerBound(const KEY &key);
    const_iterator lowerBound(const KEY &key) const;
    iterator upperBound(const KEY &key);
    const_iterator upperBound(const KEY &key) const;
    iterator insert(const KEY &key, const PAYLOAD &value);
    iterator insertMulti(const KEY &key, const PAYLOAD &value);
    BTree<KEY, PAYLOAD> &unite(const BTree<KEY, PAYLOAD> &other);

    // STL compatibility
    typedef Key key_type;
    typedef PAYLOAD mapped_type;
    typedef ptrdiff_t difference_type;
    typedef int size_type;
    bool empty() const;

};


#endif // TREE_CONTAINER_H
