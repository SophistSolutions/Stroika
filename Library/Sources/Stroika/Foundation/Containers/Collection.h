/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Collection_h_
#define	_Stroika_Foundation_Containers_Collectionh_	1


/*
 *
 * Description:
 *
 *		
 *
 * TODO:
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

            template	<typename T>	class	Collection;
            template	<typename T>	class	CollectionRep;
            template	<typename T>	class	CollectionIteratorRep;
            template	<typename T>	class	CollectionMutatorRep;

            template	<typename T>	bool	operator== (const Collection<T>& lhs, const Collection<T>& rhs);
            template	<typename T>	bool	operator!= (const Collection<T>& lhs, const Collection<T>& rhs);


            template	<typename T>	class	CollectionIterator : public Iterator<T> {
                public:
                    CollectionIterator (CollectionIteratorRep<T>* it);
            };

            template	<typename T>	class	CollectionMutator : public Iterator<T> {
                public:
                    CollectionMutator (BagMutatorRep<T>* it);

                public:
                    void	RemoveCurrent ();
                    void	UpdateCurrent (T newValue);

                protected:
                    nonvirtual  CollectionMutatorRep<T>*	GetMutatorRep ();
            };

            template	<typename T>	class	Collection {
                protected:
                    Collection (const Collection<T>& collection);

                protected:
                    Collection (CollectionRep<T>* rep);

                public:
                    nonvirtual	Collection<T>& operator= (const Collection<T>& collection);

                    nonvirtual	size_t	GetLength () const;
                    nonvirtual	bool	IsEmpty () const;
                    nonvirtual	bool	Contains (T item) const;
                    nonvirtual	void	RemoveAll ();
                    nonvirtual	void	Compact ();
                    nonvirtual	operator Iterator<T> () const;

                public:
                    nonvirtual	void	Add (T item);
                    nonvirtual	void	Add (const Collection<T>& items);


                /*
                 * It is legal to remove something that is not there.
                 */
                public:
                    nonvirtual	void	Remove (T item);
                    nonvirtual	void	Remove (const Collection<T>& items);

 
                /*
                 *		+=/-= are equivilent Add() and Remove(). They
                 *	are just syntactic sugar.
                 */
                public:
                    nonvirtual	Collection<T>&	operator+= (T item);
                    nonvirtual	Collection<T>&	operator+= (const Bag<T>& items);
                    nonvirtual	Collection<T>&	operator-= (T item);
                    nonvirtual	Collection<T>&	operator-= (const Bag<T>& items);


                /*
                 *	Build BagIterators or Mutators.
                 */
                public:
                    nonvirtual	operator CollectionIterator<T> () const;
                    nonvirtual	operator CollectionMutator<T> ();

                    // Support for ranged for, and stl syntax in general
                    nonvirtual  Iterator<T>    begin () const;
                    nonvirtual  IterationState end () const;

                private:
                    Memory::SharedByValue<CollectionRep<T> >	fRep;
            };


            template	<typename T>	Collection<T>	operator+ (const Collection<T>& lhs, const Collection<T>& rhs);
            template	<typename T>	Collection<T>	operator- (const Collection<T>& lhs, const Collection<T>& rhs);



            template	<typename T>	class	CollectionIteratorRep : public IteratorRep<T> {
                protected:
                    BagIteratorRep ();
            };

            template	<typename T>	class	CollectionMutatorRep : public BagIteratorRep<T> {
                protected:
                    CollectionMutatorRep ();

                public:
                    virtual	void	RemoveCurrent () 			=	0;
                    virtual	void	UpdateCurrent (T newValue) 	=	0;
            };

            template	<typename T>	class	CollectionRep {
                protected:
                    CollectionRep ();

                public:
                    virtual ~CollectionRep ();

                public:
                    virtual	BagRep<T>*	Clone () const					=	0;
                    virtual	bool		Contains (T item) const			=	0;
                    virtual	size_t		GetLength () const 				=	0;
                    virtual	void		Compact ()						=	0;
                    virtual	void		RemoveAll () 					=	0;

                    virtual	void		Add (T item)					=	0;
                    virtual	void		Remove (T item)					=	0;

                    virtual	IteratorRep<T>*		MakeIterator () 		=	0;
                    virtual	CollectionIteratorRep<T>*	MakeCollectionIterator () 		=	0;
                    virtual	CollectionMutatorRep<T>*	MakeCollectionMutator () 		=	0;
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
#include	"Collection.inl"

