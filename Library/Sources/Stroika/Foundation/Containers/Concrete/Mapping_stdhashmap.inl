/*
    * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
    */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdhashmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdhashmap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../../Common/Compare.h"
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    namespace Private_ {
        // WAG - @todo consider
        constexpr size_t kDefaultMapping_stdhashmapHashSize_ = 16u;
    }

    /*
     ********************************************************************************
     ****** Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ **********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ : public Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {};

    /*
     ********************************************************************************
     ********** Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ***************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    class Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_,
                                                                  public Memory::UseBlockAllocationIfAppropriate<Rep_<HASH, KEY_EQUALS_COMPARER>> {
    public:
        static_assert (not is_reference_v<HASH>);
        static_assert (not is_reference_v<KEY_EQUALS_COMPARER>);

    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const HASH& hashFun, const KEY_EQUALS_COMPARER& equalsComparer)
            : fData_{Private_::kDefaultMapping_stdhashmapHashSize_, hashFun, equalsComparer}
        {
        }
        Rep_ (STDHASHMAP<>&& src)
            : fData_{move (src)}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

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
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const shared_ptr<typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep>& thisSharedPtr,
                                           const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (thisSharedPtr, that, seq); // @todo rewrite to use fData
        }

        // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            // @todo fix shouldn't need this cast!!!
            return Common::DeclareEqualsComparer ((function<bool (KEY_TYPE, KEY_TYPE)>)fData_.key_eq ());
        }
        virtual shared_ptr<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fData_.hash_function (), fData_.key_eq ()); // keep hash/comparer, but lose data
        }
        virtual shared_ptr<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            result->fData_.MoveIteratorHereAfterClone (&Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ()).fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<MAPPED_VALUE_TYPE>* item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  i = fData_.find (key);
            if (i == fData_.end ()) {
                if (item != nullptr) {
                    *item = nullopt;
                }
                return false;
            }
            else {
                if (item != nullptr) {
                    *item = i->second;
                }
                return true;
            }
        }
        virtual bool Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt, AddReplaceMode addReplaceMode) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            bool result{};
            switch (addReplaceMode) {
                case AddReplaceMode::eAddReplaces:
                    result = fData_.insert_or_assign (key, newElt).second;
                    break;
                case AddReplaceMode::eAddIfMissing:
                    result = fData_.insert ({key, newElt}).second;
                    break;
                default:
                    AssertNotReached ();
            }
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
            return result;
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI); // advance to next item if deleting current one
            }
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            (void)fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext           declareWriteContext{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_
                .remove_constness (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ())
                ->second = newValue;
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDHASHMAP<HASH, KEY_EQUALS_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     *************** Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE> ****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap ()
        requires (Cryptography::Digest::IsHashFunction<std::hash<KEY_TYPE>, KEY_TYPE> and IEqualsComparer<std::equal_to<KEY_TYPE>, KEY_TYPE>)
        : Mapping_stdhashmap{std::hash<KEY_TYPE>{}, std::equal_to<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (STDHASHMAP<>&& src)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        requires (is_default_constructible_v<Mapping_stdhashmap>)
#endif
        : inherited{Memory::MakeSharedPtr<Rep_<typename STDHASHMAP<>::hasher, typename STDHASHMAP<>::key_equal>> (move (src))}
    {
#if qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        static_assert (is_default_constructible_v<Mapping_stdhashmap>);
#endif
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer)
        requires (Cryptography::Digest::IsHashFunction<HASH, KEY_TYPE>)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<HASH>, remove_cvref_t<KEY_EQUALS_COMPARER>>> (
              forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        requires (is_default_constructible_v<Mapping_stdhashmap>)
#endif
        : Mapping_stdhashmap{}
    {
#if qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        static_assert (is_default_constructible_v<Mapping_stdhashmap>);
#endif
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer,
                                                                                const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        requires (Cryptography::Digest::IsHashFunction<HASH, KEY_TYPE>)
        : Mapping_stdhashmap{forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERABLE_OF_ADDABLE>
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (ITERABLE_OF_ADDABLE&& src)
        requires (is_default_constructible_v<Mapping_stdhashmap> and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and
                  not is_base_of_v<Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>)
        : Mapping_stdhashmap{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE>
    inline Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        requires (Cryptography::Digest::IsHashFunction<HASH, KEY_TYPE> and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and
                  not is_base_of_v<Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>, decay_t<ITERABLE_OF_ADDABLE>>)
        : Mapping_stdhashmap{forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif

    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <input_iterator ITERATOR_OF_ADDABLE>
    Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)

#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        requires (is_default_constructible_v<Mapping_stdhashmap>)
#endif
        : Mapping_stdhashmap{}
    {
#if qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        static_assert (is_default_constructible_v<Mapping_stdhashmap>);
#endif
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
    Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_stdhashmap (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer,
                                                                         ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (Cryptography::Digest::IsHashFunction<HASH, KEY_TYPE>)
        : Mapping_stdhashmap{forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }

    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping_stdhashmap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_stdhashmap_inl_ */
