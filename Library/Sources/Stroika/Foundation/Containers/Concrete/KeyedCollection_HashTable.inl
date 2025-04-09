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
            #if 1
            return Iterator<value_type>{};
            #else
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
            #endif
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
            #if 1
            AssertNotImplemented();
            #else
            fData_.Apply (doToElement);
            #endif
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
            #if 1
            AssertNotImplemented();
            return nullptr;
            #else
            Ensure ((found == fData_.end () and this->inherited::Find_equal_to (v, seq) == Iterator<value_type>{nullptr}) or
                    (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->inherited::Find_equal_to (v, seq).ConstGetRep ())
                                  .fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, found)};
            #endif
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
            #if 1
            return nullptr;
            #else
            return Memory::MakeSharedPtr<Rep_> (this->fKeyExtractor_, this->fElementHasher_.fKeyHasher, this->fElementComparer_.fKeyComparer); // lose data but keep compare/extractor functions
        #endif
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
            #if 0
            auto                                                  i = fData_.find (key);
            if (i == fData_.end ()) {
                if (item != nullptr) {
                    *item = nullopt;
                }
                return false;
            }
            else {
                if (item != nullptr) {
                    *item = *i;
                }
                return true;
            }
            #endif
            return false;
        }
        virtual bool Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            #if 0
            pair<typename DataStructureImplType_::iterator, bool>  flagAndI = fData_.insert (item);
            if (flagAndI.second) {
                return true;
            }
            else {
                // @todo must patch!!!
                // in case of update, set<> wont update the value so we must remove and re-add, but todo that, use previous iterator as hint
                typename DataStructureImplType_::iterator hint = flagAndI.first;
                ++hint;
                fData_.erase (flagAndI.first);
                fData_.insert (hint, item);
                fChangeCounts_.PerformedChange ();
                return false;
            }
            #endif
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir      = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            #if 1
            AssertNotImplemented ();
            #else
            auto  nextStdI = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, nextStdI)};
            }
            #endif
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            #if 1
            AssertNotImplemented();
            #else
        auto                                                   i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                return true;
            }
            #endif
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
                                      ElementHash{keyExtractor, keyHasher}, ElementEqualsComparer{keyExtractor, keyComparer}}}
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