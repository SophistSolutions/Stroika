/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Tally_Array_inl_
#define	_Stroika_Foundation_Containers_Tally_Array_inl_	1



#include "Private/Array.h"

#include "../Memory/BlockAllocated.h"




namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {

			using namespace Private;


			template	<typename T>	class	Tally_ArrayMutatorRep;

			template	<typename T>	class	Tally_ArrayRep : public TallyRep<T> {
				public:
					Tally_ArrayRep ();

					virtual	size_t	GetLength () const override;
					virtual	bool	Contains (T item) const override;
					virtual	void	Compact () override;
					virtual	TallyRep<T>*	Clone () const override;
					virtual	void	RemoveAll () override;

					virtual	void	Add (T item, size_t count) override;
					virtual	void	Remove (T item, size_t count) override;
					virtual	size_t	TallyOf (T item) const override;

					virtual	IteratorRep<TallyEntry<T> >*	MakeTallyIterator () override;
					virtual	TallyMutatorRep<T>*				MakeTallyMutator () override;

					nonvirtual	void	RemoveAt (size_t index);

					static	void*	operator new (size_t size);
					static	void	operator delete (void* p);

				private:
					Array_Patch<TallyEntry<T> >	fData;

					enum {
						kNotFound = (size_t)-1,
					};
					nonvirtual	size_t	Find (TallyEntry<T>& item) const;

				friend	class	Tally_Array<T>;
				friend	class	Tally_ArrayMutatorRep<T>;
			};

			template	<typename T> class	Tally_ArrayMutatorRep : public TallyMutatorRep<T> {
				public:
					Tally_ArrayMutatorRep (Tally_ArrayRep<T>& owner);

					virtual	bool			More (TallyEntry<T>* current, bool advance) override;

					virtual	IteratorRep<TallyEntry<T> >*	Clone () const override;

					virtual	void	RemoveCurrent () override;
					virtual	void	UpdateCount (size_t newCount) override;

					static	void*	operator new (size_t size);
					static	void	operator delete (void* p);

				private:
					ForwardArrayMutator_Patch<TallyEntry<T> >	fIterator;
				friend	class	Tally_ArrayRep<T>;
			};








			// class Tally_Array<T>
			template	<typename T>	inline	Tally_Array<T>::Tally_Array (const Tally_Array<T>& src) :
				Tally<T> (src)
			{
			}

			template	<typename T>	inline	Tally_Array<T>&	Tally_Array<T>::operator= (const Tally_Array<T>& src)
			{
				Tally<T>::operator= (src);
				return (*this);
			}



			/*
			 ********************************************************************************
			 ********************************* Tally_ArrayMutatorRep ************************
			 ********************************************************************************
			 */
			template	<class	T>	inline	void*	Tally_ArrayMutatorRep<T>::operator new (size_t size)
			{
				return (Memory::BlockAllocated<Tally_ArrayMutatorRep<T> >::operator new (size));
			}

			template	<class	T>	inline	void	Tally_ArrayMutatorRep<T>::operator delete (void* p)
			{
				Memory::BlockAllocated<Tally_ArrayMutatorRep<T> >::operator delete (p);
			}

			template	<class	T>	Tally_ArrayMutatorRep<T>::Tally_ArrayMutatorRep (Tally_ArrayRep<T>& owner) :
				TallyMutatorRep<T> (),
				fIterator (owner.fData)
			{
			}


			template	<class	T>	bool	Tally_ArrayMutatorRep<T>::More (TallyEntry<T>* current, bool advance)
			{
				return (fIterator.More(current, advance));
			}

			template	<typename T>	IteratorRep<TallyEntry<T> >*	Tally_ArrayMutatorRep<T>::Clone () const
			{
				return (new Tally_ArrayMutatorRep<T> (*this));
			}

			template	<typename T>	void	Tally_ArrayMutatorRep<T>::RemoveCurrent ()
			{
				fIterator.RemoveCurrent ();
			}

			template	<typename T>	void	Tally_ArrayMutatorRep<T>::UpdateCount (size_t newCount)
			{
				if (newCount == 0) {
					fIterator.RemoveCurrent ();
				}
				else {
					TallyEntry<T>	c	=	fIterator.Current ();
					c.fCount = newCount;
					fIterator.UpdateCurrent (c);
				}
			}





			/*
			 ********************************************************************************
			 ********************************* Tally_ArrayRep *******************************
			 ********************************************************************************
			 */
			template	<class	T>	inline	void*	Tally_ArrayRep<T>::operator new (size_t size)
			{
				return (Memory::BlockAllocated<Tally_ArrayRep<T> >::operator new (size));
			}

			template	<class	T>	inline	void	Tally_ArrayRep<T>::operator delete (void* p)
			{
				Memory::BlockAllocated<Tally_ArrayRep<T> >::operator delete (p);
			}

			template	<typename T>	inline	Tally_ArrayRep<T>::Tally_ArrayRep () :
				TallyRep<T> (),
				fData ()
			{
			}

			template	<typename T>	size_t	Tally_ArrayRep<T>::GetLength () const
			{
				return (fData.GetLength ());
			}

			template	<typename T>	bool	Tally_ArrayRep<T>::Contains (T item) const
			{
				TallyEntry<T> tmp (item);
				return (bool (Find (tmp) != kNotFound));
			}

			template	<typename T>	void	Tally_ArrayRep<T>::Compact ()
			{
				fData.Compact ();
			}

			template	<typename T>	TallyRep<T>*	Tally_ArrayRep<T>::Clone () const
			{
				return (new Tally_ArrayRep<T> (*this));
			}

			template	<typename T>	void	Tally_ArrayRep<T>::Add (T item, size_t count)
			{
				TallyEntry<T> tmp (item, count);

				size_t index = Find (tmp);
				if (index == kNotFound) {
					fData.InsertAt (tmp, GetLength ());
				}
				else {
					tmp.fCount += count;
					fData.SetAt (tmp, index);
				}
			}

			template	<typename T>	void	Tally_ArrayRep<T>::Remove (T item, size_t count)
			{
				TallyEntry<T> tmp (item);

				size_t index = Find (tmp);
				if (index != kNotFound) {
					Assert (index < GetLength ());
					Assert (tmp.fCount >= count);
					tmp.fCount -= count;
					if (tmp.fCount == 0) {
						RemoveAt (index);
					}
					else {
						fData.SetAt (tmp, index);
					}
				}
			}

			template	<typename T>	void	Tally_ArrayRep<T>::RemoveAll ()
			{
				fData.RemoveAll ();
			}

			template	<typename T>	size_t	Tally_ArrayRep<T>::TallyOf (T item) const
			{
				TallyEntry<T> tmp (item);

				size_t index = Find (tmp);
				Require (index >= 0);
				Require (index < GetLength ());
				return (tmp.fCount);
			}

			template	<typename T>	IteratorRep<TallyEntry<T> >*	Tally_ArrayRep<T>::MakeTallyIterator ()
			{
				return (new Tally_ArrayMutatorRep<T> (*this));
			}

			template	<typename T>	TallyMutatorRep<T>*	Tally_ArrayRep<T>::MakeTallyMutator ()
			{
				return (new Tally_ArrayMutatorRep<T> (*this));
			}

			template	<typename T>	void	Tally_ArrayRep<T>::RemoveAt (size_t index)
			{
				fData.RemoveAt (index);
			}

			template	<typename T>	size_t	Tally_ArrayRep<T>::Find (TallyEntry<T>& item) const
			{
				size_t length = fData.GetLength ();
				for (size_t i = 0; i < length; i++) {
					if (fData.GetAt (i).fItem == item.fItem) {
						item = fData.GetAt (i);
						return (i);
					}
				}
				return (kNotFound);
			}







			/*
			 ********************************************************************************
			 ********************************* Tally_Array **********************************
			 ********************************************************************************
			 */

			template	<typename T>	Tally_Array<T>::Tally_Array () :
				Tally<T> (new Tally_ArrayRep<T> ())
			{
			}

			template	<typename T>	Tally_Array<T>::Tally_Array (const T* items, size_t size) :
				Tally<T> (new Tally_ArrayRep<T> ())
			{
				SetSlotsAlloced (size);
				AddItems (items, size);
			}

			template	<typename T>	Tally_Array<T>::Tally_Array (const Tally<T>& src) :
				Tally<T> (new Tally_ArrayRep<T> ())
			{
				SetSlotsAlloced (src.GetLength ());
				operator+= (src);
			}

			template	<typename T>	inline	const Tally_ArrayRep<T>*	Tally_Array<T>::GetRep () const
			{
				return ((const Tally_ArrayRep<T>*) Tally<T>::GetRep ());
			}

			template	<typename T>	inline	Tally_ArrayRep<T>*	Tally_Array<T>::GetRep ()
			{
				return ((Tally_ArrayRep<T>*) Tally<T>::GetRep ());
			}

			template	<typename T>	size_t	Tally_Array<T>::GetSlotsAlloced () const
			{
				return (GetRep ()->fData.GetSlotsAlloced ());
			}

			template	<typename T>	void	Tally_Array<T>::SetSlotsAlloced (size_t slotsAlloced)
			{
				GetRep ()->fData.SetSlotsAlloced (slotsAlloced);
			}

		}
	}
}


#endif	/* _Stroika_Foundation_Containers_Tally_Array_inl_ */


