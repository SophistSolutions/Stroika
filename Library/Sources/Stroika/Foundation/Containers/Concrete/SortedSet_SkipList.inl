/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *************************** SortedSet_SkipList<T>::Rep_ ************************
     ********************************************************************************
     */
    template <typename T>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IThreeWayComparer<T>) COMPARER>
    class SortedSet_SkipList<T>::Rep_ : public Private::SkipListBasedContainerRepImpl<Rep_<COMPARER>, IImplRepBase_>,
                                        public Memory::UseBlockAllocationIfAppropriate<Rep_<COMPARER>> {
    private:
        using inherited = Private::SkipListBasedContainerRepImpl<Rep_<COMPARER>, IImplRepBase_>;

    public:
        static_assert (not is_reference_v<COMPARER>);

    public:
        Rep_ (const COMPARER& comparer)
            : fData_{comparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<T>::_IRep> Clone () const override
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
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Invariant ();
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply ([&] (const auto& k) { doToElement (k.fKey); });
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (that, seq); // @todo rewrite to use fData
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.Find (v);
#if 0
            Ensure ((found == fData_.end () and this->inherited::Find_equal_to (v, seq) == Iterator<value_type>{nullptr}) or
                    (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->inherited::Find_equal_to (v, seq).ConstGetRep ())
                                  .fIterator.GetUnderlyingIteratorRep ()));
#endif
            return Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, found)};
        }

        // Set<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementEqualityComparerType{Common::EqualsComparerAdapter<T, COMPARER>{fData_.key_comp ()}};
        }
        virtual shared_ptr<typename Set<T>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data in clone
        }
        virtual shared_ptr<typename Set<T>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Iterable<value_type>::_IRep& rhs) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Lookup (ArgByValueType<value_type> item, optional<value_type>* oResult, Iterator<value_type>* iResult) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  i       = fData_.Find (item);
            bool                                                  notDone = i != fData_.end ();
            if (oResult != nullptr and notDone) {
                *oResult = i->fKey;
            }
            if (iResult != nullptr and notDone) {
                *iResult = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, i)};
            }
            return notDone;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Add (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual bool RemoveIf (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            auto i = fData_.Find (item);
            if (i != fData_.end ()) [[likely]] {
                fData_.Remove (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            mir.fIterator.AssertDataMatches (&fData_);
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

        // SortedSet<T>::_IRep overrides
    public:
        virtual ElementInOrderComparerType GetInOrderComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementInOrderComparerType{Common::InOrderComparerAdapter<T, COMPARER>{fData_.key_comp ()}};
        }

    private:
        using DataStructureImplType_ = SKIPLIST<COMPARER>;
        struct IteratorRep_ : Private::IteratorImplHelper_<value_type, DataStructureImplType_> {
            using inherited = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;
            using inherited::inherited; // forward base class constructors
            // override to map just the key part to 'T'
            virtual void More (optional<T>* result, bool advance) override
            {
                RequireNotNull (result);
                this->ValidateChangeCount ();
                if (advance) [[likely]] {
                    Require (not this->fIterator.Done ());
                    ++this->fIterator;
                }
                if (this->fIterator.Done ()) [[unlikely]] {
                    *result = nullopt;
                }
                else {
                    *result = this->fIterator->fKey;
                }
            }
            virtual auto Clone () const -> unique_ptr<typename Iterator<T>::IRep> override
            {
                this->ValidateChangeCount ();
                return make_unique<IteratorRep_> (*this);
            }
        };

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend inherited;
    };

    /*
     ********************************************************************************
     **************************** SortedSet_SkipList<T> *****************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedSet_SkipList<T>::SortedSet_SkipList ()
        : SortedSet_SkipList{compare_three_way{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IThreeWayComparer<T> COMPARER>
    inline SortedSet_SkipList<T>::SortedSet_SkipList (COMPARER&& comparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<COMPARER>>> (forward<COMPARER> (comparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IThreeWayComparer<T> COMPARER>
    inline SortedSet_SkipList<T>::SortedSet_SkipList (COMPARER&& comparer, const initializer_list<T>& src)
        : SortedSet_SkipList{forward<COMPARER> (comparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedSet_SkipList<T>>)
    inline SortedSet_SkipList<T>::SortedSet_SkipList (ITERABLE_OF_ADDABLE&& src)
        : SortedSet_SkipList{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <IThreeWayComparer<T> COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedSet_SkipList<T>::SortedSet_SkipList (COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src)
        : SortedSet_SkipList (forward<COMPARER> (comparer))
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedSet_SkipList<T>::SortedSet_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedSet_SkipList{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IThreeWayComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedSet_SkipList<T>::SortedSet_SkipList (COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedSet_SkipList (forward<COMPARER> (comparer))
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void SortedSet_SkipList<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
