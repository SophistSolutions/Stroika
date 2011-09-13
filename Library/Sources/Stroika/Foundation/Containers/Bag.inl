/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_inl_
#define _Stroika_Foundation_Containers_Bag_inl_

#include	"../Debug/Assertions.h"
#include "Bag_Array.h"  // needed for default constructor



namespace	Stroika {
	namespace	Foundation {
		namespace	Containers {

             // class Bag<T>
            template	<typename T>	inline	Bag<T>::Bag (const Bag<T>& bag) :
                fRep (bag.fRep)
            {
            }

            template	<typename T>	inline	Bag<T>::Bag (BagRep<T>* rep) :
                fRep (rep, &Clone)
            {
            }

            template	<class T>	Bag<T>::Bag () :
                fRep (0, &Clone)
            {
                *this = Bag_Array<T> ();
            }

            template	<class T>	Bag<T>::Bag (const T* items, size_t size) :
                fRep (0, &Clone)
            {
                *this = Bag_Array<T> (items, size);
            }

            template	<typename T>	inline	Bag<T>&	Bag<T>::operator= (const Bag<T>& bag)
            {
                fRep = bag.fRep;
                return (*this);
            }

            template	<typename T>	inline	size_t	Bag<T>::GetLength () const
            {
                return (fRep->GetLength ());
            }

            template	<typename T>	inline	bool	Bag<T>::IsEmpty () const
            {
                return (bool (GetLength () == 0));
            }

            template	<typename T>	inline	bool	Bag<T>::Contains (T item) const
            {
                return (fRep->Contains (item));
            }

            template	<typename T>	inline	void	Bag<T>::RemoveAll ()
            {
                fRep->RemoveAll ();
            }

            template	<typename T>	inline	void	Bag<T>::Compact ()
            {
                fRep->Compact ();
            }

            template	<typename T>	inline	Bag<T>::operator Iterator<T> () const
            {
                Iterator<T> it = const_cast<Bag<T> *> (this)->fRep->MakeIterator ();
                it.More ();
                return it;
            }

            template	<typename T>	inline	Iterator<T>    Bag<T>::begin () const
            {
                return operator Iterator<T> ();
            }

            template	<typename T>	inline	IterationState    Bag<T>::end () const
            {
                return (IterationState::kAtEnd);
            }

            template	<typename T>	inline	Bag<T>&	Bag<T>::operator+= (T item)
            {
                Add (item);
                return (*this);
            }

            template	<typename T>	inline	Bag<T>&	Bag<T>::operator+= (const Bag<T>& items)
            {
                Add (items);
                return (*this);
            }

            template	<typename T>	inline	Bag<T>&	Bag<T>::operator-= (T item)
            {
                Remove (item);
                return (*this);
            }

            template	<typename T>	inline	Bag<T>&	Bag<T>::operator-= (const Bag<T>& items)
            {
                Remove (items);
                return (*this);
            }

            template	<typename T>	inline	Bag<T>::operator BagIterator<T> () const
            {
                BagIterator<T> it = const_cast<Bag<T> *> (this)->fRep->MakeBagIterator ();
                it.More ();
                return it;
            }

            template	<typename T>	inline	Bag<T>::operator BagMutator<T> ()
            {
                BagMutator<T> it = fRep->MakeBagMutator ();
                it.More ();
                return it;
            }

            template	<class T> void	Bag<T>::Add (T item)
            {
                GetRep ()->Add (item);
                Ensure (not IsEmpty ());
            }

            template	<class T> void	Bag<T>::Add (Bag<T> items)
            {
                ForEach (T, it, items) {
                    GetRep ()->Add (it.Current ());
                }
            }

            template	<class T> void	Bag<T>::Remove (T item)
            {
                GetRep ()->Remove (item);
            }

            template	<class T> void	Bag<T>::Remove (const Bag<T>& items)
            {
                if (GetRep () == items.GetRep ()) {
                    RemoveAll ();
                }
                else {
                    ForEach (T, it, items) {
                        GetRep ()->Remove (it.Current ());
                    }
                }
            }

            template	<class T> size_t	Bag<T>::TallyOf (T item) const
            {
                size_t count = 0;
                ForEach (T, it, (*this)) {
                    if (it.Current () == item) {
                        count++;
                    }
                }
                return (count);
            }

            template	<class T> void	Bag<T>::AddItems (const T* items, size_t size)
            {
                for (size_t i = 0; i < size; i++) {
                    Add (items[i]);
                }
            }

            template	<class T>	BagRep<T>*	Bag<T>::Clone (const BagRep<T>& rep)
            {
                return (rep.Clone ());
            }



            /*
             ********************************************************************************
             ************************************ operators *********************************
             ********************************************************************************
             */
            template	<class T> bool	operator== (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                if (lhs.GetRep () == rhs.GetRep ()) {
                    return (true);
                }
                if (lhs.GetLength () != rhs.GetLength ()) {
                    return (false);
                }

                // n^2 algorithm!!!
                ForEach (T, it, lhs) {
                    if (lhs.TallyOf (it.Current ()) != rhs.TallyOf (it.Current ())) {
                        return (false);
                    }
                }
                return (true);
            }

            template	<class T> Bag<T>	operator+ (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>	temp	= lhs;
                temp += rhs;
                return (temp);
            }

            template	<class T> Bag<T>	operator- (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>	temp	= lhs;
                temp -= rhs;
                return (temp);
            }

            // operator!=
            template	<typename T>	inline	bool	operator!= (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                return (not operator== (lhs, rhs));
            }

            // class BagIteratorRep<T>
            template	<typename T>	inline	BagIteratorRep<T>::BagIteratorRep () :
                IteratorRep<T> ()
            {
            }

            // class BagMutatorRep<T>
            template	<typename T>	inline	BagMutatorRep<T>::BagMutatorRep () :
                BagIteratorRep<T> ()
            {
            }

            // class BagIterator<T>
            template	<typename T>	inline	BagIterator<T>::BagIterator (BagIteratorRep<T>* it) :
                Iterator<T> (it)
            {
            }

            // class BagMutator<T>
            template	<typename T>	inline	BagMutator<T>::BagMutator (BagMutatorRep<T>* it) :
                Iterator<T> (it)
            {
            }

            template	<typename T>    inline  BagMutatorRep<T>* BagMutator<T>::GetMutatorRep ()
            {
                /*
                 * Because of the way we construct BagMutators, it is guaranteed that
                 * this cast is safe. We could have kept an extra var of the right
                 * static type, but this would have been a waste of time and memory.
                 */
                 // SSW 8/8/2011: If I don't scope fIterator, I get an error in gcc 'fIterator was not declared in this scope
                 // see http://stackoverflow.com/questions/11405/gcc-problem-using-a-member-of-a-base-class-that-depends-on-a-template-argument
                 return (dynamic_cast<BagMutatorRep<T>*> (this->fIterator));
            }

            template	<typename T>	inline	void	BagMutator<T>::RemoveCurrent ()
            {
                GetMutatorRep ()->RemoveCurrent ();
            }

            template	<typename T>	inline	void	BagMutator<T>::UpdateCurrent (T newValue)
            {
                GetMutatorRep ()->UpdateCurrent (newValue);
            }

            // class BagRep<T>
            template	<typename T>	inline	BagRep<T>::BagRep ()
            {
            }

            template	<typename T>	inline	BagRep<T>::~BagRep ()
            {
            }

            template	<typename T>	inline	const BagRep<T>*	Bag<T>::GetRep () const
            {
                return (fRep.GetPointer ());
            }

            template	<typename T>	inline	BagRep<T>*	Bag<T>::GetRep ()
            {
                return (fRep.GetPointer ());
            }
		}
	}
}

#endif /* _Stroika_Foundation_Containers_Bag_inl_ */
