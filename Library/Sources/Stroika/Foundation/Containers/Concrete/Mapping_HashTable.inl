/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********** Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ***************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (DataStructures::HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_VALUE_TYPE>) HASH_TABLE_TRAITS>
#if !qCompilerAndStdLib_template_ConstraintDiffersInTemplateRedeclaration_Buggy
        requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces)
#endif
    class Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_
        : public Private::HashTableBasedContainerRepImpl<Rep_<HASH_TABLE_TRAITS>, IImplRepBase_>,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<HASH_TABLE_TRAITS>> {
    private:
        using inherited = Private::HashTableBasedContainerRepImpl<Rep_<HASH_TABLE_TRAITS>, IImplRepBase_>;

    public:
        Rep_ (const typename HASH_TABLE_TRAITS::KeyHasherType& hashFun, const typename HASH_TABLE_TRAITS::KeyEqualsComparerType& equalsComparer)
            : fData_{hashFun, equalsComparer}
        {
        }
        Rep_ (HASHTABLE<HASH_TABLE_TRAITS>&& src)
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
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            fData_.Invariant ();
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (bool findFirst, const function<bool (ArgByValueType<value_type> item)>& that,
                                           Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return this->inherited::Find (findFirst, that, seq); // @todo rewrite to use fData
        }

        // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            return fData_.key_eq ();
        }
        virtual shared_ptr<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            // @todo UNCLEAR if this should also clone # buckets - but probably not
            return Memory::MakeSharedPtr<Rep_> (fData_.hash_function (), fData_.key_eq ()); // keep hash/comparer, but lose data
        }
        virtual shared_ptr<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            auto                                                    result = Memory::MakeSharedPtr<Rep_> (*this);
            result->fData_.MoveIteratorHereAfterClone (&Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ()).fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<MAPPED_VALUE_TYPE>* item) const override
        {
            Debug::AssertExternallySynchronizedChecker::ReadContext declareContext{fData_};
            auto                                                    i = fData_.find (key);
            if (i == fData_.end ()) {
                if (item != nullptr) {
                    *item = nullopt;
                }
                return false;
            }
            else {
                if (item != nullptr) {
                    *item = i->fValue;
                }
                return true;
            }
        }
        virtual bool Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt, AddReplaceMode addReplaceMode) override
        {
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            fData_.Invariant ();
            bool result{};
            // NOTE BOOL RESULT MEANDING DIFFERS BETWEEN MAPPING AND HASHTABLE!!!!
            switch (addReplaceMode) {
                case AddReplaceMode::eAddReplaces: {
                    // HashTable returns true iff change, but Mapping returns true if size enlarged
                    size_t oldSize = fData_.size ();
                    fData_.Add (key, newElt);
                    result = oldSize != fData_.size ();
                } break;
                case AddReplaceMode::eAddIfMissing: {
                    auto i = fData_.find (key);
                    if (i == fData_.end ()) {
                        result = fData_.Add (key, newElt);
                        Assert (result);
                    }
                } break;
                default:
                    AssertNotReached ();
            }
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
            return result;
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.Remove (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.AtEnd ());
            Debug::AssertExternallySynchronizedChecker::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            mir.fIterator.AssertDataMatches (&fData_);
            if (nextI == nullptr) {
                fData_.Remove (mir.fIterator);
                fChangeCounts_.PerformedChange ();
            }
            else {
                typename DataStructureImplType_::ForwardIterator hNextI;
                fData_.Remove (mir.fIterator, &hNextI);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, hNextI.GetUnderlyingIteratorRep ())};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedChecker::WriteContext declareWriteContext{fData_};
            auto iterRep = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            fData_.Update (iterRep.fIterator, newValue); // doesn't invalidate iterator
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iterRep.fIterator.GetUnderlyingIteratorRep ())};
            }
        }

    private:
        using DataStructureImplType_ = HASHTABLE<HASH_TABLE_TRAITS>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                       fData_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend inherited; // for HashTableBasedContainerRepImpl
    };

    /*
     ********************************************************************************
     **************** Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE> ****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable ()
        requires (Cryptography::Digest::IHashFunction<std::hash<KEY_TYPE>, KEY_TYPE> and IEqualsComparer<std::equal_to<KEY_TYPE>, KEY_TYPE>)
        : Mapping_HashTable{std::hash<KEY_TYPE>{}, std::equal_to<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <DataStructures::HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_VALUE_TYPE> HASH_TABLE_TRAITS>
        requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces)
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (HASHTABLE<HASH_TABLE_TRAITS>&& src)
        : inherited{Memory::MakeSharedPtr<Rep_<HASH_TABLE_TRAITS>> (move (src))}
    {
#if qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        static_assert (default_initializable<Mapping_HashTable>);
#endif
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <Cryptography::Digest::IHashFunction<KEY_TYPE> HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<DefaultTraits<remove_cvref_t<HASH>, remove_cvref_t<KEY_EQUALS_COMPARER>>>> (
              forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        requires (default_initializable<Mapping_HashTable>)
#endif
        : Mapping_HashTable{}
    {
#if qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        static_assert (default_initializable<Mapping_HashTable>);
#endif
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        requires (Cryptography::Digest::IHashFunction<HASH, KEY_TYPE>)
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer,
                                                                              const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping_HashTable{forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOfTo<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        requires (
#if qCompilerAndStdLib_default_initializable_broken_Buggy
            is_default_constructible_v<Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>
#else
            default_initializable<Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>
#endif
            and not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (ITERABLE_OF_ADDABLE&& src)
        : Mapping_HashTable{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOfTo<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        requires (Cryptography::Digest::IHashFunction<HASH, KEY_TYPE> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        : Mapping_HashTable{forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        requires (default_initializable<Mapping_HashTable>)
#endif
        : Mapping_HashTable{}
    {
#if qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        static_assert (default_initializable<Mapping_HashTable>);
#endif
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename HASH, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (HASH&& hasher, KEY_EQUALS_COMPARER&& keyComparer,
                                                                       ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (Cryptography::Digest::IHashFunction<HASH, KEY_TYPE>)
        : Mapping_HashTable{forward<HASH> (hasher), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}