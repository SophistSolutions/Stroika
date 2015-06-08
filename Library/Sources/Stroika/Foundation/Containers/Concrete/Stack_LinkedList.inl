/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                using   Traversal::IteratorOwnerID;


                /*
                 ********************************************************************************
                 **************************** Stack_LinkedList<T>::Rep_ *************************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Stack_LinkedList<T>::Rep_ : public Stack<T>::_IRep {
                private:
                    using   inherited   =   typename    Stack<T>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ () = default;
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<T>                 MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                      GetLength () const override;
                    virtual bool                        IsEmpty () const override;
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                 FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Stack<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual void                Push (T item) override;
                    virtual T                   Pop () override;
                    virtual T                   Top () const override;

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList<T, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_  fData_;
                };


                /*
                ********************************************************************************
                **************************** Stack_LinkedList<T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T>
                inline  Stack_LinkedList<T>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T>
                typename Stack_LinkedList<T>::Rep_::_IterableSharedPtrIRep  Stack_LinkedList<T>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _IterableSharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                Iterator<T>  Stack_LinkedList<T>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        tmpRep = typename Iterator<T>::SharedIRepPtr (Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ()));
#else
                        tmpRep = typename Iterator<T>::SharedIRepPtr (Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T>
                size_t  Stack_LinkedList<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.GetLength ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Stack_LinkedList<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.IsEmpty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Stack_LinkedList<T>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                Iterator<T>     Stack_LinkedList<T>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    using   RESULT_TYPE =   Iterator<T>;
                    shared_ptr<IteratorRep_> resultRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        auto iLink = fData_.FindFirstThat (doToElement);
                        if (iLink == nullptr) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        resultRep = shared_ptr<IteratorRep_> (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ()));
#else
                        resultRep = shared_ptr<IteratorRep_> (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_));
#endif
                        resultRep->fIterator.SetCurrentLink (iLink);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
#if         qCompilerAndStdLib_FunnyUsingTemplateInFunctionBug_Buggy
                    return RESULT_TYPE (typename Iterator<T>::SharedIRepPtr (resultRep));
#else
                    return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
#endif
                }
                template    <typename T>
                typename Stack_LinkedList<T>::Rep_::_SharedPtrIRep  Stack_LinkedList<T>::Rep_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    if (fData_.HasActiveIterators ()) {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Traversal::IterableBase::SharedPtrImplementationTemplate<Rep_> (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                            r->fData_.RemoveAll ();
                            return r;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    else {
                        return _SharedPtrIRep (new Rep_ ());
                    }
                }
                template    <typename T>
                void    Stack_LinkedList<T>::Rep_::Push (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.Append (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                T    Stack_LinkedList<T>::Rep_::Pop ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        T   result  =   fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        // FIX/PATCH
                        return result;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                T    Stack_LinkedList<T>::Rep_::Top () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.GetFirst ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                *********************************** Stack_LinkedList<T> ************************
                ********************************************************************************
                */
                template    <typename T>
                Stack_LinkedList<T>::Stack_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (inherited::template MakeSharedPtr<Rep_> ()))
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
                    AssertMember (&inherited::_ConstGetRep (), Rep_);
#endif
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_ */
