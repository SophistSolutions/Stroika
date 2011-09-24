/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Containers_Tally_LinkedList_inl_
#define	_Stroika_Foundation_Containers_Tally_LinkedList_inl_	1



#include "Private/LinkedList.h"

#include "../Memory/BlockAllocated.h"






namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {



			template	<typename T>	class	Tally_LinkedListMutatorRep;
			template	<typename T>	class	Tally_LinkedListRep : public TallyRep<T> {
				public:
					Tally_LinkedListRep ();
					Tally_LinkedListRep (const Tally_LinkedListRep<T>& from);

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

					static	void*	operator new (size_t size);
					static	void	operator delete (void* p);

				private:
					LinkedList_Patch<TallyEntry<T> >	fData;

				friend	class	Tally_LinkedListMutatorRep<T>;
			};

			template	<typename T>	class	Tally_LinkedListMutatorRep : public TallyMutatorRep<T> {
				public:
					Tally_LinkedListMutatorRep (Tally_LinkedListRep<T>& owner);
					Tally_LinkedListMutatorRep (const Tally_LinkedListMutatorRep<T>& from);

					virtual	bool			More (TallyEntry<T>* current, bool advance) override;

					virtual	IteratorRep<TallyEntry<T> >*	Clone () const override;

					virtual	void	RemoveCurrent () override;
					virtual	void	UpdateCount (size_t newCount) override;

					static	void*	operator new (size_t size);
					static	void	operator delete (void* p);

				private:
					LinkedListMutator_Patch<TallyEntry<T> >	fIterator;
			};






			/*
			 ********************************************************************************
			 ******************************* Implementation Details *************************
			 ********************************************************************************
			 */

				template	<typename T> inline	Tally_LinkedList<T>::Tally_LinkedList (const Tally_LinkedList<T>& src) :
					Tally<T> (src)
				{
				}
				template	<typename T> inline	Tally_LinkedList<T>& Tally_LinkedList<T>::operator= (const Tally_LinkedList<T>& src)
				{
					Tally<T>::operator= (src);
					return (*this);
				}


			/*
			 ********************************************************************************
			 ****************************** Tally_LinkedListMutatorRep ************************
			 ********************************************************************************
			 */
			template	<class	T>	inline	void*	Tally_LinkedListMutatorRep<T>::operator new (size_t size)
			{
				return (Memory::BlockAllocated<Tally_LinkedListMutatorRep<T> >::operator new (size));
			}

			template	<class	T>	inline	void	Tally_LinkedListMutatorRep<T>::operator delete (void* p)
			{
				Memory::BlockAllocated<Tally_LinkedListMutatorRep<T> >::operator delete (p);
			}

			template	<typename T>	Tally_LinkedListMutatorRep<T>::Tally_LinkedListMutatorRep (Tally_LinkedListRep<T>& owner) :
				fIterator (owner.fData)
			{
			}

			template	<typename T>	Tally_LinkedListMutatorRep<T>::Tally_LinkedListMutatorRep (const Tally_LinkedListMutatorRep<T>& from) :
				fIterator (from.fIterator)
			{
			}

			template	<typename T>	bool	Tally_LinkedListMutatorRep<T>::More (TallyEntry<T>* current, bool advance)
			{
				return (fIterator.More (current, advance));
			}


			template	<typename T>	IteratorRep<TallyEntry<T> >*	Tally_LinkedListMutatorRep<T>::Clone () const
			{
				return (new Tally_LinkedListMutatorRep<T> (*this));
			}

			template	<typename T> void	Tally_LinkedListMutatorRep<T>::RemoveCurrent ()
			{
				fIterator.RemoveCurrent ();
			}

			template	<typename T> void	Tally_LinkedListMutatorRep<T>::UpdateCount (size_t newCount)
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
			 ********************************* Tally_LinkedListRep ****************************
			 ********************************************************************************
			 */
			template	<class	T>	inline	void*	Tally_LinkedListRep<T>::operator new (size_t size)
			{
				return (Memory::BlockAllocated<Tally_LinkedListRep<T> >::operator new (size));
			}

			template	<class	T>	inline	void	Tally_LinkedListRep<T>::operator delete (void* p)
			{
				Memory::BlockAllocated<Tally_LinkedListRep<T> >::operator delete (p);
			}

			template	<typename T>	inline	Tally_LinkedListRep<T>::Tally_LinkedListRep () :
				fData ()
			{
			}

			template	<typename T>	inline	Tally_LinkedListRep<T>::Tally_LinkedListRep (const Tally_LinkedListRep<T>& from) :
				fData (from.fData)
			{
			}

			template	<typename T> size_t	Tally_LinkedListRep<T>::GetLength () const
			{
				return (fData.GetLength ());
			}

			template	<typename T> bool	Tally_LinkedListRep<T>::Contains (T item) const
			{
				TallyEntry<T>	c;
				for (LinkedListIterator<TallyEntry<T> > it (fData); it.More (&c, true); ) {
					if (c.fItem == item) {
						Assert (c.fCount != 0);
						return (true);
					}
				}
				return (false);
			}

			template	<typename T> void	Tally_LinkedListRep<T>::Compact ()
			{
			}

			template	<typename T> TallyRep<T>*	Tally_LinkedListRep<T>::Clone () const
			{
				return (new Tally_LinkedListRep<T> (*this));
			}

			template	<typename T> void	Tally_LinkedListRep<T>::Add (T item, size_t count)
			{
				if (count != 0) {
					TallyEntry<T>	current (item);
					for (LinkedListMutator_Patch<TallyEntry<T> > it (fData); it.More (&current, true); ) {
						if (current.fItem == item) {
							current.fCount += count;
							it.UpdateCurrent (current);
							return;
						}
					}
					fData.Prepend (TallyEntry<T> (item, count));
				}
			}

			template	<typename T> void	Tally_LinkedListRep<T>::Remove (T item, size_t count)
			{
				if (count != 0) {
					TallyEntry<T>	current (item);
					for (LinkedListMutator_Patch<TallyEntry<T> > it (fData); it.More (&current, true); ) {

						if (current.fItem == item) {
							if (current.fCount > count) {
								current.fCount -= count;
							}
							else {
								current.fCount = 0;		// Should this be an underflow excpetion, assertion???
							}
							if (current.fCount == 0) {
								it.RemoveCurrent ();
							}
							else {
								it.UpdateCurrent (current);
							}
							break;
						}
					}
				}
			}

			template	<typename T> void	Tally_LinkedListRep<T>::RemoveAll ()
			{
				fData.RemoveAll ();
			}

			template	<typename T> size_t	Tally_LinkedListRep<T>::TallyOf (T item) const
			{
				TallyEntry<T>	c;
				for (LinkedListIterator<TallyEntry<T> > it (fData); it.More (&c, true); ) {
					if (c.fItem == item) {
						Ensure (c.fCount != 0);
						return (c.fCount);
					}
				}
				return (0);
			}

			template	<typename T> IteratorRep<TallyEntry<T> >*	Tally_LinkedListRep<T>::MakeTallyIterator ()
			{
				return (new Tally_LinkedListMutatorRep<T> (*this));
			}

			template	<typename T> TallyMutatorRep<T>*	Tally_LinkedListRep<T>::MakeTallyMutator ()
			{
				return (new Tally_LinkedListMutatorRep<T> (*this));
			}




			/*
			 ********************************************************************************
			 ********************************* Tally_ *******************************
			 ********************************************************************************
			 */
			template	<class	T>	class	Tally_LinkedList;	// Tmp hack so GenClass will fixup following CTOR/DTORs
															// Harmless, but silly.

			template	<typename T> Tally_LinkedList<T>::Tally_LinkedList () :
				Tally<T> (new Tally_LinkedListRep<T> ())
			{
			}

			template	<typename T> Tally_LinkedList<T>::Tally_LinkedList (const T* items, size_t size) :
				Tally<T> (new Tally_LinkedListRep<T> ())
			{
				AddItems (items, size);
			}

			template	<typename T> Tally_LinkedList<T>::Tally_LinkedList (const Tally<T>& src) :
				Tally<T> (new Tally_LinkedListRep<T> ())
			{
				operator+= (src);
			}

		}
	}
}


#endif /* _Stroika_Foundation_Containers_Tally_LinkedList_inl_ */
