/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/SkipList.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********************** SortedCollection_SkipList<T>::Rep_ *********************
     ********************************************************************************
     */
    template <typename T>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (Common::IThreeWayComparer<T>) COMPARER>
    class SortedCollection_SkipList<T>::Rep_ : public Private::SkipListBasedContainerRepImpl<Rep_<COMPARER>, IImplRepBase_>,
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
            fData_.Apply ([&] (auto arg) { doToElement (arg.fKey); });
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (auto iLink = fData_.Find ([&] (auto arg) { return that (arg.fKey); })) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Collection<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Collection<T>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data in clone
        }
        virtual shared_ptr<typename Collection<T>::_IRep> CloneAndPatchIterator (Iterator<T>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual void Add (ArgByValueType<value_type> item, Iterator<value_type>* oAddedI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (oAddedI == nullptr) [[likely]] {
                fData_.Add (item);
                fChangeCounts_.PerformedChange ();
            }
            else {
                typename DataStructureImplType_::ForwardIterator retIt;
                fData_.Add (item, &retIt);
                fChangeCounts_.PerformedChange ();
                *oAddedI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, retIt)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareWriteContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            fData_.Remove (mir.fIterator);
            if (nextI == nullptr) {
                fData_.Add (newValue);
                fChangeCounts_.PerformedChange ();
            }
            else {
                typename DataStructureImplType_::ForwardIterator retIt;
                fData_.Add (newValue, &retIt);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, retIt)};
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (nextI == nullptr) [[likely]] {
                fData_.Remove (mir.fIterator);
                fChangeCounts_.PerformedChange ();
            }
            else {
                auto ret = fData_.erase (mir.fIterator);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, ret)};
            }
        }

        // SortedCollection<T>::_IRep overrides
    public:
        virtual InOrderComparerType GetInOrderComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Common::InOrderComparerAdapter<T, COMPARER>{fData_.key_comp ()};
        }
        virtual bool Contains (ArgByValueType<value_type> item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return not fData_.Find (item).Done ();
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Remove (item);
            fChangeCounts_.PerformedChange ();
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
     *********************** SortedCollection_SkipList<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList ()
        : SortedCollection_SkipList{compare_three_way{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IThreeWayComparer<T> COMPARER>
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList (COMPARER&& inorderComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<COMPARER>>> (inorderComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList (const initializer_list<T>& src)
        : SortedCollection_SkipList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IThreeWayComparer<T> COMPARER>
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList (COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedCollection_SkipList{forward<COMPARER> (inOrderComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedCollection_SkipList<T>>)
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList (ITERABLE_OF_ADDABLE&& src)
        : SortedCollection_SkipList{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <IThreeWayComparer<T> COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList (COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedCollection_SkipList{forward<COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedCollection_SkipList{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IThreeWayComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedCollection_SkipList<T>::SortedCollection_SkipList (COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedCollection_SkipList{forward<COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void SortedCollection_SkipList<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
