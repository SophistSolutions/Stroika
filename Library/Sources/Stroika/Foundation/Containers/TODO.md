TODO (Foundation::Containers)


	(o)		Maybe enhance API for stuff like PriorityQueue<> and Tally<T> to have method
			AsIterable<T> () - that returns an Iterable<T> (the undlering thing stuff is ta tallyof or
			priorityq of, so you can write for (i : priqiryQ.AsIterable<T> ()) {}
			AsIterable<T> would return a logical COPY of the container (but using smart pointers not cfost
			much of anything) - and would let you make iterators you could traverse with. NOT for 
			Mapping/Dictionary - there have method Keys() that returns same thing (container of keys)

			Iterable<T> AsIterable<T> ()

	(o)		Fix (already documented in Iterator.h todo) T fCurrent stuff - so no default CTOR required,
			and then fix the crazy extra operator++ funky stuff needed in the MakeIterator calls.

	(o)		Probably ready todo Sequence<> next.

	(o)		Seriously consider implementing all of Container code using
			http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3341.pdf
			STM
			Do macros which map to regular locks when stm not available, but using GCC experimental STM 
			when available

	(o)		CONCEPTS: - consider using bits/boost_concepts - or at least assume they have the right names
			to use?
		(o)		See comment "Consider adding class TWithCompareEquals<T> to add Iterable<T> like functions -
				where we can count on "T".
				Perhaps implement with a Require (TWithCompareEquals<T>) in CTORs for class?".

				Do this. See design spec for CONCEPTS (rejected for C++11 but probably in C++next).
				Maybe just do as DOCS. OR maybe real classes. Note - looking at the gcc 4.5.x implementation 
				of libstdc++, it appears they already do something similar.

	(o)		When this code matures (when we have most containers implemented to first draft level),
			plan is to OBSOLETE/DELETE the Collection code. Make sure the appropraite APIs from
			that code are migrated into the appropriate containers, and see if any additional
			abstractions (like Iterable<T>) are appropriate to extract from Colleciton<T> to
			make shared/common bases.

	(o)		Suggestion from kdj (around 2013-01-15)
			http://google-opensource.blogspot.com/2013/01/c-containers-that-save-memory-and-time.html 

	(o)		Consider using optimistic looking implementations - perhaps using an STM library or 
			somehow the ideas of STM (see http://en.wikipedia.org/wiki/Software_transactional_memory).

			This is especailly important here, but also useful to explore through the rest of Stroika

	(o)		Maybe implement Treaps for Set<>, Mapping<>, Dictionary<>, Sorting<>?? using Treaps:
			http://www.cs.cornell.edu/Courses/cs312/2003sp/lectures/lec26.html
