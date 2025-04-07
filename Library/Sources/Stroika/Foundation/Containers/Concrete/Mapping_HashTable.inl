/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
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
    template <DataStructures::HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_VALUE_TYPE> HASH_TABLE_TRAITS>
    class Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_,
                                                                 public Memory::UseBlockAllocationIfAppropriate<Rep_<HASH_TABLE_TRAITS>> {
    private:
        using inherited = IImplRepBase_;

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
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (that, seq); // @todo rewrite to use fData
        }

        // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
#if 1
            AssertNotImplemented ();
            return KeyEqualsCompareFunctionType{};
#else
            // @todo fix shouldn't need this cast!!!
            //return Common::DeclareEqualsComparer (fData_.key_eq ());
            return Common::DeclareEqualsComparer ((function<bool (KEY_TYPE, KEY_TYPE)>)fData_.key_eq ());
#endif
        }
        virtual shared_ptr<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
#if 1
            AssertNotImplemented ();
            return nullptr;
#else
            return Memory::MakeSharedPtr<Rep_> (fData_.hash_function (), fData_.key_eq ()); // keep hash/comparer, but lose data
#endif
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
#if 1
            AssertNotImplemented ();
            return false;
#else
            auto i = fData_.find (key);
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
#endif
        }
        virtual bool Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt, AddReplaceMode addReplaceMode) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            bool result{};
#if 1
            AssertNotImplemented ();
#else
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
#endif
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
            return result;
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
#if 1
            AssertNotImplemented ();
#else
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
#endif
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
#if 1
            AssertNotImplemented ();
#else
            Require (not i.Done ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI); // advance to next item if deleting current one
            }
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            mir.fIterator.AssertDataMatches (&fData_);
            (void)fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
#endif
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
#if 1
            AssertNotImplemented ();
#else
            Debug::AssertExternallySynchronizedMutex::WriteContext           declareWriteContext{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_
                .remove_constness (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ())
                ->second = newValue;
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
#endif
        }

    private:
        using DataStructureImplType_ = HASHTABLE<HASH_TABLE_TRAITS>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     *************** Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE> ****************
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
    inline Mapping_HashTable<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping_HashTable (HASHTABLE<HASH_TABLE_TRAITS>&& src)
#if !qCompilerAndStdLib_requires_breaks_soemtimes_but_static_assert_ok_Buggy
        requires (default_initializable<Mapping_HashTable>)
#endif
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
        : inherited{Memory::MakeSharedPtr<Rep_<DataStructures::HashTable_Support::DefaultTraits<KEY_TYPE, MAPPED_VALUE_TYPE, remove_cvref_t<HASH>, remove_cvref_t<KEY_EQUALS_COMPARER>>>> (
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