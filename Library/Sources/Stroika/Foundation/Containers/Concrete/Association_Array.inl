/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Association_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/Array.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ******* Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ **********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {
    public:
        virtual void   shrink_to_fit ()              = 0;
        virtual size_t capacity () const             = 0;
        virtual void   reserve (size_t slotsAlloced) = 0;
    };

    /*
     ********************************************************************************
     ************ Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ **************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    class Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_,
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
            return fData_.size () == 0;
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement, seq);
        }
        virtual Iterator<value_type>
        Find ([[maybe_unused]] const shared_ptr<typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep>& thisSharedPtr,
              const function<bool (ArgByValueType<value_type> item)>& that, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (optional<size_t> i = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *i)};
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
            for (typename DataStructureImplType_::ForwardIterator it (&fData_); not it.Done (); ++it) {
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    result.push_back (it.Current ().fValue);
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
                if (fKeyEqualsComparer_ (it.Current ().fKey, key)) {
                    fData_.RemoveAt (it.CurrentIndex ());
                    return true;
                }
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            optional<size_t>                                       savedUnderlyingIndex;
            static_assert (is_same_v<size_t, typename DataStructureImplType_::UnderlyingIteratorRep>); // else must do slightly differently
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
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
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }

        // IImplRepBase_ overrides
    public:
        virtual void shrink_to_fit () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.shrink_to_fit ();
        }
        virtual size_t capacity () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.capacity ();
        }
        virtual void reserve (size_t slotsAlloced) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.reserve (slotsAlloced);
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
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
        reserve (src.size ());
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer,
                                                                              const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association_Array{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        reserve (src.size ());
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
            reserve (src.size ());
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
                reserve (end - start);
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
                reserve (end - start);
            }
        }
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename Iterable<value_type>::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor{this}._GetWriteableRep ().shrink_to_fit ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline size_t Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::capacity () const
    {
        using _SafeReadRepAccessor = typename Iterable<value_type>::template _SafeReadRepAccessor<IImplRepBase_>;
        return _SafeReadRepAccessor{this}._ConstGetRep ().capacity ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::reserve (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename Iterable<value_type>::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor{this}._GetWriteableRep ().reserve (slotsAlloced);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Association_Array_inl_ */
