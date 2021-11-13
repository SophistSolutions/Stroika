/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/LinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::IImplRepBase_*********
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    class Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::IImplRepBase_ : public Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep {
    };

    /*
     ********************************************************************************
     *********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Rep_*****************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER>
    class Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (Bijection_Base::InjectivityViolationPolicy injectivityViolationPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
            : fInjectivityViolationPolicy_{injectivityViolationPolicy}
            , fDomainEqualsComparer_{domainEqualsComparer}
            , fRangeEqualsComparer_{rangeEqualsComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        const Bijection_Base::InjectivityViolationPolicy        fInjectivityViolationPolicy_;
        [[NO_UNIQUE_ADDRESS_ATTR]] const DOMAIN_EQUALS_COMPARER fDomainEqualsComparer_;
        [[NO_UNIQUE_ADDRESS_ATTR]] const RANGE_EQUALS_COMPARER  fRangeEqualsComparer_;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.GetLength ();
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return fData_.IsEmpty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> FindFirstThat (const function<bool (ArgByValueType<value_type> item)>& doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        iLink = fData_.FindFirstThat (doToElement);
            if (iLink == nullptr) {
                return nullptr;
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (&fData_, &fChangeCounts_);
            resultRep->fIterator.SetCurrentLink (iLink);
            return Iterator<value_type>{move (resultRep)};
        }

        // Bijection<DOMAIN_TYPE, RANGE_TYPE::BijectionTraitsType>::_IRep overrides
    public:
        virtual _BijectionRepSharedPtr CloneEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (fInjectivityViolationPolicy_, fDomainEqualsComparer_, fRangeEqualsComparer_);
        }
        virtual _BijectionRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            auto                                                        result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto&                                                       mir    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep& rhs) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual DomainEqualsCompareFunctionType GetDomainEqualsComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return DomainEqualsCompareFunctionType{fDomainEqualsComparer_};
        }
        virtual RangeEqualsCompareFunctionType GetRangeEqualsComparer () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return RangeEqualsCompareFunctionType{fRangeEqualsComparer_};
        }
        virtual Iterable<DOMAIN_TYPE> Preimage () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_PreImage_Reference_Implementation ();
        }
        virtual Iterable<RANGE_TYPE> Image () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            return this->_Image_Reference_Implementation ();
        }
        virtual bool Lookup (ArgByValueType<DOMAIN_TYPE> key, optional<RANGE_TYPE>* item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            for (typename DataStructures::LinkedList<value_type>::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fDomainEqualsComparer_ (it.Current ().first, key)) {
                    if (item != nullptr) {
                        *item = it.Current ().second;
                    }
                    return true;
                }
            }
            if (item != nullptr) {
                *item = nullopt;
            }
            return false;
        }
        virtual bool InverseLookup (ArgByValueType<RANGE_TYPE> key, optional<DOMAIN_TYPE>* item) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedMutex> readLock{fData_};
            for (typename DataStructures::LinkedList<value_type>::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fRangeEqualsComparer_ (it.Current ().second, key)) {
                    if (item != nullptr) {
                        *item = it.Current ().first;
                    }
                    return true;
                }
            }
            if (item != nullptr) {
                *item = nullopt;
            }
            return false;
        }
        virtual void Add (ArgByValueType<DOMAIN_TYPE> key, ArgByValueType<RANGE_TYPE> newElt) override
        {
            // @todo check fInjectivityViolationPolicy_
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            switch (fInjectivityViolationPolicy_) {
                case Bijection_Base::InjectivityViolationPolicy::eAssertionError: {
#if qDebug
                    optional<DOMAIN_TYPE> back;
                    if (InverseLookup (newElt, &back)) {
                        Require (fDomainEqualsComparer_ (key, *back));
                    }

#endif
                } break;
                case Bijection_Base::InjectivityViolationPolicy::eThrowException: {
                    optional<DOMAIN_TYPE> back;
                    if (InverseLookup (newElt, &back)) {
                        if (not fDomainEqualsComparer_ (key, *back)) [[UNLIKELY_ATTR]] {
                            Execution::Throw (Bijection_Base::InjectivityViolation{});
                        }
                    }
                } break;
                default:
                    AssertNotReached ();
            }
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fDomainEqualsComparer_ (it.Current ().first, key)) {
                    fData_.SetAt (it, value_type{key, newElt});
                    return;
                }
            }
            fData_.Prepend (value_type{key, newElt}); // order meaningless for bijection, and prepend cheaper on linked list
            fChangeCounts_.PerformedChange ();
        }
        virtual void RemoveDomainElement (ArgByValueType<DOMAIN_TYPE> d) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fDomainEqualsComparer_ (it.Current ().first, d)) {
                    fData_.RemoveAt (it);
                    fChangeCounts_.PerformedChange ();
                    return;
                }
            }
        }
        virtual void RemoveRangeElement (ArgByValueType<RANGE_TYPE> r) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fRangeEqualsComparer_ (it.Current ().second, r)) {
                    fData_.RemoveAt (it);
                    fChangeCounts_.PerformedChange ();
                    return;
                }
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            scoped_lock<Debug::AssertExternallySynchronizedMutex> writeLock{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            fData_.RemoveAt (mir.fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                          fData_;
        [[NO_UNIQUE_ADDRESS_ATTR]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     **************** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE> *****************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList ()
        : Bijection_LinkedList{equal_to<DOMAIN_TYPE>{}, equal_to<RANGE_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_TYPE, DOMAIN_EQUALS_COMPARER> () and Common::IsPotentiallyComparerRelation<RANGE_TYPE, RANGE_EQUALS_COMPARER> ()>*>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
        : Bijection_LinkedList (InjectivityViolationPolicy::eDEFAULT, domainEqualsComparer, rangeEqualsComparer)
    {
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Configuration::is_callable<DOMAIN_EQUALS_COMPARER>::value and Configuration::is_callable<RANGE_EQUALS_COMPARER>::value>*>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (InjectivityViolationPolicy injectivityCheckPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer, const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
        : inherited (inherited::template MakeSmartPtr<Rep_<DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>> (injectivityCheckPolicy, domainEqualsComparer, rangeEqualsComparer))
    {
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename CONTAINER_OF_ADDABLE>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (const CONTAINER_OF_ADDABLE& src)
        : Bijection_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename COPY_FROM_ITERATOR_KVP_T, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_KVP_T>>*>
    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (COPY_FROM_ITERATOR_KVP_T start, COPY_FROM_ITERATOR_KVP_T end)
        : Bijection_LinkedList{}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Bijection_LinkedList_inl_ */
