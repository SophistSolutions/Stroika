/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Bag_inl_
#define _Stroika_Foundation_Containers_Bag_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#include    "Concrete/Bag_Factory.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             *********************************** Bag<T> *************************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Bag<T>::Bag ()
                : inherited (Concrete::mkBag_Default<T> ())
            {
            }
            template    <typename T>
            inline  Bag<T>::Bag (const Bag<T>& b)
                : inherited (static_cast<const inherited&> (b))
            {
            }
            template    <typename T>
            inline  Bag<T>::Bag (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Bag<T>::Bag (const CONTAINER_OF_T& b)
                : inherited (Concrete::mkBag_Default<T> ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                this->AddAll (b);
            }
            template    <typename T>
            inline  const typename  Bag<T>::_IRep&    Bag<T>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);   // static_cast<> should perform better, but assert to verify safe
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename    Bag<T>::_IRep&  Bag<T>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);   // static_cast<> should perform better, but assert to verify safe
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  bool    Bag<T>::Contains (T item) const
            {
                return (_GetRep ().Contains (item));
            }
            template    <typename T>
            inline  void    Bag<T>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Bag<T>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Bag<T>::AddAll (const CONTAINER_OF_T& c)
            {
                /*
                 * Because adding items to a Bag COULD result in those items appearing in a running iterator,
                 * for the corner case of s.AddAll(s) - we want to assure we don't infinite loop.
                 */
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    CONTAINER_OF_T  tmp =   c;
                    AddAll (std::begin (tmp), std::end (tmp));
                }
                else {
                    AddAll (std::begin (c), std::end (c));
                }
            }
            template    <typename T>
            inline  bool  Bag<T>::Equals (const Bag<T>& rhs) const
            {
                return (_GetRep ().Equals (rhs._GetRep ()));
            }
            template    <typename T>
            inline  void    Bag<T>::Add (T item)
            {
                _GetRep ().Add (item);
                Ensure (not this->IsEmpty ());
            }
            template    <typename T>
            inline  void    Bag<T>::Update (const Iterator<T>& i, T newValue)
            {
                _GetRep ().Update (i, newValue);
            }
            template    <typename T>
            inline  void  Bag<T>::Remove (T item)
            {
                _GetRep ().Remove (item);
            }
            template    <typename T>
            inline  void    Bag<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Bag<T>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Bag<T>::RemoveAll (const CONTAINER_OF_T& c)
            {
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    RemoveAll ();
                }
                else {
                    RemoveAll (std::begin (c), std::end (c));
                }
            }
            template    <typename T>
            inline  void    Bag<T>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T>
            inline  size_t    Bag<T>::TallyOf (const Iterator<T>& i) const
            {
                // @todo - add appropriate virtual method to Rep, and implement more efficiently...
                Require (not i.Done ());
                return _GetRep ().TallyOf (*i);
            }
            template    <typename T>
            inline  size_t    Bag<T>::TallyOf (T item) const
            {
                return _GetRep ().TallyOf (item);
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
                AddAll (items);
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
                RemoveAll (items);
                return (*this);
            }
            template    <typename T>
            inline  bool  Bag<T>::operator== (const Bag<T>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T>
            inline  bool    Bag<T>::operator!= (const Bag<T>& rhs) const
            {
                return (not Equals (rhs));
            }


            /*
             ********************************************************************************
             ******************************** Bag<T>::_IRep *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Bag<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Bag<T>::_IRep::~_IRep ()
            {
            }
            template    <typename T>
            bool  Bag<T>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
            {
                if (this == &rhs) {
                    return true;
                }
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }

                // Needlessly horrible algorithm, but correct, and adequate as a reference
                for (Iterator<T> i = this->MakeIterator (); not i.Done (); ++i) {
                    if (this->TallyOf (*i) != rhs.TallyOf (*i)) {
                        return (false);
                    }
                }
                for (Iterator<T> i = rhs.MakeIterator (); not i.Done (); ++i) {
                    if (this->TallyOf (*i) != rhs.TallyOf (*i)) {
                        return (false);
                    }
                }
                return (true);
            }
            template    <typename T>
            size_t    Bag<T>::_IRep::_TallyOf_Reference_Implementation (T item) const
            {
                size_t  count = 0;
                for (Iterator<T> i = this->MakeIterator (); not i.Done (); ++i) {
                    if (*i == item) {
                        count++;
                    }
                }
                return count;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_Bag_inl_ */
