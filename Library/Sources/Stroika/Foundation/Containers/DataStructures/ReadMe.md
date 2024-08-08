Overview
--------
	This module contains concrete, directly usable container data structures.

	These data structures are very similar - structurally - to those in STL (using STL-style naming, patterns, etc).

	These data structures are all 'ranges aware' - and all valid ranges.

	They
		-	are not <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a> (externally synchronized)
		-	they are not organized around the Stroika Container abstractions/access patterns
		-	they are primarily building blocks used in Stroika's concrete container library,
			though they can be used directly if someone wishes to

	They are named based on the underlying data structure, not access pattern.

	These are NOT meant to be used DIRECTLY by programmers, EXCEPT in VERY narrow performance 
	intensive situations (code hot-spots).

	These can be neatly 'moved' into Stroika containers.

	These can be used to understand/review the underlying implementations of the normal
	Stroika containers.

## Supported Data Structures
---

- [Array\<T>](Array.h)
  - basic dynamically sized array type (similar to std::vector)
