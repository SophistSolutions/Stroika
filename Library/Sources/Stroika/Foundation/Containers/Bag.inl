/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_inl_
#define _Stroika_Foundation_Containers_Bag_inl_

#include    "../Debug/Assertions.h"

#include	"Concrete/Bag_Array.h"  // needed for default constructor



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {

            // class Bag<T>
            template    <typename T>    
			inline  Bag<T>::Bag (const Bag<T>& bag)
                : fRep (bag.fRep)
            {
            }

            template    <typename T>    
			inline  Bag<T>::Bag (typename Bag<T>::BagRep* rep)
                : fRep (rep)
            {
            }

            template    <class T>   
			Bag<T>::Bag ()
                : fRep (0)
            {
                *this = Concrete::Bag_Array<T> ();
            }

            template    <class T>   Bag<T>::Bag (const T* start, const T* end)
                : fRep (0)
            {
                *this = Concrete::Bag_Array<T> (start, end);
            }

            template    <typename T>    
			inline  Bag<T>& Bag<T>::operator= (const Bag<T>& bag)
            {
                fRep = bag.fRep;
                return (*this);
            }

            template    <typename T>    
			inline  size_t  Bag<T>::GetLength () const
            {
                return (fRep->GetLength ());
            }

            template    <typename T>    
			inline  bool    Bag<T>::IsEmpty () const
            {
                return (bool (GetLength () == 0));
            }

            template    <typename T>    
			inline  bool    Bag<T>::Contains (T item) const
            {
                return (fRep->Contains (item));
            }

            template    <typename T>    
			inline  void    Bag<T>::RemoveAll ()
            {
                fRep->RemoveAll ();
            }

            template    <typename T>    
			inline  void    Bag<T>::Compact ()
            {
                fRep->Compact ();
            }

            template    <typename T>    
			inline  Iterator<T>	Bag<T>::MakeIterator () const
            {
                Iterator<T> it (const_cast<Bag<T> *> (this)->fRep->MakeIterator ());
                ++it;
                return it;
            }

            template    <typename T>    
			inline  Iterator<T>    Bag<T>::begin () const
            {
                return MakeIterator ();
            }

            template    <typename T>    
			inline  Iterator<T>    Bag<T>::end () const
            {
                return (Iterator<T>::GetSentinal ());
            }

            template    <typename T>    
			inline  typename Bag<T>::Mutator    Bag<T>::begin ()
            {
                return MakeMutator ();
            }

            template    <typename T>   
			inline  typename Bag<T>::Mutator    Bag<T>::end ()
            {
                return (Bag<T>::Mutator (nullptr));
            }

            template    <typename T>    
			inline  Bag<T>& Bag<T>::operator+= (T item)
            {
                Add (item);
                return (*this);
            }

            template    <typename T>    
			inline  Bag<T>& Bag<T>::operator+= (const Bag<T>& items)
            {
                Add (items);
                return (*this);
            }

            template    <typename T>    
			inline  Bag<T>& Bag<T>::operator-= (T item)
            {
                Remove (item);
                return (*this);
            }

            template    <typename T>    
			inline  Bag<T>& Bag<T>::operator-= (const Bag<T>& items)
            {
                Remove (items);
                return (*this);
            }

            template    <typename T>
            inline   typename	Bag<T>::Mutator  Bag<T>::MakeMutator ()
            {
                Bag<T>::Mutator it = Bag<T>::Mutator (fRep->MakeBagMutator ());
                ++it;
                return it;
            }
            template    <class T>
            inline	void	Bag<T>::Add (T item)
            {
                GetRep ()->Add (item);
                Ensure (not IsEmpty ());
            }
            template    <class T>
            void  Bag<T>::Add (Bag<T> items)
            {
                For (it, items) {
                    GetRep ()->Add (it.Current ());
                }
            }
            template    <class T>
            inline	void  Bag<T>::Remove (T item)
            {
                GetRep ()->Remove (item);
            }
            template    <class T>
            void  Bag<T>::Remove (const Bag<T>& items)
            {
                if (GetRep () == items.GetRep ()) {
                    RemoveAll ();
                }
                else {
                    For (it, items) {
                        GetRep ()->Remove (it.Current ());
                    }
                }
            }
            template    <class T>
            size_t    Bag<T>::TallyOf (T item) const
            {
                size_t count = 0;
                For (it, (*this)) {
                    if (it.Current () == item) {
                        count++;
                    }
                }
                return (count);
            }
            template    <class T>
            void  Bag<T>::AddItems (const T* items, size_t size)
            {
                for (size_t i = 0; i < size; i++) {
                    Add (items[i]);
                }
            }
            template    <class T>
            typename Bag<T>::BagRep*  Bag<T>::Clone (const typename Bag<T>::BagRep& rep)
            {
                return (rep.Clone ());
            }





            /*
             ********************************************************************************
             ************************************ operators *********************************
             ********************************************************************************
             */
            template    <class T> 
			bool  operator== (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                if (lhs.GetRep () == rhs.GetRep ()) {
                    return (true);
                }
                if (lhs.GetLength () != rhs.GetLength ()) {
                    return (false);
                }

                // n^2 algorithm!!!
                For (it, lhs) {
                    if (lhs.TallyOf (it.Current ()) != rhs.TallyOf (it.Current ())) {
                        return (false);
                    }
                }
                return (true);
            }

            template    <class T> 
			Bag<T>    operator+ (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>  temp    = lhs;
                temp += rhs;
                return (temp);
            }

            template    <class T> 
			Bag<T>    operator- (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                Bag<T>  temp    = lhs;
                temp -= rhs;
                return (temp);
            }

            // operator!=
            template    <typename T>    
			inline  bool    operator!= (const Bag<T>& lhs, const Bag<T>& rhs)
            {
                return (not operator== (lhs, rhs));
            }


            // class BagMutatorRep<T>
            template    <typename T>    
			inline  Bag<T>::BagMutatorRep::BagMutatorRep () :
                Iterator<T>::Rep ()
            {
            }


			// class Bag<T>::Mutator
            template    <typename T>    
			inline  Bag<T>::Mutator::Mutator (typename Bag<T>::BagMutatorRep* it) :
                Iterator<T> (it)
            {
            }
            template    <typename T>    
			inline  typename	Bag<T>::BagMutatorRep* Bag<T>::Mutator::GetMutatorRep_ ()
            {
                /*
                 * Because of the way we construct Mutators, it is guaranteed that
                 * this cast is safe. We could have kept an extra var of the right
                 * static type, but this would have been a waste of time and memory.
                 */
                return &(dynamic_cast<BagMutatorRep&> (*this->fIterator));
            }
            template    <typename T>    
			inline  void    Bag<T>::Mutator::RemoveCurrent ()
            {
                GetMutatorRep_ ()->RemoveCurrent ();
            }
            template    <typename T>    
			inline  void    Bag<T>::Mutator::UpdateCurrent (T newValue)
            {
                GetMutatorRep_ ()->UpdateCurrent (newValue);
            }





			// class BagRep<T>
            template    <typename T>    
			inline  Bag<T>::BagRep::BagRep ()
            {
            }
            template    <typename T>    
			inline  Bag<T>::BagRep::~BagRep ()
            {
            }
            template    <typename T>    
			inline  const typename	Bag<T>::BagRep*    Bag<T>::GetRep () const
            {
                return (fRep.GetPointer ());
            }
            template    <typename T>    
			inline  typename	Bag<T>::BagRep*  Bag<T>::GetRep ()
            {
                return (fRep.GetPointer ());
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Bag_inl_ */
