#include "tree_struct_test.h"

//Now modified for posix
#include <QtGlobal>
#include <QString>
#include <time.h>
//#include <strstream>
#include <sstream>
#include <QDataStream>
#include "tree_struct_test.h"

#include "btree.h"
#include "btree_impl.h"
#include <QHash>
#include <QDebug>


//A "torture test" is useful because it can find corner cases in the B-tree routines...

#define ITEM_LIMIT 100000//000


const PQString ALL_INDEX("All_Index");
const PQString ODD_INDEX("Odd_Index");
const PQString EVEN_INDEX ("Even_Index");
const PQString MOD10_INDEX("Mod10_Index");

#define destroyTestPtr(arg) (MPointer<TestStruct >::deallocate(arg))


struct TestStruct {
    int val1;
    int val2;
    char foo[100];
    MPointer<TestStruct> prevPtr;
    TestStruct(int v=0){
        val1 = v;
        val2 = v+2;
    }
    bool matches(TestStruct* t){
        return val1 == t->val1 && (val2 = t->val2);
    }
    static QString typeName(){ return "TestStruct";}
};


template <class TYPE> bool operator==(const TestStruct& x, const TestStruct& y){
//    qint64 xoff = x.getOffset();
//    qint64 yoff = y.getOffset();
    //Q_ASSERT(xoff != yoff || (y.value()== y.value()));
    return x.val1 = y.val1 && (x.val1 == y.val2);
}

template <class TYPE> bool operator==(TestStruct& x, TestStruct& y){
    //    qint64 xoff = x.getOffset();
    //    qint64 yoff = y.getOffset();
    //Q_ASSERT(xoff != yoff || (y.value()== y.value()));
    return x.val1 = y.val1 && (x.val1 == y.val2);
}


void test_pointer_load_save(){
    typedef MPointer<TestStruct > TPointer;
    TPointer::createAllocator();
    vector< MPointer<TestStruct >  > nodeList;

    for(int i=0; i<ITEM_LIMIT;i++){
        TPointer::tryReallocateMem();
        TPointer t = TPointer((new(TPointer::allocate()) TestStruct(i)));
        TestStruct* tRawPtr = t.valuePtr();
        int v1 = tRawPtr->val1;
        Q_ASSERT(v1 == i);
        if(i>0){
            t->prevPtr = nodeList[i-1];
        }
        nodeList.push_back(t);
    }

    for(int i=0; i<ITEM_LIMIT;i++){
        Q_ASSERT(nodeList[i]->val1 == i);
        if(i>0){
            Q_ASSERT(nodeList[i]->prevPtr->val1 == i-1);
        }
    }

    for(int i=20; i<ITEM_LIMIT;i++){
        if(i==ITEM_LIMIT-1){
            int i=0;
        }
        TPointer::deallocate(nodeList[i].valuePtr());
    }
    TPointer::testLoadSave();

    for(int i=0; i<ITEM_LIMIT;i++){
        TPointer::tryReallocateMem();
        TPointer t = TPointer((new(TPointer::allocate()) TestStruct(i)));
        //TestStruct* tRawPtr = t.valuePtr();
        //int v1 = tRawPtr->val1;
        nodeList[i] = t;
        if(i>1){
            t->prevPtr = nodeList[i-2];
        }
    }
    TPointer::testLoadSave();

    for(int i=0; i<ITEM_LIMIT;i++){
        Q_ASSERT(nodeList[i]->val1 == i);
        if(i>1){
            Q_ASSERT(nodeList[i]->prevPtr->val1 == i-2);
        }
    }
    TPointer::testLoadSave();
    for(int i=0; i<ITEM_LIMIT;i++){
        TPointer t = nodeList[i];
        TestStruct* tRawPtr = t.valuePtr();
        int v1 = tRawPtr->val1;
        Q_ASSERT(v1 == i);
        //Q_ASSERT((nodeList[i])->val1=i);
    }
    TPointer::destroyAllocator();

}



void test_load_save(){

    QString forSprintf;
    QString str;

    vector<TestStringContainer> search_vect;
    search_vect.resize (ITEM_LIMIT);
    vector<long> itK;
    itK.resize(ITEM_LIMIT);

    { //block so index1 is closed
    qDebug() << "test load save";
    MultiTree<TestStringContainer, PQString> index1;
    Q_ASSERT(index1.count()==0);
    index1.addIndex(ALL_INDEX);
    index1.addIndex(ODD_INDEX);
    index1.addIndex(EVEN_INDEX);
    index1.addIndex(MOD10_INDEX);


//    long max_size = index1.max_size();
//    cout << "max index size: " << max_size;

    index1.testStruct();
    for (int i=0; i<ITEM_LIMIT; i++) {
        QString sstr;
        itK[i] = i;
        sstr = sstr.sprintf("%s%d", " key value ", i);
        TestStringContainer ts(sstr);
        search_vect[i] = ts;
        TestStringContainer t = search_vect[i];
        index1.addEntry(itK[i],t,ALL_INDEX);
//        index.add(itK[i/10],t,MOD10_INDEX);
        if( i % 2 == 0){ index1.addEntry(itK[i],t,EVEN_INDEX); }
        if( i % 2 == 1){ index1.addEntry(itK[i],t,ODD_INDEX); }
        index1.testStruct();
    }
    }//end block

    MultiTree<TestStringContainer> index2(true);

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer r = index2.find(itK[i],ALL_INDEX);
        Q_ASSERT( search_vect[i] == r);
        if( i % 2 == 0){
            r = index2.find(itK[i],EVEN_INDEX);
            Q_ASSERT( search_vect[i] == r);
        }
        if( i % 2 == 1){
            r = index2.find(itK[i],ODD_INDEX);
            Q_ASSERT( search_vect[i] ==r);
        }

    }
    index2.testStruct();

    //Revalue every seventh item
    //This creates DUPLICATE values for some KEYS, which the thing
    //SHOULD handle gracefully...
    for (long i=0; i<ITEM_LIMIT; i+=7) {
        long new_key = i*1000;
        index2.addEntry(Tuple(new_key), search_vect[i], ALL_INDEX);
        itK[i] = new_key;
    }
    index2.testStruct();

    //Remove every third item
    for (int i=0; i<ITEM_LIMIT; i+=3) {
        bool b = index2.removeObject(search_vect[i]);
        Q_ASSERT(b);
    }
    index2.testStruct();

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer t = index2.find(itK[i],ALL_INDEX);
        if(i %3 == 0){
            Q_ASSERT(t.isEmpty());
        } else {
            if(i % 7 !=0)
                Q_ASSERT( t == search_vect[i] );
        }

        if( i % 2 == 0){
            if(i %3 == 0){
                Q_ASSERT( index2.find(i,EVEN_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index2.find(i,EVEN_INDEX)); }
        }
        if( i % 2 == 1){
            if(i %3 == 0){
                Q_ASSERT( index2.find(i,ODD_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index2.find(i,ODD_INDEX)); }
        }
    }
    index2.testStruct();

}

void test_mem1(){
    // the main function is just some code to test the b-tree with the appropriate manipulations
    // between testing tree integrity, testing that result ascend and testing counts are correct,
    // the correctness of the various algorithms should be assured.

    Node<Tuple,Tuple,TestStruct>::m_failure.invalidate();
    Node<Tuple,Tuple,TestStruct>::m_failure.m_key = Tuple();
    Element<Tuple,Tuple,TestStruct> elem;

    LBTree<Tuple,Tuple,TestStruct> tracker;  // maintains a pointer to the current root of the b-tree
    tracker.createBase();

    vector<Tuple> search_vect;
    // prepare the key Tuples
    search_vect.resize (ITEM_LIMIT);
    const int STEP = 2;
    int j;
    for (int i=0; i<ITEM_LIMIT; i++) {
        j = (i/STEP)*STEP; //get up in steps...
        search_vect.push_back(Tuple(j,j+10));
    }
    QString s;
    cout << "finished preparing key strings\n";
    timespec begin_Spec;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin_Spec);
    MPointer< Node<Tuple, Tuple, TestStruct> > base_node;
    for (unsigned int i=0; i<ITEM_LIMIT; i++) {
        elem.m_key = Tuple((long)i);
        elem.m_payload = search_vect[i];
        tracker.get_base()->tree_insert(elem);
        base_node = tracker.get_base();
        unsigned int nodes_found = base_node->node_size();
        Q_ASSERT( nodes_found == i+1);
    }

    int elem_size = sizeof(Element<Tuple,Tuple,TestStruct>);
    int node_size = sizeof(Node<Tuple,Tuple,TestStruct>);
    float badratio = 0.40;

    long node_cout = tracker.get_base()->testCountBadNodes(1);
    long bad_nodes = tracker.get_base()->testCountBadNodes(badratio);
    long all_mem = tracker.get_base()->testAllocatedSize();
    long node_count = tracker.get_base()->node_size();

    long node_aloc = node_cout * node_size;
    long elem_aloc = node_count *  elem_size;

    cout << "Total items: " << ITEM_LIMIT;
    cout << "sizes: allocated element:" <<  node_count << " * " <<  elem_size << "="  << elem_aloc;
    cout << "\nnodes filled to less than "<< badratio << ": " <<  bad_nodes;
    cout << "\nall nodes, supposed:" <<  node_cout;
    cout << "\nmemory in nodes:" <<  node_cout * node_size;
    cout << "\nexplicitly allocated memory, supposedly:" <<  all_mem;
    cout << "\nun used memory, supposedly:" <<  all_mem - (node_aloc + elem_aloc);

}

void test_mem2(){
    MultiTree<TestStringContainer> index;
    index.clear();
    index.addIndex(ALL_INDEX);
    index.addIndex(ODD_INDEX);
    index.addIndex(EVEN_INDEX);
    index.addIndex(MOD10_INDEX);

    vector<TestStringContainer> search_vect;
    search_vect.resize (ITEM_LIMIT);
    vector<long> itK;
    itK.resize(ITEM_LIMIT);

//    long max_size = index.max_size();
//    cout << "max index size: " << max_size;


    for (int i=0; i<ITEM_LIMIT; i++) {
        QString sstr;
        itK[i] = i;
        sstr = sstr.sprintf("%s%d", " key value ", i);
        TestStringContainer ts(sstr);
        search_vect[i] = ts;
        TestStringContainer t = search_vect[i];
        index.addEntry(itK[i],t,ALL_INDEX);
        if( i % 2 == 0){ index.addEntry(itK[i],t,EVEN_INDEX); }
        if( i % 2 == 1){ index.addEntry(itK[i],t,ODD_INDEX); }
    }
    index.testStruct();


}

void test3(){
#ifdef FILEDB_IMPL
    //Our first hash exists on
    {
    KCHash<TestStringContainer> h("test3");
    h.clear();

    Tuple t1("12#0");
    Q_ASSERT(t1.getFirst() ==12);
    Q_ASSERT(t1.getSecond() ==0);

    Tuple t2("12#1");
    Q_ASSERT(t2.getFirst() ==12);
    Q_ASSERT(t2.getSecond() ==1);

    TestStringContainer zeb("zebra donky");
    h.set(t1,zeb);
    TestStringContainer ant("antler hornblower");
    h.set(t2,ant);

    TestStringContainer res1 = h.get(t1);
    TestStringContainer res2 = h.get(t2);

    Q_ASSERT(res1 =="zebra donky");
    Q_ASSERT(res2 =="antler hornblower");

    }
    { //tests persistance
    KCHash<TestStringContainer> h("test3");

    Tuple t1("12#0");
    Q_ASSERT(t1.getFirst() ==12);
    Q_ASSERT(t1.getSecond() ==0);

    Tuple t2("12#1");
    Q_ASSERT(t2.getFirst() ==12);
    Q_ASSERT(t2.getSecond() ==1);

    TestStringContainer res1 = h.get(t1);
    TestStringContainer res2 = h.get(t2);

    Q_ASSERT(res1 =="zebra donky");
    Q_ASSERT(res2 =="antler hornblower");

    h.remove(t1);
    res1 = h.get(t1);
    Q_ASSERT(res1.isEmpty());
    res2 = h.get(t2);
    Q_ASSERT(res2 =="antler hornblower");
    h.remove(t2);
    res2 = h.get(t2);
    Q_ASSERT(res2.isEmpty());

    h.clear();

    }
    cout << "test3 finished";
#endif
}

void test4(){
    Bucket<QString, TestStringContainer> b("test4_1");

    TestStringContainer tsc("test4_1");
    Tuple t1 = b.insertOrReplace(tsc);
    Tuple t2 = b.insertOrReplace((tsc));

    Q_ASSERT(b.get(t1) == tsc);
    Q_ASSERT(t1 == t2);
    Q_ASSERT(!t1.isEmpty());
    Q_ASSERT(t1 == b.getExistingTupleId(tsc));

    TestStringContainer tsc2("test4_2");
    Tuple t3 = b.insertOrReplace(tsc2);
    Tuple t4 = b.insertOrReplace(tsc2);

    Q_ASSERT(b.get(t3) == tsc2);
    Q_ASSERT(t3 == t4);
    Q_ASSERT(t3 != t2);
    Q_ASSERT(!t3.isEmpty());
    Q_ASSERT(t3 == b.getExistingTupleId(tsc2));


}



void test2(){
    MultiTree<TestStringContainer> index;
    index.clear();
    index.addIndex(ALL_INDEX);
    index.addIndex(ODD_INDEX);
    index.addIndex(EVEN_INDEX);
    index.addIndex(MOD10_INDEX);

    QString forSprintf;
    QString str;

    vector<TestStringContainer> search_vect;
    search_vect.resize (ITEM_LIMIT);
    vector<long> itK;
    itK.resize(ITEM_LIMIT);

    index.testStruct();
    for (int i=0; i<ITEM_LIMIT; i++) {
        QString sstr;
        itK[i] = i;
        sstr = sstr.sprintf("%s%d", " key value ", i);
        TestStringContainer ts(sstr);
        search_vect[i] = ts;
        TestStringContainer t = search_vect[i];
        //if(i>6200 && i < 6401){ index.testStruct();}
//        if(i>6800 && i < 6850){ index.testStruct();}

        index.addEntry(itK[i],t,ALL_INDEX);
        if( i % 2 == 0){ index.addEntry(itK[i],t,EVEN_INDEX); }
        if( i % 2 == 1){ index.addEntry(itK[i],t,ODD_INDEX); }
    }

    index.testStruct();

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer r = index.find(itK[i],ALL_INDEX);
        Q_ASSERT( search_vect[i] == r);
        if( i % 2 == 0){
            r = index.find(itK[i],EVEN_INDEX);
            Q_ASSERT( search_vect[i] == r);
        }
        if( i % 2 == 1){
            r = index.find(itK[i],ODD_INDEX);
            Q_ASSERT( search_vect[i] ==r);
        }

    }

    MPointer<TestStringContainer>::testLoadSave();

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer r = index.find(itK[i],ALL_INDEX);
        Q_ASSERT( search_vect[i] == r);
        if( i % 2 == 0){
            r = index.find(itK[i],EVEN_INDEX);
            Q_ASSERT( search_vect[i] == r);
        }
        if( i % 2 == 1){
            r = index.find(itK[i],ODD_INDEX);
            Q_ASSERT( search_vect[i] ==r);
        }

    }
    index.testStruct();

    MPointer<TestStringContainer>::testLoadSave();
    //Revalue every seventh item
    //This creates DUPLICATE values for some KEYS, which the thing
    //SHOULD handle gracefully...
    for (long i=0; i<ITEM_LIMIT; i+=7) {
        long new_key = i*1000;
        if(i==28){
            int foo =0;
            foo=foo+1;
        }
        index.addEntry(Tuple(new_key), search_vect[i], ALL_INDEX);
        itK[i] = new_key;
    }
    index.testStruct();

    //Remove every third item
    for (int i=0; i<ITEM_LIMIT; i+=3) {
        bool b = index.removeObject(search_vect[i]);
        Q_ASSERT(b);
    }
    index.testStruct();

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer t = index.find(itK[i],ALL_INDEX);
        if(i %3 == 0){
            Q_ASSERT(t.isEmpty());
        } else {
            if(i % 7 !=0)
                Q_ASSERT( t == search_vect[i] );
        }

        if( i % 2 == 0){
            if(i %3 == 0){
                Q_ASSERT( index.find(i,EVEN_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index.find(i,EVEN_INDEX)); }
        }
        if( i % 2 == 1){
            if(i %3 == 0){
                Q_ASSERT( index.find(i,ODD_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index.find(i,ODD_INDEX)); }
        }
    }
    index.testStruct();
}

//Call this AFTER test2
void test2_quickRestore(){
    MultiTree<TestStringContainer> index(true);

    vector<TestStringContainer> search_vect;
    search_vect.resize (ITEM_LIMIT);
    vector<long> itK;
    itK.resize(ITEM_LIMIT);

    index.testStruct();

    for (int i=0; i<ITEM_LIMIT; i++) {
        QString sstr;
        itK[i] = i;
        sstr = sstr.sprintf("%s%d", " key value ", i);
        TestStringContainer ts(sstr);
        search_vect[i] = ts;
    }

    for (int i=0; i<ITEM_LIMIT; i++) {
        TestStringContainer t = index.find(itK[i],ALL_INDEX);
        if(i %3 == 0){
            Q_ASSERT(t.isEmpty());
        } else {
            if(i % 7 !=0)
                Q_ASSERT( t == search_vect[i] );
        }

        if( i % 2 == 0){
            if(i %3 == 0){
                Q_ASSERT( index.find(i,EVEN_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index.find(i,EVEN_INDEX)); }
        }
        if( i % 2 == 1){
            if(i %3 == 0){
                Q_ASSERT( index.find(i,ODD_INDEX).isEmpty());
            } else { Q_ASSERT( search_vect[i] == index.find(i,ODD_INDEX)); }
        }
    }
    index.testStruct();
}


void test1()
{
    // the main function is just some code to test the b-tree with the appropriate manipulations
    // between testing tree integrity, testing that result ascend and testing counts are correct,
    // the correctness of the various algorithms should be assured.

    Node<string,string,TestStruct>::m_failure.invalidate();
    Node<string,string,TestStruct>::m_failure.m_key = "";
    Element<string,string,TestStruct> elem;

    LBTree<string,string,TestStruct> tracker;  // maintains a pointer to the current root of the b-tree
    //Node<string,string,TestStruct>* root_ptr = new Node<string,string,TestStruct>(tracker);
    tracker.createBase();

    vector<string> search_vect;
    // prepare the key strings
    search_vect.resize (ITEM_LIMIT);
	int search_count = 0;
    const int STEP = 2;
    int j;
    for (int i=0; i<ITEM_LIMIT; i++) {
        j = (i/STEP)*STEP; //get up in steps...
//		strstream stm;
        stringstream  stm;
        stm << j;
		stm >> search_vect[search_count++];
	}
    QString s;
	cout << "finished preparing key strings\n";
	timespec begin_Spec;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin_Spec);
    MPointer< Node<string,string,TestStruct> > base_node;
    for (unsigned int i=0; i<ITEM_LIMIT; i++) {
        elem.m_key = search_vect[i];
        elem.m_payload = search_vect[i]+" hi you";
        tracker.get_base()->tree_insert(elem);
        base_node = tracker.get_base();
        // base_node->testTreeStructure(dummy_height);
        unsigned int nodes_found = base_node->node_size();
        Q_ASSERT( nodes_found == i+1);
    }
    timespec end_Spec;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_Spec);
    //Now we're always returning nsecs, if the clock isn't accurate, it's approximate
    unsigned long total = (end_Spec.tv_nsec - begin_Spec.tv_nsec);
    cout << "total millisec for adding 100000 elements: " << total << endl;
    // base_node = tracker.get_base();
    // Elem* dummy=0;
    // base_node->testTreeStructure(dummy_height, dummy);
    cout << "finished inserting elements\n";
    MPointer< Node<string,string,TestStruct> > last;
    Element<string,string,TestStruct> desired;
    for (int i=ITEM_LIMIT-1; i>=0; i--) {
        desired.m_key = search_vect[i];
        desired.m_payload = (search_vect[i]+" hi you").c_str();
        Element<string,string,TestStruct>& result = tracker.get_base()->search (desired, last);
        QString f = result.m_payload.c_str();
        QString s = (search_vect[i]+" hi you").c_str();
        Q_ASSERT(f == s);
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_Spec);
    //Now we're always returning nsecs, if the clock isn't accurate, it's approximate
    total = (end_Spec.tv_nsec - begin_Spec.tv_nsec);
    cout << "total millisec for searching 100000 elements: " << total << endl;

    for (int i=ITEM_LIMIT-1; i>=0; i--) {
        Element<string,string,TestStruct> desired;
        desired.m_key = search_vect[i] + ".5";
        int indexGL = 0;
        MPointer< Node<string,string,TestStruct> > result = tracker.get_base()->searchGL (desired, indexGL);
        if(result){
            QString f = result->at(indexGL).m_payload.c_str();
            QString s = (search_vect[i]+" hi you").c_str();
            Q_ASSERT(f == s);
        }
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_Spec);
    //Now we're always returning nsecs, if the clock isn't accurate, it's approximate
    total = (end_Spec.tv_nsec - begin_Spec.tv_nsec);
    cout << "total millisec for searching 100000 elements: " << total << endl;

    //QString order DIFFERENTLY than the numbers...
    //
    QString g = "999999"; //Highest string
    QString f;
    //tracker.get_base()->testTreeStructure(dummy_height, d);
    for (int i=ITEM_LIMIT-1; i>=0; i--) {
        Element<string,string,TestStruct> target;
		target.m_key = search_vect[i];
        j = (i/STEP)*STEP; //get up in steps...
        Element<string,string,TestStruct>& result = tracker.get_base()->ordinalSearch(i+1);
        int item_pos = tracker.get_position(result.m_key);
        Q_ASSERT(item_pos == j+1);

        f = result.m_payload.c_str();
        Q_ASSERT(f.length() >0);
        //QString e = search_vect[i]+" hi you";
        Q_ASSERT(!(f > g));
        g = f;
    }

    std::vector<string> v = tracker.get_range(std::string("0"), std::string("99999999"));
    g=f ="";
    for (int i=0; i < 100; i++) {
        g = v[i].c_str();
        Q_ASSERT(f < g);
        g = f;
    }

    //By counting and pigeon-hole, get_range must function correctly
    const int increment = 1000;
    Element<string,string,TestStruct> prev = tracker.get_base()->ordinalSearch(1);
    for (int i=increment ; i< ITEM_LIMIT-1; i+=increment) {
        Element<string,string,TestStruct> result = tracker.get_base()->ordinalSearch(i+1);
        string prev_key = prev.m_key;
        std::vector<string> v = tracker.get_range(prev_key, result.m_key);
        int result_size = v.size();
        Q_ASSERT( result_size == (increment));
        prev = result;
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_Spec);
    //Now we're always returning nsecs, if the clock isn't accurate, it's approximate
    total = (end_Spec.tv_nsec - begin_Spec.tv_nsec);
    cout << "total millisec for ordinal find 100000 elements: " << total << endl;
    cout << "finished ordinal search for elements\n";

    tracker.get_base()->testStruct();
    //Delete unique even STEP, from the bottom
    for (int i=0; i< ITEM_LIMIT; i+=2*STEP) {
//        Element<string,string,TestStruct> target;
//        target.m_key = search_vect[i];
//        base_node =  tracker.get_base();
//        //base_node->testStruct();
//        base_node->delete_element (target);
        bool sucess = tracker.delete_all_at(search_vect[i]);
        Q_ASSERT(sucess);
        desired.m_key = search_vect[i];
        Element<string,string,TestStruct>& result = tracker.get_base()->search (desired, last);
        Q_ASSERT( (result.matches( Node<string,string,TestStruct>::m_failure)));

    }
    //Now, odds from the top
    for (int i=(STEP*ITEM_LIMIT)/STEP - STEP; i>=STEP ; i-=2*STEP) {
//        Element<string,string,TestStruct> target;
//        target.m_key = search_vect[i];
//        base_node =  tracker.get_base();
//        //base_node->testStruct();
//        base_node->delete_element (target);
        bool sucess = tracker.delete_all_at(search_vect[i]);
        Q_ASSERT(sucess);
        desired.m_key = search_vect[i];
        qDebug() << "index:" << i;
        qDebug() << "Deleted Key:" << search_vect[i].c_str();
        Element<string,string,TestStruct>& result = tracker.get_base()->search (desired, last);
        Q_ASSERT( result.matches(Node<string,string,TestStruct>::m_failure));
        //tracker.get_base()->testStruct();

    }
    Q_ASSERT(tracker.size()==0);

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_Spec);
	//Now we're always returning nsecs, if the clock isn't accurate, it's approximate
    total = (end_Spec.tv_nsec - begin_Spec.tv_nsec);
    cout << "total millisec for 100000 elements: " <<  total << endl;
	cout << "after deletion" << endl;
    //tracker.get_base()->dump();
    //getchar();
    return;
}

