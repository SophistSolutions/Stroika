# Stroika::[Foundation](../)::Containers

This folder contains all the Stroika Library [Foundation](../)::Containers source code.

## Overview

---

Containers are objects that collect together a related (parameterized) collection of objects.
For example, a Stack\<T>, or Set\<T>, or Sequence\<T>. Unlike std c++ container classes (e.g. vector\<T>) Stroika containers are not uniquely tied to a particular data structure (e.g. Sequence\<T> could be implemented as a linked list or vector).

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
 x = Sequence_LinkedList<int>{x};       // convert x to use a linked-list representation
 Assert (x[1] == 3);                    // all data same as before
 x = Sequence_stdvector<int>{x};        // convert x to use a std::vector representation
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
      Mapping<int,int> m = Mapping_Array<int,int>{};  // good while small
      ... while constructing map
      if (m.size () > 100) {
        m = Mapping_stdmap<int,int>{m};
      }
      ... now switch to hash lookup once structure is stable
      m = Mapping_Hashtable<int,int>{m};
      ... all the while, code to operator on mapping same - using Mapping<int,int> interface
      ```

- iterators are much easier to deal with, and typed more sensibly, because their type is the ONLY dependent on the type they iterate over, not the type of the container they iterate over (or worse in stdc++ containers the type of data structure used, or even comparison function to sort them).

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

  Similarly, because Iterator\<T> has an operator bool (returns true on END) - this means (unlike STL containers but like ptr based null-checking) you can do)
  ```
  if (auto p = something_that_returns_iterator()) {}
  ```
  

- Stroika containers are lazy copied (copy-on-write - aka COW)
  - Performance tradeoff: some access patterns are slower due to extra vtable lookup per operation, but most common operations (copying etc) much faster, and abstraction allowing changing representations often makes faster still. Plus, if you use functional apis, these generally avoid much of the vtable lookup cost. And smart compilers (especially link time codegen) can avoid most of the cost.
- A sensible taxonmy of containers (Archetypes) based on access pattern, and for each, multiple backend data structures to implement them.

- <a name="Alternate-Backends-Feature"></a>Each Archetype container (access pattern) comes with multiple data structure backends, and you can start with the default (generally good), but when optimizing, transparently switch backends (data structure implementations) to easily adjust your performance characteristics
- Linq-like rich variety of functional accessors, like Apply (), Find (), Where, Select (), Distinct (), OrderBy (), Accumulate (), Min/Max (), Any (), etc inherited from [Iterable\<T>](../Traversal/Iterable.h)

- Block-Allocation by default - even for STL-based containers. This helps make use of SortedSet_stdset\<T> often faster than std::set\<T>, for example.

- Internal thread safety checks, (generally) assure threadsafe access (see [Debug::AssertExternallySynchronizedMutex](../Debug/AssertExternallySynchronizedMutex.h))

## Supported Containers Archetypes
---

- [Association\<KEY_TYPE, VALUE_TYPE>](Association.h)
  - Allows for the association of two elements: a key and one or more associated values
  - Similar to Mapping<> - except multi-valued (like std::multimap)
  - Supported backends: [Array](Concrete/Association_Array.h), [LinkedList](Concrete/Association_LinkedList.h), [SkipList](Concrete/SortedAssociation_SkipList.h), [std::multimap](Concrete/SortedAssociation_stdmultimap.h)
- [Bijection\<DOMAIN_TYPE, RANGE_TYPE>](Bijection.h)
  - Bijection allows for the bijective (1-1) association of two elements
  - Supported backends: [LinkedList](Concrete/Bijection_LinkedList.h)
- [Collection\<T>](Collection.h)
  - a container to manage an un-ordered collection of items, without equality defined for T
  - Supported backends: [Array](Concrete/Collection_Array.h), [LinkedList](Concrete/Collection_LinkedList.h), [std::forward_list](Concrete/Collection_stdforwardlist.h), [SkipList](Concrete/SortedCollection_SkipList.h), [std::multiset](Concrete/SortedCollection_stdmultiset.h)
- [DataHyperRectangle\< T, ... INDEXES>](DataHyperRectangle.h)
  - a multi-dimensional Sequence\<T>
  - Aliases: Data-Cube, Date Cube, Hyper-Cube, Hypercube
- [DenseDataHyperRectangle\< T, ... INDEXES>](DenseDataHyperRectangle.h)
  - Supported backends: Vector
- [Deque\<T>](Deque.h)
  - A Deque is a Queue that allows additions and removals at either end
  - Supported backends: [DoublyLinkedList](Concrete/Deque_DoublyLinkedList.h)
- [KeyedCollection\<T, KEY_TYPE, TRAITS>](KeyedCollection.h)
  - KeyedCollection adds most access patterns used in Mapping to a Collection, but stores only a single object. It takes a parameter saying how to 'extract' the key from the collected value 'T'.
  - Supported backends: [Array](Concrete/KeyedCollection_Array_.h),[LinkedList](Concrete/KeyedCollection_LinkedList_.h),[std::hashset](Concrete/KeyedCollection_stdhashset.h), [SkipList](Concrete/SortedKeyedCollection_SkipList.h), [std::set](Concrete/SortedKeyedCollection_stdset.h)
- [Mapping\<KEY_TYPE, VALUE_TYPE>](Mapping.h)
  - Allows for the association of two elements: a key and a value. The key UNIQUELY specifies its associated value
  - Supported backends: [Array](Concrete/Mapping_Array_.h), [LinkedList](Concrete/Mapping_LinkedList_.h), [std::hash_map](Concrete/Mapping_stdhashmap_.h), [SkipList](Concrete/SortedMapping_SkipList.h),  [SkipList](Concrete/SortedMapping_SkipList.h),  [std::map](Concrete/SortedMapping_stdmap.h)
  - A collection of elements where each time you add something, the MultiSet tallies the number of times that thing has been entered. This is not a commonly used class, but handy when you want to count things
- [PriorityQueue\<T>](PriorityQueue.h)
  - PriorityQueues are a like a Queue that allows retrieval based the priority assigned an item
  - NYI
- [Queue\<T>](Queue.h)
  - Queue is a first-in-first-out (FIFO) data structure, where elements are arranged in well-ordered fashion, from HEAD to TAIL
  - Supported backends:  [Array](Concrete/Queue_Array.h),  [DoublyLinkedList](Concrete/Queue_DoublyLinkedList.h)
- [Sequence\<T>](Sequence.h)
  - A generalization of a vector: a container whose elements are keyed by the natural numbers.
  - Supported backends: [Array](Concrete/Sequence_Array.h), [DoublyLinkedList](Concrete/Sequence_DoublyLinkedList.h), [LinkedList](Concrete/Sequence_LinkedList.h), [std::vector](Concrete/Sequence_stdvector.h)
- [Set\<T>](Set.h)
  - a container of T, where once an item is added, additionally adds () do nothing (so items added at most once).
  - Supported backends: [Array](Concrete/Set_Array.h), [LinkedList](Concrete/Set_LinkedList.h), [SkipList](Concrete/SorteSet_SkipList.h), [std::set](Concrete/SorteSet_stdset.h)
  - Set vs std::set<T>:
    Stroika's Set\<T> is like std::set\<T>, except that
    - you can separately select different algorithms (besides red-black tree) and not change the API used (Set<T>).
    - You don't need to have a less\<T> method defined. You just need to provide some mechanism (either operator== or argument to constructor)  saying how to compare elements for equality
    - If you have a less\<T> already defined, like std::set<T>, this will be used by default to construct a tree-based set.
    - Sets can also be implemented by hash-tables, etc.
- [SortedAssociation\<KEY_TYPE, VALUE_TYPE>](SortedAssociation.h)
  - see Association; but adds parameter saying how KEY items sorted
  - Supported backends: [SkipList](Concrete/SortedAssociation_SkipList.h), [std::multimap](Concrete/SortedAssociation_stdmultimap.h)
- [SortedCollection\<T>](SortedCollection.h)
  - See Collection; but adds parameter saying how T items sorted
  - Supported backends: [LinkedList](Concrete/SortedCollection_LinkedList.h),[std::multiset](Concrete/SortedCollection_stdmultiset.h)
- [SortedKeyedCollection\<T, KEY_TYPE, TRAITS>](SortedKeyedCollection.h)
  - See KeyedCollection; but adds parameter saying how T items sorted (by key)
  - Supported backends: [SkipList](Concrete/SortedKeyedCollection_SkipList_.h), [std::set](Concrete/SortedKeyedCollection_stdset_.h)
- [SortedMapping\<KEY_TYPE, VALUE_TYPE>](SortedMapping.h)
  - See Mapping; but adds parameter saying how KEY_TYPE items sorted
  - Supported backends: [SkipList](Concrete/SortedMapping_SkipList.h),  [std::map](Concrete/SortedMapping_stdmap.h)
- [SortedMultiSet\<T>](SortedMultiSet.h)
  - See MultiSet; but adds parameter saying how T items sorted
  - Supported backends: [SkipList](Concrete/SortedMultiSet_SkipList.h), [std::map](Concrete/SortedMultiSet_stdmap.h)
- [SortedSet\<T>](SortedSet.h)
  - See Set; but adds parameter saying how T items sorted
  - Supported backends: [SkipList](Concrete/SortedSet_SkipList.h), [std::set](Concrete/SortedSet_stdset.h)
- [SparseDataHyperRectangle\< T, ... INDEXES>](SparseDataHyperRectangle.h)
  - A DataHyperRectangle where you specify a special 'default' value, which will appear in any cell you 'get' without first setting (as if the hyper-rectangle was pre-initialized to that value).
  - Supported backends: std::map
- [Stack\<T>](Stack.h)
  - Standard LIFO (Last in first out) Stack. See Sedgewick, 30-31. Iteration proceeds from the top to the bottom of the stack. Top is the FIRST IN (also first out).
  - Supported backends: [LinkedList](Concrete/Stack_LinkedList.h)

## Container Element comparisons

Most containers have no intrinsic comparison required on their elements. Those that do
require such, use the default comparison automatically (T==T, or T\<=\>T if equals_comparable/totally_ordered) and always allow this to be
overridden at the time of container construction (this is very unlike STL where the comparison function is baked into the type of the container).

Generally you want comparers to be inlined into the algorithms. But you also want
to be able to generically refer to them, and copy them as objects.
	 
These 'requirements' pull in opposing directions.
	 
Stroika manages this with the container class library by:
	 
 - have concepts that test if something is an equals/threeway/or strict-inorder comparer
 - Allow CONSTRUCTORS (and factories) for containers to take explicit comparers
	 	  as arguments that can be perfectly forwarded (zero cost abstraction) to the container
	 	  backend implementation, so there is zero cost in using them.
- Provides std::function based wrappers on all these, so containers generally can respond
	 	  to GetElementInOrderComparer (), or GetKeyEqualsComparer (), etc, as appporiate.
	 	
users may construct a container using the comparers returned from these functions, but
with the overhad of a function object.
	
Mostly - thats fine, and exceedingly rarely needed. But where its a performance issue, simply
arrange to know via other application logic the exact type of the comparer, and use that
when instantiating your container.
	
In my experience, the Get....Comparer () routines are only needed in testing code,
and so their performance doesn't matter.

## Container Constructors

Each container Archetype has its own set of arguments that make sense for its constructor, but here are the general guidelines the apply to all of them.

- Default constructor - this is generally supported where possible, though sometimes there are required items (like comparers or extractors) needed for a container. Sometimes these can be automatically inferred from the type 'T' - in which case you still have something like this.
  ~~~
  CONTAINER ();
  ~~~

- Move and Copy constructors, defaulted/noexcept, since they all just propagate this to their underlying base class (Iterable<>) and this just increments or adjusts reference counts as appropriate.

    ~~~
    CONTAINER (CONTAINER&& src) noexcept      = default;
    CONTAINER (const CONTAINER& src) noexcept = default;
    ~~~

  - Note about **containers after move from** - they CANNOT be used/accessed.
    ~~~
    7.6.5.15 [lib.types.movedfrom]

    Objects of types defined in the C++ standard library may be moved from (12.8). Move operations may be explicitly specified or implicitly generated. Unless otherwise specified, such moved-from objects shall be placed in a valid but unspecified state
    ~~~

    This strongly suggests that each container - after the move, should continue to support operations like size() and Append, etc, but with largely undefined behavior. This isn't terribly useful, and IS quite costly. So in Stroika, we simply choose to assert-out if you perform operations on a container (other than destructor) - after it's been moved from. Pragmatically, I've never seen this to cause any problems.

- initializer_list<value_type> constructor provided so you can use a direct list of elements (e.g. CONTAINER{1, 2, 3}).
    ~~~
    CONTAINER (const initializer_list<value_type>& src);
    ~~~

- Copy other container. This allows for converting nearly anything sensible (iterable with convertible_to constraint applied to its iterable value_type) into a CONTAINER.

  - Note, the reason for the not derived_from<> magic is to avoid ambiguity with copy constructor (not SURE WHY this is needed but it was needed in some cases - maybe due to bugs - maybe can lose - I recall docs on C++ suggest NOT needed). https://stackoverflow.com/questions/4419375/templated-constructor-vs-templated-copy-constructor seems to suggest it is needed (not authoritative). But I seem to recall reading (https://stackoverflow.com/questions/23244665/c-overloading-takes-precedence-over-specialization) that explictly defined copy constructors take precedence over template defined ones. So confused by this, but it appears still needed (2021-12-03).

    ~~~
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
    CONTAINER (ITERABLE_OF_ADDABLE&& src)
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, CONTAINER<T>>)
    {
        something-like-AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        // and more to initialize container
    }
    ~~~

- Construct the CONTAINER using an iterable; This typically just constructs the container by default and calls AddItems(start, end)
    ~~~
    template <input_iterator ITERATOR_OF_ADDABLE>
    CONTAINER (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
    // note perfect forwarding on iterator tends to avoid a needless rep clone when assigned auto i = start to iterate
    ~~~

- Construct from an underlying rep smart pointer. This is principally (exclusively?) used in constructing concrete container types.
  ~~~
  protected:
      explicit CONTAINER (shared_ptr<_IRep>&& src) noexcept;
      explicit CONTAINER (const shared_ptr<_IRep>& src) noexcept;
  ~~~

- CONTAINERS can generally be move copied/assigned, just generically manipulating by the base class Iterable<> implementation - really just done by SharedByValue used in Iterable<>.
  ~~~
  public:
      nonvirtual CONTAINER& operator= (CONTAINER&& rhs) noexcept = default;
      nonvirtual CONTAINER& operator= (const CONTAINER& rhs) = default;
  ~~~

### ArchTypes additional required data

Often ArchTypes will have additional required data, like an Extractor function, or InOrderComparer. Support for these parameters will generally multiply several of the above overloads allowing those parameters to be specified. Those functions will generally be checked with 'concept-like' SFINAE type checkers to avoid constructor ambiguity.

~~~
template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER = equal_to<KEY_TYPE>>
        KeyedCollection (ITERABLE_OF_ADDABLE&& src)
            requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                      not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, KeyedCollection<T, KEY_TYPE, TRAITS>>)
~~~

## Note About Iterators

- Stroika container iterators must have shorter lifetime than the container they are iterating over.

- Stroika container iterators become invalidated when their underlying container has changed. In debug builds, use of an iterator (other than destroying it) results in an assertion failure. This is in SHARP CONTRAST to the (safe iterator patching) Stroika used todo before 2.1b14.

## Note About Sub-Containers/Derived Parts Containers

- Stroika containers can have subsets or projections taken of them, either through Iterable LINQ-like APIs, or
  through methods like Mapping<>::Key(), or Multiset<>::Elements () etc. These become logically detatched from thier creator,
  and can be hung into indefinitely, do not allow modifications, and do not see modifications of thier source container.

## Other Modules
  - [Adapters/](Adapters/)
  - [Concrete/](Concrete/)
  - [LockFreeDataStructures/](LockFreeDataStructures/)
  - [Factory/](Factory/)
  - [Support/](Support/)
  - [STL/](STL/)

## Implementation notes
---

- Due to use of COW, const methods of reps need no locking (just use Debug::AssertExternallySyncrhonized).
- Due to use of COW, non-const methods of reps ALSO don't need locking, since the COW code assures there is only one reference at a time (and therefore one Envelope class, which itself asserts externally synchronized)

- Generally have body functions of overloads have static_assert(TYPE REQUIREMENT) instead of using
  enable_if_t, just because enable_if_t tends to require more compiler bug workarounds, and produce
  inferior warnings. When I convert to using concepts, we may reverse this. Either way, the requirement
  is still there: the difference is just in the error message and POSSIBLY RARELY in confusing selection
  of an overload. This probably just happens with Constructors, and maybe not at all.


## Possible Future Changes/Ideas

- [Bag\<T>]()
  - The idea is to mimic that of a black bag (not like SmallTalk Bag\<T> which Stroika Collection<> is closest to) - but randomized collection.
  - So far idea rejected, because I dont have a clear use case of where this would be helpful. MAYBE in game/simulation? Need a more concrete use case though to be sure this is helpful, and not more easily done another way
  - Seriously consider renaming this file to RandomizedCollection<>. Maybe can             even SUBCLASS from Collection.
  - Method shake creates new randomized collection from same items (or updates in place)

- [SequenceSet\<T>]()

  This would be a 'Set' - in that entries can only appear once, but where you can specify the ordering
  not by 'Sorted' function - but by explicitly preserving the order as added (and move methods etc).

  The use case I have in mind - is a set of URLs (like OpenAPI server urls). You probably want to not
  have (allow) dups, but probably want to specify which one is top/first. So very minor, not sure worth
  the effort. Need more use cases (and datastructure backend idea that make is helpful).

- Make KEYs a UNIFYING concept for containers
  - Think about somehow making KEYs a UNIFYING concept for containers?
    Special case of unsticky keys -
    like array indexes? In that sense, a KEY is almost like an ITERATOR. In fact,
    an interator is somewhat better than a key.

## Rejected Ideas

---

- We may want a[i] or some rough analog for sequnces to get a sequence - offset by I.
  - **Reason Rejected**:
    This really only applies to randomly accessed containers (so not stack, deque etc).
    Though its possible to define for them, not usefully. Its probably better to just
    keep key as a 'key' concept for Map<>, and use 'index' - which is analagous - but different -
    for sequence (important difference is stickiness of assocation when container is modified).

- No generic Compact()/shrink_to_fit() methods
  - Stroika 1.0 had Compact() methods - that could be used to generically request that a container
    be compacted.
    We decided against that for Stroika v2 because
    1.  We COULD always add it back.
    2.  Its just a class of optimziations which makes sense for some backends. But other
        optimizations make sense for other classes of backends. You can always retain a smartptr
        with OUR type of backend! So really can be added just for \_Array<> impls. Note - this
        rationale doesn't work perfectly due to copy-by-values semantics with 'casts' but still
        OK.
    3.  Note that shrink_to_fit() etc are common methods in concrete containers like Sequence_Array, etc...

- Iterator Patching
  Before Stroika 2.1b14, updates to containers used to automatically update all existing running iterators. This was a nice, elegant feature. But it cost
  too much for its quite modest value.
  - It made the backend implementations of containers significantly more complex, due to having to implement a patching strategy
    for all kinds of iterators, on all kinds of container modifying operations. This wasn't that hard (as it fell into a few cases), but it was some work.
    And it added a lot of template mixin mumbo-jumbo, just to get all containers tracking all their iterators.
  - It forced introduction of this concept of IteratorOwners (MAY still need that for other replacement debug checking - TBD - LGP 2021-10-05).
  - It added SIGNIFICANT (though never really measured - at least not yet) - performance overhead, due to the fact that all teh code to
    create iterators/and destroy them required adding and removing objects to a linked list and more importantly, that operation - at least as impleemented -
    required LOCKS (so big performance cost).
  - And due to the now prevalance of threads, and thread safety issues, this was just a very minor, and similar sort of problem. If the iterators weren't magically
    threadsafe, there was little value in other sorts of safety (use while iterating). The casea where thsi comes up are narrow.
  - EXAMPLE
    code like:
    ```
      for (view* v : s) {
          if (v->invisible)  {
              s.remove (v);
          }
      }
    ```
    must now be written as:  
    ```
      for (Iterator<view> i = s.begin() ; i != s.end (); ) {
          if ((*i))->invisible) {
            s.Remove (i, &i);
          }
          else {
            ++i;
          }
      }
    ```
    for NOW, failure to do this MAY result in an undetected error, but my PLAN is to add back similar logic, but only in DEBUG code
    for these sorts of cases.

    Note also, a good practice would be to do this instead:
    ```
      s.RemoveAll ([] (view* v) { return v->visible; });
    ```
