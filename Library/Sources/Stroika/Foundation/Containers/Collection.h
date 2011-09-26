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
 * Notes:
 *
 *
 *
 */


#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Memory/SharedByValue.h"

#include    "Iterator.h"





namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {



			// MAYBE SEPARATE MODULE FOR THIS
			template	<typename T>
				struct	TWithCompareEquals {
					inline	bool	opererator== (const T& lhs, const T& rhs)
						{
							return lhs == rhs;
						}
				};



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
					public:
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


						/*
						 *	DESCRIPTION:
						 *		Compact () has no operational semantics, except that if the object in question has allocated extra memory - in order to optimize future additions,
						 *	that extra memory is expunged. This function MAY do nothing. But - it may also take quite some time to accomplish, and save quite  bit of memory.
						 *
						 *
						 *	TODO:
						 *			NOTE SURE WE WANT THIS HERE??? PROBABLY NO. We DO want it - but maybe only in subtypes?
						 */
						nonvirtual	void	Compact ();


					// STL-style wrapper names
					public:
						nonvirtual	bool	empty () const;
						nonvirtual	size_t	length () const;
						nonvirtual	void	clear ();


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
						 *				return true;
						 *			}
						 *		}
						 *		return false;
						 *
						 *	However, in threading scenarios, this maybe preferable, since it counts as an atomic operation that will happen to each element without other
						 *	threads intervening to modify the container.
						 *
						 *	This function returns true iff doToElement ever returned true.
						 */
						nonvirtual	bool	ApplyUntil (bool (*doToElement) (T item)) const;


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



			template	<typename T, typename TTRAITS = TWithCompareEquals<T>>
				class	Collection::IRep {
					protected:
						IRep ();
					public:
	                    virtual ~IRep ();

					public:
						virtual	size_t		GetLength () const 						=	0;
						virtual	bool		IsEmpty () const 						=	0;
						virtual	bool		Contains (const T& item) const			=	0;
						virtual	void		RemoveAll () 							=	0;
						virtual	void		Compact ()								=	0;

					public:
						virtual	typename Memory::SharedByValue<Iterator<T>::Rep>	MakeIterator () const	=	0;
					public:
						virtual	void		Add (const T& item)						=	0;
						virtual	void		Remove (const T& item)					=	0;
				};


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

