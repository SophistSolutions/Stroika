/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_inl_
#define _Stroika_Foundation_Containers_SortedMapping_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedMapping_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************** SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS> *********************
             ********************************************************************************
             */
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping ()
                : inherited (move<inherited> (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                : inherited (static_cast<const inherited&> (src))
            {
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const initializer_list<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move<inherited> (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const initializer_list<pair<KEY_TYPE, VALUE_TYPE>>& src)
                : inherited (move<inherited> (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const CONTAINER_OF_PAIR_KEY_T& src)
                : inherited (move<inherited> (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            template    <typename COPY_FROM_ITERATOR_KEY_T>
            SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : inherited (move<inherited> (Concrete::SortedMapping_Factory<KEY_TYPE, VALUE_TYPE, TRAITS>::mk ()))
            {
                _AssertRepValidType ();
                this->AddAll (start, end);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (const _SharedPtrIRep& src)
                : inherited (static_cast<const typename inherited::_SharedPtrIRep&> (src))
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping (_SharedPtrIRep&& src)
                : inherited (move<typename inherited::_SharedPtrIRep> (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
            {
                _AssertRepValidType ();
                inherited::operator= (static_cast<const inherited&> (rhs));
                _AssertRepValidType ();
                return *this;
            }
            template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
            inline  void    SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_AssertRepValidType () const
            {
#if     qDebug
                AssertMember (&inherited::_ConstGetRep (), _IRep);
#endif
            }


        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedMapping_inl_ */
