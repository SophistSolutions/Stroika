/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <map>

#include    "../../Memory/BlockAllocated.h"
#include    "../STL/Compare.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                ********************************************************************************
                *** Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>::UpdateSafeIterationContainerRep_ ***
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>::UpdateSafeIterationContainerRep_ : public Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep {
                private:
                    using   inherited   =   typename    Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep;
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
                    virtual _IterableSharedPtrIRep                              Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            UpdateSafeIterationContainerRep_*   NON_CONST_THIS = const_cast<UpdateSafeIterationContainerRep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                    }
                    virtual size_t                                              GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.Invariant ();
                            return fData_.size ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                                                IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.Invariant ();
                            return fData_.empty ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                                                Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                            // use iterator (which currently implies lots of locks) with this->_Apply ()
                            fData_.Apply (doToElement);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        return this->_FindFirstThat (doToElement, suggestedOwner);
                    }

                    // Association<KEY_TYPE, VALUE_TYPE, typename TRAITS::AssociationTraitsType>::_IRep overrides
                public:
                    virtual _SharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> (const_cast<UpdateSafeIterationContainerRep_*> (this), forIterableEnvelope);
                                r->fData_.clear_WithPatching ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ();
                        }
                    }
                    virtual Iterable<KEY_TYPE>  Keys () const override
                    {
                        return this->_Keys_Reference_Implementation ();
                    }
                    virtual bool                Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            auto i = fData_.find (key);
                            if (i == fData_.end ()) {
                                if (item != nullptr) {
                                    item->clear ();
                                }
                                return false;
                            }
                            else {
                                if (item != nullptr) {
                                    *item = i->second;
                                }
                                return true;
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Add (KEY_TYPE key, VALUE_TYPE newElt) override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.Invariant ();
                            auto i = fData_.find (key);
                            if (i == fData_.end ()) {
                                i = fData_.insert (pair<KEY_TYPE, VALUE_TYPE> (key, newElt)).first;
                                // no need to patch map<>
                            }
                            else {
                                i->second = newElt;
                            }
                            fData_.Invariant ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Remove (KEY_TYPE key) override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.Invariant ();
                            auto i = fData_.find (key);
                            if (i != fData_.end ()) {
                                fData_.erase_WithPatching (i);
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                Remove (const Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>& i) override
                    {
                        const typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep&    ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&    mir = dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            mir.fIterator.RemoveCurrent ();
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
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper <map <KEY_TYPE, VALUE_TYPE, Common::STL::less<KEY_TYPE, typename TRAITS::KeyWellOrderCompareFunctionType>>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, VALUE_TYPE>, DataStructureImplType_>;

                private:
                    DataStructureImplType_  fData_;
                };


                /*
                 ********************************************************************************
                 ************* Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS> ************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_stdmultimap (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<UpdateSafeIterationContainerRep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_stdmultimap (const Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>::Association_stdmultimap (const CONTAINER_OF_PAIR_KEY_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : Association_stdmultimap (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  void    Association_stdmultimap<KEY_TYPE, VALUE_TYPE, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<UpdateSafeIterationContainerRep_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_inl_ */
