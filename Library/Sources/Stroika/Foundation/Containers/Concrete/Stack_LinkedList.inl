/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/LinkedList.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                using   Traversal::IteratorOwnerID;


                /*
                 ********************************************************************************
                 ************* Stack_LinkedList<T>::UpdateSafeIterationContainerRep_ ************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Stack_LinkedList<T>::UpdateSafeIterationContainerRep_ : public Stack<T>::_IRep {
                private:
                    using   inherited   =   typename    Stack<T>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    UpdateSafeIterationContainerRep_ () = default;
                    UpdateSafeIterationContainerRep_ (const UpdateSafeIterationContainerRep_& from) = delete;
                    UpdateSafeIterationContainerRep_ (UpdateSafeIterationContainerRep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  UpdateSafeIterationContainerRep_& operator= (const UpdateSafeIterationContainerRep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (UpdateSafeIterationContainerRep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<T>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<T>                 MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<T>::SharedIRepPtr tmpRep;
                        {
                            std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                            UpdateSafeIterationContainerRep_*   NON_CONST_THIS  =   const_cast<UpdateSafeIterationContainerRep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        return Iterator<T> (tmpRep);
                    }
                    virtual size_t                      GetLength () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.GetLength ();
                    }
                    virtual bool                        IsEmpty () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.IsEmpty ();
                    }
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    virtual Iterator<T>                 FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<T>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        SHARED_REP_TYPE resultRep;
                        {
                            std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                            UpdateSafeIterationContainerRep_*   NON_CONST_THIS  =   const_cast<UpdateSafeIterationContainerRep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetCurrentLink (iLink);
                        }
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Stack<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            std::lock_guard<std::mutex> critSec (fData_.fActiveIteratorsMutex_);
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<T>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ();
                        }
                    }
                    virtual void                Push (ArgByValueType<T> item) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.Append (item);
                    }
                    virtual T                   Pop () override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        T   result  =   fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        // FIX/PATCH
                        return result;
                    }
                    virtual T                   Top () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.GetFirst ();
                    }

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList<T>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_  fData_;
                };


                /*
                ********************************************************************************
                *********************************** Stack_LinkedList<T> ************************
                ********************************************************************************
                */
                template    <typename T>
                Stack_LinkedList<T>::Stack_LinkedList ()
                    : inherited (inherited::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Stack_LinkedList<T>::Stack_LinkedList (const Stack_LinkedList<T>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Stack_LinkedList<T>&   Stack_LinkedList<T>::operator= (const Stack_LinkedList<T>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename T>
                inline  void    Stack_LinkedList<T>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<UpdateSafeIterationContainerRep_> tmp { this };   // for side-effect of AssertMember
#endif
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_ */
