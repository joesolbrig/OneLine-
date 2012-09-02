#ifndef MMAP_POINTER_H
#define MMAP_POINTER_H

#include <new>
#include <QFile>
#include <QLinkedList>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
//This will be a linked list with memory - to avoid issues
//of c++ adding junk, I'll deal with array, qint64 and Type* only

template <class TYPE> class InternalAllocator;
template <class TYPE> class MPointer;

const int DIRTY_HIDDEN_INDEX(10);
const int UNALLOCATED_OFFSET(11);
const int ALLOCATED_NODES_OFFSET(12);
const int ALLOCATED_MEM_OFFSET(13);
const int TREE_NODE_BASE_OFFSET(14);

const int MIN_INDEX(20);

//const int SIZE_HIDDEN_INDEX(1);
const int NULL_PTR_INDEX(-1);
const int INVALID_INDEX(-2);


const int PTR_TEST_RECUR(3);

template <class TYPE> struct MNode;

template <class TYPE> class Allocator {
    friend class InternalAllocator<TYPE>;

public:
    static InternalAllocator<TYPE>* m_innerAllocator;
    static QString m_path;

    Allocator(){
        //MPointer<TYPE>::setBase(m_innerAllocator->base());
        create();
    }

    void create(){
        if(!m_innerAllocator){
            m_innerAllocator = new InternalAllocator<TYPE>();
        }
    }

    bool isClear(){
        return (m_innerAllocator->i_topIndexMemItems == MIN_INDEX);
    }

    TYPE* allocateMem(){
        return m_innerAllocator->allocate();
    }

    void tryReallocateMem(){
        if(m_innerAllocator->shouldReallocate()){
            //MPointer<TYPE>::testPtr(m_innerAllocator->i_topAlocatdLinkedList,10);
            testMemDown(MPointer<TYPE>(m_innerAllocator->i_topAlocatdLinkedList,true));

            qint64 bottumUnAlocdLinkList =m_innerAllocator->i_bottumUnAlocdLinkList;
            qint64 topAlocatdLinkedList =m_innerAllocator->i_topAlocatdLinkedList;
            qint64 topIndexMemItems =m_innerAllocator->i_topIndexMemItems;
            qint64 memSize = m_innerAllocator->memLength();
            qint64 pageCount= m_innerAllocator->m_pageCount;

            qint64 newPageCount= pageCount*(1.5) + 10;
            MNode<TYPE>* oldBase = m_innerAllocator->base();
            MNode<TYPE>* copyBuffer = (MNode<TYPE>*)malloc((topIndexMemItems+4)*sizeof(MNode<TYPE>)+10);
            copyMNodes(copyBuffer, oldBase, topIndexMemItems+2);

            delete m_innerAllocator;
            m_innerAllocator = new InternalAllocator<TYPE>(newPageCount, true);//override written size!
            qint64 newMemLength = m_innerAllocator->memLength();
            Q_ASSERT(newMemLength > memSize);

            MNode<TYPE>* newBase = m_innerAllocator->base();
            //memcpy(newBase,m_copyBuffer, memSize);
            //            qint64 chunks =(memSize/sizeof(TYPE));
            //            Q_ASSERT(chunks> topIndexMemItems);
            copyMNodes(newBase,copyBuffer, topIndexMemItems+2);
            free(copyBuffer);
            m_innerAllocator->setMemLength(newMemLength);
            m_innerAllocator->m_pageCount = newPageCount;
            m_innerAllocator->i_bottumUnAlocdLinkList = bottumUnAlocdLinkList;
            m_innerAllocator->i_topAlocatdLinkedList = topAlocatdLinkedList;
            m_innerAllocator->i_topIndexMemItems = topIndexMemItems;
            msync(m_innerAllocator->base(),m_innerAllocator->memLength(),MS_SYNC);
            //MPointer<TYPE>::testPtr(m_innerAllocator->i_topAlocatdLinkedList,10);
            //testMemDown(MPointer<TYPE>(m_innerAllocator->i_topAlocatdLinkedList,true));
            TYPE* t = m_innerAllocator->allocate();
            MPointer<TYPE> tPtr(t);
            testMemDown((tPtr));
        }
    }

    void copyMNodes(MNode<TYPE>* target, MNode<TYPE>* source, qint64 length){
        for(qint64 i=0; i< length; i++){
            copyMNode(target+i, source+i); }
    }

    //Test just in case we're in weird part of memory...
    void copyMNode(MNode<TYPE>* target, MNode<TYPE>* source){
        for(qint64 i=0; i< sizeof(TYPE); i++){
            target->T[i] = source->T[i];
            Q_ASSERT(target->T[i] == source->T[i]);
        }
        target->m_prev = source->m_prev;
        Q_ASSERT(target->m_prev == source->m_prev);
        target->m_next = source->m_next;
        Q_ASSERT(target->m_next == source->m_next);
    }

    void testMemDown(MPointer<TYPE> p){
        while(p.getOffset() >=MIN_INDEX){
            //MPointer<TYPE>::testPtr(p);
            qint64 prev = m_innerAllocator->prevPos(p.getOffset());
            MPointer<TYPE> n(prev,true);
            p=n;
        }

    }

    ~Allocator(){
        if(m_innerAllocator){
            delete m_innerAllocator;
            m_innerAllocator = 0;
        }
    }
    bool testLoadSave(){
        //testMemDown(MPointer<TYPE>(m_innerAllocator->i_topAlocatdLinkedList));
        delete m_innerAllocator;
        m_innerAllocator = new InternalAllocator<TYPE>();
        //MPointer<TYPE>::testPtr(m_innerAllocator->i_topAlocatdLinkedList,10);
        testMemDown(MPointer<TYPE>(m_innerAllocator->i_topAlocatdLinkedList,true));
        return true;
    }

    bool clear(){
        delete m_innerAllocator;
        m_innerAllocator =0;
        m_innerAllocator->clearFile();
        create();
        return true;
    }

    void deallocateMem(MPointer<TYPE> ptr){
        m_innerAllocator->deallocate(ptr);
    }

    MNode<TYPE>* getBase() const{
        return m_innerAllocator->base();
    }

};

template <class TYPE> struct MNode {
    char T[sizeof(TYPE)];
    qint64 m_prev;
    qint64 m_next;
};


template <class TYPE> class InternalAllocator {
    friend class MPointer<TYPE>;
    friend class Allocator<TYPE>;

private:
    qint64 unused;
    qint64 m_memLength;
    MNode<TYPE>* topMemPtr(){ return m_baseMemPtr + m_memLength; }
    void setMemLength(qint64 l){
        m_memLength = l;
    }

protected:
    //Memory Structure
    qint64 i_bottumUnAlocdLinkList;
    qint64 i_topAlocatdLinkedList;
    qint64 i_topIndexMemItems;
    MNode<TYPE>* m_baseMemPtr;

public:

    qint64 m_pageCount;
    qint64 memLength(){ return m_memLength; }
    MNode<TYPE>* base() const{ return m_baseMemPtr; }

    //I'm either manipulated existing linked-list entries or writing further up
    //the memory mapped file
    //
    //Our linklist pointers live beyond memory area devoted to TYPE data


    InternalAllocator(int minSize=DEFAULTE_PAGE_COUNT, bool override=false){
        m_pageCount = minSize;
        int pageSize = (int)sysconf(_SC_PAGESIZE);
        Q_ASSERT( pageSize > 0);

        QFile file(fileName());
        bool reset;
        file.open(QIODevice::ReadWrite);
        qint64 readSize = readFileSize(&file);
        m_memLength = m_pageCount*pageSize;
        if(override || readSize < m_pageCount){
            QString s(m_memLength+100, 'X');
            file.write(s.toAscii());
            file.flush();
            reset = true;
        } else {
            m_pageCount = readSize;
            m_memLength = m_pageCount*pageSize;
            reset = false;
        }
        Q_ASSERT(file.isOpen());
        char* ptr = (char*)mmap(NULL,
                              m_memLength,
                              PROT_READ|PROT_WRITE,
                              MAP_FILE|MAP_SHARED,
                              file.handle(), 0);
        file.close();
        m_baseMemPtr = (MNode<TYPE>*)ptr;
        Q_ASSERT(m_baseMemPtr != MAP_FAILED);


        if(reset || (!loadAllocatorConfig() )){
            resetValues();
        }
        Q_ASSERT(canAllocate());
        //Clear and check our memory...
//#  ifndef QT_NO_DEBUG
//        for(int i=i_topIndexMemItems*sizeof(MNode<TYPE>);i<m_memLength-1; i++) {
//            ptr[i]='\0';
//            Q_ASSERT(ptr[i]=='\0');
//        }
//        Q_ASSERT(m_baseMemPtr != MAP_FAILED);
//
//#  endif
}

    void clearFile(){
        QFile file(fileName());
        file.open(QIODevice::ReadWrite);
        QString s(1000, '\0');
        file.write(s.toAscii());
        file.flush();
        file.close();
        file.open(QIODevice::Truncate);
        file.close();
    }

    //The system should free the resources but let's try anyway...
    ~InternalAllocator(){
        qDebug() << "Mmap pointer: saving allocator";
        setSysValue(UNALLOCATED_OFFSET, i_bottumUnAlocdLinkList);
        setSysValue(ALLOCATED_NODES_OFFSET, i_topAlocatdLinkedList);
        setSysValue(ALLOCATED_MEM_OFFSET, i_topIndexMemItems);
        setSysValue(DIRTY_HIDDEN_INDEX, 0);
        Q_ASSERT(getSysValue(DIRTY_HIDDEN_INDEX)==0);
        msync(m_baseMemPtr,memLength(),MS_SYNC);
        int err = munmap(m_baseMemPtr,memLength());
        if(err!=0){
            qDebug() << strerror(errno);
        }
        QString fn = fileName();
        QFile file(fn);
        qDebug() << "trying to close" << fn;
        Q_ASSERT(file.exists());
        file.open(QIODevice::ReadWrite);
        Q_ASSERT(file.isOpen());
        char data[sizeof(qint64)+4];
        sprintf(data,"%lld",m_pageCount);
        data[sizeof(qint64)+4]=0;
        qint64 res;
        sscanf(data,"%lld", &res);
        qDebug() << "res: " << res << "m_pageCount: " << m_pageCount << "data" << data;
        Q_ASSERT(res== m_pageCount);
        file.write(data,sizeof(qint64)+2);
        Q_ASSERT(readFileSize(&file) == m_pageCount);
        file.close();
    }

    qint64 readFileSize(QFile* file){
        bool suc = file->reset();
        if(!suc){
            qDebug() << strerror(errno);
            Q_ASSERT(false);
        }
        char data[sizeof(qint64)+4];
        if(!file->read(data, sizeof(qint64))){
            return 0;
        }
        long long res;
        sscanf(data,"%lld", &res);
//        bool ok=false;
//        data[sizeof(qint64)]=0;
//        qint64 requestedSize = QString(data).toLongLong(&ok);
//        if(!ok){ return 0;}
//        return requestedSize;
        return res;

    }

    qint64&  nextPos(qint64 x) {
        return (m_baseMemPtr + x)->m_next;
    }

    qint64& prevPos(qint64 x){
        return (m_baseMemPtr + x)->m_prev;
    }
    
    bool canAllocate(){
        Q_ASSERT(((i_topIndexMemItems*sizeof(MNode<TYPE>)) ) < memLength());
        Q_ASSERT(((i_topAlocatdLinkedList*sizeof(MNode<TYPE>)) ) < memLength());
        Q_ASSERT(((i_topIndexMemItems*sizeof(MNode<TYPE>)) ) < memLength());

        return ((i_topIndexMemItems*sizeof(MNode<TYPE>))) < memLength();
    }

    bool shouldReallocate(){
        Q_ASSERT(((i_topIndexMemItems*sizeof(MNode<TYPE>)) ) <= memLength());
        Q_ASSERT(((i_topAlocatdLinkedList*sizeof(MNode<TYPE>)) ) <= memLength());
        Q_ASSERT(((i_topIndexMemItems*sizeof(MNode<TYPE>)) ) <= memLength());

        return ((i_topIndexMemItems*sizeof(MNode<TYPE>))*2 + 10) > memLength();
    }

    TYPE* allocate(){
        Q_ASSERT(canAllocate());
        qint64 index;

        if(i_bottumUnAlocdLinkList>=MIN_INDEX){
            index = i_bottumUnAlocdLinkList;
            qint64 nextUnAlloc = nextPos(i_bottumUnAlocdLinkList);
            if(nextUnAlloc>=MIN_INDEX){ prevPos(nextUnAlloc) = NULL_PTR_INDEX; }
            i_bottumUnAlocdLinkList = nextUnAlloc;

        } else {
            index = i_topIndexMemItems;
            i_topIndexMemItems++;
        }
        nextPos(index) = NULL_PTR_INDEX;
        prevPos(index)=i_topAlocatdLinkedList;
        if(i_topAlocatdLinkedList >=MIN_INDEX){
            Q_ASSERT(i_topAlocatdLinkedList == MIN_INDEX || nextPos(i_topAlocatdLinkedList)==NULL_PTR_INDEX);
            Q_ASSERT((i_topAlocatdLinkedList)!=index);
            nextPos(i_topAlocatdLinkedList) = index;
        }
        qint64& prev = prevPos(index);
        if(prev>=MIN_INDEX){ nextPos(prev)=index; }
        i_topAlocatdLinkedList = index;
        TYPE* res = (TYPE*)(m_baseMemPtr + index +offsetof(MNode<TYPE>,T));
        //MPointer<TYPE>::testPtr(index);
//        if(i_bottumUnAlocdLinkList>=MIN_INDEX){
//            MPointer<TYPE>::testPtr(i_bottumUnAlocdLinkList);
//        }
        return res;
    }

    void deallocate(MPointer<TYPE> ptr){
        qint64 index = ptr.i_offset;
        Q_ASSERT(index >=MIN_INDEX);
        qint64 prevI = prevPos(index);
        qint64 nextI = nextPos(index);
        if(prevI>=MIN_INDEX){
            qint64 k = nextPos(prevI);
            Q_ASSERT(k==index);
            nextPos(prevI)= nextI;
        }
        if(nextI>=MIN_INDEX){
            Q_ASSERT(prevPos(nextI)==index);
            prevPos(nextI) = prevI;
        }
        if(i_topAlocatdLinkedList==index){
            i_topAlocatdLinkedList = prevI;
            nextPos(prevI) = NULL_PTR_INDEX;
        }
        if(i_bottumUnAlocdLinkList>=MIN_INDEX){
            prevPos(i_bottumUnAlocdLinkList) = index;
        }
        prevPos(index) = NULL_PTR_INDEX;
        nextPos(index) = i_bottumUnAlocdLinkList;
        i_bottumUnAlocdLinkList=index;
        Q_ASSERT(nextPos(i_topAlocatdLinkedList)== NULL_PTR_INDEX);
        //MPointer<TYPE>::testPtr(index);
//        if(i_topIndexMemItems>=MIN_INDEX){
//            MPointer<TYPE>::testPtr(i_topIndexMemItems);
//        }
//        if(prevI>=MIN_INDEX){
//            MPointer<TYPE>::testPtr(prevI);
//        }
//        if(nextI>=MIN_INDEX){
//            MPointer<TYPE>::testPtr(nextI);
//        }
    }


    static QString fileName(){
        if(!Allocator<TYPE >::m_path.isEmpty()){
            return Allocator<TYPE >::m_path + "/" + QString("IndexMemMap_") + TYPE::typeName() + ".mmp";
        }
        return QString("IndexMemMap_") + TYPE::typeName() + ".mmp";
    }


    bool loadAllocatorConfig(){
        i_bottumUnAlocdLinkList=getSysValue(UNALLOCATED_OFFSET);
        i_topAlocatdLinkedList=getSysValue(ALLOCATED_NODES_OFFSET);
        i_topIndexMemItems=getSysValue(ALLOCATED_MEM_OFFSET);
        qint64 dirty =getSysValue(DIRTY_HIDDEN_INDEX);
        setSysValue(DIRTY_HIDDEN_INDEX, 1);
        return(dirty==0);
    }

    void resetValues(){
        i_bottumUnAlocdLinkList=NULL_PTR_INDEX;
        i_topAlocatdLinkedList=NULL_PTR_INDEX;
        i_topIndexMemItems=MIN_INDEX;
    }

    void setSysValue(qint64 privateOffset, qint64 val){
        Q_ASSERT(privateOffset < MIN_INDEX);
        Q_ASSERT(privateOffset > NULL_PTR_INDEX);
        *((qint64*)(m_baseMemPtr+privateOffset + offsetof(MNode<TYPE>,T))) = val;
        Q_ASSERT(*((qint64*)(m_baseMemPtr+privateOffset + offsetof(MNode<TYPE>,T)))==val);
        Q_ASSERT(getSysValue(privateOffset)==val);

    }

    qint64 getSysValue(qint64 privateOffset){
        Q_ASSERT(privateOffset < MIN_INDEX);
        Q_ASSERT(privateOffset > NULL_PTR_INDEX);
        return *((qint64*)(m_baseMemPtr+privateOffset + offsetof(MNode<TYPE>,T)));
    }


};

#include <QPointer>
template <class TYPE> class MPointer {
    friend class InternalAllocator<TYPE>;

    qint64 i_offset;

    static Allocator<TYPE >* the_allocator;
public:

    static bool isLoaded(){
        return (the_allocator!=0 && the_allocator->m_innerAllocator);
    }

    static void setPath(QString path=""){
        if(!path.isEmpty()){
            Allocator<TYPE >::m_path = path;

        }
    }

    static void createAllocator(){
        if(the_allocator){
            destroyAllocator();
        }
        the_allocator = new Allocator<TYPE>();
    }

    static bool restoreAllocator(){
        if(the_allocator){
            return true;
        } else {
            the_allocator = new Allocator<TYPE>();
            if(the_allocator->isClear()){
                return false;
            } else {
                return true;
            }
        }
    }

    static void sync(){
    }

    static void destroyAllocator(){
        if(the_allocator){
            delete the_allocator;
            the_allocator=0;
        }
    }

    static void clear(){
        Q_ASSERT(the_allocator);
        the_allocator->clear();
    }

    static void tryReallocateMem() {
        the_allocator->tryReallocateMem();
    }

    static void* allocate() {
        return the_allocator->allocateMem();
    }
    static void deallocate(MPointer<TYPE> arg){
        the_allocator->deallocateMem(arg);
    }
    static void testLoadSave(){
        the_allocator->testLoadSave();
    }

    qint64 getOffset() const {return i_offset;}
    //static void setBase(MNode<TYPE>* b){ m_basePtr = b; }

    MNode<TYPE>* getBase() const{
        return the_allocator->getBase();
    }

    inline MPointer()  { i_offset = NULL_PTR_INDEX; }
    inline MPointer(qint64 i, bool )  {
        i_offset = i;
    }


    //Called nly AFTER the object itself has been allocated
    //
    MPointer(TYPE* b)  {
        qint64 pos = (qint64) b;
        qint64 basePos = pos - offsetof(MNode<TYPE>,T);
        i_offset = ((MNode<TYPE>*)basePos) - getBase();
        Q_ASSERT(i_offset >=MIN_INDEX || i_offset<0);
        //testPtr(this);
    }

    void static testPtr(const MPointer* p, int recur=PTR_TEST_RECUR) {
        if(recur<=0){ return;}
        Q_ASSERT((!the_allocator->m_innerAllocator && (p->i_offset <0) ) || p->i_offset < the_allocator->m_innerAllocator->memLength()  );
        Q_ASSERT(p->i_offset>-2);
        Q_ASSERT(p->i_offset!=0);
        int off = p->i_offset;
        if(p->i_offset>MIN_INDEX){
            qint64 pOff = the_allocator->m_innerAllocator->prevPos(off);
            qint64 nOff = the_allocator->m_innerAllocator->nextPos(off);
            Q_ASSERT(pOff < the_allocator->m_innerAllocator->i_topIndexMemItems);
            Q_ASSERT(nOff < the_allocator->m_innerAllocator->i_topIndexMemItems);
            MPointer nPtr(nOff,true);
            MPointer pPtr(pOff,true);
            if(pOff>=MIN_INDEX){
                qint64 npOff = the_allocator->m_innerAllocator->nextPos(pPtr.i_offset);
                MPointer npPtr(npOff,true);
                Q_ASSERT(p->i_offset==npPtr.i_offset);
            }
            if(nOff>=MIN_INDEX){
                qint64 pnOff = the_allocator->m_innerAllocator->prevPos(nPtr.i_offset);
                MPointer pnPtr(pnOff,true);
                Q_ASSERT(p->i_offset==pnPtr.i_offset);
            }
            if(pOff>=MIN_INDEX){ testPtr(&pPtr,recur-1); }
            if(nOff>=MIN_INDEX){ testPtr(&nPtr,recur-1); }
        }
    }

    void static testPtr(const qint64 offset, int recur=PTR_TEST_RECUR) {
        Q_ASSERT(offset!=0);
        MPointer<TYPE> testP(offset,true);
        MPointer<TYPE>::testPtr(&testP,recur);
    }

    inline MPointer(const MPointer<TYPE> &p) {
        i_offset = p.i_offset;
        //testPtr(i_offset);
    }

    inline MPointer<TYPE> &operator=(const MPointer<TYPE> &p){
        i_offset = p.i_offset;
        //testPtr(i_offset);
        return *this;
    }

    inline MPointer<TYPE> &operator=(MPointer<TYPE> &p){
        i_offset = p.i_offset;
        //testPtr(i_offset);
        return *this;
    }

    operator bool () { return i_offset >= 0; }

    //bool operator !(MPointer<TYPE> &p) { return m_offset != p.m_offset; }

    TYPE* operator->() const {
        //testPtr(i_offset);
        if(i_offset==NULL_PTR_INDEX){
            Q_ASSERT(false);
            return((TYPE*)(qint64)(0));
        }
        if(i_offset==INVALID_INDEX){
            Q_ASSERT(false);
            return((TYPE*)(qint64)(-1));
        }
        return ((TYPE*)(qint64)(getBase() + i_offset + offsetof(MNode<TYPE>,T)));
    }

    TYPE& operator*() const {
        //testPtr(this);
        //testPtr(i_offset);
        if(i_offset==NULL_PTR_INDEX){
            Q_ASSERT(false);
        }
        if(i_offset==NULL_PTR_INDEX){
            Q_ASSERT(false);
        }
        return *((TYPE*)(qint64)(getBase() + i_offset + offsetof(MNode<TYPE>,T)));
    }

    TYPE value(){
        //testPtr(this);
        //testPtr(i_offset);
        return *((TYPE*)(qint64)(getBase() + i_offset + offsetof(MNode<TYPE>,T)));
    }

    TYPE* valuePtr(){
        //testPtr(this);
        //testPtr(i_offset);
        return ((TYPE*)(qint64)(getBase() + i_offset + offsetof(MNode<TYPE>,T)));
    }

};

template <class TYPE> bool operator!=(const MPointer<TYPE>& x, const MPointer<TYPE>& y){
     return x.getOffset() != y.getOffset();
}

template <class TYPE> bool operator==(const MPointer<TYPE>& x, const MPointer<TYPE>& y){
//    qint64 xoff = x.getOffset();
//    qint64 yoff = y.getOffset();
    //Q_ASSERT(xoff != yoff || (y.value()== y.value()));
    return x.getOffset() == y.getOffset();
}

template <class TYPE> bool operator==(const MPointer<TYPE>& x, TYPE* y){
    qint64 pos = (qint64) y;
    qint64 basePos = pos - offsetof(MNode<TYPE>,T);
    qint64 offset = ((MNode<TYPE>*)basePos) - x.getBase();
//    qint64 xoff = x.getOffset();
//    qint64 yoff = y.getOffset();
    //Q_ASSERT(xoff != yoff || (y.value()== y.value()));
    return x.getOffset() == offset;
}


#include "mmap_pointer_impl.h"

#endif // MMAP_POINTER_H
