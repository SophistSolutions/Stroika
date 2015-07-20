/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_
#define _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ 1

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
                 ****************** SortedMultiSet_stdmap<T, TRAITS>::Rep_ **********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   SortedMultiSet_stdmap<T, TRAITS>::Rep_ : public SortedMultiSet<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    SortedMultiSet<T, TRAITS>::_IRep;

                public:
                    using   MultiSetType = MultiSet<T, typename TRAITS::MultisetTraitsType>;
                    using   _IterableSharedPtrIRep = typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep;
                    using   _MultisetSharedPtrIRep = typename MultiSetType::_SharedPtrIRep;
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
                    virtual size_t                      GetLength () const override;
                    virtual bool                        IsEmpty () const override;
                    virtual Iterator<MultiSetEntry<T>>  MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<MultiSetEntry<T>>  FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual _MultisetSharedPtrIRep      CloneEmpty (IteratorOwnerID forIterableEnvelope) const override;
                    virtual bool                        Equals (const typename MultiSetType::_IRep& rhs) const override;
                    virtual bool                        Contains (T item) const override;
                    virtual void                        Add (T item, size_t count) override;
                    virtual void                        Remove (T item, size_t count) override;
                    virtual void                        Remove (const Iterator<MultiSetEntry<T>>& i) override;
                    virtual void                        UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount) override;
                    virtual size_t                      OccurrencesOf (T item) const override;
                    virtual Iterable<T>                 Elements (const typename MultiSetType::_SharedPtrIRep& rep) const override;
                    virtual Iterable<T>                 UniqueElements (const typename MultiSetType::_SharedPtrIRep& rep) const override;
#if     qDebug
                    virtual void                        AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override;
#endif

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper <map<T, size_t, STL::less<T, typename TRAITS::WellOrderCompareFunctionType>>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<MultiSetEntry<T>, DataStructureImplType_, typename DataStructureImplType_::ForwardIterator, pair<T, size_t>>;

                private:
                    DataStructureImplType_  fData_;
                };


                /*
                 ********************************************************************************
                 **************** SortedMultiSet_stdmap<T, TRAITS>::Rep_ ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  SortedMultiSet_stdmap<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                size_t  SortedMultiSet_stdmap<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.size ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  SortedMultiSet_stdmap<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.empty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>> SortedMultiSet_stdmap<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<MultiSetEntry<T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
#if     qStroika_Foundation_Traveral_IteratorRepHoldsIterableOwnerSharedPtr_
                        tmpRep = Iterator<MultiSetEntry<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_, NON_CONST_THIS->shared_from_this ());
#else
                        tmpRep = Iterator<MultiSetEntry<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
#endif
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<MultiSetEntry<T>> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                void      SortedMultiSet_stdmap<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>>     SortedMultiSet_stdmap<T, TRAITS>::Rep_::FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    return this->_FindFirstThat (doToElement, suggestedOwner);
                }
                template    <typename T, typename TRAITS>
                typename SortedMultiSet_stdmap<T, TRAITS>::Rep_::_MultisetSharedPtrIRep  SortedMultiSet_stdmap<T, TRAITS>::Rep_::CloneEmpty (IteratorOwnerID forIterableEnvelope) const
                {
                    if (fData_.HasActiveIterators ()) {
                        CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<MultiSetEntry<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.clear_WithPatching ();
                            return r;
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    else {
                        return Iterable<MultiSetEntry<T>>::template MakeSharedPtr<Rep_> ();
                    }
                }
                template    <typename T, typename TRAITS>
                bool    SortedMultiSet_stdmap<T, TRAITS>::Rep_::Equals (const typename MultiSetType::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    SortedMultiSet_stdmap<T, TRAITS>::Rep_::Contains (T item) const
                {
                    MultiSetEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.find (item) != fData_.end ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename SortedMultiSet_stdmap<T, TRAITS>::Rep_::_IterableSharedPtrIRep    SortedMultiSet_stdmap<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<MultiSetEntry<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedMultiSet_stdmap<T, TRAITS>::Rep_::Add (T item, size_t count)
                {
                    if (count == 0) {
                        return;
                    }
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        auto i = fData_.find (item);
                        if (i == fData_.end ()) {
                            fData_.insert (typename map<T, size_t>::value_type (item, count));
                        }
                        else {
                            i->second += count;
                        }
                        // MUST PATCH
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedMultiSet_stdmap<T, TRAITS>::Rep_::Remove (T item, size_t count)
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
                template    <typename T, typename TRAITS>
                void    SortedMultiSet_stdmap<T, TRAITS>::Rep_::Remove (const Iterator<MultiSetEntry<T>>& i)
                {
                    const typename Iterator<MultiSetEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        mir.fIterator.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    SortedMultiSet_stdmap<T, TRAITS>::Rep_::UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<MultiSetEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
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
                template    <typename T, typename TRAITS>
                size_t  SortedMultiSet_stdmap<T, TRAITS>::Rep_::OccurrencesOf (T item) const
                {
                    MultiSetEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        auto i = fData_.find (item);
                        if (i == fData_.end ()) {
                            return 0;
                        }
                        return i->second;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    SortedMultiSet_stdmap<T, TRAITS>::Rep_::Elements (const typename MultiSetType::_SharedPtrIRep& rep) const
                {
                    return this->_Elements_Reference_Implementation (rep);
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    SortedMultiSet_stdmap<T, TRAITS>::Rep_::UniqueElements (const typename MultiSetType::_SharedPtrIRep& rep) const
                {
                    return this->_UniqueElements_Reference_Implementation (rep);
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    SortedMultiSet_stdmap<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                 ********************************************************************************
                 ************************ SortedMultiSet_stdmap<T, TRAITS> **********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const T* start, const T* end)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (start, end);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const SortedMultiSet_stdmap<T, TRAITS>& src) :
                    inherited (src)
                {
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<T>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const initializer_list<MultiSetEntry<T>>& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T, typename ENABLE_IF>
                SortedMultiSet_stdmap<T, TRAITS>::SortedMultiSet_stdmap (const CONTAINER_OF_T& src)
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
                {
                    AssertRepValidType_ ();
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  void    SortedMultiSet_stdmap<T, TRAITS>::AssertRepValidType_ () const
                {
                    AssertMember (&inherited::_ConstGetRep (), Rep_);
                }


            }
        }
    }
}
#endif  /* _Stroika_Foundation_Containers_SortedMultiSet_stdmap_inl_ */
