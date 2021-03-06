#ifndef BTREE_IMPL_H
#define BTREE_IMPL_H

#include <QtGlobal>
#include "mmap_pointer.h"

//Just for syntax highlight
#ifndef Nde
#define Nde Node<KEY,VALUE,PARAM_VALUE>
#endif

// * Template implementation file *
// For a class-instance created through a template declaration;
// the compiler uses this file to generate code for the class's functions.

//Don't #include "btree.h" -- header needs to be included by the file using the template but not here

template<class KEY, class VALUE, class PARAM_VALUE> class Node;

// initialize static data at file scope
template<class KEY, class VALUE, class PARAM_VALUE> Elem Nde::m_failure = Elem();

template<class KEY, class VALUE, class PARAM_VALUE> int Nde::minimum_keys () {
    // minus 1 for the empty slot left for splitting the node
    int size = m_array.size();
    int ceiling_func = (size-1)/2;
    if (ceiling_func*2 < size-1)
        ceiling_func++;
    return ceiling_func-1;  // for clarity, may increment then decrement
}

template<class KEY, class VALUE, class PARAM_VALUE> void Nde::insert_zeroth_subtree (NodeMapPtr subtree) {
    m_array[0].mp_subtree = subtree;
    KEY k;
    m_array[0].m_key = k;
    m_count = 1;
    if (subtree){
        subtree->mp_parent = this;
        m_total_elements += subtree->m_total_elements;
    }
}

template<class KEY, class VALUE, class PARAM_VALUE>  NodeMapPtr Nde::createNodePtr(NodePtr root) {
    Q_ASSERT(root->is_root());
    qint64 root_offset = root.getOffset();
    void* newMemory = MPointer<Node>::allocate();
    Nde* newNode = new(newMemory) Node();
    NodePtr newPtr(newNode);
    //NodePtr rootPtr(root_offset);
    newPtr->set_root(root_offset);
    return newPtr;
}

template<class KEY, class VALUE, class PARAM_VALUE>  Nde::Node()  {
    //m_root = this;
    //Q_ASSERT(is_root());
    m_count = 0;
    m_total_elements = 0;
    mp_parent = null_ptr;
    insert_zeroth_subtree (null_ptr);
    //Q_ASSERT(is_root());
}


//template<class KEY, class VALUE, class PARAM_VALUE>  Nde::Node(NodePtr root_track)  : m_root(root_track) {
//    m_count = 0;
//    m_total_elements = 0;
//    mp_parent = null_ptr;
//    insert_zeroth_subtree (null_ptr);
//}


//template<class KEY, class VALUE, class PARAM_VALUE>  Nde::Node(NodePtr root_track)  : m_root(*root_track) {
//    // limit the size of the vector to 4 kilobytes max and 200 entries max.
//    m_count = 0;
//    m_total_elements = 0;
//    mp_parent = null_ptr;
//    insert_zeroth_subtree (null_ptr);
//}

//template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::copy_subtrees(LBTree<KEY,VALUE,PARAM_VALUE>& new_root, NodeMapPtr new_parent) {
//    NodeMapPtr copy_node = new Node(new_root);
//    copy_node->mp_parent = new_parent;
//    copy_node->m_count = m_count;
//    copy_node->m_total_elements = m_total_elements;
//
//    for (unsigned int i=0; i< m_count; i++) {
//        copy_node->m_array[i].m_key = m_array[i].m_key;
//        copy_node->m_array[i].m_payload = m_array[i].m_payload;
//        if (!m_array[i].mp_subtree)
//            copy_node->m_array[i].mp_subtree=0;
//        else {
//            copy_node->m_array[i].mp_subtree = m_array[i].mp_subtree->copy_subtrees(new_root, this);
//        }
//    }
//    return copy_node;
//}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::find_base() {
//    NodeMapPtr current = this;
//    while (current->mp_parent)
//        current = current->mp_parent;
//    Q_ASSERT(current == m_root->get_base());
    return m_root->get_base();
}

template<class KEY, class VALUE, class PARAM_VALUE>  bool Nde::is_leaf () {
    return m_array[0].mp_subtree==null_ptr;
}

template<class KEY, class VALUE, class PARAM_VALUE>  bool Nde::is_root() {
    NodeMapPtr current = this;
    return m_root == current;
}

template<class KEY, class VALUE, class PARAM_VALUE> int Nde::delete_all_subtrees () {
// return the number of nodes deleted
    int count_deleted = 0;
    for (unsigned int i=0; i< m_count; i++) {
        if (!m_array[i].mp_subtree)
            continue;
        else {
            count_deleted += m_array[i].mp_subtree->delete_all_subtrees();
            destroyPtr(m_array[i].mp_subtree);
            count_deleted++;
        }
    }
    return count_deleted;
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::array_insert (Elem& element) {
// this method merely tries to insert the argument into the current node.
// it does not concern itself with the entire tree.
// if the element can fit into m_array, slide all the elements
// greater than the argument forward one position and copy the argument
// into the newly vacated slot, then return true.  otherwise return false.
// note: the tree_insert method will already have verified that the key
// value of the argument element is not present in the tree.

    if (m_count >= m_array.size()-1) // leave an extra slot for split_insert
        return false;
    unsigned int i = m_count;

    while (i>0 && !(m_array[i-1] < element.largest_key() )) {
        m_array[i] = m_array[i-1];
        i--;
    }
    if (element.mp_subtree){
        element.mp_subtree->mp_parent = this;
        m_total_elements += element.mp_subtree->m_total_elements;
    }
    m_total_elements++;
    m_array[i] = element;
    m_count++;

    return true;
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::array_delete(Elem& target) {
// delete a single element in the vector belonging to *this node.
// if the target is not found, do not look in subtrees, just return false.

    int target_pos = -1;
    int first = 1;
    int last = m_count-1;
    // perform binary search
    while (last-first > 1) {
        int mid = first+(last-first)/2;
        if (target>=m_array[mid])
            first = mid;
        else
            last = mid;
    }
    if (m_array[first]==target)
        target_pos = first;
    else if (m_array[last]==target)
        target_pos = last;
    else
        return false;
    // the element's subtree, if it exists, is to be deleted or re-attached
    // in a different function.  not a concern here.  just shift all the
    // elements in positions greater than target_pos.
    if(m_array[target_pos].mp_subtree){ m_total_elements -= target.mp_subtree->m_total_elements;}

    for (unsigned int i=target_pos; i<m_count; i++)
        m_array[i] = m_array[i+1];

    m_count--;
    if(target_pos > 0){ m_total_elements--;}
    return true;
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::array_delete(int target_pos) {
// delete a single element in the vector belonging to *this node.
// the element is identified by position, not value.

    //since target_pos >0, cast safe for second part
    if (target_pos<0 || (unsigned int)target_pos>=m_count)
        return false;

    //Remove target's items
    if (m_array[target_pos].mp_subtree){ m_total_elements -= m_array[target_pos].mp_subtree->m_total_elements;}


    // the element's subtree, if it exists, is to be deleted or re-attached
    // in a different function.  not a concern here.  just shift all the
    // elements in positions greater than target_pos.
    for (unsigned int i=target_pos; i<m_count; i++){
        m_array[i] = m_array[i+1];
    }

    m_count--;
    m_total_elements--;//if you delete empty 0th elem, total still reduced
    return true;
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::array_insert_for_split (Elem& element) {
// this method insert an element that is in excess of the nominal capacity of
// the node, using the extra slot that always remains unused during normal
// insertions.  this method should only be called from split_insert()

    if (m_count >= m_array.size()) {
        Q_ASSERT(false);
        throw "bad m_count in array_insert_for_split";
    }

    unsigned int i = m_count;
    while (i>0 && !(m_array[i-1] < element.largest_key() )) {
        m_array[i] = m_array[i-1];
        i--;
    }

    if (element.mp_subtree){ element.mp_subtree->mp_parent = this;}
    m_array[i] = element;
    m_count++;
    return true;
}

template<class KEY, class VALUE, class PARAM_VALUE>  NodeMapPtr Nde::split_insert(Elem& element) {

    // split_insert should only be called if node is full
    if (m_count != m_array.size()-1){
        Q_ASSERT(false);
        throw "bad m_count in split_insert";
    }

    array_insert_for_split (element);

    unsigned int split_point = m_count/2;
    if (2*split_point < m_count)  {split_point++;}// "ceiling"

    // new node receives the rightmost half of elements in *this node
    NodeMapPtr new_node = createNodePtr(m_root);
    //One for upward node
    int nodes_removed=0;
    if (element.mp_subtree){
        m_total_elements += element.mp_subtree->m_total_elements;
    }
    //Ripping out the upward element from the node...
    Elem upward_element = m_array[split_point];
    //ONLY it's CHILDREN go to new_node
    if (upward_element.mp_subtree){
        nodes_removed += upward_element.mp_subtree->m_total_elements;
    }
    new_node->insert_zeroth_subtree (upward_element.mp_subtree);
    upward_element.mp_subtree = new_node;

    // element that gets added to new_node
    for (unsigned int i=1; i<m_count-split_point; i++){
        new_node->array_insert(m_array[split_point+i]);
        //Count nodes *removed*
        nodes_removed ++;
        if (m_array[split_point+i].mp_subtree){
            nodes_removed += m_array[split_point+i].mp_subtree->m_total_elements;
        }
    }

    new_node->m_count = m_count-split_point;
    new_node->mp_parent = mp_parent;

    m_count = split_point;
    m_total_elements -= nodes_removed;

    // now insert the new node into the parent, splitting it if necessary
    if (mp_parent ){
        mp_parent->m_total_elements -= nodes_removed;
        if(mp_parent->array_insert(upward_element)){
            mp_parent->node_size();
            return mp_parent;
        }
        NodeMapPtr res = mp_parent->split_insert(upward_element);
        return res;

    } else { // this node was the root
        NodeMapPtr new_base = createNodePtr(m_root);
        new_base->insert_zeroth_subtree(this);
        this->mp_parent = new_base;
        new_node->mp_parent = new_base;
        new_base->array_insert(upward_element);
        m_root->set_base(new_base);
        new_base->mp_parent = null_ptr;
        new_base->increment_parent_totals();
        return new_base;
    }
    Q_ASSERT(false);
    return null_ptr;
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::tree_insert (Elem& element) {
    NodeMapPtr last_visited_ptr = this;
    // This has the SIDE EFFECT of finding the proper node
    // to insert into
    int index=0;
    last_visited_ptr = FindInsertPosition(element, index);
    Q_ASSERT( last_visited_ptr);
    Q_ASSERT(!last_visited_ptr->m_array[index].mp_subtree);
    if (last_visited_ptr->array_insert(element)){
        last_visited_ptr->increment_parent_totals();
        return true;
    } else {
        NodeMapPtr res = last_visited_ptr->split_insert(element);
        res->increment_parent_totals();
        return true;
    }

}

template<class KEY, class VALUE, class PARAM_VALUE> void Nde::increment_parent_totals(){
    NodeMapPtr gps = mp_parent;
    while(gps){
        gps->m_total_elements++;
        gps = gps->mp_parent;
    }

}

template<class KEY, class VALUE, class PARAM_VALUE> void Nde::decrement_parent_totals(){
    NodeMapPtr gps = mp_parent;
    while(gps){
        gps->m_total_elements--;
        gps = gps->mp_parent;
    }
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::delete_element (Elem& target) {
// target is just a package for the key value.  the reference does not
// provide the address of the Elem instance to be deleted.

    // first find the node contain the Elem instance with the given key
    NodeMapPtr node = null_ptr;
    //int parent_index_this = INVALID_INDEX;
    int index=0;
    Elem& found = search (target, node, index);
    Q_ASSERT(node);
    bool s = true;
    if (found.matches(m_failure)){
        find_base()->testStruct();
        return false;
    }
    if (!node->m_array[index].mp_subtree ) { //node->is_leaf()
        delete_node_priv(node, index);
        find_base()->testStruct();
    } else {
        //This COPIES the smallest elment UP to the existing element
        NodeMapPtr higher_node = (found.mp_subtree && found.mp_subtree->m_count>1)? found.mp_subtree : node;
        Q_ASSERT(higher_node);
        Elem& smallest_in_subtree = node->m_array[index].mp_subtree ->smallest_key_in_subtree(); //higher_node->smallest_key_in_subtree();
        found.m_key = smallest_in_subtree.m_key;
        found.m_payload = smallest_in_subtree.m_payload;
        higher_node->delete_element (smallest_in_subtree);

    }

    return s;
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::delete_key(KEY key) {
    // target is just a package for the key value.  the reference does not
    // provide the address of the Elem instance to be deleted.

    // first find the node contain the Elem instance with the given key
    NodeMapPtr node = null_ptr;
    //int parent_index_this = INVALID_INDEX;
    int index=0;
    Elem& found = search_by_key (key, node, index);
    Q_ASSERT(node);
    bool s = true;
    if (found.matches(m_failure)){
        find_base()->testStruct();
        return false;
    }
    if (!node->m_array[index].mp_subtree ) { //node->is_leaf()
        delete_node_priv(node, index);
        find_base()->testStruct();
    } else {
        //This COPIES the smallest elment UP to the existing element
        NodeMapPtr higher_node = (found.mp_subtree && found.mp_subtree->m_count>1)? found.mp_subtree : node;
        Q_ASSERT(higher_node);
        Elem& smallest_in_subtree = node->m_array[index].mp_subtree ->smallest_key_in_subtree(); //higher_node->smallest_key_in_subtree();
        higher_node->delete_key(smallest_in_subtree.m_key);

    }

    return s;
}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::delete_value (Elem& target) {

    // first find the node contain the Elem instance with the given key AND value
    int index=0;
    NodeMapPtr node = searchGL(target, index);
    do {
        go_to_next(node, index);
    } while(node && (node->m_array[index].m_payload != target.m_payload) && (index > -1) );
    if(!node){
        return false;
    }
    Q_ASSERT(node);
    Q_ASSERT(node->m_array[index].m_payload == target.m_payload);
    Q_ASSERT(node->m_array[index].m_key == target.m_key);

    bool s = true;
    if (node->m_array[index].matches(m_failure)){ return false; }
    if (node->m_array[index].m_payload !=target.m_payload){ return false; }
    if (!node->m_array[index].mp_subtree ) { //node->is_leaf()
        s = delete_node_priv(node, index);
    } else {
        //This COPIES the smallest elment UP to the existing element
        Elem& smallest_in_subtree = node->m_array[index].mp_subtree ->smallest_key_in_subtree();
        Q_ASSERT(node->m_array[index].m_payload == target.m_payload);
        Q_ASSERT(node->m_array[index].m_key == target.m_key);
        node->m_array[index].m_key = smallest_in_subtree.m_key;
        node->m_array[index].m_payload  = smallest_in_subtree.m_payload;
        node->testStruct();
        s = node->m_array[index].mp_subtree->delete_element (smallest_in_subtree);

//        if(node->m_array[index].mp_subtree && node->m_array[index].mp_subtree->m_count <2){
//            delete node->m_array[index].mp_subtree;
//            node->m_array[index].mp_subtree = null_ptr;
//        }
   }

   return s;
}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::find_value_node(Elem& target, int& index) {

    NodeMapPtr node = searchGL(target, index);
    do {
        go_to_next(node, index);
    } while(node && (node->m_array[index].m_payload != target.m_payload) && (index > -1) );
    return node;

}

template<class KEY, class VALUE, class PARAM_VALUE> bool Nde::delete_node_priv (NodePtr node, int offset) {
    //find_base()->testStruct();
    Q_ASSERT(node->m_count>1 ||(node->m_total_elements < 1));
    Q_ASSERT(!node->m_array[offset].mp_subtree);

    int parent_index_this = INVALID_INDEX;
    if (node->key_count() > node->minimum_keys()){
        bool s = node->array_delete(offset);
        Q_ASSERT(s);
        node->decrement_parent_totals();
        //find_base()->testStruct();
        return s;
    } else  {
        bool s = node->array_delete(offset);
        s =s;
        Q_ASSERT(s);
        node->decrement_parent_totals();
        //find_base()->testStruct();
        // loop invariant: if _node_ is not null_ptr, it points to a node
        // that has lost an element and needs to import one from a sibling
        // or merge with a sibling and import one from its parent.
        // after an iteration of the loop, _node_ may become null or
        // it may point to its parent if an element was imported from the
        // parent and this caused the parent to fall below the minimum
        // element count.

        //Also, total nodes won't change in the loop, so we only change
        //element value of node and either right or left siblings...
        //
        int loop = 0;

        while (node) {

            // NOTE: the "this" pointer may no longer be valid after the first
            // iteration of this loop!!!
            if (node==node->find_base() && node->is_leaf())
                break;
            if (node==node->find_base() && !node->is_leaf()){
                return true;//With equal keys, try being more flexible
                Q_ASSERT(false);// Was ... sanity check
                throw "node should not be root in delete_node loop";
            }
            // is an extra element available from the right sibling (if any)
            NodeMapPtr right = node->right_sibling(parent_index_this);
            if (right && right->key_count() > right->minimum_keys())
                node = node->rotate_from_right(parent_index_this);
            else {
                // is an extra element available from the left sibling (if any)
                NodeMapPtr left = node->left_sibling(parent_index_this);
                if (left && left->key_count() > left->minimum_keys())
                    node = node->rotate_from_left(parent_index_this);
                else if (right){
                    node = node->merge_right(parent_index_this);
                } else if (left)
                    node = node->merge_left(parent_index_this);
                else {
                    //added by HJS - always returns null
                    node=node->raise_zeroth_node(parent_index_this);
                }
            }
            //if(node){ node->testStruct(); }
            loop++;
        }
    }
    return true;
}

//Added by hjs
template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::raise_zeroth_node(int& parent_index_this){
    //This may delete 'this' ptr but it should be OK since we aren't referencing
    //further members...

    NodeMapPtr n = this;
    NodeMapPtr parent = mp_parent;
    while(n && n->mp_parent && n->m_count<2 ){
        parent_index_this = n->index_has_subtree();
        Q_ASSERT( mp_parent->at(parent_index_this).mp_subtree ==n);
        NodeMapPtr new_n = n->m_array[0].mp_subtree;
        n->mp_parent->m_array[parent_index_this].mp_subtree = new_n;
        if(!new_n){break;}
        new_n->mp_parent = n->mp_parent;
        n->mp_parent->m_array[parent_index_this].mp_subtree->mp_parent = n->mp_parent;

        n->m_array[0].mp_subtree = null_ptr;
        Q_ASSERT(new_n->mp_parent == n->mp_parent);
        destroyPtr(n);
        n = new_n;
    }

    if (parent && parent->key_count() >= mp_parent->minimum_keys()) {
        return null_ptr; // no need for parent to import an element
    } else {
        return parent; // parent must import an element
    }
}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::rotate_from_right(int parent_index_this) {
    //mp_parent->testStruct();
    // new element to be added to this node
    Elem underflow_filler = mp_parent->at(parent_index_this+1);
    // right sibling of this node
    NodeMapPtr right_sib = mp_parent->at(parent_index_this+1).mp_subtree;
    //underflow_filler.mp_subtree = (*right_sib)[0].mp_subtree;
    underflow_filler.mp_subtree = right_sib->at(0).mp_subtree;
    // copy the entire element
    mp_parent->at(parent_index_this+1) = right_sib->at(1);
    // now restore correct pointer
    mp_parent->at(parent_index_this+1).mp_subtree = right_sib;
    array_insert(underflow_filler);
    right_sib->array_delete(0);
    KEY k;
    right_sib->at(0).m_key = k;
    VALUE v;
    right_sib->at(0).m_payload = v;
    //right_sib->testStruct();

    //mp_parent->testStruct();
    return null_ptr; // parent node still has same element count
}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::rotate_from_left(int parent_index_this) {
    Q_ASSERT(mp_parent);
    //mp_parent->testStruct();
    // new element to be added to this node
    Elem underflow_filler = mp_parent->at(parent_index_this);
    // left sibling of this node
    NodeMapPtr left_sib = mp_parent->at(parent_index_this-1).mp_subtree;

    //This removes these tree with reducing the count of 'this' node...
    underflow_filler.mp_subtree = this->at(0).mp_subtree;
    //So...
    if(underflow_filler.mp_subtree){
        m_total_elements -= underflow_filler.mp_subtree->m_total_elements;
    }
    //As above
    this->at(0).mp_subtree = left_sib->at(left_sib->m_count-1).mp_subtree;
    if(left_sib->at(left_sib->m_count-1).mp_subtree){
        m_total_elements += left_sib->at(left_sib->m_count-1).mp_subtree->m_total_elements;
    }

    if (this->at(0).mp_subtree){ this->at(0).mp_subtree->mp_parent = this; }
    // copy the entire element
    mp_parent->at(parent_index_this) = left_sib->at(left_sib->m_count-1);
    // now restore correct pointer
    mp_parent->at(parent_index_this).mp_subtree = this;
    array_insert (underflow_filler);
    left_sib->array_delete(left_sib->m_count-1);

    //mp_parent->m_total_elements = initiail_par_nodes;
    //mp_parent->testStruct();
    return null_ptr; // parent node still has same element count
}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::merge_right (int parent_index_this) {
// copy elements from the right sibling into this node, along with the
// element in the parent node vector that has the right sibling as it subtree.
// the right sibling and that parent element are then deleted
    Q_ASSERT(mp_parent);
    mp_parent->testStruct();
    int initiail_par_nodes =  mp_parent->m_total_elements;

    Elem parent_elem = mp_parent->at(parent_index_this+1);
    Node<KEY,VALUE,PARAM_VALUE>* tPtry = mp_parent->at(parent_index_this+1).mp_subtree.valuePtr();
//    NodeMapPtr right_sib = (*mp_parent)[parent_index_this+1].mp_subtree;
    NodeMapPtr right_sib = tPtry;
//    parent_elem.mp_subtree = (*right_sib)[0].mp_subtree;
    tPtry = right_sib->at(0).mp_subtree.valuePtr();
    parent_elem.mp_subtree = tPtry;
    array_insert (parent_elem);
    for (unsigned int i=1; i<right_sib->m_count; i++)
        array_insert (right_sib->at(i));
    mp_parent->array_delete(parent_index_this+1);
    mp_parent->m_total_elements = initiail_par_nodes;
    //mp_parent->testStruct();
    destroyPtr(right_sib);
    if (mp_parent==find_base() && !mp_parent->key_count()) {
        m_root->set_base(this);
        destroyPtr(mp_parent);
        mp_parent = null_ptr;
        //testStruct();
        return null_ptr; // parent node still has same element count
    } else if (mp_parent==find_base() && mp_parent->key_count()){
        //mp_parent->testStruct();

        return null_ptr;
    } else  if (mp_parent&& mp_parent->key_count() >= mp_parent->minimum_keys()) {
        //mp_parent->testStruct();
        return null_ptr; // no need for parent to import an element
    } else {
        return mp_parent; // parent must import an element
    }
}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::merge_left (int parent_index_this) {
// copy all elements from this node into the left sibling, along with the
// element in the parent node vector that has this node as its subtree.
// this node and its parent element are then deleted.
    mp_parent->testStruct();
    int initiail_par_nodes =  mp_parent->m_total_elements;


    Elem parent_elem = mp_parent->at(parent_index_this);
    parent_elem.mp_subtree = this->at(0).mp_subtree;
    NodeMapPtr left_sib = mp_parent->at(parent_index_this-1).mp_subtree;
    left_sib->array_insert (parent_elem);
    for (unsigned int i=1; i < m_count; i++)
        { left_sib->array_insert (m_array[i]);}
    mp_parent->array_delete (parent_index_this);
    mp_parent->m_total_elements = initiail_par_nodes;
    //mp_parent->testStruct();
    NodeMapPtr parent_node = mp_parent;  // copy before deleting this node
    if (mp_parent==find_base() && !mp_parent->key_count()) {
        m_root->set_base(left_sib);
        destroyPtr(mp_parent);
        left_sib->mp_parent = null_ptr;
        //left_sib->testStruct();

        destroyPtr(this);
        return null_ptr;
    }
    else if (mp_parent==find_base() && mp_parent->key_count()) {
        //mp_parent->testStruct();
        destroyPtr(this);
        return null_ptr;
    }
    destroyPtr(this);
    if (parent_node->key_count() >= parent_node->minimum_keys()){
        //mp_parent->testStruct();

        return null_ptr; // no need for parent to import an element
    }
    return parent_node; // parent must import an element

}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::right_sibling (int& parent_index_this) {
    parent_index_this = index_has_subtree (); // for element with THIS as subtree
    if (parent_index_this == INVALID_INDEX)
        return null_ptr;
    // now mp_parent is known not to be null
    if ((unsigned int)parent_index_this >= mp_parent->m_count-1)
        return null_ptr;  // no right sibling
    return mp_parent->m_array[parent_index_this+1].mp_subtree;  // might be null
}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::left_sibling (int& parent_index_this) {
    parent_index_this = index_has_subtree (); // for element with THIS as subtree
    if (parent_index_this == INVALID_INDEX)
        return null_ptr;
    // now mp_parent is known not to be null
    if (parent_index_this==0)
        return null_ptr;  // no left sibling
    return mp_parent->m_array[parent_index_this-1].mp_subtree;  // might be null
}

template<class KEY, class VALUE, class PARAM_VALUE> int Nde::index_has_subtree () {
// return the element in this node's parent that has the "this" pointer as its subtree
// logic changed here to allow non-unique keys...
    if (!mp_parent)
        return INVALID_INDEX;
    int first = 0;
    int last = mp_parent->m_count-1;
    //m_count==1 means an "empty" node that has be found "blindly"
    while (m_count > 1 && last-first > 1 && (mp_parent->m_array[first] < mp_parent->m_array[last])) {
        int mid = first+(last-first)/2;
        //Elem& smallest = smallest_key();
        if (smallest_key()> mp_parent->m_array[mid])
            first = mid;
        else if(smallest_key() < mp_parent->m_array[mid])
            last = mid;
        else
            break;
    }
    for(int i=first; i<=last;i++){
        if (mp_parent->m_array[i].mp_subtree == (this)) //MPointer<Node>
            return i;
    }
    qDebug() << "error quick sorting parent's children";
    for(unsigned int i=0; i<=mp_parent->m_count-1;i++){
        if (mp_parent->m_array[i].mp_subtree == MPointer<Node>(this))
            return i;
    }
    Q_ASSERT(false);
    throw "error in index_has_subtree";
}

template<class KEY, class VALUE, class PARAM_VALUE> Elem& Nde::smallest_key_in_subtree () {
    if (is_leaf())
        return m_array[1];
    else
        return m_array[0].mp_subtree->smallest_key_in_subtree();
}

template<class KEY, class VALUE, class PARAM_VALUE> Elem& Nde::search(Elem& desired, NodeMapPtr& last_visited_ptr) {
    int dummy=0;
    return search (desired, last_visited_ptr, dummy);
}

template<class KEY, class VALUE, class PARAM_VALUE> Elem& Nde::search (Elem& desired, NodeMapPtr& last_visited_ptr, int& index) {
    // the zeroth element of the vector is a special case (no key value or
    // payload, just a subtree).  the seach starts at the *this node, not
    // at the root of the b-tree.
    NodeMapPtr current = this;
    Q_ASSERT(current);
    if (!key_count()){ current = null_ptr;}

    while (current) {
        last_visited_ptr = current;
        // if desired is less than all values in current node
        if (current->m_count>1 && desired < current->m_array[1])
            current = current->m_array[0].mp_subtree;
        // if desired is greater than all values in current node
        else if (desired>current->m_array[current->m_count-1])
            current = current->m_array[current->m_count-1].mp_subtree;
        else {
            // binary search of the node
            int first = 1;
            int last = current->m_count-1;
            while (last-first > 1) {
                int mid = first+(last-first)/2;
                if (desired> current->m_array[mid])
                    first = mid;
                else
                    last = mid;
            }
            if (current->m_array[first].matches(desired)){
                index = first;
                return current->m_array[first];
            }
            if (current->m_array[last].matches(desired)){
                index = last;
                return current->m_array[last];
            }

            if (current->m_array[last]>desired)
                current = current->m_array[first].mp_subtree;
            else
                current = current->m_array[last].mp_subtree;
        }
    }
    index = 0;
    return m_failure;
}

template<class KEY, class VALUE, class PARAM_VALUE> Elem& Nde::search_by_key(KEY key, NodeMapPtr& last_visited_ptr, int& index) {
    // the zeroth element of the vector is a special case (no key value or
    // payload, just a subtree).  the seach starts at the *this node, not
    // at the root of the b-tree.
    NodeMapPtr current = this;
    Q_ASSERT(current);
    if (!key_count()){ current = null_ptr;}

    while (current) {
        last_visited_ptr = current;
        // if key is less than all values in current node
        if (current->m_count>1 && key < current->m_array[1].m_key)
            current = current->m_array[0].mp_subtree;
        // if key is greater than all values in current node
        else if (key>current->m_array[current->m_count-1].m_key)
            current = current->m_array[current->m_count-1].mp_subtree;
        else {
            // binary search of the node
            int first = 1;
            int last = current->m_count-1;
            while (last-first > 1) {
                int mid = first+(last-first)/2;
                if (key> current->m_array[mid].m_key)
                    first = mid;
                else
                    last = mid;
            }
            if (current->m_array[first].m_key == key){
                index = first;
                return current->m_array[first];
            }
            if (current->m_array[last].m_key == key){
                index = last;
                return current->m_array[last];
            }

            if (current->m_array[last].m_key > key)
                current = current->m_array[first].mp_subtree;
            else
                current = current->m_array[last].mp_subtree;
        }
    }
    index = 0;
    return m_failure;
}

//Search Insert position
//the node next to the last visited pointer
//has to be checked to see if it's an appropriate
//insertion point!!
// --- We are allowing DUPLICATES,
template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::FindInsertPosition(Elem& desired, int& index){
    // the zeroth element of the vector is a special case (no key value or
    // payload, just a subtree).
    NodeMapPtr current = this;

    while (current) {
        // desired less than all values in current node
        if((current->m_count <= 1) || !(current->m_array[1] < desired )){
            if(!current->m_array[0].mp_subtree) {
                //Otherwise, we're below everything
                index = 0;
                //Q_ASSERT(!current->mp_parent);
                return current;
                break;
            }
            current = current->m_array[0].mp_subtree;
        // desired greater than all values in current node
        } else if (desired > current->m_array[current->m_count-1] ){
            //ABOVE ALL LEAVES CASE
            if(!current->m_array[current->m_count-1].mp_subtree) {
                index = current->m_count-1;
                return current;
            }
            current = current->m_array[current->m_count-1].mp_subtree;
        } else {
            // do a binary search of as many of the nodes as possible...
            Q_ASSERT(current->m_array[1] < desired);
            unsigned int first = 1;
            unsigned int last = current->m_count-1;
            while (last-first > 1) {
                int mid = first+(last-first)/2;
                if (desired> current->m_array[mid] )//|| (desired.identical_to(current->m_array[mid]) &&(desired.m_payload >= current->m_array[mid].m_payload ))
                    { first = mid;}
                else if(desired < current->m_array[mid])  {
                    last = mid;
                } else break;
            }
            for(unsigned int i=first;i<=last;i++){
                if(current->m_array[i] < desired &&
                   (!(current->m_array[i+1] <desired) || i+1 > current->m_count-1) ){
                    if(!current->m_array[i].mp_subtree){
                        index = i;
                        return current;
                    }
                    current = current->m_array[i].mp_subtree;
                    break;
                }
            }
        }
    }
    Q_ASSERT(false);
    return null_ptr;

}



//search for Element having greatest key less than "desired"...

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::searchGL(Elem& desired, int& local_index) {
    // the zeroth element of the vector is a special case (no key value or
    // payload, just a subtree).
    NodeMapPtr current = this;
    local_index = -1;
    if (!key_count()){
        local_index = 0;
        return null_ptr;
    }

    while (current) {
        Q_ASSERT(current->m_count > 1 );
        // desired key less than all values in current node
        if(!(current->m_array[1].m_key < desired.m_key )){
            if(!current->m_array[0].mp_subtree) {
                //Otherwise, we're below everything
                local_index = 0;
                //Q_ASSERT(!current->mp_parent);
                return null_ptr;
            }
            current = current->m_array[0].mp_subtree;
        // desired greater than all values in current node
        } else if (desired.m_key > current->m_array[current->m_count-1].m_key  ){
            //ABOVE ALL LEAVES CASE
            if(!current->m_array[current->m_count-1].mp_subtree
               || !(current->m_array[current->m_count-1].mp_subtree->smallest_key_in_subtree().m_key
                    < desired.m_key )) {
                local_index = current->m_count-1;
                return current;
            }
            current = current->m_array[current->m_count-1].mp_subtree;
        } else {
            // do a binary search of as many of the nodes as possible...
            Q_ASSERT(current->m_array[1].m_key  < desired.m_key );
            int first = 1;
            int last = current->m_count-1;
            while (last-first > 1) {
                int mid = first+(last-first)/2;
                if (desired.m_key > current->m_array[mid].m_key  )//|| (desired.identical_to(current->m_array[mid]) &&(desired.m_payload >= current->m_array[mid].m_payload ))
                    { first = mid;}
                else if(desired.m_key  < current->m_array[mid].m_key )  {
                    last = mid;
                } else break;
            }
            for(int i=first;i<=last;i++){
                if(current->m_array[i].m_key  < desired.m_key  &&
                   !(current->m_array[i+1].m_key  <desired.m_key )){
                    if(!current->m_array[i].mp_subtree
                       || !(current->m_array[i].mp_subtree->smallest_key_in_subtree().m_key  < desired.m_key  )){
                        local_index = i;
                        return current;
                    }
                    current = current->m_array[i].mp_subtree;
                    break;
                }
            }
        }
    }
    Q_ASSERT(false);
    return null_ptr;

}

//Find "half open" interval - items != end_key
template<class KEY, class VALUE, class PARAM_VALUE> vector<VALUE> Nde::get_range(KEY begin_key, KEY end_key,
    long limit, long offset){
    VALUE v;
    Elem se(begin_key, v);
    int elemIndex;
    NodeMapPtr counter_node = searchGL(se, elemIndex);
    vector<VALUE> result;
    KEY f;
    KEY g;
    if(m_count <2){
        return result;
    }
    Q_ASSERT(counter_node==null_ptr || elemIndex >0);
    go_to_next(counter_node, elemIndex);
    long counter=0;
    while((elemIndex > -1) && counter_node &&
                (counter_node->m_array[elemIndex].m_key < end_key) &&
                (limit==-1 ||(counter<limit))) {
        Q_ASSERT(counter_node);
        f = counter_node->m_array[elemIndex].m_key;
        Q_ASSERT(!(f < g) );
        Q_ASSERT(counter_node->m_array[elemIndex].m_key < end_key);
        Q_ASSERT(!(counter_node->m_array[elemIndex].m_key < begin_key));
        if(offset==-1 || limit>=offset){
            result.push_back(counter_node->m_array[elemIndex].m_payload);
        }
        go_to_next(counter_node, elemIndex);
        g = f;
        counter++;
    }
    return result;
}

//Find "half open" interval - items != end_key
template<class KEY, class VALUE, class PARAM_VALUE> vector<VALUE> Nde::get_ordinal_range(NodeMapPtr start_n, int small_index, int count){

    int elemIndex= small_index;
    NodeMapPtr counter_node = start_n;

    int prev_index =-1;
    NodeMapPtr prev_node = null_ptr;
    vector<VALUE> result;
    KEY f;
    KEY g;
    if(m_count <2){
        return result;
    }
    Q_ASSERT(counter_node==null_ptr || elemIndex >0);
    go_to_next(counter_node, elemIndex);
    int c=0;
    while((elemIndex > -1) && counter_node && c < count) {
        Q_ASSERT(counter_node);
        f = counter_node->m_array[elemIndex].m_key;
        Q_ASSERT(!(f < g) );
        result.push_back(counter_node->m_array[elemIndex].m_payload);
        prev_node = counter_node;
        prev_index = elemIndex;
        go_to_next(counter_node, elemIndex);
        g = prev_node->m_array[prev_index].m_key;
        c++;
    }
    return result;
}

template<class KEY, class VALUE, class PARAM_VALUE> NodeMapPtr Nde::smallest_node_in_subtree_ptr(){
    if (!m_array[0].mp_subtree) return this;
    return m_array[0].mp_subtree->smallest_node_in_subtree_ptr();
}

template<class KEY, class VALUE, class PARAM_VALUE> void Nde::go_to_next(NodeMapPtr& count_node, int& elemIndex){

    if(!count_node){
        elemIndex = 1;
        count_node = smallest_node_in_subtree_ptr();
        return;
    }
    KEY old_k = count_node->m_array[elemIndex].m_key;
    //NodeMapPtr old_node = count_node;

    // If we are 'within' the tree's nodes, drill all the way down...
    Q_ASSERT(count_node);
    if(elemIndex < (int)count_node->m_count &&
        count_node->m_array[elemIndex].mp_subtree){
        count_node = count_node->m_array[elemIndex].mp_subtree;
        while(count_node->m_array[0].mp_subtree){
            count_node =  count_node->m_array[0].mp_subtree;
        }
        elemIndex = 1;
        Q_ASSERT(!(old_k > count_node->m_array[elemIndex].m_key));
        return;
    }

    //or go sideways..
    elemIndex++;

    //or go up...
    while(count_node->mp_parent && elemIndex >= (int)count_node->m_count){
        elemIndex = count_node->index_has_subtree()+1;
        NodePtr old_node = count_node;
        count_node = count_node->mp_parent;
        Q_ASSERT(elemIndex !=INVALID_INDEX+1);
        Q_ASSERT(count_node->m_array[elemIndex-1].mp_subtree == old_node);
    }
//    Q_ASSERT(!(old_k > count_node->m_array[elemIndex].m_key)
//             || (unsigned int)elemIndex >= count_node->m_count);
    Q_ASSERT(elemIndex>0);
    Q_ASSERT(!count_node->mp_parent|| (unsigned int)elemIndex < count_node->m_count);

    //Indicate the end now...
    if(!count_node->mp_parent && (unsigned int)elemIndex > count_node->m_count-1){
        elemIndex = -1;
        count_node = null_ptr;
    }
    return;
}

template<class KEY, class VALUE, class PARAM_VALUE> qint32 Nde::item_position(Elem&  baseElem) {

    unsigned int order_found = 0;
    //"Numbering" is weird for this datastructure
    //For *leaf* nodes, *1* is first,
    //For *tree* nodes, 0 is first
    for(unsigned int i=0; i < m_count;i++){

        // return exact count if ! greater than implies equal to m_array[i]
        if(i>0) {
            order_found++;
            if(!(baseElem > m_array[i]) )
                { return order_found; }
        }

        // return subcount if between m_array[i] and m_array[i+1]
        // or past m_array[m_count]
        if(i+1 < m_count
            && (!(baseElem < m_array[i+1])
                && (!m_array[i].mp_subtree
                    || (baseElem > m_array[i].mp_subtree->absolute_largest_key() )))){
            if(m_array[i].mp_subtree)
                {order_found+= m_array[i].mp_subtree->m_total_elements;}
        } else {
            if(m_array[i].mp_subtree)
                { order_found +=m_array[i].mp_subtree->item_position(baseElem); }
            return order_found;
        }
    }
    //Can this happen if the key is not within the matrix??
    Q_ASSERT(order_found == m_total_elements);
    return m_total_elements;
}

//By key...
template<class KEY, class VALUE, class PARAM_VALUE> qint32 Nde::get_position(KEY key){

    unsigned int order_found = 0;
    //"Numbering" is weird for this datastructure
    //For *leaf* nodes, *1* is first,
    //For *tree* nodes, 0 is first
    for(unsigned int i=0; i < m_count;i++){

        // return exact count if ! greater than implies equal to m_array[i]
        if(i>0) {
            order_found++;
            if(!(key > m_array[i].m_key) )
            { return order_found; }
        }

        // return subcount if between m_array[i] and m_array[i+1]
        // or past m_array[m_count]
        if(i+1 < m_count
           && (!(key < m_array[i+1].m_key)
               && (!m_array[i].mp_subtree
                   || (key > m_array[i].mp_subtree->absolute_largest_key().m_key )))){
            if(m_array[i].mp_subtree)
            {order_found+= m_array[i].mp_subtree->m_total_elements;}
        } else {
            if(m_array[i].mp_subtree)
            { order_found +=m_array[i].mp_subtree->get_position(key); }
            return order_found;
        }
    }
    //Can this happen if the key is not within the matrix??
    Q_ASSERT(order_found == m_total_elements);
    return m_total_elements;
}


template<class KEY, class VALUE, class PARAM_VALUE> Elem& Nde::ordinalSearch (
            unsigned int order_desired, NodeMapPtr& outNode, unsigned int& index) {

    //Intend to begins at the base
    //if(order_desired==0) {return smallest_key_in_subtree ();}
    Q_ASSERT(m_total_elements >= order_desired);
    //Q_ASSERT(order_desired >0);
    unsigned int order_found = 0;
    unsigned int new_order = order_found;

    //"Numbering" is weird for this datastructure
    //For *leaf* nodes, *1* is first element with meaning,
    //For *tree* nodes, 0 is first element with meaning

    for(unsigned int i=0;i<m_count;i++){
        new_order = order_found;
        if(i>0) {
            new_order ++;
            if(order_desired == new_order){
                index = i;
                outNode = this;
                return m_array[i];
            }
            order_found = new_order;
        }
        if(m_array[i].mp_subtree){
            new_order += m_array[i].mp_subtree->m_total_elements;
            if(order_desired <= new_order){
                Elem& e = m_array[i].mp_subtree->ordinalSearch(
                    order_desired - (order_found), outNode, index);
                Q_ASSERT(e.valid());
                return e;
            }
            order_found = new_order;
        }
    }
    Q_ASSERT(order_found == m_total_elements);
    return m_failure;//if you beyond boundaries, you *should* 'nothing'

}

//template<class KEY, class VALUE, class PARAM_VALUE> Elem& Nde::ordinalSearch (unsigned int order_desired) {
//
//    //Intend to begins at the base
//    //if(order_desired==0) {return smallest_key_in_subtree ();}
//    Q_ASSERT(m_total_elements >= order_desired);
//    //Q_ASSERT(order_desired >0);
//    unsigned int order_found = 0;
//    unsigned int new_order = order_found;
//
//    //"Numbering" is weird for this datastructure
//    //For *leaf* nodes, *1* is first,
//    //For *tree* nodes, 0 is first
//
//    for(unsigned int i=0;i<m_count;i++){
//        new_order = order_found;
//        if(i>0) {
//            new_order ++;
//            if(order_desired == new_order){
//                //Q_ASSERT(i+ is_leaf_skip_factor!=m_count);
//                return m_array[i];
//            }
//            order_found = new_order;
//        }
//        if(m_array[i].mp_subtree){
//            new_order += m_array[i].mp_subtree->m_total_elements;
//            if(order_desired <= new_order){
//                Elem& e = m_array[i].mp_subtree->ordinalSearch(
//                    order_desired - (order_found));
//                Q_ASSERT(e.valid());
//                return e;
//            }
//            order_found = new_order;
//        }
//    }
//    Q_ASSERT(order_found == m_total_elements);
//    return m_failure;//if you beyond boundaries, you *should* 'nothing'
//
//}

template<class KEY, class VALUE, class PARAM_VALUE> int Nde::testStruct(){
#ifndef QT_NO_DEBUG
    ElemPtr dd=0;
    return testTreeStructure(dd);
#else
    return 1;
#endif
}

#ifndef QT_NO_DEBUG
template<class KEY, class VALUE, class PARAM_VALUE> int Nde::testTreeStructure(ElemPtr& lastBottomElem){
    unsigned int sub_node_count=0;
    int node_height=-1;
    int prev_node_height=-1;
    if(lastBottomElem){
        Q_ASSERT((!(*lastBottomElem > m_array[1]) ));
    }
    ElemPtr res;


    int height = -1;
    for(unsigned int i=0;i<m_count;i++){
        if(i>0) {
            sub_node_count++;
            Q_ASSERT( !(m_array[i-1] > m_array[i]));
        }
        if(m_array[i].mp_subtree) {
            unsigned int p_i = m_array[i].mp_subtree->index_has_subtree();
            Q_ASSERT(i == p_i);
            //Empty nodes need to be removed...
            //Q_ASSERT(m_array[i].mp_subtree->m_count > 1);
            sub_node_count += m_array[i].mp_subtree->m_total_elements;
            prev_node_height = node_height;
            res = &m_array[i]; //m_array lower than anything on it's tree
            int child_h = m_array[i].mp_subtree->testTreeStructure(res);
            if(height ==-1){
                height = child_h+1;
            } else {
                Q_ASSERT(height == child_h+1);
            }

            //Either its the first call or the heights are the same
            Q_ASSERT(prev_node_height ==-1 || (prev_node_height == node_height));
            Q_ASSERT((i == (m_count -1)) ||   !(*res > m_array[i+1]));
        }
    }
    Q_ASSERT(sub_node_count == m_total_elements);
    if(height ==-1){
        height = 0;
    }
    if(is_leaf()) {
        lastBottomElem = &m_array[m_count-1];
    } else {
        lastBottomElem = res;
    }

    return height;
}

#else

#endif

template<class KEY, class VALUE, class PARAM_VALUE> long Nde::testAllocatedSize(){
#ifndef QT_NO_DEBUG
    //Each node has itself plus it's vector of elems
    //sizeof should not have any idea of the space inside the vector...

    long res = sizeof(Nde);
    res += sizeof(Elem)*m_array.size();
    for(unsigned int i=0;i<m_count;i++){
        if(m_array[i].mp_subtree){
            res += m_array[i].mp_subtree->testAllocatedSize();
        }
    }
    return res;

#else
    return 1;
#endif
}

template<class KEY, class VALUE, class PARAM_VALUE> long Nde::testCountBadNodes(float badRatio){
#ifndef QT_NO_DEBUG
    //Each node has itself plus it's vector of elems
    //sizeof should not have any idea of the space inside the vector...

    long res = 0;
    if(m_count < m_array.size()*badRatio){
        res +=1;
    }
    for(unsigned int i=0;i<m_count;i++){
        if(m_array[i].mp_subtree){
            res += m_array[i].mp_subtree->testCountBadNodes(badRatio);
        }
    }
    return res;

#else
    return 1;
#endif
}


#endif // BTREE_IMPL_H
