# Containtainers::Concrete

This folder contains all the Stroika Library Foundation::Containers::Concrete source code.

	The Concrete modules are particular implementations of a high level container access
	pattern, such as using a B-Tree, or HashTable. Selectable here are also different thread
	locking patterns.

	Think of these a bit like factories. If you want to use a particular underlying data structure
	for a container, just select it here ,and assign. E.g.

~~~
		Sequence<int> s = Concrete::Sequence_DoublyLinkedList<int>{};
~~~

## All
  - [Association_Array](Association_Array.h)
  - [Association_LinkedList](Association_LinkedList.h)
  - [Bijection_LinkedList](Bijection_LinkedList.h)
  - [Collection_Array](Collection_Array.h)
  - [Collection_LinkedList](Collection_LinkedList.h)
  - [Collection_stdforwardlist](Collection_stdforwardlist.h)
  - [Deque_DoublyLinkedList](Deque_DoublyLinkedList.h)
  - [KeyedCollection_Array](KeyedCollection_Array.h)
  - [KeyedCollection_LinkedList](KeyedCollection_LinkedList.h)
  - [KeyedCollection_stdhashset](KeyedCollection_stdhashset.h)
  - [Mapping_Array](Mapping_Array.hh)
  - [Mapping_LinkedList](Mapping_LinkedList.h)
  - [Mapping_stdhashmap](Mapping_stdhashmap.h)
  - [MultiSet_Array](MultiSet_Array.h)
  - [MultiSet_LinkedList](MultiSet_LinkedList.h)
  - [Queue_Array](Queue_Array.h)
  - [Queue_DoublyLinkedList](Queue_DoublyLinkedList.h)
  - [Sequence_Array](Sequence_Array.h)
  - [Sequence_DoublyLinkedList](Sequence_DoublyLinkedList.h)
  - [Sequence_LinkedList](Sequence_LinkedList.h)
  - [Sequence_stdvector](Sequence_stdvector.h)
  - [Set_Array](Set_Array.h)
  - [Set_LinkedList](Set_LinkedList.h)
  - [SortedAssociation_stdmultimap](SortedAssociation_stdmultimap.h)
  - [SortedAssociation_SkipList](SortedAssociation_SkipList.h)
  - [SortedCollection_LinkedList](SortedCollection_LinkedList.h)
  - [SortedCollection_SkipList](SortedCollection_SkipList.h)
  - [SortedCollection_stdmultiset](SortedCollection_stdmultiset.h)
  - [SortedKeyedCollection_SkipList](SortedKeyedCollection_SkipList.h)
  - [SortedKeyedCollection_stdset](SortedKeyedCollection_stdset.h)
  - [SortedMapping_SkipList](SortedMapping_SkipList.h)
  - [SortedMapping_SkipList](SortedMapping_stdmap.h)
  - [SortedMapping_stdmap](SortedMapping_stdmap.h)
  - [SortedMultiSet_SkipList](SortedMultiSet_SkipList.h)
  - [SortedMultiSet_stdmap](SortedMultiSet_stdmap.h)
  - [SortedSet_SkipList](SortedSet_SkipList.h)
  - [SortedSet_stdset](SortedSet_stdset.h)
  - [Stack_LinkedList](Stack_LinkedList.h)
