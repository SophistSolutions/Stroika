/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_inl_
#define _Stroika_Foundation_Containers_Collection_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../Debug/Assertions.h"

#include    "Concrete/Collection_Factory.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ***************************** Collection<T,TRAITS> ************************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>::Collection ()
                : inherited (Concrete::Collection_Factory<T, TRAITS>::mk ())
            {
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>::Collection (const Collection<T, TRAITS>& b)
                : inherited (static_cast<const inherited&> (b))
            {
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>::Collection (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>::Collection (const std::initializer_list<T>& b)
                : inherited (Concrete::Collection_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (b);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Collection<T, TRAITS>::Collection (const CONTAINER_OF_T& b)
                : inherited (Concrete::Collection_Factory<T, TRAITS>::mk ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (b);
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>& Collection<T, TRAITS>::operator= (const Collection<T, TRAITS>& rhs)
            {
                inherited::operator= (rhs);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  const typename  Collection<T, TRAITS>::_IRep&    Collection<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);   // static_cast<> should perform better, but assert to verify safe
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename    Collection<T, TRAITS>::_IRep&  Collection<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);   // static_cast<> should perform better, but assert to verify safe
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  bool    Collection<T, TRAITS>::Contains (T item) const
            {
                return (_GetRep ().Contains (item));
            }
            template    <typename T, typename TRAITS>
            inline  void    Collection<T, TRAITS>::clear ()
            {
                RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Collection<T, TRAITS>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Collection<T, TRAITS>::AddAll (const CONTAINER_OF_T& c)
            {
                /*
                 * Because adding items to a Collection COULD result in those items appearing in a running iterator,
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
            template    <typename T, typename TRAITS>
            inline  bool  Collection<T, TRAITS>::Equals (const Collection<T, TRAITS>& rhs) const
            {
                return (_GetRep ().Equals (rhs._GetRep ()));
            }
            template    <typename T, typename TRAITS>
            inline  void    Collection<T, TRAITS>::Add (T item)
            {
                _GetRep ().Add (item);
                Ensure (not this->IsEmpty ());
            }
            template    <typename T, typename TRAITS>
            inline  void    Collection<T, TRAITS>::Update (const Iterator<T>& i, T newValue)
            {
                _GetRep ().Update (i, newValue);
            }
            template    <typename T, typename TRAITS>
            inline  void  Collection<T, TRAITS>::Remove (T item)
            {
                _GetRep ().Remove (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Collection<T, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Collection<T, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Collection<T, TRAITS>::RemoveAll (const CONTAINER_OF_T& c)
            {
                if (static_cast<const void*> (this) == static_cast<const void*> (std::addressof (c))) {
                    RemoveAll ();
                }
                else {
                    RemoveAll (std::begin (c), std::end (c));
                }
            }
            template    <typename T, typename TRAITS>
            inline  void    Collection<T, TRAITS>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T, typename TRAITS>
            inline  size_t    Collection<T, TRAITS>::TallyOf (const Iterator<T>& i) const
            {
                // @todo - add appropriate virtual method to Rep, and implement more efficiently...
                Require (not i.Done ());
                return _GetRep ().TallyOf (*i);
            }
            template    <typename T, typename TRAITS>
            inline  size_t    Collection<T, TRAITS>::TallyOf (T item) const
            {
                return _GetRep ().TallyOf (item);
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>& Collection<T, TRAITS>::operator+= (T item)
            {
                Add (item);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>& Collection<T, TRAITS>::operator+= (const Collection<T, TRAITS>& items)
            {
                AddAll (items);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>& Collection<T, TRAITS>::operator-= (T item)
            {
                Remove (item);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>& Collection<T, TRAITS>::operator-= (const Collection<T, TRAITS>& items)
            {
                RemoveAll (items);
                return (*this);
            }
            template    <typename T, typename TRAITS>
            inline  bool  Collection<T, TRAITS>::operator== (const Collection<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Collection<T, TRAITS>::operator!= (const Collection<T, TRAITS>& rhs) const
            {
                return (not Equals (rhs));
            }


            /*
             ********************************************************************************
             ****************************** Collection<T,TRAITS>::_IRep ****************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Collection<T, TRAITS>::_IRep::~_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            bool  Collection<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
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
            template    <typename T, typename TRAITS>
            size_t    Collection<T, TRAITS>::_IRep::_TallyOf_Reference_Implementation (T item) const
            {
                size_t  count = 0;
                for (Iterator<T> i = this->MakeIterator (); not i.Done (); ++i) {
                    if (TRAITS::EqualsCompareFunctionType::Equals (*i, item)) {
                        count++;
                    }
                }
                return count;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_Collection_inl_ */
