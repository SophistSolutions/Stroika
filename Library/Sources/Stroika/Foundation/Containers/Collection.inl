/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_inl_
#define _Stroika_Foundation_Containers_Collection_inl_

#include	"../Debug/Assertions.h"




namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {



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

					public:
						virtual	bool		Equals (const Memory::SharedByValue<Collection<T,TRAITS>::IRep>& rhs) const 		=	0;

					public:
						virtual	typename Memory::SharedByValue<Iterator<T>::Rep>	MakeIterator () const						=	0;

					public:
						virtual	void		Add (const T& item)																	=	0;
						virtual	void		Add (const Memory::SharedByValue<Collection<T,TRAITS>::IRep>& item)					=	0;
						virtual	void		Remove (const T& item)																=	0;
						virtual	void		Remove (const Memory::SharedByValue<Collection<T,TRAITS>::IRep>& item)				=	0;
						virtual	void		RetainAll (const Memory::SharedByValue<Collection<T,TRAITS>::IRep>& item)			=	0;
						virtual	void		RemoveIfPresent (const T& item)														=	0;
						virtual	void		RemoveIfPresent (const Memory::SharedByValue<Collection<T,TRAITS>::IRep>& item)		=	0;

					public:
						virtual	void	Apply (void (*doToElement) (const T& item)) const										=	0;
						virtual	typename Memory::SharedByValue<Iterator<T>::Rep>
										ApplyUntil (void (*doToElement) (const T& item)) const									=	0;
				};



			//	Collection::IRep
			template	<typename T, typename TTRAITS>
				inline	Collection<T,TRAITS>::Collection (const Memory::SharedByValue<IRep<T>>& rep)
					: fRep_ (rep)
					{
					}
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::GetLength () const
					{
						return fRep->GetLength ();
					}
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::IsEmpty () const
					{
						return fRep->IsEmpty ();
					}
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::Contains (T item) const
					{
						return fRep->Contains (item);
					}
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::RemoveAll () const
					{
						return fRep->RemoveAll ();
					}
#if 0
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::Compact ()
					{
						return fRep->Compact ();
					}
#endif
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::empty () const
					{
						return IsEmpty ();
					}
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::length () const
					{
						return GetLength ();
					}
			template	<typename T, typename TTRAITS>
				inline	size_t	Collection<T,TRAITS>::clear ()
					{
						return fRep->RemoveAll ();
					}
			template	<typename T, typename TTRAITS>
				inline	Iterator<T>	Collection<T,TRAITS>::MakeIterator () const
					{
						return fRep->MakeIterator ();
					}
			template	<typename T, typename TTRAITS>
				inline	Iterator<T>	Collection<T,TRAITS>::begin () const
					{
						return MakeIterator ();
					}
			template	<typename T, typename TTRAITS>
				inline	Iterator<T>	Collection<T,TRAITS>::end () const
					{
						return Iterator<T> (nullptr);
					}
			template	<typename T, typename TTRAITS>
				inline	void	Collection<T,TRAITS>::Add (T item) const
					{
						fRep->Add (item);
					}
			template	<typename T, typename TTRAITS>
				inline	void	Collection<T,TRAITS>::Add (const Collection<T,TTRAITS>& items)
					{
						for (Iterator<T> i = items.beign (); i != items.end (); ++i) {
							Add (*i);
						}
					}
			template	<typename T, typename TTRAITS>
				inline	void	Collection<T,TRAITS>::Remove (T item) const
					{
						fRep->Remove (item);
					}
			template	<typename T, typename TTRAITS>
				inline	void	Collection<T,TRAITS>::Remove (const Collection<T,TTRAITS>& items)
					{
						for (Iterator<T> i = items.beign (); i != items.end (); ++i) {
							Remove (*i);
						}
					}
			template	<typename T, typename TTRAITS>
				inline	void	Collection<T,TRAITS>::RemoveIfPresent (T item) const
					{
						fRep->RemoveIfPresent (item);
					}
			template	<typename T, typename TTRAITS>
				inline	void	Collection<T,TRAITS>::RemoveIfPresent (const Collection<T,TTRAITS>& items)
					{
						for (Iterator<T> i = items.beign (); i != items.end (); ++i) {
							RemoveIfPresent (*i);
						}
					}
			template	<typename T, typename TTRAITS>
				inline	Collection<T,TTRAITS>&	Collection<T,TRAITS>::operator+= (T item)
					{
						Add (item);
						return *this;
					}
			template	<typename T, typename TTRAITS>
				inline	Collection<T,TTRAITS>&	Collection<T,TRAITS>::operator+= (const Collection<T,TTRAITS>& items)
					{
						Add (items);
						return *this;
					}
			template	<typename T, typename TTRAITS>
				inline	Collection<T,TTRAITS>&	Collection<T,TRAITS>::operator+= (T item)
					{
						Remove (item);
						return *this;
					}
			template	<typename T, typename TTRAITS>
				inline	Collection<T,TTRAITS>&	Collection<T,TRAITS>::operator+= (const Collection<T,TTRAITS>& items)
					{
						Remove (items);
						return *this;
					}
			template	<typename T, typename TTRAITS>
				inline	void	Collection<T,TRAITS>::Apply (void (*doToElement) (T item))
					{
						for (Iterator<T> i = items.beign (); i != items.end (); ++i) {
							(doToElement) (*i);
						}
					}
			template	<typename T, typename TTRAITS>
				inline	bool	Collection<T,TRAITS>::ApplyUntil (bool (*doToElement) (T item))
					{
						for (Iterator<T> i = items.beign (); i != items.end (); ++i) {
							if ((doToElement) (*i)) {
								return true;
							}
						}
						return false;
					}






			//	Collection
			template	<typename T, typename TTRAITS>
				inline	Collection::IRep::IRep ()
					{
					}
			template	<typename T, typename TTRAITS>
				inline	Collection::IRep::I~Rep ()
					{
					}

		}
	}
}

#endif /* _Stroika_Foundation_Containers_Collection_inl_ */
