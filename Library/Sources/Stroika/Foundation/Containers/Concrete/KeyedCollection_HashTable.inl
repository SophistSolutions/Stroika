/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********** KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>::Rep_ ***************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <DataStructures::HashTable_Support::IValidTraits<T, void> HASH_TABLE_TRAITS>
        requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces)
    class KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRepBase_,
                                                                 public Memory::UseBlockAllocationIfAppropriate<Rep_<HASH_TABLE_TRAITS>> {

    private:
        using inherited = IImplRepBase_;

    private:
        // these are stored inside fData_ (often zero sized so no matter) - but if not zero sized - @todo just re-use the space inside fData_
        [[no_unique_address]] const KeyExtractorType fKeyExtractor_;

    public:
        Rep_ (const KeyExtractorType& keyExtractor, HASHTABLE<HASH_TABLE_TRAITS>&& src)
            : fKeyExtractor_{keyExtractor}
            , fData_{move (src)}
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
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply ([&] (auto i) { doToElement (i.fKey); }, seq); // the 'fKey' part of the hash-table elements contain our entire object - there is no fValue
        }
        virtual Iterator<T> Find (const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (that, seq); // @todo rewrite to use fData
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
#if 0
            // !todo fix!!!
            Ensure ((found == fData_.end () and this->inherited::Find_equal_to (v, seq) == Iterator<value_type>{nullptr}) or
                    (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->inherited::Find_equal_to (v, seq).ConstGetRep ())
                                  .fIterator.GetUnderlyingIteratorRep ()));
#endif
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, found.GetUnderlyingIteratorRep ())};
        }

        // KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep overrides
    public:
        virtual KeyExtractorType GetKeyExtractor () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fKeyExtractor_;
        }
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.key_eq ().fKeyComparer;
        }
        virtual shared_ptr<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            // lose data but keep compare/extractor functions
            return Memory::MakeSharedPtr<Rep_> (this->fKeyExtractor_, HASHTABLE<HASH_TABLE_TRAITS>{fData_.hash_function (), fData_.key_eq ()});
        }
        virtual shared_ptr<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            static_assert (same_as<KeyType, int>);
            static_assert (same_as<value_type, typename DataStructureImplType_::key_type>);
            auto i = fData_.find (key); // using AlternateFindType overload of find()
            if (i == fData_.end ()) {
                if (item != nullptr) {
                    *item = nullopt;
                }
                return false;
            }
            else {
                if (item != nullptr) {
                    *item = i->fKey; // the key in the HashTable is the whole object because of how we declared it
                }
                return true;
            }
            return false;
        }
        virtual bool Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            size_t                                                 oldSize = this->size ();
            (void)fData_.Add (item); // returns if there was a change, but this KeyedCollection returns true iff collection changed size
            bool newItemAdded = this->size () != oldSize;
            fChangeCounts_.PerformedChange ();
            return newItemAdded;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir      = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            auto  nextStdI = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, nextStdI)};
            }
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                return true;
            }
            return false;
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
     ************ KeyedCollection_HashTable<KEY_TYPE,MAPPED_VALUE_TYPE> *************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>::KeyedCollection_HashTable ()
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                  Cryptography::Digest::IHashFunction<hash<KEY_TYPE>, KEY_TYPE> and IEqualsComparer<std::equal_to<KEY_TYPE>, KEY_TYPE>)
        : KeyedCollection_HashTable{DEFAULT_HASHTABLE<>{}}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <DataStructures::HashTable_Support::IValidTraits<T, void> HASH_TABLE_TRAITS>
    KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>::KeyedCollection_HashTable (const KeyExtractorType& keyExtractor, HASHTABLE<HASH_TABLE_TRAITS>&& src)
        requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces)
        : inherited{Memory::MakeSharedPtr<Rep_<HASH_TABLE_TRAITS>> (keyExtractor, move (src))}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <DataStructures::HashTable_Support::IValidTraits<T, void> HASH_TABLE_TRAITS>
    KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>::KeyedCollection_HashTable (HASHTABLE<HASH_TABLE_TRAITS>&& src)
        requires (HASH_TABLE_TRAITS::kAddOrExtendOrReplace == AddOrExtendOrReplaceMode::eAddReplaces and
                  IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection_HashTable{KeyExtractorType{}, move (src)}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_HASH, typename KEY_EQUALS_COMPARER>
    KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>::KeyedCollection_HashTable (const KeyExtractorType& keyExtractor, KEY_HASH&& keyHasher,
                                                                               KEY_EQUALS_COMPARER&& keyComparer)
        requires (IEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> and Cryptography::Digest::IHashFunction<KEY_HASH, KEY_TYPE>)
        : KeyedCollection_HashTable{keyExtractor, DataStructures::HashTable<T, void, DefaultTraits<KEY_HASH, KEY_EQUALS_COMPARER>>{
                                                      ElementHash<KEY_HASH>{keyExtractor, keyHasher},
                                                      ElementEqualsComparer<KEY_EQUALS_COMPARER>{keyExtractor, keyComparer}}}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection_HashTable<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}