/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Tally_inl_
#define _Stroika_Foundation_Containers_Tally_inl_

#include	"../Debug/Assertions.h"
#include "Tally.h"

#include "Tally_Array.h"  // needed for default constructor


namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {



			template	<typename T> class	TallyRep {
				protected:
					TallyRep ();

				public:
					virtual ~TallyRep ();

				public:
					virtual	TallyRep<T>*	Clone () const							=	0;
					virtual	bool	Contains (T item) const							=	0;
					virtual	size_t	GetLength () const 								=	0;
					virtual	void	Compact ()										=	0;
					virtual	void	RemoveAll () 									=	0;

					virtual	void	Add (T item, size_t count)						=	0;
					virtual	void	Remove (T item, size_t count)					=	0;
					virtual	size_t	TallyOf (T item) const							=	0;

					nonvirtual	typename Iterator<T>::Rep*				MakeIterator ();
					virtual	typename Iterator<TallyEntry<T> >::Rep*	MakeTallyIterator ()	=	0;
					virtual	TallyMutatorRep<T>*				MakeTallyMutator ()		=	0;
			};


			template	<typename T> class	TallyMutatorRep : public Iterator<TallyEntry<T> >::Rep {
				protected:
					TallyMutatorRep ();

				public:
					virtual	void	RemoveCurrent () 				=	0;
					virtual	void	UpdateCount (size_t newCount) 	=	0;
			};

			template	<typename T> class	TallyIterateOnTRep : public Iterator<T>::Rep {
				public:
					TallyIterateOnTRep (typename Iterator<TallyEntry<T> >::Rep* it);
					~TallyIterateOnTRep ();

					virtual	bool			More (T* current, bool advance) override;
					virtual	typename Iterator<T>::Rep*	Clone () const override;

				private:
					typename Iterator<TallyEntry<T> >::Rep*	fIt;
			};





			//Tally
			template	<typename T> Tally<T>::Tally () :
				fRep (0, &Clone)
			{
				*this = Tally_Array<T> ();
			}

			template	<typename T> Tally<T>::Tally (const T* items, size_t size) :
				fRep (0, &Clone)
			{
				*this = Tally_Array<T> (items, size);
			}

			template	<typename T> void	Tally<T>::RemoveAll (T item)
			{
				Remove (item, TallyOf (item));
			}

			template	<typename T> size_t	Tally<T>::TotalTally () const
			{
				size_t sum = 0;
				For (it, It (*this)) {
					sum += it.Current ().fCount;
				}
				return (sum);
			}

			template	<typename T> Tally<T>&	Tally<T>::operator+= (const Tally<T>& t)
			{
				For (it,  It (t)) {
					Add (it.Current ().fItem, it.Current ().fCount);
				}
				return (*this);
			}

			template	<typename T> void	Tally<T>::AddItems (const T* items, size_t size)
			{
				for (size_t i = 0; i < size; i++) {
					Add (items[i]);
				}
			}

			template	<typename T>	inline	Tally<T>::Tally (const Tally<T>& src) :
				fRep (src.fRep)
			{
			}

			template	<typename T>	inline	Tally<T>::Tally (TallyRep<T>* rep) :
				fRep (rep, &Clone)
			{
			}

			template	<typename T>	inline	Tally<T>& Tally<T>::operator= (const Tally<T>& src)
			{
				fRep = src.fRep;
				return (*this);
			}

			template	<typename T>	inline	size_t	Tally<T>::GetLength () const
			{
				return (fRep->GetLength ());
			}

			template	<typename T>	inline	bool	Tally<T>::IsEmpty () const
			{
				return (bool (GetLength () == 0));
			}

			template	<typename T>	inline	bool	Tally<T>::Contains (T item) const
			{
				return (fRep->Contains (item));
			}

			template	<typename T>	inline	void	Tally<T>::RemoveAll ()
			{
				fRep->RemoveAll ();
			}

			template	<typename T>	inline	void	Tally<T>::Compact ()
			{
				fRep->Compact ();
			}

			template	<typename T>	inline	Tally<T>::operator Iterator<T> () const
			{
                Iterator<T> it (const_cast<Tally<T> *> (this)->fRep->MakeIterator ());
                ++it;
                return it;
			}

			template	<typename T>	inline	Tally<T>::operator Iterator<TallyEntry<T> > () const
			{
				Iterator<TallyEntry<T>> it (const_cast<Tally<T> *> (this)->fRep->MakeTallyIterator ());
                ++it;
                return it;
			}

			template	<typename T>	inline	Tally<T>::operator TallyMutator<T> ()
			{
				return (fRep->MakeTallyMutator ());
			}

            template	<typename T>	inline	Iterator<T>    Tally<T>::begin () const
            {
            	return operator Iterator<T> ();
            }

            template	<typename T>	inline	Iterator<T>    Tally<T>::end () const
            {
                return (Iterator<T>::GetSentinal ());
            }

            template	<typename T>	inline	TallyMutator<T>    Tally<T>::begin ()
            {
            	return operator TallyMutator<T> ();
            }

            template	<typename T>	inline	TallyMutator<T>    Tally<T>::end ()
            {
                return (TallyMutator<T> (nullptr));
            }

			template	<typename T>	inline	void	Tally<T>::Add (T item)
			{
				fRep->Add (item, 1);
			}

			template	<typename T>	inline	void	Tally<T>::Add (T item, size_t count)
			{
				fRep->Add (item, count);
			}

			template	<typename T>	inline	void	Tally<T>::Remove (T item)
			{
				fRep->Remove (item, 1);
			}

			template	<typename T>	inline	void	Tally<T>::Remove (T item, size_t count)
			{
				fRep->Remove (item, count);
			}

			template	<typename T>	inline	size_t	Tally<T>::TallyOf (T item) const
			{
				return (fRep->TallyOf (item));
			}

			template	<typename T>	inline	Tally<T>&	Tally<T>::operator+= (T item)
			{
				fRep->Add (item, 1);
				return (*this);
			}

			template	<typename T>	inline	const TallyRep<T>*	Tally<T>::GetRep () const
			{
				return fRep.GetPointer ();
			}

			template	<typename T>	inline	TallyRep<T>*		Tally<T>::GetRep ()
			{
				return (fRep.GetPointer ());
			}

			// operator!=
			template	<typename T> inline	bool	operator!= (const Tally<T>& lhs, const Tally<T>& rhs)
			{
				return (not operator== (lhs, rhs));
			}


			// typename TallyRep<T>
			template	<typename T>	inline	TallyRep<T>::TallyRep ()
			{
			}

			template	<typename T>	inline	TallyRep<T>::~TallyRep ()
			{
			}

			template	<typename T> typename Iterator<T>::Rep* TallyRep<T>::MakeIterator ()
			{
				return (new TallyIterateOnTRep<T> (MakeTallyIterator ()));
			}


			template	<typename T>	TallyRep<T>*	Tally<T>::Clone (const TallyRep<T>& rep)
			{
				return (rep.Clone ());
			}

			template	<typename T> bool	operator== (const TallyEntry<T>& lhs, const TallyEntry<T>& rhs)
			{
				if (not (lhs.fItem == rhs.fItem))  {
					return false;
				}
				return (bool (lhs.fCount == rhs.fCount));
			}

			template	<typename T> bool	operator== (const Tally<T>& lhs, const Tally<T>& rhs)
			{
				if (lhs.GetRep () == rhs.GetRep ()) {
					return (true);
				}
				if (lhs.GetLength () != rhs.GetLength ()) {
					return (false);
				}

				For (it, typename Tally<T>::It (lhs)) {
					if (it.Current ().fCount != rhs.TallyOf (it.Current ().fItem)) {
						return (false);
					}
				}
				return (true);
			}

			// typename TallyEntry<T>
			template	<typename T>	inline	TallyEntry<T>::TallyEntry (T item) :
				fItem (item),
				fCount (1)
			{
			}

			template	<typename T>	inline	TallyEntry<T>::TallyEntry (T item, size_t count) :
				fItem (item),
				fCount (count)
			{
			}

            template	<typename T> inline bool   TallyEntry<T>::operator== (const TallyEntry<T>& rhs)  const
            {
            	return (fCount == rhs.fCount and fItem == rhs.fItem);
            }

            template	<typename T> inline bool   TallyEntry<T>::operator!= (const TallyEntry<T>& rhs)  const
            {
            	return not (operator== (rhs));
            }

			// typename TallyMutator<T>
			template	<typename T>	inline	TallyMutator<T>::TallyMutator (TallyMutatorRep<T>* it) :
				Iterator<TallyEntry<T>> (it)
			{
			}

			template	<typename T>	inline	void	TallyMutator<T>::RemoveCurrent ()
			{
				dynamic_cast<TallyMutatorRep<T>*> (this->fIterator.GetPointer ())->RemoveCurrent ();
			}

			template	<typename T>	inline	void	TallyMutator<T>::UpdateCount (size_t newCount)
			{
				dynamic_cast<TallyMutatorRep<T>*> (this->fIterator.GetPointer ())->UpdateCount (newCount);
			}

			// typename TallyMutatorRep<T>
			template	<typename T> inline	TallyMutatorRep<T>::TallyMutatorRep () :
				Iterator<TallyEntry<T> >::Rep ()
			{
			}

			// typename TallyIterateOnTRep<T>
			template	<typename T> TallyIterateOnTRep<T>::TallyIterateOnTRep (typename Iterator<TallyEntry<T> >::Rep* it) :
				fIt (it)
			{
				RequireNotNull (fIt);
			}

			template	<typename T> TallyIterateOnTRep<T>::~TallyIterateOnTRep ()
			{
				delete fIt;
			}

			template	<typename T> inline	bool	TallyIterateOnTRep<T>::More (T* current, bool advance)
			{
				RequireNotNull (fIt);
				if (current == nullptr) {
					return fIt->More (nullptr, false);
				}
				else {
					TallyEntry<T> xx (*current);
					bool	result = fIt->More (&xx, advance);
					*current = xx.fItem;
					return (result);
				}
			}

			template	<typename T> inline	typename Iterator<T>::Rep*	TallyIterateOnTRep<T>::Clone () const
			{
				RequireNotNull (fIt);
				return (new TallyIterateOnTRep<T> (fIt->Clone ()));
			}



		}
	}
}



#endif /* _Stroika_Foundation_Containers_Tally_inl_ */


