/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedMultiSet_Factory.h"


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
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const SortedMultiSet<T, TRAITS>& src)
                : inherited (src)
            {
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const _SharedPtrIRep& src)
                : inherited (src)
            {
                _AssertRepValidType ();
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (_SharedPtrIRep&& src)
                : inherited (move (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<T>& src)
                : inherited (move (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            SortedMultiSet<T, TRAITS>::SortedMultiSet (const initializer_list<MultiSetEntry<T>>& src)
                : inherited (move (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            template    <typename CONTAINER_OF_T, typename ENABLE_IF>
            inline  SortedMultiSet<T, TRAITS>::SortedMultiSet (const CONTAINER_OF_T& src)
                : inherited (move (Concrete::SortedMultiSet_Factory<T, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename T, typename TRAITS>
            inline  void    SortedMultiSet <T, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_SortedMultiSet_inl_ */
