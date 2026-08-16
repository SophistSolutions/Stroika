/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <vector>

#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Containers/Support/ReserveTweaks.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    template <typename T>
    class Sequence_stdvector<T>::Rep_ : public Sequence<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Sequence<T>::_IRep;

    protected:
        static constexpr size_t _kSentinelLastItemIndex = inherited::_kSentinelLastItemIndex;

    public:
        Rep_ ()                = default;
        Rep_ (const Rep_& src) = default;
        Rep_ (vector<T>&& src)
            : fData_{move (src)}
        {
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<T>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find ([[maybe_unused]] bool findFirst, const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            auto                                                    iLink = const_cast<DataStructureImplType_&> (fData_).Find (that);
            if (iLink == fData_.end ()) {
                return nullptr;
            }
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
        }
        // fData_ IS a vector<value_type> (via STLContainerWrapper), so iteration order is storage order.
        virtual optional<span<const value_type>> PeekContiguousStorage () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return span<const value_type>{fData_.data (), fData_.size ()};
        }

        // Sequence<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Sequence<T>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> ();
        }
        virtual shared_ptr<typename Sequence<T>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            auto                                                    result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (
                &mir.fIterator, &fData_,
                [targetI = mir.fIterator.GetUnderlyingIteratorRep ()] (auto oldI, [[maybe_unused]] auto newI) { return targetI == oldI; });
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual value_type GetAt (size_t i) const override
        {
            Require (not empty ());
            Require (i == _kSentinelLastItemIndex or i < size ());
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            if (i == _kSentinelLastItemIndex) {
                i = fData_.size () - 1;
            }
            return fData_[i];
        }
        virtual BidirectionalIterator<value_type> GetBidirectionalIterator () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return BidirectionalIterator<value_type>{make_unique<RandomAccessIteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual RandomAccessIterator<value_type> GetRandomAccessIterator () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return RandomAccessIterator<value_type>{make_unique<RandomAccessIteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual void SetAt (size_t i, ArgByValueType<value_type> item) override
        {
            Require (i < size ());
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            fData_[i] = item;
            fChangeCounts_.PerformedChange ();
        }
        virtual size_t IndexOf (const Iterator<value_type>& i) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.CurrentIndex ();
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.AtEnd ());
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            const IteratorRep_& iRep = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            iRep.fIterator.AssertDataMatches (&fData_);
            auto newI = fData_.erase (iRep.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, newI)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedChecker::WriteContext declareWriteContext{fData_};
            const IteratorRep_& iRep = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = iRep.fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.Invariant ();
            *fData_.remove_constness (iRep.fIterator.GetUnderlyingIteratorRep ()) = newValue;
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
            fData_.Invariant ();
        }
        virtual void Insert (size_t at, const span<const value_type>& copyFrom) override
        {
            Require (at == _kSentinelLastItemIndex or at <= size ());
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            if (at == _kSentinelLastItemIndex) {
                at = fData_.size ();
            }
            Support::ReserveTweaks::Reserve4AddN (fData_, copyFrom.size ());
            fData_.insert (fData_.begin () + at, copyFrom.begin (), copyFrom.end ());
            fChangeCounts_.PerformedChange ();
        }
        virtual void Remove (size_t from, size_t to) override
        {
            Require ((from <= to) and (to <= this->size ()));
            if (from != to) {
                Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
                fData_.erase (fData_.begin () + from, fData_.begin () + to);
                fChangeCounts_.PerformedChange ();
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<vector<value_type>>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        // same as IteratorImplHelper_DefaultTraits<value_type, DataStructureImplType_>, but pointing at
        // DataStructureImplType_::RandomAccessIterator instead of its (forward-only) ForwardIterator
        struct RandomAccessIteratorTraits_ : Private::IteratorImplHelper_DefaultTraits<value_type, DataStructureImplType_> {
            using DataStructureIteratorT = typename DataStructureImplType_::RandomAccessIterator;
        };
        using RandomAccessIteratorRep_ = Private::RandomAccessIteratorImplHelper_<value_type, DataStructureImplType_, RandomAccessIteratorTraits_>;

    private:
        DataStructureImplType_                       fData_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend class Private::StdVectorBasedContainer<Sequence_stdvector<T>, Sequence<T>>;
    };

    /*
     ********************************************************************************
     ******************************* Sequence_stdvector<T> **************************
     ********************************************************************************
     */
    template <typename T>
    inline Sequence_stdvector<T>::Sequence_stdvector ()
        : inherited{Memory::MakeSharedPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Sequence_stdvector<T>::Sequence_stdvector (const initializer_list<value_type>& src)
        : Sequence_stdvector{}
    {
        this->reserve (src.size ());
        this->AppendAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOfTo<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence_stdvector<T>>)
    inline Sequence_stdvector<T>::Sequence_stdvector (ITERABLE_OF_ADDABLE&& src)
        : Sequence_stdvector{}
    {
        if constexpr (Common::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
            this->reserve (src.size ());
        }
        this->AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    inline Sequence_stdvector<T>::Sequence_stdvector (std::vector<T>&& src)
        : inherited{Memory::MakeSharedPtr<Rep_> (move (src))}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Sequence_stdvector<T>::Sequence_stdvector (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Sequence_stdvector{}
    {
        if constexpr (random_access_iterator<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                this->reserve (end - start);
            }
        }
        this->AppendAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Sequence_stdvector<T>::AssertRepValidType_ () const
    {
        if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
