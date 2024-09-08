/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/LinkedList.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********** Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Rep_*****************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IEqualsComparer<DOMAIN_TYPE>) DOMAIN_EQUALS_COMPARER,
              BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IEqualsComparer<RANGE_TYPE>) RANGE_EQUALS_COMPARER>
    class Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Rep_
        : public IImplRepBase_,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (DataExchange::ValidationStrategy injectivityViolationPolicy, const DOMAIN_EQUALS_COMPARER& domainEqualsComparer,
              const RANGE_EQUALS_COMPARER& rangeEqualsComparer)
            : fInjectivityViolationPolicy_{injectivityViolationPolicy}
            , fDomainEqualsComparer_{domainEqualsComparer}
            , fRangeEqualsComparer_{rangeEqualsComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        const DataExchange::ValidationStrategy             fInjectivityViolationPolicy_;
        [[no_unique_address]] const DOMAIN_EQUALS_COMPARER fDomainEqualsComparer_;
        [[no_unique_address]] const RANGE_EQUALS_COMPARER  fRangeEqualsComparer_;

        // Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (auto iLink = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep overrides
    public:
        virtual shared_ptr<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fInjectivityViolationPolicy_, fDomainEqualsComparer_, fRangeEqualsComparer_);
        }
        virtual shared_ptr<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep& rhs) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual DomainEqualsCompareFunctionType GetDomainEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return DomainEqualsCompareFunctionType{fDomainEqualsComparer_};
        }
        virtual RangeEqualsCompareFunctionType GetRangeEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return RangeEqualsCompareFunctionType{fRangeEqualsComparer_};
        }
        virtual bool Lookup (ArgByValueType<DOMAIN_TYPE> key, optional<RANGE_TYPE>* item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            for (typename DataStructures::LinkedList<value_type>::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fDomainEqualsComparer_ (it->first, key)) {
                    if (item != nullptr) {
                        *item = it->second;
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
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            for (typename DataStructures::LinkedList<value_type>::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fRangeEqualsComparer_ (it->second, key)) {
                    if (item != nullptr) {
                        *item = it->first;
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
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            switch (fInjectivityViolationPolicy_) {
                case DataExchange::ValidationStrategy::eAssertion: {
                    if constexpr (qDebug) {
                        optional<DOMAIN_TYPE> back;
                        if (InverseLookup (newElt, &back)) {
                            Require (fDomainEqualsComparer_ (key, *back));
                        }
                    }
                } break;
                case DataExchange::ValidationStrategy::eThrow: {
                    optional<DOMAIN_TYPE> back;
                    if (InverseLookup (newElt, &back)) {
                        if (not fDomainEqualsComparer_ (key, *back)) [[unlikely]] {
                            Execution::Throw (Bijection_Base::InjectivityViolation{});
                        }
                    }
                } break;
                default:
                    AssertNotReached ();
            }
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fDomainEqualsComparer_ (it->first, key)) {
                    fData_.SetAt (it, value_type{key, newElt});
                    fChangeCounts_.PerformedChange ();
                    return;
                }
            }
            fData_.push_front (value_type{key, newElt}); // order meaningless for bijection, and prepend cheaper on linked list
            fChangeCounts_.PerformedChange ();
        }
        virtual void RemoveDomainElement (ArgByValueType<DOMAIN_TYPE> d) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fDomainEqualsComparer_ (it->first, d)) {
                    fData_.Remove (it);
                    fChangeCounts_.PerformedChange ();
                    return;
                }
            }
        }
        virtual void RemoveRangeElement (ArgByValueType<RANGE_TYPE> r) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fRangeEqualsComparer_ (it->second, r)) {
                    fData_.Remove (it);
                    fChangeCounts_.PerformedChange ();
                    return;
                }
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (nextI == nullptr) {
                fData_.Remove (mir.fIterator);
                fChangeCounts_.PerformedChange ();
            }
            else {
                auto ret = fData_.erase (mir.fIterator);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, ret)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
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
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer,
                                                                                RANGE_EQUALS_COMPARER&&  rangeEqualsComparer)
        : Bijection_LinkedList{DataExchange::ValidationStrategy::eAssertion, forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer),
                               forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (DataExchange::ValidationStrategy injectivityCheckPolicy,
                                                                                DOMAIN_EQUALS_COMPARER&&         domainEqualsComparer,
                                                                                RANGE_EQUALS_COMPARER&&          rangeEqualsComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER>> (injectivityCheckPolicy, domainEqualsComparer, rangeEqualsComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (const initializer_list<value_type>& src)
        : Bijection_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&&            domainEqualsComparer,
                                                                                RANGE_EQUALS_COMPARER&&             rangeEqualsComparer,
                                                                                const initializer_list<value_type>& src)
        : Bijection_LinkedList{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>>)
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (ITERABLE_OF_ADDABLE&& src)
        : Bijection_LinkedList{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER, IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer,
                                                                                RANGE_EQUALS_COMPARER&& rangeEqualsComparer, ITERABLE_OF_ADDABLE&& src)
        : Bijection_LinkedList{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE>
    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Bijection_LinkedList{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER, IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE>
    Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::Bijection_LinkedList (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer,
                                                                         RANGE_EQUALS_COMPARER&&  rangeEqualsComparer,
                                                                         ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Bijection_LinkedList{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection_LinkedList<DOMAIN_TYPE, RANGE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}
