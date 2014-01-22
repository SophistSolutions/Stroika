/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_    1

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


                /*
                 ********************************************************************************
                 ********************* Tally_LinkedList<T, TRAITS>::Rep_ ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Tally_LinkedList<T, TRAITS>::Rep_ : public Tally<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename Tally<T, TRAITS>::_IRep;

                public:
                    using   _SharedPtrIRep = typename Iterable<TallyEntry<T>>::_SharedPtrIRep;
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
                    virtual _SharedPtrIRep                  Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual size_t                          GetLength () const override;
                    virtual bool                            IsEmpty () const override;
                    virtual Iterator<TallyEntry<T>>         MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual void                            Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<TallyEntry<T>>         ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Tally<T, TRAITS>::_IRep overrides
                public:
                    virtual bool                                Equals (const typename Tally<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                                Contains (T item) const override;
                    virtual void                                RemoveAll () override;
                    virtual void                                Add (T item, size_t count) override;
                    virtual void                                Remove (T item, size_t count) override;
                    virtual void                                Remove (const Iterator<TallyEntry<T>>& i) override;
                    virtual void                                UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) override;
                    virtual size_t                              TallyOf (T item) const override;
                    virtual Iterable<T>                         Elements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const override;
                    virtual Iterable<T>                         UniqueElements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const override;
#if     qDebug
                    virtual void                                AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) override;
#endif

                private:
                    using   NonPatchingDataStructureImplType_   =   Private::DataStructures::LinkedList<TallyEntry<T>>;
                    using   DataStructureImplType_              =   Private::PatchingDataStructures::LinkedList<TallyEntry<T>>;
                    using   IteratorRep_                        =   typename Private::IteratorImplHelper_<TallyEntry<T>, DataStructureImplType_>;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;
                };


                /*
                 ********************************************************************************
                 ********************* Tally_LinkedList<T, TRAITS>::Rep_ ************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  Tally_LinkedList<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Tally_LinkedList<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                size_t  Tally_LinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Tally_LinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>> Tally_LinkedList<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<TallyEntry<T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<TallyEntry<T>> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                void      Tally_LinkedList<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>>     Tally_LinkedList<T, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    Tally_LinkedList<T, TRAITS>::Rep_::Equals (const typename Tally<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool   Tally_LinkedList<T, TRAITS>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        TallyEntry<T>   c = item;
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true); ) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (c.fItem, item)) {
                                Assert (c.fCount != 0);
                                return (true);
                            }
                        }
                        return (false);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename Tally_LinkedList<T, TRAITS>::Rep_::_SharedPtrIRep   Tally_LinkedList<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _SharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void   Tally_LinkedList<T, TRAITS>::Rep_::Add (T item, size_t count)
                {
                    using   Traversal::kUnknownIteratorOwnerID;
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true); ) {
                                if (TRAITS::EqualsCompareFunctionType::Equals (current.fItem, item)) {
                                    current.fCount += count;
                                    fData_.SetAt (it, current);
                                    return;
                                }
                            }
                            fData_.Prepend (TallyEntry<T> (item, count));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                }
                template    <typename T, typename TRAITS>
                void   Tally_LinkedList<T, TRAITS>::Rep_::Remove (T item, size_t count)
                {
                    using   Traversal::kUnknownIteratorOwnerID;
                    if (count != 0) {
                        TallyEntry<T>   current (item);
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            for (typename DataStructureImplType_::ForwardIterator it (kUnknownIteratorOwnerID, &fData_); it.More (&current, true); ) {
                                if (TRAITS::EqualsCompareFunctionType::Equals (current.fItem, item)) {
                                    if (current.fCount > count) {
                                        current.fCount -= count;
                                    }
                                    else {
                                        current.fCount = 0;     // Should this be an underflow excpetion, assertion???
                                    }
                                    if (current.fCount == 0) {
                                        fData_.RemoveAt (it);
                                    }
                                    else {
                                        fData_.SetAt (it, current);
                                    }
                                    break;
                                }
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                }
                template    <typename T, typename TRAITS>
                void    Tally_LinkedList<T, TRAITS>::Rep_::Remove (const Iterator<TallyEntry<T>>& i)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void   Tally_LinkedList<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_LinkedList<T, TRAITS>::Rep_::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        if (newCount == 0) {
                            fData_.RemoveAt (mir.fIterator);
                        }
                        else {
                            TallyEntry<T>   c   =   mir.fIterator.Current ();
                            c.fCount = newCount;
                            fData_.SetAt (mir.fIterator, c);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t Tally_LinkedList<T, TRAITS>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T>   c = item;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        for (typename NonPatchingDataStructureImplType_::ForwardIterator it (&fData_); it.More (&c, true); ) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (c.fItem, item)) {
                                Ensure (c.fCount != 0);
                                return c.fCount;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return 0;
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    Tally_LinkedList<T, TRAITS>::Rep_::Elements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_Elements_Reference_Implementation (rep);
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    Tally_LinkedList<T, TRAITS>::Rep_::UniqueElements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_UniqueElements_Reference_Implementation (rep);
                }
#if     qDebug
                template    <typename T, typename TRAITS>
                void    Tally_LinkedList<T, TRAITS>::Rep_::AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#endif


                /*
                 ********************************************************************************
                 ************************ Tally_LinkedList<T, TRAITS> ***************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                Tally_LinkedList<T, TRAITS>::Tally_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                Tally_LinkedList<T, TRAITS>::Tally_LinkedList (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (start, end);
                }
                template    <typename T, typename TRAITS>
                Tally_LinkedList<T, TRAITS>::Tally_LinkedList (const Tally<T, TRAITS>& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (src);
                }
                template    <typename T, typename TRAITS>
                inline Tally_LinkedList<T, TRAITS>::Tally_LinkedList (const Tally_LinkedList<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                Tally_LinkedList<T, TRAITS>::Tally_LinkedList (const std::initializer_list<T>& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (s);
                }
                template    <typename T, typename TRAITS>
                Tally_LinkedList<T, TRAITS>::Tally_LinkedList (const std::initializer_list<TallyEntry<T>>& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (s);
                }
                template    <typename T, typename TRAITS>
                inline Tally_LinkedList<T, TRAITS>& Tally_LinkedList<T, TRAITS>::operator= (const Tally_LinkedList<T, TRAITS>& rhs)
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


#endif /* _Stroika_Foundation_Containers_Concrete_Tally_LinkedList_inl_ */
