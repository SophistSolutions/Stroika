/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Stack_inl_
#define _Stroika_Foundation_Containers_Stack_inl_

#include    "../Configuration/Concepts.h"
#include    "../Debug/Assertions.h"

#include    "Concrete/Stack_Factory.h"
#include    "Private/IterableUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ***************************** Stack<T, TRAITS> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  Stack<T, TRAITS>::Stack ()
                : inherited (static_cast < const inherited&& > (Concrete::Stack_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            inline  Stack<T, TRAITS>::Stack (const Stack<T, TRAITS>& s)
                : inherited (static_cast<const inherited&> (s))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T>
            inline  Stack<T, TRAITS>::Stack (const CONTAINER_OF_T& s)
                : inherited (static_cast < const inherited&& > (Concrete::Stack_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                AssertNotImplemented ();
//                AddAll (s);
            }
            template    <typename T, typename TRAITS>
            inline  Stack<T, TRAITS>::Stack (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                RequireNotNull (rep);
            }
            template    <typename T, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_OF_T>
            inline Stack<T, TRAITS>::Stack (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end)
                : inherited (static_cast < const inherited&& > (Concrete::Stack_Factory<T, TRAITS>::mk ()))
            {
                AssertMember (&inherited::_ConstGetRep (), _IRep);
                AssertNotImplemented ();
//                AddAll (start, end);
            }
            template    <typename T, typename TRAITS>
            inline  const typename  Stack<T, TRAITS>::_IRep&    Stack<T, TRAITS>::_ConstGetRep () const
            {
                EnsureMember (&inherited::_ConstGetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_ConstGetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  const typename  Stack<T, TRAITS>::_IRep&    Stack<T, TRAITS>::_GetRep () const
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<const _IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  typename    Stack<T, TRAITS>::_IRep&  Stack<T, TRAITS>::_GetRep ()
            {
                EnsureMember (&inherited::_GetRep (), _IRep);       // use static_cast cuz more efficient, but validate with assertion
                return *static_cast<_IRep*> (&inherited::_GetRep ());
            }
            template    <typename T, typename TRAITS>
            inline  void    Stack<T, TRAITS>::Push (T item)
            {
                _GetRep ().Push (item);
            }
            template    <typename T, typename TRAITS>
            inline  T       Stack<T, TRAITS>::Pop ()
            {
                return _GetRep ().Pop ();
            }
            template    <typename T, typename TRAITS>
            inline  T       Stack<T, TRAITS>::Top () const
            {
                return _ConstGetRep ().Top ();
            }
            template    <typename T, typename TRAITS>
            inline  void    Stack<T, TRAITS>::RemoveAll ()
            {
                _GetRep ().RemoveAll ();
            }
            template    <typename T, typename TRAITS>
            bool    Stack<T, TRAITS>::Equals (const Stack<T, TRAITS>& rhs) const
            {
                RequireConceptAppliesToTypeInFunction(Concept_EqualsCompareFunctionType, typename TRAITS::EqualsCompareFunctionType);
                return Private::Equals_<T, typename TRAITS::EqualsCompareFunctionType> (*this, rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool  Stack<T, TRAITS>::operator== (const Stack<T, TRAITS>& rhs) const
            {
                return Equals (rhs);
            }
            template    <typename T, typename TRAITS>
            inline  bool    Stack<T, TRAITS>::operator!= (const Stack<T, TRAITS>& rhs) const
            {
                return not Equals (rhs);
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_Stack_inl_ */
