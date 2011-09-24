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
 *		(o)		Very rough draft - not ready for Sterl to REVIEW
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



			/*
			 * TODO:
			 *		START by create a 'base class' with all the common stuff fro all contaers. EVENTUALLY also merge
			 *		the ITERATOR stuff here too...
			 */
			template	<typename T>
				class	Collection {
					public:
						class	IRep;

					protected:
						Collection (const Memory::SharedByValue<IRep<T>>& rep);

					public:
						nonvirtual	size_t	GetLength () const;
						nonvirtual	bool	IsEmpty () const;
						nonvirtual	bool	Contains (T item) const;
						nonvirtual	void	RemoveAll ();
						nonvirtual	void	Compact ();

					// STL-style wrapper names
					public:
						nonvirtual	bool	empty () const;
						nonvirtual	size_t	length () const;
						nonvirtual	void	clear ();

					public:
						nonvirtual	void	Add (T item);
						nonvirtual	void	Add (const Collection<T>& items);

					/*
					 * It is legal to remove something that is not there (but not necessarily legal for all subtypes)
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
						nonvirtual	Collection<T>&	operator+= (const Collection<T>& items);
						nonvirtual	Collection<T>&	operator-= (T item);
						nonvirtual	Collection<T>&	operator-= (const Collection<T>& items);

					private:
						Memory::SharedByValue<IRep<T> >	fRep;
				};


            template	<typename T>
				bool	operator== (const Collection<T>& lhs, const Collection<T>& rhs);
            template	<typename T>	
				bool	operator!= (const Collection<T>& lhs, const Collection<T>& rhs);
			
            template	<typename T>	
				Collection<T>	operator+ (const Collection<T>& lhs, const Collection<T>& rhs);
            template	<typename T>	
				Collection<T>	operator- (const Collection<T>& lhs, const Collection<T>& rhs);


            template	<typename T>	
				class	Collection::IRep {
					protected:
						IRep ();
	                    virtual ~IRep ();

                public:
                    virtual	size_t		GetLength () const 				=	0;
                    virtual	bool		Contains (T item) const			=	0;
                    virtual	void		RemoveAll () 					=	0;
                    virtual	void		Compact ()						=	0;

                public:
                    virtual	void		Add (T item)					=	0;
                    virtual	void		Remove (T item)					=	0;
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

