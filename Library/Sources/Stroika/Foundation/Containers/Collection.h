/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Collection_h_
#define	_Stroika_Foundation_Containers_Collection_h_	1


/*
 *
 * Description:
 *
 *		
 *
 * TODO:
 *
 *		(o)		Rough draft - ready for Sterl to REVIEW
 *
 *		(o)		Unclear if we should use MUTATORS or not here. Probably YES.
 *
 *		(o)		Since Iterator is so closely related to Collection<T,TTRAITS>, maybe it should be in the same file?
 *
 *		(o)		MUST GO THRU WRAPPER FOR APPLY – MUST BE DONE INSIDE VIRTUAL REP!!! - works as is, but to get right performance/threading behavior - put inside virtual rep!
 *				MAYBE true for some of the otehrs as well? If so - maybe write PRIVATE COLLECTIONHELPERREP class todo the gruntwork of implemetning that stufff. Or do
 *				it inline in Colleciton<T>::Rep. Not sure which is better/worse?
 *
 * Notes:
 *
 *
 *
 */


#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Memory/SharedByValue.h"

#include	"CollectionElementTraits.h"
#include    "Iterator.h"





namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {



			/*
			 * Description:
			 *		A Collection<T,TTRAITS> is a container class representing a bunch of objects of type T. Here the word 'bunch' is chosen to mean
			 *	almost nothing. The details of meaning for what happens when something is put into a Collection<T,TTRAITS> (like if duplicates are retained or not)
			 *	depends on subtypes of Collection<T,TTRAITS>.
			 *
			 *		Note also that this code uses a Letter-Envelope paradigm - mostly in order to achieve greater performance with less effort. The actual
			 *	subclassing is done with the Reps, and the subclassing relationships presented with the envelopes or mostly for documentation purposes.
			 *
			 *		Note that Collections and Iterators are INTIMATELY related. The definition of most of the Collection methods is in terms of what would happen
			 *	when you iterate.
			 *
			 *		CONCEPT (I HAVENT READ ABOUT THIS - SO I DONT KNOW THE RIGHT SYNTAX - IM FAKING IT)
			 *			Concept<T>	=	TWithCompareEquals<T>, or pass default TTRAITS param with operator==
			 */
			template	<typename T, typename TTRAITS = TWithCompareEquals<T>>
				class	Collection {
					protected:
						class	IRep;

					protected:
						explicit Collection (const Memory::SharedByValue<IRep<T,TTRAITS>>& rep);

					public:
						/*
						 *	Get length of a collection is the number of elements you would encounter when iterating over it.
						 */
						nonvirtual	size_t	GetLength () const;
						
						/*
						 *	IsEmpty () is equivilent to GetLength () == 0 (but can be more efficient).
						 */
						nonvirtual	bool	IsEmpty () const;

						/*
						 *	Contains () is true iff an iteration over the Collection<T,TTRAITS> would return at least one element such that (*it == item).
						 */
						nonvirtual	bool	Contains (T item) const;

						/*
						 *	Sets the length to zero (see GetLength ()).
						 */
						nonvirtual	void	RemoveAll ();


#if 0
						/*
						 *	DESCRIPTION:
						 *		Compact () has no operational semantics, except that if the object in question has allocated extra memory - in order to optimize future additions,
						 *	that extra memory is expunged. This function MAY do nothing. But - it may also take quite some time to accomplish, and save quite  bit of memory.
						 *
						 *
						 *	TODO:
						 *			NOTE SURE WE WANT THIS HERE??? PROBABLY NO. We DO want it - but maybe only in subtypes?
	<<AS OF EMAIL DISCUSSION WITH STERL 2011-09-28 - I lean HEAVILY towards getting rid of this. JUST DO IT with 
						Bag_Compact<T> () or Sequnece_Compact<T>()...
						 */
						nonvirtual	void	Compact ();
#endif


					// STL-style wrapper names
					public:
						nonvirtual	bool	empty () const;
						nonvirtual	size_t	length () const;
						nonvirtual	void	clear ();


					public:
						/*
						 * This function returns true iff the collections are - by value - equal. 
						 *
						 *This at least conceptually involves iterating
						 * over each item, and seeing if the results are the same.
								{{{ SEE EMIAL DISCUSSION WITH STERL - NOT ADEUqte dfeinfiotn}}}
						 *
						 *	Two containers are considered Equal if they the can be expected to give the exact same sequence of results
						 *	when iterating over their contents.
						 *
						 *	Note - this does NOT necessarily mean they will give the same results, because some amount of non-deterinism is allowed
						 *	in iteration. For example, if you have an abstract collection of random numbers (us Collection to represent a sequnce of a 
						 *	billion random numbers) - and then make a copy of that sequence (just incrementing a reference count). Intuitively one
						 *	would consdier those two Collections<> equal, and Stroika will report them as such.  But when you go to iterate over each
						 *	they might produce different answers.
						 *
						 *	For most normal cases - Arrays, Linked Lists, etc, this is a 'too subtle' and 'academic' distinction, and the obvious
						 *	interpetation applies (you iterate over both - in order - and get the same answer).
						 */
						nonvirtual	bool	Equals (const Collection<T>& rhs) const;

					public:
						/*
						 * This function returns an iterator whcih can be used to walk over the items in teh given collection.
						 */
						nonvirtual	typename Iterator<T>	MakeIterator () const;

					public:
						/*
						 *	begin ()/end() are similar to MakeIterator(), except that they allow for iterators to be used in an STL-style, which is critical
						 *	for using C++ ranged iteration. For example:
						 *
						 *		for (Iterator<T> i = c.begin (); i != c.end (); ++i) {
						 *			if (*i = T ()) {
						 *				break;
						 *			}
						 *		}
						 *
						 * OR
						 *		for (T& i : c) {
						 *			if (*i = T ()) {
						 *				break;
						 *			}
						 *		}
						 *
						 */
						 */
						nonvirtual	typename Iterator<T>	begin () const;
						nonvirtual	typename Iterator<T>	end () const;

					public:
						/*
						 *	The item argument should appear in iterations over the Collection after this method is called.
						 */
						nonvirtual	void	Add (T item);
						/*
						 *	This calls Add (item) for each element of the Items collection.
						 */
						nonvirtual	void	Add (const Collection<T,TTRAITS>& items);

					public:
						/*
						 *	This meaning of this will vary significantly across collections. It may (but will not necessarily) reduce the size of the collection.
						 *	However, it MUST modify the collection (or assert out). It is a PRECONDITION that Contains (item) - before you can remove it.
						 */
						nonvirtual	void	Remove (T item);
						/*
						 *	This calls Remove (item) for each element of the Items collection.
						 */
						nonvirtual	void	Remove (const Collection<T,TTRAITS>& items);


					public:
						/*
						 *	This calls Remove (item) for each element of the this collection not found in the argument 'items' collection.
						 *	{NOTE - from Java collection::retainAll() - not sure this is a good idea - hard todo fast if items not a 'set' type}
						 *	{NOTE- I THINK THIS AMOUTNS TO INSERSECT-WITH  - and that maybe a better name. But also - CNA be done effecitaly 
						 *		even for collection using conerter Set<T> on items - as impl - so if already a set - fast - and if not - not too bad to constrct
						 *		tmp set.
						 */
						nonvirtual	void	RetainAll (const Collection<T,TTRAITS>& items);

					public:
						/*
						 *	This function is logically equivilent to
						 *		if (Contains (item)) {
						 *			Remove (item);
						 *		}
						 *
						 *		You might want to use it in threading scenariors to safely remove something someone else might be trying to remove at the same time. If using
						 *	a 'safe' container, this will always be atomic.
						 */
						nonvirtual	void	RemoveIfPresent (T item);
						/*
						 *	This calls RemoveIfPresent (item) for each element of the Items collection.
						 */
						nonvirtual	void	RemoveIfPresent (const Collection<T,TTRAITS>& items);


					/*
					 *		+=/-= are equivilent Add() and Remove(). They
					 *	are just syntactic sugar.
					 */
					public:
						nonvirtual	Collection<T,TTRAITS>&	operator+= (T item);
						nonvirtual	Collection<T,TTRAITS>&	operator+= (const Collection<T,TTRAITS>& items);
						nonvirtual	Collection<T,TTRAITS>&	operator-= (T item);
						nonvirtual	Collection<T,TTRAITS>&	operator-= (const Collection<T,TTRAITS>& items);


					public:
						/*
						 * Take the given function argument, and call it for each element of the Collection. This is equivilent to:
						 *		for (Iterator<T> i = begin (); i != end (); ++i) {
						 *			(doToElement) (item);
						 *		}
						 *
						 *	However, in threading scenarios, this maybe preferable, since it counts as an atomic operation that will happen to each element without other
						 *	threads intervening to modify the container.
						 */
						nonvirtual	void	Apply (void (*doToElement) (T item)) const;

					public:
						/*
						 * Take the given function argument, and call it for each element of the Collection. This is equivilent to:
						 *		for (Iterator<T> i = begin (); i != end (); ++i) {
						 *			if ((doToElement) (item)) {
						 *				return it;
						 *			}
						 *		}
						 *		return end();
						 *
						 *	However, in threading scenarios, this maybe preferable, since it counts as an atomic operation that will happen to each element without other
						 *	threads intervening to modify the container.
						 *
						 *	This function returns an iteartor pointing to the element that triggered the abrupt loop end (for example the element you were searching for?).
						 *	It returns the specail iterator end () to indicate no doToElemet() functions returned true.
						 */
						nonvirtual	typename Iterator<T>	ApplyUntil (bool (*doToElement) (T item)) const;


					private:
						Memory::SharedByValue<IRep<T,TTRAITS>>	fRep_;
				};



			/* 
		     *	Two collections are Equal, if iterating over each would produce the same results (in the same order)
			 */
			template	<typename T, typename TTRAITS = TWithCompareEquals<T>>
				bool	operator== (const Collection<T,TTRAITS>& lhs, const Collection<T,TTRAITS>& rhs);
			template	<typename T, typename TTRAITS = TWithCompareEquals<T>>
				bool	operator!= (const Collection<T,TTRAITS>& lhs, const Collection<T,TTRAITS>& rhs);
			
			/* 
		     *	Here operator+ is defined in the obvious way, using the Collection<T,TTRAITS>::Add () method
			 */
			template	<typename T, typename TTRAITS = TWithCompareEquals<T>>
				Collection<T,TTRAITS>	operator+ (const Collection<T,TTRAITS>& lhs, const Collection<T,TTRAITS>& rhs);

			/* 
		     *	Here operator+ is defined in the obvious way, using the Collection<T,TTRAITS>::Remove () method
			 */
			template	<typename T, typename TTRAITS = TWithCompareEquals<T>>
				Collection<T,TTRAITS>	operator- (const Collection<T,TTRAITS>& lhs, const Collection<T,TTRAITS>& rhs);


		}
    }
}
#endif	/*_Stroika_Foundation_Containers_Collection_h_ */



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Collection.inl"

