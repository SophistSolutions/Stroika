/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_

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
     ***** Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ *******
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {};

    /*
     ********************************************************************************
     ********* Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_*************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    class Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_,
                                                                      public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        static_assert (not is_reference_v<KEY_EQUALS_COMPARER>);

    public:
        Rep_ (const KEY_EQUALS_COMPARER& keyEqualsComparer)
            : fKeyEqualsComparer_{keyEqualsComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        [[no_unique_address]] const KEY_EQUALS_COMPARER fKeyEqualsComparer_;

        // Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
        }
        virtual Iterator<value_type>
        MakeIterator ([[maybe_unused]] const shared_ptr<typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep>& thisSharedPtr) const override
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
        virtual Iterator<value_type>
        Find ([[maybe_unused]] const shared_ptr<typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep>& thisSharedPtr,
              const function<bool (ArgByValueType<value_type> item)>& that, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (auto iLink = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return KeyEqualsCompareFunctionType{fKeyEqualsComparer_};
        }
        virtual shared_ptr<typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fKeyEqualsComparer_);
        }
        virtual shared_ptr<typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneAndPatchIterator ([[maybe_unused]] Iterator<value_type>* i) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual Iterable<mapped_type> Lookup (ArgByValueType<KEY_TYPE> key) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            vector<mapped_type>                                   result;
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    result.push_back (it.Current ().fValue);
                }
            }
            return Iterable<mapped_type>{move (result)};
        }
        virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<mapped_type> newElt) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Prepend (value_type{key, newElt}); // cheaper to prepend, and order doesn't matter
            fChangeCounts_.PerformedChange ();
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    fData_.RemoveAt (it);
                    fChangeCounts_.PerformedChange ();
                    return true;
                }
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI); // advance to next item if deleting current one
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto dataStructureIterator                    = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator;
            fData_.PeekAt (dataStructureIterator)->fValue = newValue;
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, dataStructureIterator.GetUnderlyingIteratorRep ())};
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
     ************* Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE> **************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList ()
        : Association_LinkedList{equal_to<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_EQUALS_COMPARER>>> (keyEqualsComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer,
                                                                                        const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association_LinkedList{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <ranges::range ITERABLE_OF_ADDABLE, enable_if_t<not is_base_of_v<Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (ITERABLE_OF_ADDABLE&& src)
#else
    template <ranges::range ITERABLE_OF_ADDABLE>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (ITERABLE_OF_ADDABLE&& src)
        requires (not is_base_of_v<Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>)
#endif
        : Association_LinkedList{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, ranges::range ITERABLE_OF_ADDABLE>
    inline Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src)
        : Association_LinkedList{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <input_iterator ITERATOR_OF_ADDABLE>
    Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Association_LinkedList{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
    Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_LinkedList (KEY_EQUALS_COMPARER&& keyEqualsComparer,
                                                                                 ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Association_LinkedList{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_LinkedList<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_LinkedList_inl_ */
