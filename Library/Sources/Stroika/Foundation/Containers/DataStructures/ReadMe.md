Overview
--------
	This module contains concrete, directly usable container data structures.

	These data structures are very similar - structurally - to those in STL (using STL-style naming, patterns, etc).

	These data structures are all 'ranges aware' - and all valid ranges.

	They
		-	<a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a> (externally synchronized)
		-	they are not organized around the Stroika Container abstractions/access patterns
		-	they are primarily building blocks used in Stroika's concrete container library,
			though they can be used directly if someone wishes to

	They are named based on the underlying data structure, not access pattern.

	These are NOT meant to be used DIRECTLY by programmers, EXCEPT in VERY narrow performance 
	intensive situations (code hot-spots).

	These can be neatly 'moved' into Stroika containers.

	These can be used to understand/review the underlying implementations of the normal
	Stroika containers.

	These classes generally follow the c++ standard library pattern for container APIs.


## Strong Recommendation: size () is O(1)
---

	Every data structure here is strongly encouraged to provide a constant-time `size ()`, and for
	any data structure used as the BACKEND OF A STROIKA CONTAINER it should be treated as required.

	The reason is that the Stroika containers built on top of these (Sequence<T>, Set<T>,
	Mapping<T>, Collection<T>, ...) make that same guarantee to their callers, and they can only
	do so if their backend does. `size ()` is consulted by far more code than is obvious - Nth (),
	Top (n), Median (), index validation, and any pre-sizing path - so an O(N) `size ()` turns into
	an O(N) surprise well away from where it was introduced.

	It is a recommendation rather than a blanket rule because these classes are usable on their own,
	and there are designs where constant-time `size ()` is the wrong trade. The clearest is a
	lock-free or otherwise concurrent structure: an exact count needs a shared atomic counter, and
	that counter becomes a contention point serializing every mutation - which is why
	`std::forward_list` omits `size ()` entirely rather than offering a slow one. A structure used
	only standalone may reasonably make that choice. But if one is ever put under a Stroika
	container, the container must then document that ITS `size ()` is not O(1) - the guarantee has
	to break visibly, at the level callers see, rather than quietly.

	Where the count is not naturally available (the linked lists), cache it: one `size_t` per
	CONTAINER OBJECT - not per node - plus an increment/decrement in each mutator. Verify the cache
	against a walk of the elements in the class's `Invariant_ ()`, so a mutator that forgets to
	maintain it fails loudly in debug builds instead of drifting silently. (Both linked lists gained
	exactly that check, and it immediately caught a missed decrement in `RemoveFirst ()`.)

	Currently all provided containers have an O(1) size () and any that deviate from this better
	have a good reason and clearly HIGHLIGHT this in its class docs.


## Supported Data Structures
---

- [Array\<T>](Array.h)
  - basic dynamically sized array type (similar to std::vector)

- [DoublyLinkedList\<T>](DoublyLinkedList.h)
  - standard non-intrusive doubly linked list
  
- [HashTable\<KEY_TYPE, MAPPED_TYPE, TRAITS>](HashTable.h)
  - hash table implementation (open hashing, or separate chaining)
  
- [LinkedList\<T>](LinkedList.h)
  - standard singly linked non-intrusive linked list
  
- [SkipList\<KEY_TYPE, MAPPED_TYPE, TRAITS>](SkipList.h)
  - This is an ordered linked list, with most of the performance behavior of a b-tree, O(log N) inserts etc, but with more
	flexibility and tunability, and perhaps less memory overhead

- [STLContainerWrapper\<STL_CONTAINER_OF_T>](STLContainerWrapper.h)
  - This module generically wraps STL containers (such as map, vector etc), and facilitates using them as backends for Stroika containers
