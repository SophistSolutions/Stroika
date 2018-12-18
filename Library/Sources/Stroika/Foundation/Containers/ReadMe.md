## Stroika Foundation Containers

## Overview
-----------
  This folder contains all the Stroika Library Foundation::Containers source code.

  Containers are objects that collect together a related (parameterized) set of objects.
  For example, a Stack<T>, or Set<T>, or Sequence<T>.

## Quick Start
-----------
~~~
 #include "Stroika/Foundation/Containers/Seqeunce.h"

 using namespace Stroika::Foundation::Containers;

 Sequence<int> x = { 1, 3, 4 };
 Assert (x.length () == 3);
 x.push_back (3);
 Assert (x.length () == 4);
 Assert (x[1] == 3);
 x = Sequence_LinkedList<int> (x);      // convert x to use a linked-list representation
 Assert (x[1] == 3);                    // all data same as before
 x = Sequence_stdvector<int> (x);       // convert x to use a std::vector representation
 Assert (x.length () == 4);             // all data same as before
 std::list<int> y = x.As<list<int>> (); // convert to STL types
~~~


## Features
-----------

- Stroika containers are lazy copied (copy-on-write - aka COW)
  - Key advantages
    - Allows representational polymorphism (so a sequence can change representation from linked list to vector, to sparse array depending on situation while maintaining same coding interface)
    - iterators are much easier to deal with, and typed more sensibly, because their type is the ONLY dependent on the type they iterate over, not the type of the container they iterate over
      
      ~~~
      Sequence<int> a;
      Collection<int> b;
      Iterator<int> i = a.begin ();
      i = b.begin ();  // same type
      ~~~
    - A sensible taxonmy of containers based on access pattern, and for each, multiple backend data structures to implement them.
    - Linq-like rich variety of functional accessors, like Apply (), FindFirstThat (), Where, Select (), Distinct (), OrderBy (), Accumulate (), Min/Max (), Any (), etc inherited from Iterable&lt;T&gt;

## Supported Containers Archtypes
  -  [Association&lt;KEY_TYPE, VALUE_TYPE&gt;](Association.h)
     - Allows for the association of two elements, and key and one or more values
     - NYI
  -  [Bijection&lt;DOMAIN_TYPE, RANGE_TYPE&gt;](Bijection.h)
     - Bijection allows for the bijective (1-1) association of two elements
     - Supported backends: LinkedList
  -  [Bag&lt;T&gt;](Bag.h)
     - The idea is to mimic that of a black bag (not like SmallTalk Bag<T> which Stroika Collection<> is closest to) - but randomized collection.
     - NYI
  -  [Collection&lt;T&gt;](Collection.h)
     - a container to manage an un-ordered collection of items, without equality defined for T
     - Supported backends: Array, LinkedList, std::fowrad_list
  -  [DataHyperRectange&lt; T, ... INDEXES&gt;](DataHyperRectange.h)
     - Aliases: Data-Cube, Date Cube, Hyper-Cube, Hypercube
  -  [DenseDataHyperRectange&lt; T, ... INDEXES&gt;](DenseDataHyperRectange.h)
     - Supported backends: Vector
  -  [Deque&lt;T&gt;](Deque.h)
     - A Deque is a Queue that allows additions and removals at either end
     - Supported backends: DoublyLinkedList
  -  [KeyedCollection&lt;KEY_TYPE, T&gt;](KeyedCollection.h)
     - KeyedCollection adds most access patterns used in Mapping to a Collection, but stores only a single object. It takes a parameter
     - NYI
  -  [Mapping&lt;KEY_TYPE, VALUE_TYPE&gt;](Mapping.h)
     - Allows for the association of two elements: a key and a value. The key UNIQUELY specifies its associated value
     - Supported backends: Array, LinkedList, std::map
  -  [MultiSet&lt;T&gt;](MultiSet.h)
     -  A collection of elements where each time you add something, the MultiSet tallies the number of times that thing has been entered. This is not a commonly used class, but handy when you want to count things
     - Supported backends: Array, LinkedList, std::map
  -  [PriorityQueue&lt;T&gt;](PriorityQueue.h)
     - PriorityQueues are a like a Queue that allows retrieval based the priority assigned an item
     - NYI
  -  [Queue&lt;T&gt;](Queue.h)
     - Queue is a first-in-first-out (FIFO) data structure, where elements are arranged in well-ordered fashion, from HEAD to TAIL
     - Supported backends: Array, DoublyLinkedList
  -  [Sequence&lt;T&gt;](Sequence.h)
     - A generalization of a vector: a container whose elements are keyed by the natural numbers.
     - Supported backends: Array, DoublyLinkedList, LinkedList
  -  [Set&lt;T&gt;](Set.h)
     - a container of T, where once an item is added, additionally adds () do nothing.
     - Supported backends: LinkedList, std::set
  -  [SortedAssociation&lt;KEY_TYPE, VALUE_TYPE&gt;](SortedAssociation.h)
     - see Association.
     - NYI
  -  [SortedCollection&lt;T&gt;](SortedCollection.h)
     - See Collection; but adds parameter saying how T items sorted
     - Supported backends: LinkedList
  -  [SortedKeyedCollection&lt;KEY_TYPE, T&gt;](SortedKeyedCollection.h)
     - See KeyedCollection; but adds parameter saying how T items sorted
     - NYI
  -  [SortedMapping&lt;KEY_TYPE, VALUE_TYPE&gt;](SortedMapping.h)
     - See Mapping; but adds parameter saying how KEY_TYPE items sorted
     - Supported backends: std::map
  -  [SortedMultiSet&lt;T&gt;](SortedMultiSet.h)
     - See MultiSet; but adds parameter saying how T items sorted
     - Supported backends: std::map
  -  [SortedSet&lt;T&gt;](SortedSet.h)
     - See Set; but adds parameter saying how T items sorted
     - Supported backends: std::set
  -  [SparseDataHyperRectange&lt; T, ... INDEXES&gt;](SparseDataHyperRectangle.h)
     - A DataHyperRectangle where you specify a special 'default' value, which will appear in any cell you 'get' without first setting (as if the hyper-rectangle was pre-initialized to that value).
     - Supported backends: std::map
  -  [Stack&lt;T&gt;](Stack.h)
     - Standard LIFO (Last in first out) Stack. See Sedgewick, 30-31. Iteration proceeds from the top to the bottom of the stack. Top is the FIRST IN (also first out).
     - Supported backends: LinkedList


## Rejected Ideas
--------------
   *   Make KEYs a UNIFYING concept for containers
      *   Think about somehow making KEYs a UNIFYING concept for containers? 
          Special case of unsticky keys -
          like array indexes? In that sense, a KEY is almost like an ITERATOR. In fact, 
          an interator is somewhat better than a key. 
      *   We may want Find() to (optionally) return an iterator positioned at the first entry
          (unclear what ++ would do).
      *   We may want a[i] or some rough analog for sequnces to get a sequence - offset by I.
      *   **Reason Rejected**:
          This really only applies to randomly accessed containers (so not stack, deque etc).
          Though its possible to define for them, not usefully. Its probably better to just
          keep key as a 'key' concept for Map<>, and use 'index' - which is analagous - but different -
          for sequence (important difference is stickiness of assocation when container is modified).

   *   No Compact() methods
      *   Stroika 1.0 had Compact() methods - that could be used to generically request that a container
          be compacted.
             
          We decided against that for Stroika v2 because
             1.	   We COULD always add it back.
             2.    Its just a class of optimziations which makes sense for some backends. But other 
				   optimizations make snese for other classes of backends. You can always retain a smartptr
				   with OUR type of backend! So really can be added just for _Array<> impls. Note - this
				   rationale doesn't work perfectly due to copy-by-values semantics with 'casts' but still
				   OK.
