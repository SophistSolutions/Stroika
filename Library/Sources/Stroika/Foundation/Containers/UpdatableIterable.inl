/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_UpdatableIterable_inl_
#define _Stroika_Foundation_Containers_UpdatableIterable_inl_


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             *************************** UpdatableIterable<T> *******************************
             ********************************************************************************
             */
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (const _SharedPtrIRep& rep) noexcept
:
            inherited (rep)
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            , fWriteMutex_ ()
#endif
            {
            }
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (const UpdatableIterable<T>& from)
                :
                inherited (from)
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
                , fWriteMutex_ ()
#endif
            {
            }
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (UpdatableIterable<T>&& from)
                :
                inherited (std::move (from))
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
                , fWriteMutex_ ()
#endif
            {
            }
            template    <typename T>
            inline  UpdatableIterable<T>::UpdatableIterable (_SharedPtrIRep&& rep) noexcept
:
            inherited (std::move (rep))
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
            , fWriteMutex_ ()
#endif
            {
                //RequireNotNull (rep); -- logically required, but we cannot test here, must test before mem-initializers
            }
            template    <typename T>
            inline  UpdatableIterable<T>&    UpdatableIterable<T>::operator= (const UpdatableIterable<T>& rhs)
            {
                inherited::operator= (rhs);
                // intentionally don't copy the fWriteMutex_
                return *this;
            }
            template    <typename T>
            inline  UpdatableIterable<T>&    UpdatableIterable<T>::operator= (UpdatableIterable<T> && rhs)
            {
                inherited::operator= (move (rhs));
                // intentionally don't move the fWriteMutex_
                return *this;
            }


            /*
             ********************************************************************************
             *** UpdatableIterableHelper_<CONTAINER_OF_T, T>::_SafeReadWriteRepAccessor *****
             ********************************************************************************
             */
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (UpdatableIterable<T>* iterableEnvelope)
#if     qStroika_Foundation_Containers_UpdatableIterator_WriteUpdateEnvelopeMutex_
                : fEnvelopeWriteLock_ (iterableEnvelope->fWriteMutex_)
                ,
#else
                :
#endif
                fIterableEnvelope_ (iterableEnvelope)
                , fRef_ (*static_cast<REP_SUB_TYPE*> (iterableEnvelope->_fRep.get (iterableEnvelope)))
            {
                RequireNotNull (iterableEnvelope);
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::~_SafeReadWriteRepAccessor ()
            {
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  const REP_SUB_TYPE&    UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
            {
                return fRef_;
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  REP_SUB_TYPE&    UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_GetWriteableRep ()
            {
                return fRef_;
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  void    UpdatableIterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_UpdateRep (const typename _SharedByValueRepType::shared_ptr_type& sp)
            {
                fIterableEnvelope_->_fRep = sp;
            }


        }
    }
}
#endif /* _Stroika_Foundation_Containers_UpdatableIterable_inl_ */
