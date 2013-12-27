/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_stdmap_inl_

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
                ************************* Tally_stdmap<T, TRAITS>::Rep_ ************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                class   Tally_stdmap<T, TRAITS>::Rep_ : public Tally<T, TRAITS>::_IRep {
                private:
                    typedef typename    Tally<T, TRAITS>::_IRep inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy
                    virtual typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Clone () const override
                    {
                        return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Clone () const override;
#endif

                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual Iterator<TallyEntry<T>>                             MakeIterator () const override;
                    virtual void                                                Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<TallyEntry<T>>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Tally<T, TRAITS>::_IRep overrides
                public:
                    virtual bool                                    Equals (const typename Tally<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool                                    Contains (T item) const override;
                    virtual void                                    RemoveAll () override;
                    virtual void                                    Add (T item, size_t count) override;
                    virtual void                                    Remove (T item, size_t count) override;
                    virtual void                                    Remove (const Iterator<TallyEntry<T>>& i) override;
                    virtual void                                    UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount) override;
                    virtual size_t                                  TallyOf (T item) const override;
                    virtual Iterable<T>                             Elements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const override;
                    virtual Iterable<T>                             UniqueElements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const override;

                private:
                    typedef Private::PatchingDataStructures::STLContainerWrapper <
                    map<T, size_t, STL::less<T, typename TRAITS::WellOrderCompareFunctionType>>
                            >        DataStructureImplType_;

                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif
                    class   IteratorRep_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;
                };


                /*
                ********************************************************************************
                ******************** Tally_stdmap<T, TRAITS>::IteratorRep_ *********************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                class  Tally_stdmap<T, TRAITS>::Rep_::IteratorRep_ : public Iterator<TallyEntry<T>>::IRep {
                private:
                    typedef     typename Iterator<TallyEntry<T>>::IRep  inherited;

                public:
                    IteratorRep_ (Private::ContainerRepLockDataSupport_* sharedLock, DataStructureImplType_* data)
                        : inherited ()
                        , fLockSupport_ (*sharedLock)
                        , fIterator (data)
                    {
                        fIterator.More (static_cast<pair<T, size_t>*> (nullptr), true);   //tmphack cuz current backend iterators require a first more() - fix that!
                    }

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                public:
                    virtual shared_ptr<typename Iterator<TallyEntry<T>>::IRep> Clone () const override
                    {
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            return typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (*this));
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual void    More (Memory::Optional<TallyEntry<T>>* result, bool advance) override
                    {
                        RequireNotNull (result);
                        CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                            Memory::Optional<pair<T, size_t>> tmp;    /// FIX TO NOT REQUIRE NO DEFAULT CTOR
                            fIterator.More (&tmp, advance);
                            if (tmp.IsPresent ()) {
                                *result = TallyEntry<T> (tmp->first, tmp->second);
                            }
                            else {
                                result->clear ();
                            }
                        }
                        CONTAINER_LOCK_HELPER_END ();
                    }
                    virtual bool    Equals (const typename Iterator<TallyEntry<T>>::IRep* rhs) const override
                    {
                        AssertNotImplemented ();
                        return false;
                    }

                private:
                    Private::ContainerRepLockDataSupport_&                          fLockSupport_;
                public:
                    mutable typename Rep_::DataStructureImplType_::ForwardIterator  fIterator;
                };


                /*
                ********************************************************************************
                ********************** Tally_stdmap<T, TRAITS>::Rep_ ***************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Tally_stdmap<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Tally_stdmap<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t  Tally_stdmap<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Tally_stdmap<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.empty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>> Tally_stdmap<T, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<TallyEntry<T>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS = const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<TallyEntry<T>>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<TallyEntry<T>> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                void      Tally_stdmap<T, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<TallyEntry<T>>     Tally_stdmap<T, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    Tally_stdmap<T, TRAITS>::Rep_::Equals (const typename Tally<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    Tally_stdmap<T, TRAITS>::Rep_::Contains (T item) const
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return fData_.find (item) != fData_.end ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy
                template    <typename T, typename TRAITS>
                typename Iterable<TallyEntry<T>>::_SharedPtrIRep    Tally_stdmap<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<TallyEntry<T>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename T, typename TRAITS>
                void    Tally_stdmap<T, TRAITS>::Rep_::Add (T item, size_t count)
                {
                    if (count == 0) {
                        return;
                    }
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
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
                void    Tally_stdmap<T, TRAITS>::Rep_::Remove (T item, size_t count)
                {
                    if (count == 0) {
                        return;
                    }
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
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
                void    Tally_stdmap<T, TRAITS>::Rep_::Remove (const Iterator<TallyEntry<T>>& i)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir = i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir = dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_stdmap<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Tally_stdmap<T, TRAITS>::Rep_::UpdateCount (const Iterator<TallyEntry<T>>& i, size_t newCount)
                {
                    const typename Iterator<TallyEntry<T>>::IRep&    ir = i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir = dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
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
                size_t  Tally_stdmap<T, TRAITS>::Rep_::TallyOf (T item) const
                {
                    TallyEntry<T> tmp (item);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto i = fData_.find (item);
                        if (i == fData_.end ()) {
                            return 0;
                        }
                        return i->second;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    Tally_stdmap<T, TRAITS>::Rep_::Elements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_Elements_Reference_Implementation (rep);
                }
                template    <typename T, typename TRAITS>
                Iterable<T>    Tally_stdmap<T, TRAITS>::Rep_::UniqueElements (const typename Tally<T, TRAITS>::_SharedPtrIRep& rep) const
                {
                    return this->_UniqueElements_Reference_Implementation (rep);
                }


                /*
                ********************************************************************************
                ************************** Tally_stdmap<T, TRAITS> *****************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Tally_stdmap<T, TRAITS>::Tally_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                Tally_stdmap<T, TRAITS>::Tally_stdmap (const Tally_stdmap<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  Tally_stdmap<T, TRAITS>::Tally_stdmap (const CONTAINER_OF_T& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (src);
                }
                template    <typename T, typename TRAITS>
                Tally_stdmap<T, TRAITS>::Tally_stdmap (const std::initializer_list<T>& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (s);
                }
                template    <typename T, typename TRAITS>
                Tally_stdmap<T, TRAITS>::Tally_stdmap (const std::initializer_list<TallyEntry<T>>& s)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (s);
                }
                template    <typename T, typename TRAITS>
                inline  Tally_stdmap<T, TRAITS>&   Tally_stdmap<T, TRAITS>::operator= (const Tally_stdmap<T, TRAITS>& rhs)
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

#endif /* _Stroika_Foundation_Containers_Concrete_Tally_stdmap_inl_ */
