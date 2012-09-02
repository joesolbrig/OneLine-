#ifndef BTREE__H
#define BTREE__H
/*
Updated 2010 by HJS as a template implementing B-tree "With Rank"
This should perform every atomic container operation in O(log(n)):
insert (K,V), delete(V), find nth element V, get order of the element V, etc.
The algorithm has also been modified to allow non-unique keys.

This is version uses Qhash as well as STL as part of the Oneline keystroke launcher,
I may work on making a pure STL version available as well if there is interest.

--------------------------------------------------

original version:  may 9th, 2003

this file contains a class template for elements stored in a B-tree, and a
class for a B-tree node, which provides all the methods needed to search,
insert, or delete.  a sample "main" function is also provided to show
how to use the B-tree.
to understand the identifiers and comments, visualize the tree as having
its root node at the top of the diagram, its leaf nodes at the bottom of
the diagram, and each node as containing an array oriented horizontally,
with the smallest element on the left and the largest element on the right.
the zeroth element of a node contains only a subtree pointer, no key value
or payload.

a b-tree grows "upward", by splitting the root node when the node's capacity
is exceeded.  conversely, the depth of the tree is always reduced by merging
the last remaining element of the root node with the elements of its two
child nodes, so the tree contracts "from the top".

this code may be freely copied.
programmer: toucan@textelectric.net
algorithm and pseudo-code found in:
"fundamentals of data structures in pascal", by Horowitz and Sahni

there is a java applet on the web that draws a b-tree diagram and allows the
user to perform insertions and deletions, so you can see how it grows and shrinks,
at: http://www.mars.dti.ne.jp/~torao/program/structure/btree.html
*/

using namespace std;
using namespace __gnu_cxx;
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <QHash>
#include <QDebug>
#include <QStringList>
#include "limits.h"

#include "mmap_pointer.h"



class ItemTree;
class ItemLocation;
class Elem;

template<class KEY, class VALUE, class PARAM_VALUE> class Node;


//I want to use typedef but heck...
//#define NodePtr Node*
#define NodePtr MPointer< Node >

//#define LBTreePtr LBTree<KEY,VALUE,PARAM_VALUE>*
#define ElemPtr Elem*
//#define NodeMapPtr Node<KEY, VALUE, PARAM_VALUE>*
#define NodeMapPtr MPointer< Node<KEY, VALUE, PARAM_VALUE> >


//#define createNodePtr(arg)
//MPointer<Node>(void* ptr = MPointer<Node>::allocate(), (new(ptr) Node(arg)))
//#define createNodePtr(arg) MPointer<Node>((new(MPointer<Node>::allocate()) Node(arg)))
#define createBaseNodePtr() MPointer<Node<KEY, VALUE, PARAM_VALUE> >((new(MPointer<Node<KEY, VALUE, PARAM_VALUE> >::allocate()) Node<KEY, VALUE, PARAM_VALUE>()))
#define destroyPtr(arg) (MPointer<Node<KEY, VALUE, PARAM_VALUE> >::deallocate(arg))

#define invalid_ptr (MPointer< Node<KEY, VALUE, PARAM_VALUE> > (INVALID_INDEX, true))
#define null_ptr (MPointer< Node<KEY, VALUE, PARAM_VALUE> >(NULL_PTR_INDEX, true))
//#define NULL_PTR_INDEX (-1)

const unsigned int max_elements = 200;  // max elements in a node

// size limit for the array in a vector object.  best performance was
// at 800 bytes.
const unsigned int max_array_bytes = 800;

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#define SPLITCHAR "#"
#define SPLITCHARC '#'
#define SECOND_SPLITCHAR "$"
#define THIRD_SPLITCHAR "~"
#define STRING_MODEL "%1" SPLITCHAR "%2"

extern bool gTest_tree_struct;

class PQString :public QString {
public:
    static QString typeName() {return "PQString";}
    PQString(char* s): QString(s){}
    PQString(const char* s): QString(s){}
    PQString(QString s): QString(s){}

};

//Wrapped on a static array
template<class VALUE, size_t SIZE> class StaticArray {
private:
    VALUE body[SIZE];
    int i;
public:

    StaticArray(){ i=0;}
    StaticArray(const StaticArray<VALUE,SIZE>& t){
        for(int i=0; i<SIZE;i++){
            body[i] = t.body[i];
        }
    }

    VALUE &
    operator[](qint64 n) const {
        return body[n];
    }
    VALUE &
    operator[](qint64 n) {
        return body[n];
    }

    void push_back(const VALUE& v){
        Q_ASSERT(i<SIZE);
        body[i] = v;
        i++;
    }
    StaticArray<VALUE,SIZE>& operator=(const StaticArray<VALUE,SIZE>& t)  {
        for(int i=0; i<SIZE;i++){
            body[i] = t.body[i];
        }
        return *this;
    }
    qint64 size(){ return SIZE;}
};

class Tuple {
    friend QDataStream &operator << (QDataStream &out, const Tuple &item);
    friend QDataStream &operator >> (QDataStream &in, Tuple &item);

    private:
        long first;
        long second;

    public:
        //Null valued but should never be within the tree...
        Tuple() {
            first = -1;
            second = 0;

        }
        Tuple(long l) {
            Q_ASSERT(l!=-1);
            first = l;
            second = 0;
        }
        Tuple(long l, long k) {
            Q_ASSERT(l!=-1);
            first = l;
            second = k;

        }
        Tuple(int l) {
            Q_ASSERT(l!=-1);
            first = l;
            second = 0;
        }
        Tuple(const Tuple& t){
            first = t.first;
            second = t.second;
            //Q_ASSERT(first == t.first && (second == t.second));
        }

        //bottleneck - optimize
        explicit Tuple(QString s){
            int splitI=0;
            first=0;
            bool atNeg = false;

            while(s[splitI]!=QChar(SPLITCHARC)){
                if(s[splitI] != '-'){
                    first = first*10 + s[splitI].toAscii() - '0';
                    if(atNeg){
                        first = -first;
                        atNeg = false;
                    }
                } else { atNeg = !atNeg; }
                splitI++;
            }

            splitI++;
            int l = s.length();
            second = 0;
            atNeg = false;
            while(splitI < l){
                if(s[splitI] != '-'){
                    second = second*10 + s[splitI].toAscii() - '0';
                    if(atNeg){
                        second = -second;
                        atNeg = false;
                    }
                } else { atNeg = !atNeg; }
                splitI++;
            }

        }

        bool operator==(const Tuple& t) const{
            return (t.first == first && (t.second == second));
        }

        bool operator!=(const Tuple& t) const{
            return !(t.first == first && (t.second == second));
        }

        //Multiple cases for CLARITY
        bool operator > (const Tuple& t) const{
            //null is less than everything ELSE!
            if(isEmpty()){
                return false;
            } else  if(t.isEmpty()){
                return true;
            } else  if(first > t.first){
                return true;
            } else if(first < t.first){
                return false;
            } else if (second > t.second){
                return true;
            } else {
                return false;
            }
        }

        //Multiple cases for CLARITY
        bool operator < (const Tuple& t) const{
            //null is less than everything ELSE!
            if(t.isEmpty()){
                return false;
            } else  if(isEmpty()){
                    return true;
            } else  if(first < t.first){
                return true;
            } else if(first > t.first){
                return false;

            } else if (second < t.second){
                //Q_ASSERT(first == t.first);
                return true;

            } else {
                //Q_ASSERT(first == t.first);
                return false;
            }
        }

        Tuple& operator=(const Tuple& t)  {
            first = t.first;
            second = t.second;
            //Q_ASSERT(first == t.first && (second == t.second));
            return *this;
        }
        Tuple& operator=(long& t) {
            first = t;
            second = 0;
            return *this;
        }
        Tuple& operator ++(){
            if(second < LONG_MAX)
                second++;
            else {
                second = 0;
                first++;
            }
            return *(this);
        }

        long getFirst() const{
            return first;
        }

        long getSecond() const{
            return second;
        }

        bool isEmpty() const{
            return (first == -1 && (second == 0));
        }

        QString getId(){ return asString(); }

        QString asString() const {
            QString a(STRING_MODEL);
            QString res = a.arg(first).arg(second);
            return res;
        }
};

inline QDataStream &operator << (QDataStream &out, const Tuple &t){
    out << (qint32)t.first;
    out << (qint32)t.second;
    return out;
}

inline QDataStream &operator >> (QDataStream &in, Tuple &t){
    qint32 i;
    qint32 j;
    in >> i;
    in >> j;
    Tuple temp(i,j);
    t = temp;
    return in;
}

template<class KEY, class VALUE, class PARAM_VALUE> class Element {
    // contains a KEY value, a VALUE, and a pointer toward the subtree
    // containing KEY values greater than this->m_KEY but lower than the
    // KEY value of the next element to the right
    public:
        KEY m_key;
        VALUE m_payload;
        NodeMapPtr mp_subtree;
        public:

        bool lessThan(Element& other) const {
            if( m_key <  other.m_key){ return true;}
            if( m_key >  other.m_key){ return false;}
            if (m_payload < other.m_payload){ return true; }
            return false;
        }

        bool operator<(Element& other) const {
            return lessThan(other);
        }

        bool operator>(Element& other) const {
            return !(lessThan(other) || (m_payload == other.m_payload));
        }

        bool operator<=(Element& other) const {
            return (lessThan(other) || (m_payload == other.m_payload)); }

        bool operator>=(Element& other) const {
            return !(lessThan(other)); }

        bool matches(Element& other){return m_key == other.m_key;}
        bool identical_to(Element& other){return m_key == other.m_key
                                       &&(m_payload == other.m_payload );}
        //bool operator==(Element& other) const { return m_key == other.m_key; }
        bool operator!=(Element& other) const { return !(m_key == other.m_key); }
        bool valid() const { return mp_subtree.getOffset() != INVALID_INDEX; }
        void invalidate(){ mp_subtree = invalid_ptr; }
        bool payload_below(Element m){
            return ((*this) == m  &&(m_payload > m.m_payload ));
        }
        Element(KEY k, VALUE v){
            m_key = k;
            m_payload = v;
            mp_subtree =null_ptr;
        }
        Element(KEY k){
            m_key = k;
            VALUE v;
            m_payload = v;
            mp_subtree =null_ptr;
        }
        Element& operator=(const Element& other) {
            m_key = other.m_key;
            m_payload = other.m_payload;
            mp_subtree = other.mp_subtree;
            return *this;
            }
        Element(){ mp_subtree = null_ptr; }
        Element(const Element& other){
            m_key = other.m_key;
            m_payload = other.m_payload;
            mp_subtree = other.mp_subtree;
        }
        Element& largest_key(){
            if(!mp_subtree){
                return *this;
            } else {
                return mp_subtree->largest_key();
            }
        }
        void dump();
};

template<class KEY, class VALUE, class PARAM_VALUE> void Element<KEY, VALUE, PARAM_VALUE>::dump(){
    cout << "KEY=" << m_key << "sub=" << mp_subtree << ' ';
}

#define Elem Element<KEY, VALUE, PARAM_VALUE>
#define Nde Node<KEY, VALUE, PARAM_VALUE>

template <class VALUE, class INDEX_TYPE=PQString >  class MultiTree;

template<class KEY, class VALUE, class PARAM_VALUE> class LBTree;

template<class KEY, class VALUE, class PARAM_VALUE > class Node {
    friend class MultiTree<VALUE, class INDEX_TYPE>;
private:

    // locality of reference, beneficial to effective cache utilization and pickling to disk
    // is provided by a StaticArray object, a thin wrapper on a c-array rather than an STL "list"
    StaticArray< Elem, MAX(MIN(max_elements,(max_array_bytes/sizeof(Elem))),6) > m_array;
    // number of elements currently in m_array, including the zeroth element
    // which has only a subtree, no KEY value or payload.
    unsigned int m_count;
    unsigned int m_total_elements;// nodes below this node
    NodeMapPtr mp_parent;
public:
    NodePtr m_root;

    static QString typeName(){ return QString("Nodeof_") + PARAM_VALUE::typeName();}

    bool is_leaf();
    bool is_root();
    unsigned int totalElements () const{ return m_total_elements;}
    bool array_insert(Elem& element);
    bool array_insert_for_split(Elem& element);
    NodeMapPtr split_insert(Elem& element);
    void increment_parent_totals();
    void decrement_parent_totals();
    bool array_delete(Elem& target);
    bool array_delete(int target_pos);
    bool delete_node_priv(NodeMapPtr node, int offset);
    void insert_zeroth_subtree(NodeMapPtr subtree);
    Elem& smallest_key(){ return m_array[1]; }
    int index_has_subtree();
    NodeMapPtr raise_zeroth_node(int& parent_index_this);
    NodeMapPtr right_sibling(int& parent_index_this);
    NodeMapPtr left_sibling(int& parent_index_this);
    NodeMapPtr rotate_from_left(int parent_index_this);
    NodeMapPtr rotate_from_right(int parent_index_this);
    NodeMapPtr merge_right(int parent_index_this);
    NodeMapPtr merge_left(int parent_index_this);
    bool merge_into_root();
    int minimum_keys();
public:

    explicit Node();
    //Node(NodePtr root_track);
    void set_root(qint64 rootOffset){
        m_root = NodePtr(rootOffset, true);

    }
    NodePtr createNodePtr(NodePtr root_track);
    
    //Node(NodePtr root_track);
    int key_count(){ return m_count-1; }
    Elem& smallest_key_in_subtree();
    NodePtr smallest_node_in_subtree_ptr();
    vector<VALUE> get_range(KEY begin_key, KEY end_key, long limit=-1, long offset=-1);
    vector<VALUE> get_ordinal_range(NodeMapPtr start_n, int small_index, int count);
    void go_to_next(NodeMapPtr& count_node, int& elemIndex);
    Elem& search(KEY k){
            Elem e(k);
            return search(e);
    }
    Elem& search(Elem& desired){
        NodeMapPtr dummy;
        return search(desired, dummy);
    }
    Elem& search(Elem& desired, NodeMapPtr& last_visited);
    Elem& search(Elem& desired, NodeMapPtr& last_visited, int& index);
    Elem& search_by_key(KEY key, NodeMapPtr& last_visited_ptr, int& index);
    Elem& search_by_key(KEY key){
        NodeMapPtr dummy;
        int d;
        return search_by_key(key, dummy, d);
    }

    NodeMapPtr FindInsertPosition(Elem& desired, int& index);
    NodePtr searchGL(Elem& desired, int& local_index);//HJS
    qint32 item_position(Elem& base_elem);
    qint32 get_position(KEY k);
    Elem& ordinalSearch(unsigned int orderDesired, NodeMapPtr& outNode, unsigned int& index);
    Elem& ordinalSearch(unsigned int orderDesired){
        NodeMapPtr start_n;
        unsigned int index;
        return ordinalSearch(orderDesired, start_n, index);
    }



    bool tree_insert(Elem& element);
    unsigned int node_size(){return m_total_elements;}
    bool delete_element(Elem& target);
    bool delete_key(KEY key);
    bool delete_value(Elem& target);
    NodePtr find_value_node(Elem& target, int& index);
    int delete_all_subtrees();
    NodeMapPtr copy_subtrees(LBTree<KEY,VALUE,PARAM_VALUE>& new_root, NodeMapPtr new_parent=null_ptr);
    NodeMapPtr find_base();
        // to return a reference when a search fails.
        static Elem m_failure;
        // the root of the tree may change.  this attribute keeps it accessible.
    //LBTree<KEY,VALUE,PARAM_VALUE>& m_root;
    //Elem& operator[](int i) { return m_array[i]; }
    Elem& at(int i) { return m_array[i]; }
        // node cannot be instantiated without a root tracker
    int testStruct();
    Elem& largest_key(){ return m_array[m_count-1]; }
    Elem& absolute_largest_key(){
        if(!m_array[m_count-1].mp_subtree ||
           m_array[m_count-1].mp_subtree->m_count <1)
            return m_array[m_count-1];
        else
            return m_array[m_count-1].mp_subtree->absolute_largest_key();
    }
//    void set_debug();
//    void dump();
    int testTreeStructure(ElemPtr& lastBottomElem);
    long testAllocatedSize();
    long testCountBadNodes(float badRatio);

    void set_base(NodeMapPtr new_base) {
        Q_ASSERT(is_root());
        new_base->m_root == this;
        mp_parent = new_base;
    }

    NodeMapPtr get_base() {
        Q_ASSERT(is_root());
        return mp_parent;
    }
};

//template<class KEY, class VALUE, class PARAM_VALUE> class TreePosition {
//    public:
//    NodeMapPtr node;
//    KEY key;
//    long index;
//    TreePosition(){
//        index = -1;
//    }
//
//    TreePosition(NodeMapPtr n, KEY k, long i){
//        node = n;
//        key = k;
//        index = i;
//    }
//};

template<class KEY, class VALUE, class PARAM_VALUE>  class LBTree {
        // all the node instances that belong to a given tree have a reference to one
    // instance of LBTree.  as the Node instance that is the root may change
        // or the original root may be deleted, Node instances must access the root
        // of the tree through this tracker, and update the root pointer when they
        // perform insertions or deletions.  using a static attribute in the Node
        // class to hold the root pointer would limit a program to just one B-tree.
        friend class MultiTree<VALUE, class INDEX_TYPE>;
    protected:
        NodeMapPtr mp_root;
        bool allow_duplicates;
        short int insert_index;//short to make changes more atomic...
    public:
        //Used to save the position of an element
        struct Item_Position{
            NodeMapPtr node;
            Elem& elem;
            int& index;
        };

        LBTree(bool ad=true):mp_root(null_ptr) {
            allow_duplicates = ad;
            insert_index = 0;
        }
        ~LBTree(){
            //Our tree is persistent so we don't to destroy anything

//            if(mp_root) {
//                get_base()->delete_all_subtrees();
//                destroyPtr(get_base());
//                destroyPtr(mp_root);
//            }
        }
        bool insert_at(KEY k, VALUE v){
            NodeMapPtr::tryReallocateMem();
            Elem e(k,v);
            insert_index++;
            NodeMapPtr base = get_base();
            NodeMapPtr containedRoot = base->m_root;
            //Q_ASSERT(containedRoot == mp_root);
            return base->tree_insert(e);
        }

        void createBase(){
            Q_ASSERT(!mp_root);
            mp_root = createBaseNodePtr();
            void* newMemory = MPointer<Node<KEY, VALUE, PARAM_VALUE> >::allocate();
            Node<KEY, VALUE, PARAM_VALUE>* newNode = new(newMemory) Node<KEY, VALUE, PARAM_VALUE>();
            mp_root = newNode;
            qint64 root_offset = mp_root.getOffset();
            mp_root->set_root(root_offset);
            Q_ASSERT(mp_root->is_root());

            NodeMapPtr basePtr = createBaseNodePtr();
            basePtr->set_root(mp_root.getOffset());
            mp_root->set_base(basePtr);
        }

        void set_root(NodeMapPtr new_root) {
            NodeMapPtr::tryReallocateMem();
            //Q_ASSERT(new_root->is_root());
            mp_root = new_root;
            //Q_ASSERT(mp_root->is_root());
        }
        NodeMapPtr get_base(){ return mp_root->get_base(); }
        NodeMapPtr get_root(){
            Q_ASSERT(mp_root->is_root());
            return mp_root;
        }
        long size() {
            return get_base()->node_size();
        }
        bool get_allow_duplicates(){return allow_duplicates;}
        bool delete_at(KEY k){
            insert_index++;
            Elem e(k);
            return get_base()->delete_element(e);
        }

        bool delete_all_at(KEY k){
            int dummy_i;
            NodeMapPtr dummy = get_base();
            Elem n = get_base()->search_by_key(k, dummy, dummy_i);
            while(n.valid()){
                get_base()->delete_key(k);
                n = get_base()->search_by_key(k, dummy, dummy_i);
            }
            return true;
        }

        bool delete_object(KEY k, VALUE v){
            insert_index++;
            Elem e(k, v);
            bool res = get_base()->delete_value(e);
            //get_base()->testStruct();
            return res;
        }
        VALUE find(KEY k){
            Elem e(k);
            NodeMapPtr dummy = get_base();
            Elem& n = get_base()->search(e, dummy);
            return n.m_payload;
        }

        VALUE operator[](KEY k){
            return find(k);
        }
        VALUE ordinal_find(int i){
            Elem& n = get_base()->ordinalSearch(i);
            return n.m_payload;
        }
        qint32 get_position(KEY k){
            return get_base()->get_position(k);

        }
//{
//            VALUE v;
//            Elem e(k, v);
//            if(!get_base()){
//
//            }
//            return get_base()->item_position(e)-1;
//        }
        qint32 get_position(Elem e){
            NodeMapPtr res;
            Elem& n = get_base()->search(e, res);
            return get_base()->item_position(n);
        }


        qint32 get_range_count(KEY begin, KEY end){
            return MAX(get_position(end) - get_position(begin),0);
        }

        qint32 get_range_count(Elem& begin_elem, Elem& end_elem){
            return MAX(get_position(end_elem) - get_position(begin_elem),0);
        }

        //Find half open interval
        vector<VALUE> get_range(KEY begin_key, KEY end_key){
            return get_base()->get_range(begin_key, end_key);
        }


};

//#include "multi_tree.h"
#include "multi_tree_db.h"




#endif
