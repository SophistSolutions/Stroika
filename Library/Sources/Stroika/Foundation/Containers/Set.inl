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
                : inherited (Concrete::mkSet_Default<T> ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            inline  Set<T>::Set (const Set<T>& s)
                : inherited (s)
            {
                AssertMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  Set<T>::Set (const CONTAINER_OF_T& s)
                : inherited (Concrete::mkSet_Default<T> ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (s);
            }
            template    <typename T>
            inline  Set<T>::Set (const _SharedPtrIRep& rep)
                : inherited (typename Iterable<T>::_SharedByValueRepType (rep))
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR>
            inline Set<T>::Set (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
                : inherited (Concrete::mkSet_Default<T> ())
            {
                AssertMember (&inherited::_GetRep (), _IRep);
                AddAll (start, end);
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
#if 0
            template    <typename T>
            inline  bool    Set<T>::Equals (const Iterable<T>& rhs) const
            {
                return Private::Equals_ (*this, rhs);
            }
#endif
            template    <typename T>
            inline  void    Set<T>::Add (T item)
            {
                _GetRep ().Add (item);
            }
            template    <typename T>
            template    <typename COPY_FROM_ITERATOR>
            void    Set<T>::AddAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
            {
                for (auto i = start; i != end; ++i) {
                    Add (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T>::AddAll (const CONTAINER_OF_T& s)
            {
                AddAll (s.begin (), s.end ());
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
            template    <typename COPY_FROM_ITERATOR>
            void    Set<T>::RemoveAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end)
            {
                for (auto i = start; i != end; ++i) {
                    Remove (*i);
                }
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            inline  void    Set<T>::RemoveAll (const CONTAINER_OF_T& s)
            {
                RemoveAll (s.begin (), s.end ());
            }
            template    <typename T>
            inline  void    Set<T>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
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


        }
    }
}
#endif /* _Stroika_Foundation_Containers_Set_inl_ */
