/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */

#include "Private/LinkedList.h"

#include "../Memory/BlockAllocated.h"





namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {

			template	<typename T>	class	Bag_LinkedListMutatorRep;
			template	<typename T>	class	Bag_LinkedListRep : public BagRep<T> {
				public:
					Bag_LinkedListRep ();
					Bag_LinkedListRep (const Bag_LinkedListRep<T>& from);

					virtual	size_t		GetLength () const override;
					virtual	bool		Contains (T item) const override;
					virtual	void		Compact () override;
					virtual	BagRep<T>*	Clone () const override;

					virtual	IteratorRep<T>*		MakeIterator () override;
					virtual	BagIteratorRep<T>*	MakeBagIterator () override;
					virtual	BagMutatorRep<T>*	MakeBagMutator () override;

					virtual	void	Add (T item) override;
					virtual	void	Remove (T item) override;
					virtual	void	RemoveAll () override;

					static	void*	operator new (size_t size);
					static	void	operator delete (void* p);

				private:
					LinkedList_Patch<T>	fData;

				friend	class	Bag_LinkedListMutatorRep<T>;
			};



			template	<typename T> class	Bag_LinkedListMutatorRep : public BagMutatorRep<T> {
				public:
					Bag_LinkedListMutatorRep (Bag_LinkedListRep<T>& owner);
					Bag_LinkedListMutatorRep (Bag_LinkedListMutatorRep<T>& from);

					virtual	bool	More (T* current, bool advance) override;

					virtual	IteratorRep<T>*		Clone () const override;

					virtual	void	RemoveCurrent () override;
					virtual	void	UpdateCurrent (T newValue) override;


					static	void*	operator new (size_t size);
					static	void	operator delete (void* p);


				private:
					LinkedListMutator_Patch<T>	fIterator;
			};






			// Bag_LinkedList
			template	<typename T>	inline	Bag_LinkedList<T>& Bag_LinkedList<T>::operator= (const Bag_LinkedList<T>& bag)
			{
				Bag<T>::operator= (bag);
				return (*this);
			}

			template	<class	T>	inline	void*	Bag_LinkedListRep<T>::operator new (size_t size)
			{
				return (Memory::BlockAllocated<Bag_LinkedListRep<T> >::operator new (size));
			}

			template	<class	T>	inline	void	Bag_LinkedListRep<T>::operator delete (void* p)
			{
				Memory::BlockAllocated<Bag_LinkedListRep<T> >::operator delete (p);
			}

			template	<typename T>	inline	Bag_LinkedListRep<T>::Bag_LinkedListRep () :
				fData ()
			{
			}

			template	<typename T>	inline	Bag_LinkedListRep<T>::Bag_LinkedListRep (const Bag_LinkedListRep<T>& from) :
				fData (from.fData)
			{
			}

			template	<typename T>	size_t	Bag_LinkedListRep<T>::GetLength () const
			{
				return (fData.GetLength ());
			}

			template	<typename T>	void	Bag_LinkedListRep<T>::Compact ()
			{
			}

			template	<typename T>	BagRep<T>*	Bag_LinkedListRep<T>::Clone () const
			{
				return (new Bag_LinkedListRep<T> (*this));
			}

			template	<class	T>	bool	Bag_LinkedListRep<T>::Contains (T item) const
			{
				return (fData.Contains (item));
			}

			template	<typename T>	IteratorRep<T>*	Bag_LinkedListRep<T>::MakeIterator ()
			{
				return (new Bag_LinkedListMutatorRep<T> (*this));
			}

			template	<typename T>	BagIteratorRep<T>*	Bag_LinkedListRep<T>::MakeBagIterator ()
			{
				return (new Bag_LinkedListMutatorRep<T> (*this));
			}

			template	<typename T>	BagMutatorRep<T>*	Bag_LinkedListRep<T>::MakeBagMutator ()
			{
				return (new Bag_LinkedListMutatorRep<T> (*this));
			}

			template	<typename T>	void	Bag_LinkedListRep<T>::Add (T item)
			{
				fData.Prepend (item);
			}

			template	<typename T>	void	Bag_LinkedListRep<T>::Remove (T item)
			{
				fData.Remove (item);
			}

			template	<typename T>	void	Bag_LinkedListRep<T>::RemoveAll ()
			{
				fData.RemoveAll ();
			}


			// Bag_LinkedList
			template	<typename T>	Bag_LinkedList<T>::Bag_LinkedList () :
				Bag<T> (new Bag_LinkedListRep<T> ())
			{
			}

			template	<typename T>	Bag_LinkedList<T>::Bag_LinkedList (const T* items, size_t size) :
				Bag<T> (new Bag_LinkedListRep<T> ())
			{
				AddItems (items, size);
			}

			template	<typename T>	Bag_LinkedList<T>::Bag_LinkedList (const Bag<T>& src) :
				Bag<T> (new Bag_LinkedListRep<T> ())
			{
				operator+= (src);
			}

			template	<typename T>	Bag_LinkedList<T>::Bag_LinkedList (const Bag_LinkedList<T>& src) :
				Bag<T> (src)
			{
			}


			//Bag_LinkedListMutatorRep
			template	<class	T>	inline	void*	Bag_LinkedListMutatorRep<T>::operator new (size_t size)
			{
				return (Memory::BlockAllocated<Bag_LinkedListMutatorRep<T> >::operator new (size));
			}

			template	<class	T>	inline	void	Bag_LinkedListMutatorRep<T>::operator delete (void* p)
			{
				Memory::BlockAllocated<Bag_LinkedListMutatorRep<T> >::operator delete (p);
			}

			template	<typename T>	Bag_LinkedListMutatorRep<T>::Bag_LinkedListMutatorRep (Bag_LinkedListRep<T>& owner) :
				fIterator (owner.fData)
			{
			}

			template	<typename T>	Bag_LinkedListMutatorRep<T>::Bag_LinkedListMutatorRep (Bag_LinkedListMutatorRep<T>& from) :
				fIterator (from.fIterator)
			{
			}

			template	<typename T>	bool	Bag_LinkedListMutatorRep<T>::More (T* current, bool advance)
			{
				return (fIterator.More(current, advance));
			}

			template	<typename T>	IteratorRep<T>*	Bag_LinkedListMutatorRep<T>::Clone () const
			{
				return (new Bag_LinkedListMutatorRep<T> (*const_cast<Bag_LinkedListMutatorRep<T>*> (this)));
			}

			template	<class	T>	void	Bag_LinkedListMutatorRep<T>::UpdateCurrent (T newValue)
			{
				fIterator.UpdateCurrent (newValue);
			}

			template	<class	T>	void	Bag_LinkedListMutatorRep<T>::RemoveCurrent ()
			{
				fIterator.RemoveCurrent ();
			}

		}
	}
}


