/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "Stroika/Foundation/Containers/DataStructures/Array.h"
#include "Stroika/Foundation/Containers/Private/ArraySupport.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ************ Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ **************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IEqualsComparer<KEY_TYPE>) KEY_EQUALS_COMPARER>
    class Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_
        : public Private::ArrayBasedContainerRepImpl<Rep_<KEY_EQUALS_COMPARER>, IImplRepBase_>,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EQUALS_COMPARER>> {
    private:
        using inherited = Private::ArrayBasedContainerRepImpl<Rep_<KEY_EQUALS_COMPARER>, IImplRepBase_>;

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
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement, seq);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (auto i = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, i)};
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
            return Memory::MakeSharedPtr<Rep_> (fKeyEqualsComparer_); // keep comparer, but lose data
        }
        virtual shared_ptr<typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
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
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fKeyEqualsComparer_ (it->fKey, key)) {
                    result.push_back (it->fValue);
                }
            }
            return Iterable<mapped_type>{move (result)};
        }
        virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.push_back (value_type{key, newElt});
            fChangeCounts_.PerformedChange ();
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (fKeyEqualsComparer_ (it->fKey, key)) {
                    fData_.RemoveAt (it.CurrentIndex ());
                    fChangeCounts_.PerformedChange ();
                    return true;
                }
            }
            return false;
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
                auto retI = fData_.erase (mir.fIterator);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, retI)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext           declareWriteContext{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.PeekAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.CurrentIndex ())->fValue = newValue;
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend inherited;
    };

    /*
     ********************************************************************************
     ************* Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE> *******************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array ()
        : Association_Array{equal_to<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_EQUALS_COMPARER>>> (keyEqualsComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association_Array{}
    {
        this->reserve (src.size ());
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer,
                                                                              const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association_Array{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        this->reserve (src.size ());
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (ITERABLE_OF_ADDABLE&& src)
        : Association_Array{}
    {
        if constexpr (Configuration::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
            this->reserve (src.size ());
        }
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src)
        : Association_Array{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        reserve (src.size ());
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Association_Array{}
    {
        if constexpr (random_access_iterator<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                this->reserve (end - start);
            }
        }
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE&& start,
                                                                       ITERATOR_OF_ADDABLE&& end)
        : Association_Array{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        if constexpr (random_access_iterator<ITERATOR_OF_ADDABLE>) {
            if (start != end) {
                this->reserve (end - start);
            }
        }
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}
