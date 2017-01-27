/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
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


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                ********************************************************************************
                ********* MultiSet_stdmap<T, TRAITS>::Rep_ *********
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                class   MultiSet_stdmap<T, TRAITS>::Rep_ : public MultiSet<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    MultiSet<T, TRAITS>::_IRep;

                public:
                    using   _IterableSharedPtrIRep = typename Iterable<CountedValue<T>>::_SharedPtrIRep;
                    using   _SharedPtrIRep = typename inherited::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   CounterType = typename inherited::CounterType;

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
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual size_t                      GetLength () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.size ();
                    }
                    virtual bool                        IsEmpty () const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.empty ();
                    }
                    virtual Iterator<CountedValue<T>>  MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
                        typename Iterator<CountedValue<T>>::SharedIRepPtr tmpRep;
                        {
                            Rep_*   NON_CONST_THIS = const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                            tmpRep = Iterator<CountedValue<T>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        }
                        return Iterator<CountedValue<T>> (tmpRep);
                    }
                    virtual void                        Apply (_APPLY_ARGTYPE doToElement) const override
                    {
                        this->_Apply (doToElement);
                    }
                    virtual Iterator<CountedValue<T>>  FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return this->_FindFirstThat (doToElement, suggestedOwner);
                    }

                    // MultiSet<T, TRAITS>::_IRep overrides
                public:
                    virtual _SharedPtrIRep              CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.clear_WithPatching ();
                            return r;
                        }
                        else {
                            return Iterable<CountedValue<T>>::template MakeSharedPtr<Rep_> ();
                        }
                    }
                    virtual bool                        Equals (const typename MultiSet<T, TRAITS>::_IRep& rhs) const override
                    {
                        return this->_Equals_Reference_Implementation (rhs);
                    }
                    virtual bool                        Contains (ArgByValueType<T> item) const override
                    {
                        CountedValue<T> tmp (item);
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        return fData_.find (item) != fData_.end ();
                    }
                    virtual void                        Add (ArgByValueType<T> item, CounterType count) override
                    {
                        if (count == 0) {
                            return;
                        }
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        auto i = fData_.find (item);
                        if (i == fData_.end ()) {
                            fData_.insert (typename map<T, CounterType>::value_type (item, count));
                        }
                        else {
                            i->second += count;
                        }
                        // MUST PATCH
                    }
                    virtual void                        Remove (ArgByValueType<T> item, CounterType count) override
                    {
                        if (count == 0) {
                            return;
                        }
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
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
                    virtual void                        Remove (const Iterator<CountedValue<T>>& i) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir = dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        mir.fIterator.RemoveCurrent ();
                    }
                    virtual void                        UpdateCount (const Iterator<CountedValue<T>>& i, CounterType newCount) override
                    {
                        const typename Iterator<CountedValue<T>>::IRep&    ir = i.GetRep ();
                        AssertMember (&ir, IteratorRep_);
                        auto&       mir = dynamic_cast<const IteratorRep_&> (ir);
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        if (newCount == 0) {
                            mir.fIterator.RemoveCurrent ();
                        }
                        else {
                            mir.fIterator.fStdIterator->second = newCount;
                        }
                        // TODO - PATCH
                    }
                    virtual CounterType                 OccurrencesOf (ArgByValueType<T> item) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        auto i = fData_.find (item);
                        if (i == fData_.end ()) {
                            return 0;
                        }
                        return i->second;
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
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec { fData_ };
                        fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
                    }
#endif

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::STLContainerWrapper <map<T, CounterType, Common::STL::less<T, typename TRAITS::WellOrderCompareFunctionType>>>;
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
                inline  MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap ()
                    : inherited (inherited::template MakeSharedPtr<Rep_> ())
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
                inline  MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const CONTAINER_OF_T& src)
                    : MultiSet_stdmap ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const initializer_list<T>& src)
                    : MultiSet_stdmap ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                MultiSet_stdmap<T, TRAITS>::MultiSet_stdmap (const initializer_list<CountedValue<T>>& src)
                    : MultiSet_stdmap ()
                {
                    this->AddAll (src);
                    AssertRepValidType_ ();
                }
                template    <typename T, typename TRAITS>
                inline  void    MultiSet_stdmap<T, TRAITS>::AssertRepValidType_ () const
                {
#if     qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_> tmp { this };   // for side-effect of AssertMemeber
#endif
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_MultiSet_stdmap_inl_ */
