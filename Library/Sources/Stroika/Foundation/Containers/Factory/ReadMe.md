# Stroika::Foundation::[Containers](../ReadMe.md)::Factory

Generally Strioka users don't need to be concerned with container factories. But when you create
a container without specifcying the backend data structure to represent it, an appropriate backend
is constructed automatically.

~~~
Sequence<int> x;  // the Sequence_Factory is used automatically to pick the appropriate data structure to implement the Sequence
~~~

You can override how this defaulting is done in this particular case by using
~~~
Sequence<int> x = Concrete::Sequence_DoublyLinkedList<int>{};
~~~

or change All sequeunce default constructors by replacing the factory

~~~
 Sequence_Factory<int>::Register([](){return Concrete::Sequence_DoublyLinkedList<int> ();});
 Sequence<int> x;  // now this is using Sequence_DoublyLinkedList<int>
~~~


- Design Notes

  In Stroika 2.1 and earlier, the factories could be replaced (new defaults) set anytime. But this made
  calls to construct new containers more expensive due to the needed locking.

  As of Stroika v3, we require calls to container factory 'Register' methods to happen before the first
  container is constructed, so that this is no longer an issue, and container construction is
  typically more performant.


- Modules
  - [Association_Factory.h](Association_Factory.h)
  - [Bijection_Factory.h](Bijection_Factory.h)
  - [Collection_Factory.h](Collection_Factory.h)
  - [DenseDataHyperRectangle_Factory.h](DenseDataHyperRectangle_Factory.h)
  - [Deque_Factory.h](Deque_Factory.h)
  - [KeyedCollection_Factory.h](KeyedCollection_Factory.h)
  - [Mapping_Factory.h](Mapping_Factory.h)
  - [MultiSet_Factory.h](MultiSet_Factory.h)
  - [Queue_Factory.h](Queue_Factory.h)
  - [Sequence_Factory.h](Sequence_Factory.h)
  - [Set_Factory.h](Set_Factory.h)
  - [SortedAssociation_Factory.h](SortedAssociation_Factory.h)
  - [SortedCollection_Factory.h](SortedCollection_Factory.h)
  - [SortedKeyedCollection_Factory.h](SortedKeyedCollection_Factory.h)
  - [SortedMapping_Factory.h](SortedMapping_Factory.h)
  - [SortedMultiSet_Factory.h](SortedMultiSet_Factory.h)
  - [SortedSet_Factory.h](SortedSet_Factory.h)
  - [SparseDataHyperRectangle_Factory.h](SparseDataHyperRectangle_Factory.h)
  - [Stack_Factory.h](Stack_Factory.h)

