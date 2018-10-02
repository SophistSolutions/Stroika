TODO (Foundation::Containers)


	(o)		Performance tweak: where its costly to allocate a mutex (in each iterable<> wrapper object)
			allocate it as a free pool of objects (a bit like the block-allocated code but they remain
			constructed and I don't make an array of them).

	(o)		In PatchingDataStructures - we do more or less the same thing on top of each basic
			cotnainer. Especially code to track iterators.

			Try writing  a GENERIC helper layer - that can be used to share code among these
			various Patching implementations.

			Do that BEFORE trying to incorporate SkipLists and other sterl stuff, as it will make doing
			so easier.

	(o)		Rep_::CloneEmpty () - is needlessly inefficient, for each concrete type, but only for the case of during iteration.
			Not a biggie, but probably worth fixing. Issue is our existing code to do the move of running iterators
			ALSO does copy of data.

	(o)		IMPORTANT to do with Comparers - like EqualsComparer. We need to be able to pass in 
			object (so they work with lambdas - with closures), and store no space when there is
			no space used (what we do now).

			Implement with SFINAE

	(o)		Should we have a SortedBijection<> template? If not - document why not. I think
			yes.

	(o)		Should Mapping<> have mehtods Domain() and Range () - like Bijection?

	(o)		Mapping<> really is more like math function, and Association<> is really more
			like math mapping.

	(o)		Consider moving the LOCKING code into the Patching classes? Maybe using TRAITS???

	(o)		Containers must store linked list of OwnerIDs in each rep, DEBUG ONLY - just for the
			purpose of assertion checking. Then add that assertion checking (that all contained
			iterators - active iterators - belong to the given reps owners - at least one of
			them).

	(o)		Reconsider the question of operator overloading for operator==, operator!=, operator+, operator-,
			and perhaps others.
	
			Question is - nested inside class, or 'global'. Ignoring the issue of namespaces, global seems to
			probably work best.

			But taking into account namespaces - which we must - it appears - direct members work best.

			Go with Direct memebers for now - and DOCUMENT this SOMEPLACE as an overarching design choice.
			But reconsider possibly...
					-- LGP 2013-05-19

	(o)		Maybe enhance API for stuff like PriorityQueue<> and MultiSet<T> to have method
			AsIterable<T> () - that returns an Iterable<T> (the undlering thing stuff is ta tallyof or
			priorityq of, so you can write for (i : priqiryQ.AsIterable<T> ()) {}
			AsIterable<T> would return a logical COPY of the container (but using smart pointers not cfost
			much of anything) - and would let you make iterators you could traverse with. NOT for 
			Mapping/Dictionary - there have method Keys() that returns same thing (container of keys)

			Iterable<T> AsIterable<T> ()

	(o)		Fix the crazy extra operator++ funky stuff needed in the MakeIterator calls.

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

	(o)		Suggestion from kdj (around 2013-01-15)
			http://google-opensource.blogspot.com/2013/01/c-containers-that-save-memory-and-time.html 

	(o)		Consider using optimistic looking implementations - perhaps using an STM library or 
			somehow the ideas of STM (see http://en.wikipedia.org/wiki/Software_transactional_memory).

			This is especially important here, but also useful to explore through the rest of Stroika

	(o)		Maybe implement Treaps for Set<>, Mapping<>, Dictionary<>, Sorting<>?? using Treaps:
			http://www.cs.cornell.edu/Courses/cs312/2003sp/lectures/lec26.html


	(o)		Consider adding another category of containers - "OrderPreserving" - like "Sorted" - but
			where additions done later than earlier additions come out in the same relative 
			iteration order as how they were added.

			A doubly linked list could be used for this (cuz easy to append and find from front). Hard to
			do and make efficient for large sets. But COULD be handy sometimes for things like
			URLQuery class IF there was a desire to make a few changes, but otherwise preserve the order
			of entries. MAYBE defer for Stroika v3?
			

Notes from obsolete Collection.h
	TRAITS:
		Consider using
				typename TTRAITS = TWithCompareEquals<T>
			as in
					template    <typename T, typename TTRAITS = TWithCompareEquals<T>>
					class   Collection {
					protected:
						class   IRep;


	COLLECTION DOCS:
        /*
            * Description:
            *      A Collection<T,TTRAITS> is a container class representing a bunch of objects of type T. Here the word 'bunch' is chosen to mean
            *  almost nothing. The details of meaning for what happens when something is put into a Collection<T,TTRAITS> (like if duplicates are retained or not)
            *  depends on subtypes of Collection<T,TTRAITS>.
            *
            *      Note also that this code uses a Letter-Envelope paradigm - mostly in order to achieve greater performance with less effort. The actual
            *  subclassing is done with the Reps, and the subclassing relationships presented with the envelopes or mostly for documentation purposes.
            *
            *      Note that Collections and Iterators are INTIMATELY related. The definition of most of the Collection methods is in terms of what would happen
            *  when you iterate.
            *
            *      CONCEPT (I HAVENT READ ABOUT THIS - SO I DON'T KNOW THE RIGHT SYNTAX - IM FAKING IT)
            *          Concept<T>  =   TWithCompareEquals<T>, or pass default TTRAITS param with operator==
            */

	MUTABLE<T>:
	    LIKE we have with ITERABLE, cosndier adding MUTABLE<T>.


	AddAll/RemoveAll
		Review each container. I think I did a DECENT job of this with Sequence::AppendAll() versus Sequence::Append(). Do
		as appropriate for each container type, and documetn the 'addallish' semantics of each appropriate CTOR.

	RetainAll
            public:
                /*
                 *  This calls Remove (item) for each element of the this collection not found in the argument 'items' collection.
                 *  {NOTE - from Java collection::retainAll() - not sure this is a good idea - hard todo fast if items not a 'set' type}
                 *  {NOTE- I THINK THIS AMOUTNS TO INSERSECT-WITH  - and that maybe a better name. But also - CNA be done effecitaly
                 *      even for collection using conerter Set<T> on items - as impl - so if already a set - fast - and if not - not too bad to constrct
                 *      tmp set.
                 */
                nonvirtual  void    RetainAll (const Collection<T, TTRAITS>& items);

	RemoveIfPresent:
                /*
                 *  This function is logically equivalent to
                 *      if (Contains (item)) {
                 *          Remove (item);
                 *      }
                 *
                 *      You might want to use it in threading scenariors to safely remove something someone else might be trying to remove at the same time. If using
                 *  a 'safe' container, this will always be atomic.
                 */
                nonvirtual  void    RemoveIfPresent (T item);
                /*
                 *  This calls RemoveIfPresent (item) for each element of the Items collection.
                 */
                nonvirtual  void    RemoveIfPresent (const Collection<T, TTRAITS>& items);


                /*
                 *      +=/-= are equivalent Add() and Remove(). They
                 *  are just syntactic sugar.
                 */
            public:
                nonvirtual  Collection<T, TTRAITS>&  operator+= (T item);
                nonvirtual  Collection<T, TTRAITS>&  operator+= (const Collection<T, TTRAITS>& items);
                nonvirtual  Collection<T, TTRAITS>&  operator-= (T item);
                nonvirtual  Collection<T, TTRAITS>&  operator-= (const Collection<T, TTRAITS>& items);

	Global EQUALS and PLUS and MINUS operators:
            /*
             *  Two collections are Equal, if iterating over each would produce the same results (in the same order)
             */
            template    <typename T, typename TTRAITS = TWithCompareEquals<T>>
            bool    operator== (const Collection<T, TTRAITS>& lhs, const Collection<T, TTRAITS>& rhs);
            template    <typename T, typename TTRAITS = TWithCompareEquals<T>>
            bool    operator!= (const Collection<T, TTRAITS>& lhs, const Collection<T, TTRAITS>& rhs);

            /*
             *  Here operator+ is defined in the obvious way, using the Collection<T,TTRAITS>::Add () method
             */
            template    <typename T, typename TTRAITS = TWithCompareEquals<T>>
            Collection<T, TTRAITS>   operator+ (const Collection<T, TTRAITS>& lhs, const Collection<T, TTRAITS>& rhs);

            /*
             *  Here operator+ is defined in the obvious way, using the Collection<T,TTRAITS>::Remove () method
             */
            template    <typename T, typename TTRAITS = TWithCompareEquals<T>>
            Collection<T, TTRAITS>   operator- (const Collection<T, TTRAITS>& lhs, const Collection<T, TTRAITS>& rhs);
		(unclear if these should be done per container type - eg for mapping<> etc - or globally?)

	Container-Factories:
		>	DOCUMENT well (in common container section) Container factory strategy.



