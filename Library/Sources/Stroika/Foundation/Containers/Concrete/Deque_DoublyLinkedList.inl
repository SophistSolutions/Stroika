/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/DoublyLinkedList.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                template    <typename T>
                class   Deque_DoublyLinkedList<T>::Rep_ : public Deque<T>::_IRep {
                private:
                    using   inherited   =   typename    Deque<T>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _QueueSharedPtrIRep = typename Queue<T>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ () = default;
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual Iterator<T>                 MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<T>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<T> (tmpRep);
                    }
                    virtual size_t                      GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetLength ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                        IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.IsEmpty ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                            // use iterator (which currently implies lots of locks) with this->_Apply ()
                            fData_.Apply (doToElement);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<T>                 FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<T>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        SHARED_REP_TYPE     resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            auto iLink = fData_.FindFirstThat (doToElement);
                            if (iLink == nullptr) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetCurrentLink (iLink);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Queue<T>::_IRep overrides
                public:
                    virtual _QueueSharedPtrIRep     CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<T>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                                r->fData_.RemoveAll ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<Rep_> ();
                        }
                    }
                    virtual void                    AddTail (ArgByValueType<T> item) override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.Append (item);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual T                       RemoveHead () override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            T   item =  fData_.GetFirst ();
                            fData_.RemoveFirst ();
                            return item;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Memory::Optional<T>     RemoveHeadIf () override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            if (fData_.IsEmpty ()) {
                                return Memory::Optional<T> ();
                            }
                            T   item =  fData_.GetFirst ();
                            fData_.RemoveFirst ();
                            return item;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual T                       Head () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetFirst ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Memory::Optional<T>     HeadIf () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            if (fData_.IsEmpty ()) {
                                return Memory::Optional<T> ();
                            }
                            return fData_.GetFirst ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#if     qDebug
                    virtual void                    AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#endif

                    // Deque<T>::_IRep overrides
                public:
                    virtual void        AddHead (ArgByValueType<T> item) override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.Append (item);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual T           RemoveTail () override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            T   item =  fData_.GetFirst ();
                            fData_.RemoveLast ();
                            return item;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual T           Tail () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetLast ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::DoublyLinkedList<T, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 *************************** Deque_DoublyLinkedList<T> **************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList (const Deque_DoublyLinkedList<T>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline  Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList (const CONTAINER_OF_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Deque_DoublyLinkedList (containerUpdateSafetyPolicy)
                {
                    this->InsertAll (0, src);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                template    <typename COPY_FROM_ITERATOR_OF_T>
                inline Deque_DoublyLinkedList<T>::Deque_DoublyLinkedList (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Deque_DoublyLinkedList (containerUpdateSafetyPolicy)
                {
                    this->Append (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  void    Deque_DoublyLinkedList<T>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Deque_DoublyLinkedList_inl_ */
