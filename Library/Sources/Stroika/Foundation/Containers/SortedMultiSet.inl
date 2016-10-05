/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_inl_

#include    "../Debug/Assertions.h"
#include    "Factory/SortedMultiSet_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************ SortedMultiSet<T, TRAITS> *****************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet ()
                : inherited (move (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const SortedMultiSet<T, TRAITS>& src) noexcept
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (SortedMultiSet<T, TRAITS>&& src) noexcept
                : inherited (move (src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const _SharedPtrIRep& src) noexcept
                : inherited ((RequireNotNull (src), src))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (_SharedPtrIRep&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<T>& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<CountedValue<T>>& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const CONTAINER_OF_T& src)
                : SortedMultiSet ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  void    SortedMultiSet <T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                _SafeReadRepAccessor<_IRep> { this };
#endif
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedMultiSet_inl_ */
