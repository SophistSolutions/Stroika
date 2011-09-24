/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Bag_h_
#define	_Stroika_Foundation_Containers_Bag_h_	1


/*
 *
 * Description:
 *
 *		A Bag is the simplest kind of collection. It allows addition and
 *	removal of elements, but makes no guarantees about element ordering. Two
 *	bags are considered equal if they contain the same items, even if iteration
 *	order is different.
 *
 *		Bags are typically designed to optimize item addition and iteration.
 *	They are fairly slow at item access (as they have no keys). Removing items
 *	is usually slow, except in the context of a BagMutator, where it is usually
 *	very fast. Bag comparison (operator==) is often very slow in the worst
 *	case (n^2) and this worst case is the relatively common case of identical
 *	bags.
 *
 *		Although Bag has an TallyOf () method, it is nonvirtual, and therefore
 *	not optimized for the various backends. There is a separate class, Tally,
 *	for cases where you are primarily interested in keeping an summary count
 *	of the occurences of each item.
 *
 *		Bags allow calls to Remove with an item not contained within the bag.
 *
 *		As syntactic sugar, using either functional (Add, Remove) or
 *	operator (+,-) is allowed.
 *
 *
 * TODO:
 *		+	Do CTOR () that takes ITERATOR<T> - but not til after next release....
 *
 *		+	Have Bag_Difference/Union/Interesection??? methods/?? Do research....
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

            template	<typename T>	class	Bag;
            template	<typename T>	class	BagRep;
            template	<typename T>	class	BagIteratorRep;
            template	<typename T>	class	BagMutatorRep;

            template	<typename T>	bool	operator== (const Bag<T>& lhs, const Bag<T>& rhs);
            template	<typename T>	bool	operator!= (const Bag<T>& lhs, const Bag<T>& rhs);


            template	<typename T>	class	BagIterator : public Iterator<T> {
                public:
                    BagIterator (BagIteratorRep<T>* it);
            };

            template	<typename T>	class	BagMutator : public Iterator<T> {
                public:
                    BagMutator (BagMutatorRep<T>* it);

                public:
                    void	RemoveCurrent ();
                    void	UpdateCurrent (T newValue);

                protected:
                    nonvirtual  BagMutatorRep<T>*	GetMutatorRep ();
            };

            template	<typename T>	class	Bag {
                public:
                    Bag ();
                    Bag (const Bag<T>& bag);
                    Bag (const T* items, size_t size);

                protected:
                    Bag (BagRep<T>* rep);

                public:
                    nonvirtual	Bag<T>& operator= (const Bag<T>& bag);

                    nonvirtual	size_t	GetLength () const;
                    nonvirtual	bool	IsEmpty () const;
                    nonvirtual	bool	Contains (T item) const;
                    nonvirtual	void	RemoveAll ();
                    nonvirtual	void	Compact ();
                    nonvirtual	operator Iterator<T> () const;

                public:
                    nonvirtual	void	Add (T item);
                    nonvirtual	void	Add (Bag<T> items);


                /*
                 * It is legal to remove something that is not there.
                 */
                public:
                    nonvirtual	void	Remove (T item);
                    nonvirtual	void	Remove (const Bag<T>& items);

                // Have Bag_Difference/Union/Interesection??? methods/??


                /*
                 *		+=/-= are equivilent Add() and Remove(). They
                 *	are just syntactic sugar.
                 */
                public:
                    nonvirtual	Bag<T>&	operator+= (T item);
                    nonvirtual	Bag<T>&	operator+= (const Bag<T>& items);
                    nonvirtual	Bag<T>&	operator-= (T item);
                    nonvirtual	Bag<T>&	operator-= (const Bag<T>& items);


                /*
                 *	Build BagIterators or Mutators.
                 */
                public:
                    nonvirtual	operator BagIterator<T> () const;
                    nonvirtual	operator BagMutator<T> ();

                    // Support for ranged for, and stl syntax in general
                    nonvirtual  Iterator<T> begin () const;
                    nonvirtual  Iterator<T> end () const;

					// support for non-default iterator. By default, we iterator with Bag_Iterator<T>
					typedef	RangedForIterator<Bag<T>, BagMutator<T> >		Mutator;

                public:
                    nonvirtual	size_t	TallyOf (T item) const;


                protected:
                    nonvirtual	void	AddItems (const T* items, size_t size);

                    // Are these allowed to return 0??? If not, add asserts!!!
                    nonvirtual	const BagRep<T>*	GetRep () const;
                    nonvirtual	BagRep<T>*			GetRep ();

                private:
                    Memory::SharedByValue<BagRep<T> >	fRep;

                    static	BagRep<T>*	Clone (const BagRep<T>& rep);

                    // SSW 9/16/2011: note weird syntax needed for friend declaration (and had to be forwarded above)
                	friend  bool	operator==<T> (const Bag<T>& lhs, const Bag<T>& rhs);	// friend to check if reps equal...
            };


            template	<typename T>	Bag<T>	operator+ (const Bag<T>& lhs, const Bag<T>& rhs);
            template	<typename T>	Bag<T>	operator- (const Bag<T>& lhs, const Bag<T>& rhs);



            template	<typename T>	class	BagIteratorRep : public IteratorRep<T> {
                protected:
                    BagIteratorRep ();
            };

            template	<typename T>	class	BagMutatorRep : public BagIteratorRep<T> {
                protected:
                    BagMutatorRep ();

                public:
                    virtual	void	RemoveCurrent () 			=	0;
                    virtual	void	UpdateCurrent (T newValue) 	=	0;
            };

            template	<typename T>	class	BagRep {
                protected:
                    BagRep ();

                public:
                    virtual ~BagRep ();

                public:
                    virtual	BagRep<T>*	Clone () const					=	0;
                    virtual	bool		Contains (T item) const			=	0;
                    virtual	size_t		GetLength () const 				=	0;
                    virtual	void		Compact ()						=	0;
                    virtual	void		RemoveAll () 					=	0;

                    virtual	void		Add (T item)					=	0;
                    virtual	void		Remove (T item)					=	0;

                    virtual	IteratorRep<T>*		MakeIterator () 		=	0;
                    virtual	BagIteratorRep<T>*	MakeBagIterator () 		=	0;
                    virtual	BagMutatorRep<T>*	MakeBagMutator () 		=	0;
            };

		}
    }
}


#endif	/*_Stroika_Foundation_Containers_Bag_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Bag.inl"

