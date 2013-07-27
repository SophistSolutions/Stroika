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
             ***************************** Set<T, TRAITS> ***********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set ()
                : inherited (static_cast<const inherited&> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (const Set<T, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Set<T, TRAITS>::Set (const CONTAINER_OF_T& src)
                : inherited (static_cast<const inherited&> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (src);
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::Set (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Set<T, TRAITS>::Set (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast<const inherited&> (Concrete::Set_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (start, end);
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator= (const Set<T, TRAITS>& rhs)
            {
                inherited::operator= (rhs);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  const typename  Set<T, TRAITS>::_IRep&    Set<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename    Set<T, TRAITS>::_IRep&  Set<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  bool    Set<T, TRAITS>::Contains (T item) const
            {
                return _GetRep ().Contains (item);
            }
            template    <typename T, typename TRAITS>
            inline  Memory::Optional<T>    Set<T, TRAITS>::Lookup (T item) const
            {
                return _GetRep ().Lookup (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::Add (T item)
            {
                _GetRep ().Add (item);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Set<T, TRAITS>::AddAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T, TRAITS>::AddAll (const CONTAINER_OF_T& s)
            {
                // Note - unlike Bag<T, TRAITS> - we dont need to check for this != &s because if we
                // attempt to add items that already exist, it would do nothing, and not lead to
                // an infinite loop
                AddAll (std::begin (s), std::end (s));
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::Remove (T item)
            {
                _GetRep ().Remove (item);
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::Remove (const Iterator<T>& i)
            {
                _GetRep ().Remove (i);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            void    Set<T, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i);
                }
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T, TRAITS>::RemoveAll (const CONTAINER_OF_T& s)
            {
                RemoveAll (std::begin (s), std::end (s));
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            bool  Set<T, TRAITS>::Equals (const Set<T, TRAITS>& rhs) const
            {
                return (_GetRep ().Equals (rhs._GetRep ()));
            }
            template    <typename T, typename TRAITS>
            inline  bool  Set<T, TRAITS>::operator== (const Set<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Set<T, TRAITS>::operator!= (const Set<T, TRAITS>& rhs) const
            {
                return (not Equals (rhs));
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator+= (T item)
            {
                Add (item);
                return *this;
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator+= (const CONTAINER_OF_T& items)
            {
                AddAll (items);
                return (*this);
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator-= (T item)
            {
                Remove (item);
                return *this;
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Set<T, TRAITS>& Set<T, TRAITS>::operator-= (const CONTAINER_OF_T& items)
            {
                RemoveAll (items);
                return *this;
            }
            template    <typename T, typename TRAITS>
            inline  void    Set<T, TRAITS>::clear ()
            {
                RemoveAll ();
            }


            /*
             ********************************************************************************
             ************************** Set<T, TRAITS>::_IRep *******************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::_IRep::_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            inline  Set<T, TRAITS>::_IRep::~_IRep ()
            {
            }
            template    <typename T, typename TRAITS>
            bool    Set<T, TRAITS>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
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
