/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_inl_
#define _Stroika_Foundation_Containers_Set_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/Set_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************************* Set<T> ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Set<T>::Set ()
                : inherited (static_cast<const inherited&> (Concrete::mkSet_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Set<T>::Set (const Set<T>& s)
                : inherited (static_cast<const inherited&> (s))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Set<T>::Set (const CONTAINER_OF_T& s)
                : inherited (static_cast<const inherited&> (Concrete::mkSet_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (s);
            }
            template    <typename T>
            inline  Set<T>::Set (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Set<T>::Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast<const inherited&> (Concrete::mkSet_Default<T> ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (start, end);
            }
            template    <typename T>
            inline  Set<T>& Set<T>::operator= (const Set<T>& rhs)
            {
                inherited::operator= (rhs);
                return *this;
            }
            template    <typename T>
            inline  const typename  Set<T>::_IRep&    Set<T>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  typename    Set<T>::_IRep&  Set<T>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T>
            inline  bool    Set<T>::Contains (T item) const
            {
                return _GetRep ().Contains (item);
            }
            template    <typename T>
            inline  Memory::Optional<T>    Set<T>::Lookup (T item) const
            {
                return _GetRep ().Lookup (item);
            }
            template    <typename T>
            inline  void    Set<T>::Add (T item)
            {
                _GetRep ().Add (item);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Set<T>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T>::AddAll (const CONTAINER_OF_T& s)
            {
                // Note - unlike Bag<T> - we dont need to check for this != &s because if we
                // attempt to add items that already exist, it would do nothing, and not lead to
                // an infinite loop
                AddAll (std::begin (s), std::end (s));
            }
            template    <typename T>
            inline  void    Set<T>::Remove (T item)
            {
                _GetRep ().Remove (item);
            }
            template    <typename T>
            inline  void    Set<T>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Set<T>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T>::RemoveAll (const CONTAINER_OF_T& s)
            {
                RemoveAll (std::begin (s), std::end (s));
            }
            template    <typename T>
            inline  void    Set<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T>
            bool  Set<T>::Equals (const Set<T>& rhs) const
            {
                RequireElementTraitsInMethod(RequireOperatorEquals, T);
                return (_GetRep ().Equals (rhs._GetRep ()));
            }
            template    <typename T>
            inline  bool  Set<T>::operator== (const Set<T>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T>
            inline  bool    Set<T>::operator!= (const Set<T>& rhs) const
            {
                return (not Equals (rhs));
            }
            template    <typename T>
            inline  Set<T>& Set<T>::operator+= (T item)
            {
                Add (item);
                return (*this);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Set<T>& Set<T>::operator+= (const CONTAINER_OF_T& items)
            {
                AddAll (items);
                return (*this);
            }
            template    <typename T>
            inline  Set<T>& Set<T>::operator-= (T item)
            {
                Remove (item);
                return (*this);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Set<T>& Set<T>::operator-= (const CONTAINER_OF_T& items)
            {
                RemoveAll (items);
                return (*this);
            }
            template    <typename   T>
            template    <typename   CONTAINER_OF_T>
            inline  CONTAINER_OF_T  Set<T>::As () const
            {
                return CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename   T>
            template    <typename   CONTAINER_OF_T>
            inline  void    Set<T>::As (CONTAINER_OF_T* into) const
            {
                RequireNotNull (into);
                *into = CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename T>
            inline  void    Set<T>::clear ()
            {
                RemoveAll ();
            }


            /*
             ********************************************************************************
             ****************************** Set<T>::_IRep ***********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Set<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Set<T>::_IRep::~_IRep ()
            {
            }
            template    <typename T>
            bool    Set<T>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
            {
                if (this == &rhs) {
                    return true;
                }
                if (this->GetLength () != rhs.GetLength ()) {
                    return false;
                }
                // Note - no need to iterate over rhs because we checked sizes the same
                for (auto i = this->MakeIterator (); not i.Done (); ++i) {
                    if (not rhs.Contains (*i)) {
                        return false;
                    }
                }
                return true;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Set_inl_ */
