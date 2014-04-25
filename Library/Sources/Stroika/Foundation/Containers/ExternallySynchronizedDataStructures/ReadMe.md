Overview
--------
	This module contains concrete, directly usable container data structures.

	These data structures are very similar - structurally - to those in STL.

	They
		-	are not threadsafe (externally synchonized)
		-	are not update during iteration safe (except as with STL)
		-	they are not organized around the Stroika Container abstractions/access patterns

	They are named based on the underlying data structure, not access pattern.

	These are NOT meant to be used DIRECTLY by programmers, EXCEPT in VERY narrow performance 
	intensive situations (code hot-spots).

	These can be neatly 'moved' into Stroika containers.

	These can be used to understand/review the underlying implementations of the normal
	Stroika containers.