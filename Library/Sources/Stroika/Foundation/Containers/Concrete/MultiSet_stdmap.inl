/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_

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
                **************** MultiSet_stdmap<T, TRAITS>::Rep_InternalSync_ *****************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                class   MultiSet_stdmap<T, TRAITS>::Rep_InternalSync_ : public MultiSet<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    MultiSet<T, TRAITS>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<CountedValue<T>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   CounterType = typename inherited::CounterType;

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
                    virtual _IterableSharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                    }
                    virtual size_t                      GetLength () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.size ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool                        IsEmpty () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.empty ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterator<CountedValue<T>>  MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<CountedValue<T>>::SharedIRepPtr tmpRep;
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                            Rep_InternalSync_*   NON_CONST_THIS = const_cast<Rep_InternalSync_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        return Iterator<CountedValue<T>> (tmpRep);
                    }
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        this->_Apply (doToElement);
                    }
                    virtual Iterator<CountedValue<T>>  FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        return this->_FindFirstThat (doToElement, suggestedOwner);
                    }

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep              CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_START (fData_.fLockSupport) {
                                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                                auto r = Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_InternalSync_> (const_cast<Rep_InternalSync_*> (this), forIterableEnvelope);
                                r->fData_.clear_WithPatching ();
                                return r;
                            }
                            CONTAINER_LOCK_HELPER_ITERATORLISTUPDATE_END ();
                        }
                        else {
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_InternalSync_> ();
                        }
                    }
                    virtual bool                        Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool                        Contains (ArgByValueType<T> item) const override
                    {
                        CountedValue<T> tmp (item);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            return fData_.find (item) != fData_.end ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                        Add (ArgByValueType<T> item, CounterType count) override
                    {
                        if (count == 0) {
                            return;
                        }
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            auto i = fData_.find (item);
                            if (i == fData_.end ()) {
                                fData_.insert (typename map<T, CounterType>::value_type (item, count));
                            }
                            else {
                                i->second += count;
                            }
                            // MUST PATCH
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                        Remove (ArgByValueType<T> item, CounterType count) override
                    {
                        if (count == 0) {
                            return;
                        }
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            auto i = fData_.find (item);
                            Require (i != fData_.end ());
                            if (i != fData_.end ()) {
                                Require (i->second >= count);
                                i->second -= count;
                                if (i->second == 0) {
                                    fData_.erase_WithPatching (i);
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                        Remove (const Iterator<CountedValue<T>>& i) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir = dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            mir.fIterator.RemoveCurrent ();
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void                        UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir = dynamic_cast<const IteratorRep_&> (ir);
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            if (newCount == 0) {
                                mir.fIterator.RemoveCurrent ();
                            }
                            else {
                                mir.fIterator.fStdIterator->second = newCount;
                            }
                            // TODO - PATCH
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual CounterType                 OccurrencesOf (ArgByValueType<T> item) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            auto i = fData_.find (item);
                            if (i == fData_.end ()) {
                                return 0;
                            }
                            return i->second;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual Iterable<T>                 Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override
                    {
                        return this->_Elements_Reference_Implementation (rep);
                    }
                    virtual Iterable<T>                 UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override
                    {
                        return this->_UniqueElements_Reference_Implementation (rep);
                    }
#if     qDebug
                    virtual void                        AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
                    {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
#endif

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper <map<T, CounterType, Common::STL::less<T, typename TRAITS::WellOrderCompareFunctionType>>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<CountedValue<T>, DataStructureImplType_, typename DataStructureImplType_::ForwardIterator, pair<T, CounterType>>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                 ********************************************************************************
                 ************************ MultiSet_stdmap<T, TRAITS> ****************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : inherited (inherited::template MakeSharedPtr<Rep_InternalSync_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const MultiSet_stdmap<T, TRAITS>& src)
                    : inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                template <typename CONTAINER_OF_T, typename ENABLE_IF>
                inline  MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const CONTAINER_OF_T& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_stdmap (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const initializer_list<T>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_stdmap (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const initializer_list<CountedValue<T>>& src, ContainerUpdateIteratorSafety containerUpdateSafetyPolicy)
                    : MultiSet_stdmap (containerUpdateSafetyPolicy)
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  void    MultiSet_stdmap<T, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_InternalSync_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_ */
