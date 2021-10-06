# Stroika::[Foundation](../ReadMe.md)::Containers

This folder contains all the Stroika Library [Foundation](../ReadMe.md)::Containers source code.

## Overview

---

Containers are objects that collect together a related (parameterized) set of objects.
For example, a Stack\<T>, or Set\<T>, or Sequence\<T>.

## Quick Start

---

```
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
```

## Features

---

- Stroika containers abstract acccess patterns and have virtual 'implementation' (representational polymorphism)

  This allows implementations of that access pattern in a variety of ways. You can even transparently transform / change representation in one part of a program and still access it through the same access pattern in other parts of the program).
  -  Examples
     - a Mapping<> as a linkedlist, tree, or hashtable
     - Sequence can change representation from linked list to vector, to sparse array

      ```
      Mapping<int,int> m = Mapping_Array<int,int> ();  // good while small
      ... while constructing map
      if (m.size () > 100) {
        m = Mapping_stdmap<int,int>{m);
      }
      ... now switch to hash lookup once structure is stable
      m = Mapping_Hashtable<int,int>{m};
      ... all the while, code to operator on mapping same - using Mapping<int,int> interface
      ```

- iterators are much easier to deal with, and typed more sensibly, because their type is the ONLY dependent on the type they iterate over, not the type of the container they iterate over

  ```
  Sequence<int> a;
  Collection<int> b;
  Iterator<int> i = a.begin ();
  i = b.begin ();  // same type
  ```

  NOTE that this means you can declare
  ```
  void f(Iterator<T> start, Iterator<T> end) {...}
  ```
  instead of 
  ```
  template<typename IT>
  void f(IT start, IT end) {...}
  ```
  and still have 'f' work with different types of containers.
  

- Stroika containers are lazy copied (copy-on-write - aka COW)
  - Performance tradeoff: some access patterns are slower due to extra vtable lookup per operation, but most common operations (copying etc) much faster, and abstraction allowing changing representations often makes faster still. Plus, if you use functional apis, these generally avoid much of the vtable lookup cost. And smart compiles (especially link time codegen) can avoid most of the cost.
- A sensible taxonmy of containers based on access pattern, and for each, multiple backend data structures to implement them.
- Linq-like rich variety of functional accessors, like Apply (), FindFirstThat (), Where, Select (), Distinct (), OrderBy (), Accumulate (), Min/Max (), Any (), etc inherited from Iterable\<T>

- Internal thread safety checks, (generally) assure threadsafe access (see Debug::AssertExternallySyncrhonized)

- PATCHING - DISCUSS WITH STERLING


## Supported Containers Archtypes
---

- [Association\<KEY_TYPE, VALUE_TYPE>](Association.h)
  - Allows for the association of two elements, and key and one or more values
  - NYI
- [Bijection\<DOMAIN_TYPE, RANGE_TYPE>](Bijection.h)
  - Bijection allows for the bijective (1-1) association of two elements
  - Supported backends: LinkedList
- [Collection\<T>](Collection.h)
  - a container to manage an un-ordered collection of items, without equality defined for T
  - Supported backends: Array, LinkedList, std::fowrad_list
- [DataHyperRectange\< T, ... INDEXES>](DataHyperRectange.h)
  - a multi-dimensional Sequence\<T>
  - Aliases: Data-Cube, Date Cube, Hyper-Cube, Hypercube
- [DenseDataHyperRectange\< T, ... INDEXES>](DenseDataHyperRectange.h)
  - Supported backends: Vector
- [Deque\<T>](Deque.h)
  - A Deque is a Queue that allows additions and removals at either end
  - Supported backends: DoublyLinkedList
- [KeyedCollection\<T, KEY_TYPE, TRAITS>](KeyedCollection.h)
  - KeyedCollection adds most access patterns used in Mapping to a Collection, but stores only a single object. It takes a parameter
  - Supported backends: LinkedList, std::set
- [Mapping\<KEY_TYPE, VALUE_TYPE>](Mapping.h)
  - Allows for the association of two elements: a key and a value. The key UNIQUELY specifies its associated value
  - Supported backends: Array, LinkedList, std::map
- [MultiSet\<T>](MultiSet.h)
  - A collection of elements where each time you add something, the MultiSet tallies the number of times that thing has been entered. This is not a commonly used class, but handy when you want to count things
  - Supported backends: Array, LinkedList, std::map
- [PriorityQueue\<T>](PriorityQueue.h)
  - PriorityQueues are a like a Queue that allows retrieval based the priority assigned an item
  - NYI
- [Queue\<T>](Queue.h)
  - Queue is a first-in-first-out (FIFO) data structure, where elements are arranged in well-ordered fashion, from HEAD to TAIL
  - Supported backends: Array, DoublyLinkedList
- [Sequence\<T>](Sequence.h)
  - A generalization of a vector: a container whose elements are keyed by the natural numbers.
  - Supported backends: Array, DoublyLinkedList, LinkedList
- [Set\<T>](Set.h)
  - a container of T, where once an item is added, additionally adds () do nothing.
  - Supported backends: LinkedList, std::set
- [SortedAssociation\<KEY_TYPE, VALUE_TYPE>](SortedAssociation.h)
  - see Association.
  - NYI
- [SortedCollection\<T>](SortedCollection.h)
  - See Collection; but adds parameter saying how T items sorted
  - Supported backends: LinkedList
- [SortedKeyedCollection\<T, KEY_TYPE, TRAITS>](SortedKeyedCollection.h)
  - See KeyedCollection; but adds parameter saying how T items sorted (by key)
  - Supported backends: std::set
- [SortedMapping\<KEY_TYPE, VALUE_TYPE>](SortedMapping.h)
  - See Mapping; but adds parameter saying how KEY_TYPE items sorted
  - Supported backends: std::map
- [SortedMultiSet\<T>](SortedMultiSet.h)
  - See MultiSet; but adds parameter saying how T items sorted
  - Supported backends: std::map
- [SortedSet\<T>](SortedSet.h)
  - See Set; but adds parameter saying how T items sorted
  - Supported backends: std::set
- [SparseDataHyperRectange\< T, ... INDEXES>](SparseDataHyperRectangle.h)
  - A DataHyperRectangle where you specify a special 'default' value, which will appear in any cell you 'get' without first setting (as if the hyper-rectangle was pre-initialized to that value).
  - Supported backends: std::map
- [Stack\<T>](Stack.h)
  - Standard LIFO (Last in first out) Stack. See Sedgewick, 30-31. Iteration proceeds from the top to the bottom of the stack. Top is the FIRST IN (also first out).
  - Supported backends: LinkedList


## Implementation notes
---

- Due to use of COW, const methods of reps need no locking (just use Debug::AssertExternallySyncrhonized).
- Due to use of COW, non-const methods of reps ALSO don't need loocking, since the COW code assures there is only one reference at a time (and therefore one Envelope class, which itself asserts externally synchronized)

## Rejected Ideas

---

- Make KEYs a UNIFYING concept for containers
  - Think about somehow making KEYs a UNIFYING concept for containers?
    Special case of unsticky keys -
    like array indexes? In that sense, a KEY is almost like an ITERATOR. In fact,
    an interator is somewhat better than a key.
- We may want Find() to (optionally) return an iterator positioned at the first entry (like STL).
  - **Reason Rejected**:
    unclear what ++ would do. I know what it does in STL, but thats fairly meaningless/arbitrary. Better to return optional<>
- We may want a[i] or some rough analog for sequnces to get a sequence - offset by I.
  - **Reason Rejected**:
    This really only applies to randomly accessed containers (so not stack, deque etc).
    Though its possible to define for them, not usefully. Its probably better to just
    keep key as a 'key' concept for Map<>, and use 'index' - which is analagous - but different -
    for sequence (important difference is stickiness of assocation when container is modified).

- No Compact() methods
  - Stroika 1.0 had Compact() methods - that could be used to generically request that a container
    be compacted.
    We decided against that for Stroika v2 because
    1.  We COULD always add it back.
    2.  Its just a class of optimziations which makes sense for some backends. But other
        optimizations make snese for other classes of backends. You can always retain a smartptr
        with OUR type of backend! So really can be added just for \_Array<> impls. Note - this
        rationale doesn't work perfectly due to copy-by-values semantics with 'casts' but still
        OK.

- [Bag\<T>](Bag.h)
  - The idea is to mimic that of a black bag (not like SmallTalk Bag\<T> which Stroika Collection<> is closest to) - but randomized collection.
  - So far idea rejected, because I dont have a clear use case of where this would be helpful. MAYBE in game/simulation? Need a more concrete use case though to be sure this is helpful, and not more easily done another way
  - Seriously consider renaming this file to RandomizedCollection<>. Maybe can             even SUBCLASS from Collection.
  - Method shake creates new randomized collection from same items (or updates in place)
