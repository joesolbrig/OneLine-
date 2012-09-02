#include <QtGlobal>

#include "btree.h"
 
template<class KEY, class VALUE> int Nde::minimum_keys () {
    // minus 1 for the empty slot left for splitting the node
    int size = m_vector.size();
    int ceiling_func = (size-1)/2;
    if (ceiling_func*2 < size-1)
        ceiling_func++;
    return ceiling_func-1;  // for clarity, may increment then decrement
}
 
#ifndef QT_NO_DEBUG
template<class KEY, class VALUE> void Nde::set_debug() {
// the contents of an STL vector are not visible in the visual C++ debugger,
// so this function copies up to eight elements from the STL vector into
// a simple C++ array.
    unsigned int i;
    for (i=0; i<m_count && i<8; i++) {
        debug[i] = m_vector[i];
        if (m_vector[i].mp_subtree)
            m_vector[i].mp_subtree->set_debug();
    }
    for ( ; i<8; i++)
        debug[i] = m_failure;
}
#endif

#ifndef QT_NO_DEBUG
template<class KEY, class VALUE> void Nde::insert_zeroth_subtree (Nde* subtree) {
    m_vector[0].mp_subtree = subtree;
    m_vector[0].m_key = "";
    m_count = 1;
    if (subtree){
        subtree->mp_parent = this;
        m_total_nodes += subtree->m_total_nodes;
    }
}
#endif
 
template<class KEY, class VALUE> void Nde::dump (){
// write out the keys in this node and all its subtrees, along with
// node adresses, for debugging purposes
    if (this == m_root.get_root())
        cout << "ROOT\n";
    cout << "\nthis=" << this << endl;
    cout << "parent=" << mp_parent << " count=" << m_count << endl;
    for (unsigned int i=0; i<m_count; i++)
        m_vector[i].dump();
    for (unsigned int i=0; i<m_count; i++)
        if (m_vector[i].mp_subtree)
            m_vector[i].mp_subtree->dump();
    cout << endl;
}
 
template<class KEY, class VALUE>  Nde::Node(RootTracker<KEY, VALUE>& root_track)  : m_root(root_track) {
    // limit the size of the vector to 4 kilobytes max and 200 entries max.
    int num_elements = max_elements*sizeof(Elem)<=max_array_bytes ?
                       max_elements : max_array_bytes/sizeof(Elem);
    // in case key or payload is really huge
    if (num_elements < 6){ num_elements = 6;}
    m_vector.resize (num_elements);
    m_count = 0;
    m_total_nodes = 0;
    mp_parent = 0;
    insert_zeroth_subtree (0);
}

//template<class KEY, class VALUE> Nde* Nde::copy_subtrees(RootTracker<KEY, VALUE>& new_root, Nde* new_parent) {
//    Nde* copy_node = new Node(new_root);
//    copy_node->mp_parent = new_parent;
//    copy_node->m_count = m_count;
//    copy_node->m_total_nodes = m_total_nodes;
//
//    for (unsigned int i=0; i< m_count; i++) {
//        copy_node->m_vector[i].m_key = m_vector[i].m_key;
//        copy_node->m_vector[i].m_payload = m_vector[i].m_payload;
//        if (!m_vector[i].mp_subtree)
//            copy_node->m_vector[i].mp_subtree=0;
//        else {
//            copy_node->m_vector[i].mp_subtree = m_vector[i].mp_subtree->copy_subtrees(new_root, this);
//        }
//    }
//    return copy_node;
//}
 
template<class KEY, class VALUE> Nde* Nde::find_root () {
    Nde* current = this;
    while (current->mp_parent)
        current = current->mp_parent;
    return current;
}
 
template<class KEY, class VALUE>  bool Nde::is_leaf () {
    return m_vector[0].mp_subtree==0;
}
 
template<class KEY, class VALUE> int Nde::delete_all_subtrees () {
// return the number of nodes deleted
    int count_deleted = 0;
    for (unsigned int i=0; i< m_count; i++) {
        if (!m_vector[i].mp_subtree)
            continue;
        else if (m_vector[i].mp_subtree->is_leaf()) {
            delete m_vector[i].mp_subtree;
            count_deleted++;
        }
        else
            count_deleted += m_vector[i].mp_subtree->delete_all_subtrees();
    }
    return count_deleted;
}


 
template<class KEY, class VALUE> bool Nde::vector_insert (Elem& element) {
// this method merely tries to insert the argument into the current node.
// it does not concern itself with the entire tree.
// if the element can fit into m_vector, slide all the elements
// greater than the argument forward one position and copy the argument
// into the newly vacated slot, then return true.  otherwise return false.
// note: the tree_insert method will already have verified that the key
// value of the argument element is not present in the tree.
   
    if (m_count >= m_vector.size()-1) // leave an extra slot for split_insert
        return false;
    unsigned int i = m_count;
   
    while (i>0 && m_vector[i-1]>element) {
        m_vector[i] = m_vector[i-1];
        i--;
    }
    if (element.mp_subtree){
        element.mp_subtree->mp_parent = this;
        m_total_nodes += element.mp_subtree->m_total_nodes;
    }
    m_total_nodes++;
    m_vector[i] = element;
    m_count++;

    return true;
}
 
template<class KEY, class VALUE> bool Nde::vector_delete (Elem& target) {
// delete a single element in the vector belonging to *this node.
// if the target is not found, do not look in subtrees, just return false.
 
    int target_pos = -1;
    int first = 1;
    int last = m_count-1;
    // perform binary search
    while (last-first > 1) {
        int mid = first+(last-first)/2;
        if (target>=m_vector[mid])
            first = mid;
        else
            last = mid;
    }
    if (m_vector[first]==target)
        target_pos = first;
    else if (m_vector[last]==target)
        target_pos = last;
    else
        return false;
    // the element's subtree, if it exists, is to be deleted or re-attached
    // in a different function.  not a concern here.  just shift all the
    // elements in positions greater than target_pos.
    for (unsigned int i=target_pos; i<m_count; i++)
        m_vector[i] = m_vector[i+1];
   
    m_count--;
    m_total_nodes--;
    if (target.mp_subtree){ m_total_nodes -= target.mp_subtree->m_total_nodes;}
    return true;
}
 
template<class KEY, class VALUE> bool Nde::vector_delete (int target_pos) {
// delete a single element in the vector belonging to *this node.
// the element is identified by position, not value.
 
    //since target_pos >0, cast safe for second part
    if (target_pos<0 || (unsigned int)target_pos>=m_count)
        return false;
  
    //Remove target's items
    if (m_vector[target_pos].mp_subtree){ m_total_nodes -= m_vector[target_pos].mp_subtree->m_total_nodes;}

    
    // the element's subtree, if it exists, is to be deleted or re-attached
    // in a different function.  not a concern here.  just shift all the
    // elements in positions greater than target_pos.
    for (unsigned int i=target_pos; i<m_count; i++){
        m_vector[i] = m_vector[i+1];
    }
   
    m_count--;
    m_total_nodes--;
    return true;
}
 
template<class KEY, class VALUE> bool Nde::vector_insert_for_split (Elem& element) {
// this method insert an element that is in excess of the nominal capacity of
// the node, using the extra slot that always remains unused during normal
// insertions.  this method should only be called from split_insert()
 
    if (m_count >= m_vector.size()) // error
        {throw "bad m_count in vector_insert_for_split";}

    unsigned int i = m_count;
    while (i>0 && m_vector[i-1]>element) {
        m_vector[i] = m_vector[i-1];
        i--;
    }

    if (element.mp_subtree){ element.mp_subtree->mp_parent = this;}
    m_vector[i] = element;
    m_count++;
    return true;
}
 
template<class KEY, class VALUE>  Nde* Nde::split_insert(Elem& element) {
 
#ifndef QT_NO_DEBUG
    int existing_p_nodes = 0;
    if(mp_parent){
        existing_p_nodes = mp_parent->m_total_nodes;
    }
#endif

    // split_insert should only be called if node is full
    if (m_count != m_vector.size()-1)
        throw "bad m_count in split_insert";
 
    vector_insert_for_split (element);

    unsigned int split_point = m_count/2;
    if (2*split_point < m_count)  {split_point++;}// "ceiling"

    // new node receives the rightmost half of elements in *this node
    Nde* new_node = new Node(m_root);
    //One for upward node
    int nodes_removed=0;
    if (element.mp_subtree){
        m_total_nodes += element.mp_subtree->m_total_nodes;
    }
    //Ripping out the upward element from the node...
    Elem upward_element = m_vector[split_point];
    //ONLY it's CHILDREN go to new_node
    if (upward_element.mp_subtree){
        nodes_removed += upward_element.mp_subtree->m_total_nodes;
    }
    new_node->insert_zeroth_subtree (upward_element.mp_subtree);
    upward_element.mp_subtree = new_node;

    // element that gets added to new_node
    for (unsigned int i=1; i<m_count-split_point; i++){
        new_node->vector_insert(m_vector[split_point+i]); 
        //Count nodes *removed*
        nodes_removed ++;
        if (m_vector[split_point+i].mp_subtree){
            nodes_removed += m_vector[split_point+i].mp_subtree->m_total_nodes;
        }
    }

    new_node->m_count = m_count-split_point;
    new_node->mp_parent = mp_parent;

    m_count = split_point;
    m_total_nodes -= nodes_removed;

//    int n_total = new_node->m_total_nodes + m_total_nodes;

//    int d;
//    Elem* dd=0;
//    testTreeStructure(d, dd);
//    dd = 0;
//    new_node->testTreeStructure(d, dd);

    // now insert the new node into the parent, splitting it if necessary
    if (mp_parent ){
        mp_parent->m_total_nodes -= nodes_removed;
        if(mp_parent->vector_insert(upward_element)){
            mp_parent->node_size();
//            dd = 0;
//            mp_parent->testTreeStructure(d,dd);
            return mp_parent;
        }
        Nde* res = mp_parent->split_insert(upward_element);
//        dd =0;
//        mp_parent->testTreeStructure(d, dd);
        return res;

    } else { // this node was the root
        Nde* new_root = new Node(m_root);
        new_root->insert_zeroth_subtree(this);
        this->mp_parent = new_root;
        new_node->mp_parent = new_root;
        new_root->vector_insert (upward_element);
        m_root.set_root (m_root.get_root(),  new_root);
        new_root->mp_parent = 0;
        new_root->adjust_parent_totals();
//        dd =0;
//        new_root->testTreeStructure(d, dd);
        return new_root;
    }
    Q_ASSERT(false);
    return 0;
}
 
template<class KEY, class VALUE> bool Nde::tree_insert (Elem& element) {
    Nde* last_visited_ptr = this;
    // This has the SIDE EFFECT of finding the proper node
    // to insert into, BAD, BAD effect programmers!!
    if (search(element, last_visited_ptr).valid()){
        if(!m_root.allow_duplicates){
            return false;
        }
    }
    if (last_visited_ptr->vector_insert(element)){
        last_visited_ptr->adjust_parent_totals();
        return true;
    } else {
        Nde* res = last_visited_ptr->split_insert(element);
        res->adjust_parent_totals();
        return true;
    }

}

template<class KEY, class VALUE> void Nde::adjust_parent_totals(){
    Nde* gps = mp_parent;
    while(gps){
        gps->m_total_nodes++;
        gps = gps->mp_parent;
    }

}
 
template<class KEY, class VALUE> bool Nde::delete_element (Elem& target) {
// target is just a package for the key value.  the reference does not
// provide the address of the Elem instance to be deleted.

    // first find the node contain the Elem instance with the given key
    Nde* node = 0;
    int parent_index_this = invalid_index;
    Elem& found = search (target, node);
    if (!found.valid()){ return false; }
 
    if (node->is_leaf() && node->key_count() > node->minimum_keys()){
        return node->vector_delete (target);
    } else if (node->is_leaf()) {
        node->vector_delete (target);
        // loop invariant: if _node_ is not null_ptr, it points to a node
        // that has lost an element and needs to import one from a sibling
        // or merge with a sibling and import one from its parent.
        // after an iteration of the loop, _node_ may become null or
        // it may point to its parent if an element was imported from the
        // parent and this caused the parent to fall below the minimum
        // element count.
        while (node) {
            // NOTE: the "this" pointer may no longer be valid after the first
            // iteration of this loop!!!
            if (node==node->find_root() && node->is_leaf())
                break;
            if (node==node->find_root() && !node->is_leaf()) // sanity check
                throw "node should not be root in delete_element loop";
            // is an extra element available from the right sibling (if any)
            Nde* right = node->right_sibling(parent_index_this);
            if (right && right->key_count() > right->minimum_keys())
                node = node->rotate_from_right(parent_index_this);
            else {
                // is an extra element available from the left sibling (if any)
                Nde* left = node->left_sibling(parent_index_this);
                if (left && left->key_count() > left->minimum_keys())
                    node = node->rotate_from_left(parent_index_this);
                else if (right)
                    node = node->merge_right(parent_index_this);
                else if (left)
                    node = node->merge_left(parent_index_this);
            }
        }
    } else {
        Elem& smallest_in_subtree = found.mp_subtree->smallest_key_in_subtree();
        found.m_key = smallest_in_subtree.m_key;
        found.m_payload = smallest_in_subtree.m_payload;
        found.mp_subtree->delete_element (smallest_in_subtree);
    }
    return true;
}
 
template<class KEY, class VALUE> Nde* Nde::rotate_from_right(int parent_index_this) {
    // new element to be added to this node
    Elem underflow_filler = (*mp_parent)[parent_index_this+1];
    // right sibling of this node
    Nde* right_sib = (*mp_parent)[parent_index_this+1].mp_subtree;
    underflow_filler.mp_subtree = (*right_sib)[0].mp_subtree;
    // copy the entire element
    (*mp_parent)[parent_index_this+1] = (*right_sib)[1];
    // now restore correct pointer
    (*mp_parent)[parent_index_this+1].mp_subtree = right_sib;
    vector_insert (underflow_filler);
    right_sib->vector_delete(0);
    (*right_sib)[0].m_key = "";
    (*right_sib)[0].m_payload = "";
    return null_ptr; // parent node still has same element count
}
 

template<class KEY, class VALUE> Nde* Nde::rotate_from_left(int parent_index_this) {
    // new element to be added to this node
    Elem underflow_filler = (*mp_parent)[parent_index_this];
    // left sibling of this node
    Nde* left_sib = (*mp_parent)[parent_index_this-1].mp_subtree;
    underflow_filler.mp_subtree = (*this)[0].mp_subtree;
    (*this)[0].mp_subtree = (*left_sib)[left_sib->m_count-1].mp_subtree;
    if ((*this)[0].mp_subtree)
        (*this)[0].mp_subtree->mp_parent = this;
    // copy the entire element
    (*mp_parent)[parent_index_this] = (*left_sib)[left_sib->m_count-1];
    // now restore correct pointer
    (*mp_parent)[parent_index_this].mp_subtree = this;
    vector_insert (underflow_filler);
    left_sib->vector_delete(left_sib->m_count-1);
    return null_ptr; // parent node still has same element count
}
 
template<class KEY, class VALUE> Nde* Nde::merge_right (int parent_index_this) {
// copy elements from the right sibling into this node, along with the
// element in the parent node vector that has the right sibling as it subtree.
// the right sibling and that parent element are then deleted
 
    Elem parent_elem = (*mp_parent)[parent_index_this+1];
    Nde* right_sib = (*mp_parent)[parent_index_this+1].mp_subtree;
    parent_elem.mp_subtree = (*right_sib)[0].mp_subtree;
    vector_insert (parent_elem);
    for (unsigned int i=1; i<right_sib->m_count; i++)
        vector_insert ((*right_sib)[i]);
    mp_parent->vector_delete (parent_index_this+1);
    delete right_sib;
    if (mp_parent==find_root() && !mp_parent->key_count()) {
        m_root.set_root(m_root.get_root(), this);
        delete mp_parent;
        mp_parent = 0;
        return null_ptr;
    }
    else if (mp_parent==find_root() && mp_parent->key_count())
        return null_ptr;
    if (mp_parent&& mp_parent->key_count() >= mp_parent->minimum_keys())
        return null_ptr; // no need for parent to import an element
    return mp_parent; // parent must import an element
}
 
template<class KEY, class VALUE> Nde* Nde::merge_left (int parent_index_this) {
// copy all elements from this node into the left sibling, along with the
// element in the parent node vector that has this node as its subtree.
// this node and its parent element are then deleted.
 
    Elem parent_elem = (*mp_parent)[parent_index_this];
    parent_elem.mp_subtree = (*this)[0].mp_subtree;
    Nde* left_sib = (*mp_parent)[parent_index_this-1].mp_subtree;
    left_sib->vector_insert (parent_elem);
    for (unsigned int i=1; i<m_count; i++)
        left_sib->vector_insert (m_vector[i]);
    mp_parent->vector_delete (parent_index_this);
    Nde* parent_node = mp_parent;  // copy before deleting this node
    if (mp_parent==find_root() && !mp_parent->key_count()) {
        m_root.set_root(m_root.get_root(), left_sib);
        delete mp_parent;
        left_sib->mp_parent = null_ptr;
        delete this;
        return null_ptr;
    }
    else if (mp_parent==find_root() && mp_parent->key_count()) {
        delete this;
        return null_ptr;
    }
    delete this;
    if (parent_node->key_count() >= parent_node->minimum_keys())
        return null_ptr; // no need for parent to import an element
    return parent_node; // parent must import an element
 
} //_______________________________________________________________________
 
template<class KEY, class VALUE> Nde* Nde::right_sibling (int& parent_index_this) {
    parent_index_this = index_has_subtree (); // for element with THIS as subtree
    if (parent_index_this == invalid_index)
        return 0;
    // now mp_parent is known not to be null
    if (parent_index_this >= mp_parent->m_count-1)
        return 0;  // no right sibling
    return mp_parent->m_vector[parent_index_this+1].mp_subtree;  // might be null
}
 
template<class KEY, class VALUE> Nde* Nde::left_sibling (int& parent_index_this) {
    parent_index_this = index_has_subtree (); // for element with THIS as subtree
    if (parent_index_this == invalid_index)
        return 0;
    // now mp_parent is known not to be null
    if (parent_index_this==0)
        return 0;  // no left sibling
    return mp_parent->m_vector[parent_index_this-1].mp_subtree;  // might be null
}
 
template<class KEY, class VALUE> int Nde::index_has_subtree () {
// return the element in this node's parent that has the "this" pointer as its subtree
    if (!mp_parent)
        return invalid_index;
    int first = 0;
    int last = mp_parent->m_count-1;
    while (last-first > 1) {
        int mid = first+(last-first)/2;
        //Elem& smallest = smallest_key();
        if (smallest_key()>=mp_parent->m_vector[mid])
            first = mid;
        else
            last = mid;
    }
    if (mp_parent->m_vector[first].mp_subtree == this)
        return first;
    else if (mp_parent->m_vector[last].mp_subtree == this)
        return last;
    else
        throw "error in index_has_subtree";
}
 
template<class KEY, class VALUE> Elem& Nde::smallest_key_in_subtree () {
    if (is_leaf())
        return m_vector[1];
    else
        return m_vector[0].mp_subtree->smallest_key_in_subtree();
}

template<class KEY, class VALUE> Elem& Nde::largest_key_in_subtree () {
    if (!m_vector[m_count-1].mp_subtree){
        return m_vector[m_count-1];
    } else {
        return m_vector[m_count-1].mp_subtree->largest_key_in_subtree();
    }
}
 
template<class KEY, class VALUE> Elem& Nde::search (Elem& desired, Nde*& last_visited_ptr) {
    // the zeroth element of the vector is a special case (no key value or
    // payload, just a subtree).  the seach starts at the *this node, not
    // at the root of the b-tree.
    Nde* current = this;
    if (!key_count()){ current = 0;} 
    
    while (current) {
        last_visited_ptr = current;
        // if desired is less than all values in current node
        if (current->m_count>1 && desired<current->m_vector[1])
            current = current->m_vector[0].mp_subtree;
        // if desired is greater than all values in current node
        else if (desired>current->m_vector[current->m_count-1])
            current = current->m_vector[current->m_count-1].mp_subtree;
        else {
            // binary search of the node
            int first = 1;
            int last = current->m_count-1;
            while (last-first > 1) {
                int mid = first+(last-first)/2;
                if (desired>=current->m_vector[mid])
                    first = mid;
                else
                    last = mid;
            }
            if (current->m_vector[first]==desired)
                return current->m_vector[first];
            if (current->m_vector[last]==desired)
                return current->m_vector[last];
            else if (current->m_vector[last]>desired)
                current = current->m_vector[first].mp_subtree;
            else
                current = current->m_vector[last].mp_subtree;
        }
    }
 
    return m_failure;
}

//search for Element have greatest value less than desired...
template<class KEY, class VALUE> Elem& Nde::searchGL(Elem& desired) {
    // the zeroth element of the vector is a special case (no key value or
    // payload, just a subtree).  the seach starts at the *this node, not
    // at the root of the b-tree.
    Nde* current = this;
    if (!key_count()){ current = 0;}

    while (current) {
        // desired less than all values in current node
        if (current->m_count>1 && desired<current->m_vector[1]){
            current = current->m_vector[0].mp_subtree;

        // desired greater than all values in current node
        } else if (desired>current->m_vector[current->m_count-1]){
            if(!current->m_vector[current->m_count-1].mp_subtree)
                //ABOVE ALL LEAVES CASE
                //We're at the end of the sequence
                { return current->m_vector[current->m_count-1]; }
            else {current = current->m_vector[current->m_count-1].mp_subtree;}
        } else {
            // binary search of the node
            int first = 1;
            int last = current->m_count-1;
            while (last-first > 1) {
                int mid = first+(last-first)/2;
                if (desired>=current->m_vector[mid]){ first = mid;}
                else{ last = mid;}
            }
            //Exact MATCH TO LEAF CASES
            if(current->m_vector[first]==desired) {return current->m_vector[first];}
            if(current->m_vector[last]==desired) {return current->m_vector[last];}

            //BETWEEN TWO LEAVES CASE
            //within an interval on level last = first +1 !!
            if(!current->m_vector[first].mp_subtree && !current->m_vector[last].mp_subtree){
                return current->m_vector[first];
            }

            Elem& lGelem = current->m_vector[last].mp_subtree->smallest_key_in_subtree();
            if(desired < lGelem) {
                current = current->m_vector[last].mp_subtree;
            } else {
                current = current->m_vector[first].mp_subtree;
            }
        }
    }

    //BELOW ALL LEAVES CASE
    return m_failure;
}

//What position is a item in...
template<class KEY, class VALUE> int Nde::ItemPosition(Nde* base_node, Nde* top_node) {
    Nde* scan_node = base_node;
    int running_total = scan_node->m_total_nodes;
    while(scan_node != top_node ){
        int order_found = 0;
        for(int i=0; (scan_node->m_vector[i].mp_subtree != scan_node);i++){
            //Node plus children
            order_found += 1;
            if(scan_node->m_vector[i].mp_subtree){
                order_found += order_found + scan_node->m_vector[i].mp_subtree->m_total_nodes;
            }
        }
        running_total += order_found;
    }
    return running_total;
}
 
template<class KEY, class VALUE> Elem& Nde::ordinalSearch (unsigned int order_desired) {
	
    //Intend to begins at the base
    //if(order_desired==0) {return smallest_key_in_subtree ();}
    Q_ASSERT(m_total_nodes >= order_desired);
    //Q_ASSERT(order_desired >0);
    unsigned int order_found = 0;
    unsigned int new_order = order_found;

    //"Numbering" is weird for this datastructure
    //For *leaf* nodes, *1* is first,
    //For *tree* nodes, 0 is first

    for(unsigned int i=0;i<m_count;i++){
        new_order = order_found;
        if(i>0) {
            new_order ++;
            if(order_desired == new_order){
                //Q_ASSERT(i+ is_leaf_skip_factor!=m_count);
                return m_vector[i];
            }
            order_found = new_order;
        }
        if(m_vector[i].mp_subtree){
            new_order += m_vector[i].mp_subtree->m_total_nodes;
            if(order_desired <= new_order){
                Elem& e = m_vector[i].mp_subtree->ordinalSearch(
                    order_desired - (order_found));
                Q_ASSERT(e.valid());
                return e;
            }
            order_found = new_order;
        }
    }
    Q_ASSERT(order_found == m_total_nodes);
    return m_failure;//if you beyond boundaries, you *should* 'nothing'
 
}



#ifndef QT_NO_DEBUG
template<class KEY, class VALUE> bool Nde::testTreeStructure(int& height, Elem*& lastBottomElem){
    unsigned int sub_node_count=0;
    int node_height=-1;
    int prev_node_height=-1;
    if(lastBottomElem){ Q_ASSERT((*lastBottomElem) < m_vector[1]); }
    Elem* res;

    for(unsigned int i=0;i<m_count;i++){
        if(i>0) {
            sub_node_count++;
            Q_ASSERT( m_vector[i-1] < m_vector[i]);
        }
        if(m_vector[i].mp_subtree) {
            sub_node_count += m_vector[i].mp_subtree->m_total_nodes;
            prev_node_height = node_height;
            res = &m_vector[i]; //m_vector lower than anything on it's tree
            m_vector[i].mp_subtree->testTreeStructure(node_height, res);
            //Either its the first call or the heights are the same
            Q_ASSERT(prev_node_height ==-1 || (prev_node_height == node_height));
            Q_ASSERT((i == (m_count -1)) ||   *res < m_vector[i+1]);
        }
    }
    Q_ASSERT(sub_node_count == m_total_nodes);
    if(node_height ==-1){
        height = 0;
    } else {
        height = node_height;
    }
    if(is_leaf()) {
        lastBottomElem = &m_vector[m_count-1];
    } else {
        lastBottomElem = res;
    }

    return true;
}
#endif



 
// initialize static data at file scope
template<class KEY, class VALUE> Elem Nde::m_failure = Elem();
 
 
 
