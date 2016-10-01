/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/Array.h"
#include    "../Private/IteratorImplHelper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ************** Collection_Array<T,TRAITS>::Rep_InternalSync_ *******************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Collection_Array<T>::Rep_InternalSync_ : public Collection<T>::_IRep {
                private:
                    using   inherited = typename Collection<T>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename Collection<T>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_InternalSync_ () = default;
                    Rep_InternalSync_ (const Rep_InternalSync_& from) = delete;
                    Rep_InternalSync_ (Rep_InternalSync_* from, IteratorOwnerID forIterableEnvelope)
                        : fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual  Rep_InternalSync_& operator= (const Rep_InternalSync_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_InternalSync_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep  Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<T>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual Iterator<T>             MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<T>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<T> (tmpRep);
                    }
                    virtual size_t                  GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetLength ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                    IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.GetLength () == 0;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                    Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                            // use iterator (which currently implies lots of locks) with this->_Apply ()
                            fData_.Apply (doToElement);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<T>             FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using   RESULT_TYPE     =   Iterator<T>;
                        using   SHARED_REP_TYPE =   Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        SHARED_REP_TYPE resultRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            size_t i = fData_.FindFirstThat (doToElement);
                            if (i == fData_.GetLength ()) {
                                return RESULT_TYPE::GetEmptyIterator ();
                            }
                            Rep_InternalSync_*   NON_CONST_THIS  =   const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            resultRep = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                            resultRep->fIterator.SetIndex (i);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
                    }

                    // Collection<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<T>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                                r->fData_.RemoveAll ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<T>::template MakeSharedPtr<Rep_InternalSync_> ();
                        }
                    }
                    virtual void                Add (ArgByValueType<T> item) override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // Appending is fastest
                            fData_.InsertAt (fData_.GetLength (), item);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
                    {
                        const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.SetAt (mir.fIterator, newValue);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Remove (const Iterator<T>& i) override
                    {
                        const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&   mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#if     qDebug
                    virtual void                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#endif

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::Array <T, Private::ContainerRepLockDataSupport_, DataStructures::Array_DefaultTraits<T, void>>;

                private:
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ************************* Collection_Array<T,TRAITS> ***************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Collection_Array<T>::Collection_Array (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_InternalSync_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_Array<T>::Collection_Array (const Collection<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Collection_Array (containerUpdateSafetyPolicy)
                {
                    SetCapacity (src.GetLength ());
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_Array<T>::Collection_Array (const T* start, const T* end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Collection_Array (containerUpdateSafetyPolicy)
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    AssertRepValidType_ ();
                    if (start != end) {
                        SetCapacity (end - start);
                        this->AddAll (start, end);
                    }
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_Array<T>::Collection_Array (const Collection_Array<T>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_Array<T>&   Collection_Array<T>::operator= (const Collection_Array<T>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename T>
                inline  void    Collection_Array<T>::Compact ()
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_InternalSync_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  size_t  Collection_Array<T>::GetCapacity () const
                {
                    using   _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_>;
                    _SafeReadRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        return accessor._ConstGetRep ().fData_.GetCapacity ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  void    Collection_Array<T>::SetCapacity (size_t slotsAlloced)
                {
                    using   _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<Rep_InternalSync_>;
                    _SafeReadWriteRepAccessor accessor { this };
                    CONTAINER_LOCK_HELPER_START (accessor._ConstGetRep ().fData_.fLockSupport) {
                        accessor._GetWriteableRep ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                inline  void    Collection_Array<T>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Collection_Array_inl_ */
