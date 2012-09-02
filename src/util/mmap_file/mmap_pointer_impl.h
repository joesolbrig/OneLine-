
#include <QHash>
#include <QLinkedList>
#include "mmap_pointer.h"

//template<class TYPE> qint64 Allocator<TYPE >::m_bottumUnAlocdLinkList;
//template<class TYPE> qint64 Allocator<TYPE >::m_topAlocatdLinkedList;
//template<class TYPE> qint64 Allocator<TYPE >::m_topIndexMemItems;
//template<class TYPE> TYPE* Allocator<TYPE >::m_baseMemPtr;
//template<class TYPE> TYPE* Allocator<TYPE >::m_topMemPtr;

//template< class TYPE> MNode<TYPE>* MPointer< TYPE >::m_basePtr=0;
template<class TYPE> Allocator<TYPE>*  MPointer< TYPE >::the_allocator=0;
template<class TYPE> InternalAllocator<TYPE>* Allocator< TYPE>::m_innerAllocator=0;
template<class TYPE> QString Allocator<TYPE >::m_path;
