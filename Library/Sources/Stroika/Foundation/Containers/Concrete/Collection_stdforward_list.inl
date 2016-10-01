/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <forward_list>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 *************** Collection_stdforward_list<T>::Rep_InternalSync_ ***************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Collection_stdforward_list<T>::Rep_InternalSync_ : public Collection<T>::_IRep {
                private:
                    using   inherited   =   typename    Collection<T>::_IRep;

                public:
                    using   _IterableSharedPtrIRep  =   typename Iterable<T>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename Collection<T>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE  = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_InternalSync_ () = default;
                    Rep_InternalSync_ (const Rep_InternalSync_& from) = delete;
                    Rep_InternalSync_ (Rep_InternalSync_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
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
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
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
                            size_t  cnt = 0;
                            for (auto i = fData_.begin (); i != fData_.end (); ++i, cnt++)
                                ;
                            return cnt;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                    IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.empty ();
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
                        return this->_FindFirstThat (doToElement, suggestedOwner);
                    }

                    // Collection<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<T>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                                r->fData_.clear_WithPatching ();
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
                            fData_.push_front (item);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
                    {
                        const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&      mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            Assert (not i.Done ());
                            *mir.fIterator.fStdIterator = newValue;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Remove (const Iterator<T>& i) override
                    {
                        const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&      mir =   dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            //mir.fIterator_.RemoveCurrent ();

                            // HORRIBLE BUT ADEQUITE IMPL...FOR NOW...
                            {
                                auto ei = fData_.before_begin ();
                                for (auto i = fData_.begin (); i != fData_.end (); ++i) {
                                    if (i == mir.fIterator.fStdIterator) {
                                        Memory::SmallStackBuffer<typename DataStructureImplType_::ForwardIterator*>   items2Patch (0);
                                        fData_.TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
                                        auto newI = fData_.erase_after (ei);
                                        fData_.TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                                        return;
                                    }
                                    ei = i;
                                }
                            }
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
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper<std::forward_list<T>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ************************* Collection_stdforward_list<T> ************************
                 ********************************************************************************
                 */
                template    <typename T>
                inline  Collection_stdforward_list<T>::Collection_stdforward_list (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_InternalSync_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_stdforward_list<T>::Collection_stdforward_list (const T* start, const T* end, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Collection_stdforward_list (containerUpdateSafetyPolicy)
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_stdforward_list<T>::Collection_stdforward_list (const Collection<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Collection_stdforward_list (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_stdforward_list<T>::Collection_stdforward_list (const Collection_stdforward_list<T>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T>
                inline  Collection_stdforward_list<T>& Collection_stdforward_list<T>::operator= (const Collection_stdforward_list<T>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertRepValidType_ ();
                    return *this;
                }
                template    <typename T>
                inline  void    Collection_stdforward_list<T>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_ */
