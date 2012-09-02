#ifndef BTREE__H
#define BTREE_H
/*
Updated 2010 by HJS as a template implementing B-tree "With Rank"
This should perform every atomic container operation in O(log(n)):
insert (K,V), delete(V), find nth element V, get order of the element V, etc.
The algorithm has also been modified to allow non-unique keys.

This is version uses Qhash as well as STL as party of the Oneline keystroke launcher,
I plan to work on a pure STL version available as well.

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
//#include <QDebug>
#include "limits.h"


#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

extern bool gTest_tree_struct;


template<class KEY, class VALUE> class Node;
#define invalid_ptr (reinterpret_cast<Node<KEY, VALUE>*> (-1))
#define null_ptr (reinterpret_cast<Node<KEY, VALUE>*> (0))
const int invalid_index = -1;
const int max_elements = 200;  // max elements in a node

// size limit for the array in a vector object.  best performance was
// at 800 bytes.
const unsigned int max_array_bytes = 800;

//typedef pair < long, long> LPair;
class Tuple {
    //just for debugging test subproject... friend QDataStream &operator << (QDataStream &out, Tuple &item);

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
            Q_ASSERT(first == t.first && (second == t.second));
        }
        bool operator==(const Tuple t) const{
            return (t.first == first && (t.second == second));
        }

        bool operator!=(const Tuple t) const{
            return !(t.first == first && (t.second == second));
        }

        //Multiple cases for CLARITY
        bool operator > (const Tuple t) const{
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
        bool operator < (const Tuple t) const{
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
                Q_ASSERT(first == t.first);
                return true;

            } else {
                Q_ASSERT(first == t.first);
                return false;
            }
        }

        Tuple& operator=(const Tuple t) {
            first = t.first;
            second = t.second;
            Q_ASSERT(first == t.first && (second == t.second));
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
        bool isEmpty() const{
            return (first == -1 && (second == 0));
        }
};

//QDataStream &operator << (QDataStream &out, Tuple &t){
//    out << (qint64)t.first;
//    out << (qint64)t.second;
//    return out;
//}

template<class KEY, class VALUE> class Element {
    // contains a KEY value, a VALUE, and a pointer toward the subtree
    // containing KEY values greater than this->m_KEY but lower than the
    // KEY value of the next element to the right
	public:
        KEY m_key;
        VALUE m_payload;
        Node<KEY, VALUE>* mp_subtree;
	public:
        bool operator>(Element& other) const {
            return (m_key >  other.m_key );
        }
        bool operator<(Element& other) const { return m_key <  other.m_key ; }
        bool operator>=(Element& other) const {
            return (m_key > other.m_key || (m_key == other.m_key)); }
        bool operator<=(Element& other) const {
            return (m_key < other.m_key || (m_key == other.m_key)); }
        bool matches(Element& other){return m_key == other.m_key;}
        bool identical_to(Element& other){return m_key == other.m_key
                                       &&(m_payload == other.m_payload );}
        //bool operator==(Element& other) const { return m_key == other.m_key; }
        bool operator!=(Element& other) const { return !(m_key == other.m_key); }
        bool valid() const { return mp_subtree != invalid_ptr; }
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

template<class KEY, class VALUE> void Element<KEY, VALUE>::dump(){
    cout << "KEY=" << m_key << "sub=" << mp_subtree << ' ';
}
 
//typedef Element<string, string> Elem;

#define Elem Element<KEY, VALUE>
#define Nde Node<KEY, VALUE>

template <class VALUE, class ID_TYPE=QString, class INDEX_TYPE=QString>  class MultiTree;
 
template<class KEY, class VALUE> class LBTree;
 
template<class KEY, class VALUE> class Node {
    friend class MultiTree<VALUE>;
public:
	// locality of reference, beneficial to effective cache utilization,
	// is provided by a "vector" container rather than a "list"
	vector<Elem> m_vector;
    // number of elements currently in m_vector, including the zeroth element
    // which has only a subtree, no KEY value or payload.
	unsigned int m_count;
    // HJS ADDED!!! TOTAL Number of nodes below this node
    unsigned int m_total_elements;
    Nde* mp_parent;

	bool is_leaf();
    bool vector_insert(Elem& element);
    bool vector_insert_for_split(Elem& element);
    Nde* split_insert(Elem& element);
    void increment_parent_totals();
    void decrement_parent_totals();
    bool vector_delete(Elem& target);
    bool vector_delete(int target_pos);
    bool delete_node_priv(Nde* node, int offset);
    void insert_zeroth_subtree(Nde* subtree);
    Elem& smallest_key(){ return m_vector[1]; }
    int index_has_subtree();
    Nde* raise_zeroth_node(int& parent_index_this);
    Nde* right_sibling(int& parent_index_this);
    Nde* left_sibling(int& parent_index_this);
    Nde* rotate_from_left(int parent_index_this);
    Nde* rotate_from_right(int parent_index_this);
    Nde* merge_right(int parent_index_this);
    Nde* merge_left(int parent_index_this);
    bool merge_into_root();
    int minimum_keys();
public:
    Node(LBTree<KEY,VALUE>& root_track);
    Node(LBTree<KEY,VALUE>* root_track);
    int key_count(){ return m_count-1; }
    Elem& smallest_key_in_subtree();
    Node* smallest_node_in_subtree_ptr();
    vector<VALUE> get_range(KEY begin_key, KEY end_key);
    vector<VALUE> get_ordinal_range(Nde* start_n, int small_index, int count);
    void go_to_next(Nde*& count_node, int& elemIndex);
    Elem& search(KEY k){
            Elem e(k);
            return search(e);
    }
    Elem& search(Elem& desired){
        Nde* dummy;
        return search(desired, dummy);
    }
    Elem& search(Elem& desired, Nde*& last_visited);
    Elem& search(Elem& desired, Nde*& last_visited, int& index);
    Nde* FindInsertPosition(Elem& desired, int& index);
    Node* searchGL(Elem& desired, int& local_index);//HJS
    qint64 item_position(Elem& base_elem);
    Elem& ordinalSearch(unsigned int orderDesired, Nde*& outNode, unsigned int& index);
    Elem& ordinalSearch(unsigned int orderDesired){
        Nde* start_n;
        unsigned int index;
        return ordinalSearch(orderDesired, start_n, index);
    }



    bool tree_insert(Elem& element);
    unsigned int node_size(){return m_total_elements;}
    bool delete_element(Elem& target);
    bool delete_value(Elem& target);
    Node* find_value_node(Elem& target, int& index);
    int delete_all_subtrees();
    Nde* copy_subtrees(LBTree<KEY,VALUE>& new_root, Nde* new_parent=0);
    Nde* find_root();
	// to return a reference when a search fails.
	static Elem m_failure;
	// the root of the tree may change.  this attribute keeps it accessible.
    LBTree<KEY,VALUE>& m_root;
    Elem& operator[](int i) { return m_vector[i]; }
	// node cannot be instantiated without a root tracker
    bool testStruct();
    Elem& largest_key(){ return m_vector[m_count-1]; }
    Elem& absolute_largest_key(){
        if(!m_vector[m_count-1].mp_subtree ||
           m_vector[m_count-1].mp_subtree->m_count <1)
            return m_vector[m_count-1];
        else
            return m_vector[m_count-1].mp_subtree->absolute_largest_key();
    }
#ifndef QT_NO_DEBUG
//    void set_debug();
//    void dump();
    int testTreeStructure(Elem*& lastBottomNode);
#endif
};

//template<class KEY, class VALUE> class TreePosition {
//    public:
//    Nde* node;
//    KEY key;
//    long index;
//    TreePosition(){
//        index = -1;
//    }
//
//    TreePosition(Nde* n, KEY k, long i){
//        node = n;
//        key = k;
//        index = i;
//    }
//};
 
template<class KEY, class VALUE>  class LBTree {
	// all the node instances that belong to a given tree have a reference to one
    // instance of LBTree.  as the Node instance that is the root may change
	// or the original root may be deleted, Node instances must access the root
	// of the tree through this tracker, and update the root pointer when they
	// perform insertions or deletions.  using a static attribute in the Node
	// class to hold the root pointer would limit a program to just one B-tree.
        friend class MultiTree<VALUE>;
    protected:
        Node<KEY, VALUE>* mp_root;
        bool allow_duplicates;
        short int insert_index;//short to make changes more atomic...
    public:
        //Used to save the position of an element
        struct Item_Position{
            Nde* node;
            Elem& elem;
            int& index;

        };

        LBTree(bool ad=true):mp_root(null_ptr) {
            allow_duplicates = ad;
            insert_index = 0;
        }
        ~LBTree(){
            // safety measure
            if(mp_root) {
                mp_root->delete_all_subtrees();
                delete mp_root;
            }
        }
        long size() {
            return mp_root->node_size();
        }
        bool get_allow_duplicates(){return allow_duplicates;}
        void set_root(Node<KEY, VALUE>* old_root, Node<KEY, VALUE>* new_root) {
            // ensure that the root is only being changed by a node belonging to the
            // same tree as the current root
            if(old_root != mp_root){
                Q_ASSERT(false);
                throw "wrong old_root in LBTree::set_root";
            } else  {mp_root = new_root;}
	    }
        Nde* get_root(){ return mp_root; }
        bool insert_at(KEY k, VALUE v){
            Elem e(k,v);
            insert_index++;
            return get_root()->tree_insert(e);
        }
        bool delete_at(KEY k){
            insert_index++;
            Elem e(k);
            return get_root()->delete_element(e);
        }

        bool delete_all_at(KEY k){
            Elem e(k);
            Nde* dummy = get_root();
            Elem n = get_root()->search(e, dummy);
            while(n.valid()){
                delete_at(k);
                n = get_root()->search(e, dummy);
            }
            return true;
        }

        bool delete_object(KEY k, VALUE v){
            insert_index++;
            Elem e(k, v);
            bool res = get_root()->delete_value(e);
            //get_root()->testStruct();
            return res;
        }
        VALUE find(KEY k){
            Elem e(k);
            Nde* dummy = get_root();
            Elem& n = get_root()->search(e, dummy);
            return n.m_payload;
        }

        VALUE operator[](KEY k){
            return find(k);
        }
        VALUE ordinal_find(int i){
            Elem& n = get_root()->ordinalSearch(i);
            return n.m_payload;
        }
        qint64 get_position(KEY k){
            VALUE v;
            Elem e(k, v);
            if(!get_root()){

            }
            return get_root()->item_position(e)-1;
        }
        qint64 get_position(Elem e){
            Nde* res;
            Elem& n = get_root()->search(e, res);
            return get_root()->item_position(n);
        }


        qint64 get_range_count(KEY begin, KEY end){
            return MAX(get_position(end) - get_position(begin),0);
        }

        qint64 get_range_count(Elem& begin_elem, Elem& end_elem){
            return MAX(get_position(end_elem) - get_position(begin_elem),0);
        }

        //Find half open interval
        vector<VALUE> get_range(KEY begin_key, KEY end_key){
            return get_root()->get_range(begin_key, end_key);
        }


};

#include "multi_tree.h"



#endif
