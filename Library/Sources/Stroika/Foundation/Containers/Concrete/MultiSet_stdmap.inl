/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
                ************************* MultiSet_stdmap<T, TRAITS>::Rep_ ************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                class   MultiSet_stdmap<T, TRAITS>::Rep_ : public MultiSet<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    MultiSet<T, TRAITS>::_IRep;

                public:
                    using   _SharedPtrIRep = typename Iterable<MultiSetEntry<T>>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep              Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual size_t                      GetLength () const override;
                    virtual bool                        IsEmpty () const override;
                    virtual Iterator<MultiSetEntry<T>>  MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<MultiSetEntry<T>>  ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual bool                        Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                        Contains (T item) const override;
                    virtual void                        RemoveAll () override;
                    virtual void                        Add (T item, size_t count) override;
                    virtual void                        Remove (T item, size_t count) override;
                    virtual void                        Remove (const Iterator<MultiSetEntry<T>>& i) override;
                    virtual void                        UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount) override;
                    virtual size_t                      MultiSetOf (T item) const override;
                    virtual Iterable<T>                 Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override;
                    virtual Iterable<T>                 UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const override;
#if     qDebug
                    virtual void                        AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) override;
#endif

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper <map<T, size_t, STL::less<T, typename TRAITS::WellOrderCompareFunctionType>>, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   Private::IteratorImplHelper_<MultiSetEntry<T>, DataStructureImplType_, typename DataStructureImplType_::ForwardIterator, pair<T, size_t>>;

                private:
                    DataStructureImplType_      fData_;
                };


                /*
                ********************************************************************************
                ********************** MultiSet_stdmap<T, TRAITS>::Rep_ ***************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  MultiSet_stdmap<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_stdmap<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                size_t  MultiSet_stdmap<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.size ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  MultiSet_stdmap<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.empty ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>> MultiSet_stdmap<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<MultiSetEntry<T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS = const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<MultiSetEntry<T>>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<MultiSetEntry<T>> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                void      MultiSet_stdmap<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<MultiSetEntry<T>>     MultiSet_stdmap<T, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    return this->_ApplyUntilTrue (doToElement, suggestedOwner);
                }
                template    <typename T, typename TRAITS>
                bool    MultiSet_stdmap<T, TRAITS>::Rep_::Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    MultiSet_stdmap<T, TRAITS>::Rep_::Contains (T item) const
                {
                    MultiSetEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.find (item) != fData_.end ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename MultiSet_stdmap<T, TRAITS>::Rep_::_SharedPtrIRep    MultiSet_stdmap<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _SharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_stdmap<T, TRAITS>::Rep_::Add (T item, size_t count)
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
                void    MultiSet_stdmap<T, TRAITS>::Rep_::Remove (T item, size_t count)
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
                void    MultiSet_stdmap<T, TRAITS>::Rep_::Remove (const Iterator<MultiSetEntry<T>>& i)
                {
                    const typename Iterator<MultiSetEntry<T>>::IRep&    ir = i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir = dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        mir.fIterator.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_stdmap<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    MultiSet_stdmap<T, TRAITS>::Rep_::UpdateCount (const Iterator<MultiSetEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<MultiSetEntry<T>>::IRep&    ir = i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir = dynamic_cast<const IteratorRep_&> (ir);
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
                size_t  MultiSet_stdmap<T, TRAITS>::Rep_::MultiSetOf (T item) const
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
                Iterable<T>    MultiSet_stdmap<T, TRAITS>::Rep_::Elements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_Elements_Reference_Implementation (rep);
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    MultiSet_stdmap<T, TRAITS>::Rep_::UniqueElements (const typename MultiSet<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_UniqueElements_Reference_Implementation (rep);
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    MultiSet_stdmap<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted)
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                ********************************************************************************
                ************************** MultiSet_stdmap<T, TRAITS> *****************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const MultiSet_stdmap<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const CONTAINER_OF_T& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (src);
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const std::initializer_list<T>& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (s);
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const std::initializer_list<MultiSetEntry<T>>& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (s);
                }
                template    <typename T, typename TRAITS>
                inline  MultiSet_stdmap<T, TRAITS>&   MultiSet_stdmap<T, TRAITS>::operator= (const MultiSet_stdmap<T, TRAITS>& rhs)
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    inherited::operator= (rhs);
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_ */
