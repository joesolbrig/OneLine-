#ifndef MMAP_POINTER_H
#define MMAP_POINTER_H

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

//This will be a linked list with memory - to avoid issues
//of c++ adding junk, I'll deal with array, qint64 and Type* only

template <class TYPE> class IAllocator;
template <class TYPE> class MPointer;

//This should be a singleton

template <class TYPE> struct MNode {
    TYPE T;
    qint64 m_prev;
    qint64 m_prev;
};

template <class TYPE> class Allocator {
    IAllocator<TYPE>* m_innerAllocator;
    TYPE* m_copyBuffer;

public:

    Allocator(){
        m_innerAllocator = new IAllocator<TYPE>();
        m_copyBuffer = (TYPE*)malloc(m_innerAllocator->memLength()*sizeof(TYPE));
        MPointer<TYPE>::setBase(m_innerAllocator->base());
    }
    TYPE* allocate(){
        if(!m_innerAllocator->canAllocate()){
            int memLength = m_innerAllocator->memLength();
            int pageCount= m_innerAllocator->m_pageCount;
            memcpy(m_copyBuffer, m_innerAllocator->base(), memLength);
            delete m_innerAllocator;
            m_innerAllocator = new IAllocator<TYPE>(pageCount*(1.5) + sizeof(TYPE)*10);
            memcpy(m_innerAllocator->base(),m_copyBuffer, memLength);
            free(m_copyBuffer);
            m_copyBuffer = (TYPE*)malloc(m_innerAllocator->memLength()*sizeof(TYPE));
            MPointer<TYPE>::setBase(m_innerAllocator->base());
        }
        return m_innerAllocator->allocate();
    }
    void deallocate(MPointer<TYPE> ptr){
        m_innerAllocator->deallocate(ptr);
    }
};


template <class TYPE> class IAllocator {
    friend class MPointer<TYPE>;


private:
    TYPE* m_topMemPtr;
    TYPE* topMemPtr(){ return m_topMemPtr; }
    void setTopMemPtr(TYPE* p){ m_topMemPtr = p; }

protected:
    //Memory Structure
    qint64 m_bottumUnAlocdLinkList;
    qint64 m_topAlocatdLinkedList;
    qint64 m_topIndexMemItems;
    TYPE* m_baseMemPtr;

    qint64 chunkSize(){ return sizeof(TYPE) + 2*sizeof(qint64);}

public:

    qint64 m_pageCount;
    qint64 memLength(){ return m_topMemPtr - m_baseMemPtr; }
    TYPE* base(){ return m_baseMemPtr; }

    //I'm either manipulated existing linked-list entries or writing further up
    //the infinite memory mapped file
    //
    //Our linklist pointers should be beyond memory area devoted to TYPE


    qint64&  nextPos(qint64 x) {
        qint64* ptr = (qint64*)(((int*)m_baseMemPtr) + chunkSize()*(x) + sizeof(TYPE));
        return *(ptr);
    }
    void  setNextPos(qint64 x,qint64 val) {
        qint64* ptr = (qint64*)(((int*)m_baseMemPtr) + chunkSize()*(x) + sizeof(TYPE));
        *(ptr) = val;
    }
    qint64& prevPos(qint64 x){
        return *((qint64*) (((int*)m_baseMemPtr) + chunkSize()*(x) + sizeof(TYPE)
                 + sizeof(qint64)) );

    }
    void* itemAt(qint64 x){
        return ((void*)(m_baseMemPtr + (x)*chunkSize()));
    }
    
    bool canAllocate(){
        return ((m_baseMemPtr +m_topAlocatdLinkedList) + 1) < topMemPtr();
    }

    TYPE* allocate(){
        qint64 index;
        if(m_bottumUnAlocdLinkList){
            index = m_bottumUnAlocdLinkList;
            prevPos(index) = m_topAlocatdLinkedList;
            if(nextPos(index)){
                prevPos(nextPos(index))=0;
            }
        } else {
            index = m_topIndexMemItems+1;
        }
        setNextPos(index,0);
        prevPos(index)=m_topIndexMemItems;
        nextPos(prevPos(index))=index;
        m_topIndexMemItems = index;
        void* res = itemAt(index);
        Q_ASSERT((TYPE*)res < topMemPtr());
        return (TYPE*)res;
    }

    void deallocate(MPointer<TYPE> ptr){
        Q_ASSERT( ((ptr.m_offset) % chunkSize()) ==0);
        qint64 i = (ptr.m_offset)/chunkSize();
        Q_ASSERT(i >= 0 &&  i<=(m_topIndexMemItems ));

        qint64 j = prevPos(i);
        if(j){
            qint64 k = nextPos(j);
            Q_ASSERT(k==i);
            nextPos(prevPos(i))= nextPos(i);
        }
        if(nextPos(i)){
            Q_ASSERT(prevPos(nextPos(i))==i);
            prevPos(nextPos(i)) = prevPos(i);
        }

        if(m_bottumUnAlocdLinkList){
            prevPos(m_bottumUnAlocdLinkList) = i;
        }
        prevPos(i) = 0;
        nextPos(i) = m_bottumUnAlocdLinkList;
        m_bottumUnAlocdLinkList=i;
    }


    void createSpace(int size){
        QFile file(QString("OneLineMemMapIndex") + TYPE::typeName() + ".txt");
        int pageSize = (int)sysconf(_SC_PAGESIZE);
        if ( pageSize < 0) { Q_ASSERT(false); }
        int length= size*pageSize;

        QString s(length, 'X');
        file.open(QIODevice::ReadWrite);
        file.write(s.toAscii());
        file.close();


        file.open(QIODevice::ReadWrite);
        Q_ASSERT(file.isOpen());

        int* ptr = (int*)mmap(NULL,
                          length,
                          PROT_READ|PROT_WRITE,
                          MAP_FILE|MAP_SHARED,
                          file.handle(), 0);
        file.close();

        m_baseMemPtr = (TYPE*)ptr;
        if (m_baseMemPtr == MAP_FAILED){ Q_ASSERT(false); }

        setTopMemPtr(m_baseMemPtr + length);
        m_bottumUnAlocdLinkList=0;
        m_topAlocatdLinkedList=0;
        m_topIndexMemItems=0;
        file.close();
    }


    void expandSpace(){
        //write data right now
//        msync(m_baseMemPtr, memLength(), MS_SYNC);
//
//        int pageSize = (int)sysconf(_SC_PAGESIZE);
//        if ( pageSize < 0) { Q_ASSERT(false); }
//        int length= size*pageSize;
//        int toWrite = length - length();
//        Q_ASSERT(toWrite >0);
//        Q_ASSERT(toWrite % pageSize =0);
//        QString s(toWrite, 'X');
//        m_file.open(QIODevice::Append);
//        m_file.write(s.toAscii());
//        m_file.close();
//
//        m_baseMemPtr = mremap(m_baseMemPtr, length(),length, MREMAP_MAYMOVE);
//        setTopMemPtr(length);

    }

    void closeMM(){

    }

    IAllocator(int p = 4000){
        m_pageCount = p;
        createSpace(m_pageCount);
    }

    //The system should free the resources but let's try anyway...
    ~IAllocator(){
        munmap(m_baseMemPtr,memLength());
    }

};

#include <QPointer>
template <class TYPE> class MPointer {
    friend class IAllocator<TYPE>;
    static TYPE* m_basePtr;

    qint64 m_offset;

    static Allocator<TYPE > the_allocator;
public:
    static void* allocate() {
        return the_allocator.allocate();
    }
    static void deallocate(MPointer<TYPE> arg){
        the_allocator.deallocate(arg);
    }
    qint64 getOffset() const {return m_offset;}
    static void setBase(TYPE* b){ m_basePtr = b; }

    inline MPointer(int i=0)  { m_offset = i; }
    //inline MPointer()  { m_offset = -1; }

    //Only AFTER the thing has been allocated
    MPointer(TYPE* b)  { m_offset = b -m_basePtr; }

    inline MPointer(const MPointer<TYPE> &p) {
        m_offset = p.m_offset;
    }

    inline MPointer<TYPE> &operator=(const MPointer<TYPE> &p){
        m_offset = p.m_offset;
        return *this;
    }

    inline MPointer<TYPE> &operator=(MPointer<TYPE> &p){
        m_offset = p.m_offset;
        return *this;
    }

    //inline bool isNull() const { return m_offset < 0; }
    operator bool () { return m_offset > 0; }

    //bool operator !(MPointer<TYPE> &p) { return m_offset != p.m_offset; }
    //friend template <class TYPE> bool operator!=(const MPointer<TYPE>& x, const MPointer<TYPE>& y);

    TYPE* operator->() const {
        return ((m_basePtr + m_offset));
    }

    TYPE& operator*() const
        { return *((m_basePtr + m_offset)); }

};

template <class TYPE> bool operator!=(const MPointer<TYPE>& x, const MPointer<TYPE>& y){
     return x.getOffset() != y.getOffset();

}

#include "mmap_pointer_impl.h"

#endif // MMAP_POINTER_H
