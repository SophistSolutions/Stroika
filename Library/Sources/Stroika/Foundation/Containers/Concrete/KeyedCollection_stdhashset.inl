/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdhashset_inl_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdhashset_inl_

#include "../../Configuration/Concepts.h"
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"
#include "../STL/Compare.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *** KeyedCollection_stdhashset<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ *****
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection_stdhashset<T, KEY_TYPE, TRAITS>::IImplRepBase_ : public KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {};

    /*
     ********************************************************************************
     ********** KeyedCollection_stdhashset<T, KEY_TYPE, TRAITS>::Rep_ ***************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_HASH, typename KEY_EQUALS_COMPARER>
    class KeyedCollection_stdhashset<T, KEY_TYPE, TRAITS>::Rep_
        : public IImplRepBase_,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_HASH, KEY_EQUALS_COMPARER>> {
    public:
        static_assert (not is_reference_v<KEY_HASH>);
        static_assert (not is_reference_v<KEY_EQUALS_COMPARER>);

    private:
        using inherited = IImplRepBase_;

    private:
        // these are stored inside fData_ (often zero sized so no matter) - but if not zero sized - @todo just re-use the space inside fData_
        [[no_unique_address]] const KeyExtractorType                           fKeyExtractor_;
        [[no_unique_address]] const ElementHash<KEY_HASH>                      fElementHasher_;
        [[no_unique_address]] const ElementEqualsComparer<KEY_EQUALS_COMPARER> fElementComparer_;

    public:
        Rep_ (const KeyExtractorType& keyExtractor, const KEY_HASH& keyHash, const KEY_EQUALS_COMPARER& keyEqualsComparer)
            : fKeyExtractor_{keyExtractor}
            , fElementHasher_{keyExtractor, keyHash}
            , fElementComparer_{keyExtractor, keyEqualsComparer}
            , fData_{11u, fElementHasher_, fElementComparer_}
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
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr) const override
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
        virtual Iterator<T> Find (const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr,
                                  const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (thisSharedPtr, that, seq); // @todo rewrite to use fData
        }
        virtual Iterator<value_type> Find_equal_to ([[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr,
                                                    const ArgByValueType<value_type>& v, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
            Ensure ((found == fData_.end () and this->inherited::Find_equal_to (thisSharedPtr, v, seq) == Iterator<value_type>{nullptr}) or
                    (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->inherited::Find_equal_to (thisSharedPtr, v, seq).ConstGetRep ())
                                  .fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, found)};
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
            return KeyEqualityComparerType{fElementComparer_.fKeyComparer};
        }
        virtual shared_ptr<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (this->fKeyExtractor_, this->fElementHasher_.fKeyHasher, this->fElementComparer_.fKeyComparer); // lose data but keep compare/extractor functions
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
            return false;
        }
        virtual bool Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
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
                return false;
            }
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
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDHASHSET<KEY_HASH, KEY_EQUALS_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     *********** KeyedCollection_stdhashset<KEY_TYPE,MAPPED_VALUE_TYPE> *************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_HASH, typename KEY_EQUALS_COMPARER>
    KeyedCollection_stdhashset<T, KEY_TYPE, TRAITS>::KeyedCollection_stdhashset (const KeyExtractorType& keyExtractor, KEY_HASH&& keyHasher,
                                                                                 KEY_EQUALS_COMPARER&& keyComparer)
        requires (IEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> and Cryptography::Digest::IsHashFunction<KEY_HASH, KEY_TYPE>)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_HASH>, remove_cvref_t<KEY_EQUALS_COMPARER>>> (
              keyExtractor, forward<KEY_HASH> (keyHasher), forward<KEY_EQUALS_COMPARER> (keyComparer))}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection_stdhashset<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_stdhashset_inl_ */
