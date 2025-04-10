/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include <random>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/Common.h"

namespace Stroika::Foundation::Containers::DataStructures {

    /*
     ********************************************************************************
     ****************** HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS> ********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::HashTable (const KeyHasherType& hashFunction, const KeyEqualsComparerType& keyComparer)
        : HashTable{kBufferedBuckets_, hashFunction, keyComparer}
    {
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::HashTable (size_t bucketCount, const KeyHasherType& hashFunction, const KeyEqualsComparerType& keyComparer)
        : fHasher_{hashFunction}
        , fKeyComparer_{keyComparer}
    {
        ReHash (bucketCount);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::hash_function () const -> KeyHasherType
    {
        return fHasher_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::key_eq () const -> KeyEqualsComparerType
    {
        return fKeyComparer_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::begin () -> ForwardIterator
    {
        return ForwardIterator{this};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::end () -> ForwardIterator
    {
        return ForwardIterator{};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::MoveIteratorHereAfterClone (ForwardIterator* pi, const HashTable* movedFrom) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        RequireNotNull (pi);
        RequireNotNull (movedFrom);
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (pi->fData_ == movedFrom);
#endif
        Require (this->bucket_count () == movedFrom->bucket_count ());
        //Require (this->fHasher_ == movedFrom->fHasher_); // logically required but not equals comparable
        // Also require no changes to this after clone!!! - cuz those could re-order elements
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            // then easy - cuz iterator rep is same - index into bucket list and index into array within bucket
        }
#if qStroika_Foundation_Debug_AssertionsChecked
        pi->fData_ = this;
#endif
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const value_type& t)
    {
        size_t hashVal = Hash_ (t.fKey);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            switch (TraitsType::kAddOrExtendOrReplace) {
                case AddOrExtendOrReplaceMode::eAddIfMissing: {
                    for (auto i : fBuckets_[hashVal].fElements) {
                        if (this->fKeyComparer_ (i.fKey, t.fKey)) {
                            return false;
                        }
                    }
                    // fall through and do default - append
                } break;
                case AddOrExtendOrReplaceMode::eAddReplaces: {
                    // must scan to see if present...
                    for (auto i = fBuckets_[hashVal].fElements.begin (); i != fBuckets_[hashVal].fElements.end (); ++i) {
                        if (this->fKeyComparer_ (i->fKey, t.fKey)) {
                            if constexpr (same_as<MAPPED_TYPE, void>) {
                                *i = t;
                            }
                            else {
                                i->fValue = t.fValue;
                            }
                            return true;
                        }
                    }
                    // fall through and do default - append
                } break;
                case AddOrExtendOrReplaceMode::eAddExtras: {
                    // fall through and do default - append
                } break;
                case AddOrExtendOrReplaceMode::eDuplicatesRejected: {
                    for (auto i : fBuckets_[hashVal].fElements) {
                        if (this->fKeyComparer_ (i.fKey, t.fKey)) {
                            static const auto kExcept_ = Execution::RuntimeErrorException<logic_error>{"Duplicates not allowed"sv};
                            Execution::Throw (kExcept_);
                        }
                    }
                    // fall through and do default - append
                } break;
                default:
                    AssertNotReached ();
            }
            // common case handled by fallthrough
            fBuckets_[hashVal].fElements.push_back (t);
            ++fCachedSize_;
            ReHashIfNeeded ();
            return true;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const key_type& t)
        requires (same_as<MAPPED_TYPE, void>)
    {
        return Add (Common::KeyValuePair<key_type, void>{t});
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    template <same_as<MAPPED_TYPE> MAPPED_TYPE2>
    inline bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const key_type& t, const MAPPED_TYPE2& m)
        requires (not same_as<MAPPED_TYPE, void>)
    {
        return Add (Common::KeyValuePair<key_type, mapped_type>{t, m});
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::insert (const pair<KEY_TYPE, MAPPED_TYPE>& p)
    {
        Add (p.first, p.second);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Lookup (const key_type& t) -> optional<value_type>
    {
        size_t hashVal = Hash_ (t);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            for (auto i : fBuckets_[hashVal].fElements) {
                if (this->fKeyComparer_ (i.fKey, t)) {
                    return i;
                }
            }
        }
        return nullopt;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Remove (const ForwardIterator& i, ForwardIterator* nextI)
    {
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            fBuckets_[i.fBucketIndex_].fElements.Remove (i.fIntraBucketIndex_);
            --fCachedSize_;
            if (nextI != nullptr) {
                *nextI = i;
                if (nextI->fIntraBucketIndex_ == fBuckets_[nextI->fBucketIndex_].fElements.size ()) {
                    ++nextI->fBucketIndex_;
                    nextI->fIntraBucketIndex_ = 0;
                    nextI->AdvanceOverEmptyBuckets_ ();
                }
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Remove (const key_type& t)
    {
        Verify (RemoveIf (t));
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveIf (const key_type& t)
    {
        size_t hashVal = Hash_ (t);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            for (auto i = fBuckets_[hashVal].fElements.begin (); i != fBuckets_[hashVal].fElements.end (); ++i) {
                if (this->fKeyComparer_ (i->fKey, t)) {
                    fBuckets_[hashVal].fElements.Remove (i - fBuckets_[hashVal].fElements.begin ());
                    --fCachedSize_;
                    return true;
                }
            }
        }
        return false;
    }

    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::erase (const ForwardIterator& i) -> ForwardIterator
    {
        ForwardIterator next{};
        Remove (i, &next);
        return next;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::erase (const UnderlyingIteratorRep& i) -> UnderlyingIteratorRep
    {
        ForwardIterator next{};
        Remove (ForwardIterator{this, i}, &next);
        return next.GetUnderlyingIteratorRep ();
    }

    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ReHash (size_t newBucketCount)
    {
        size_t useBucketCount = Math::AtLeast (Math::PrimeAtLeastThisBig (newBucketCount), kBufferedBuckets_);
        if (useBucketCount != fBuckets_.size ()) {
            if (this->empty ()) {
                fBuckets_.resize (newBucketCount);
            }
            else {
                Debug::TraceContextBumper ctx{"ReHash - rehashing"};
                // fill in new by iterating, so basically cost of a whole new copy of all the data
                HashTable n{newBucketCount, fHasher_, fKeyComparer_};
                for (auto i : *this) {
                    n.Add (i);
                }
                // this move is expensive - perhaps better to indirect buckets_ into HEAP object so this is cheaper
                fBuckets_ = move (n.fBuckets_);
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ReHashIfNeeded ()
    {
        // @todo consider the logic that makes sense here - look at std c++ unordered_set impl - and compare...
        float lf = load_factor ();
        if constexpr (TRAITS::kAutoShrinkBucketCount) {
            float thresholdBelowWhichWeShouldShrink = fMaxLoadFactor_ / 10;
            if (lf < thresholdBelowWhichWeShouldShrink) {
                float targetLoadFactor = fMaxLoadFactor_ * 1.5; // NO IDEA how much to use here?
                size_t targetBucketCount = Support::ReserveTweaks::GetScaledUpCapacity (static_cast<size_t> (targetLoadFactor * fCachedSize_ + 1));
                ReHash (targetBucketCount);
                return;
            }
        }
        if (lf > fMaxLoadFactor_) {
            float targetLoadFactor = fMaxLoadFactor_ * 1.5f; // NO IDEA how much to use here?
            size_t targetBucketCount = Support::ReserveTweaks::GetScaledUpCapacity (static_cast<size_t> (targetLoadFactor * fCachedSize_ + 1));
            ReHash (targetBucketCount);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline size_t HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::bucket_count () const
    {
        return fBuckets_.size ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline size_t HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::bucket_size (size_t bucketIdx) const
    {
        Require (bucketIdx < bucket_count ());
        return fBuckets_[bucketIdx].fElements.size ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline size_t HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::size () const
    {
        return fCachedSize_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::empty () const
    {
        return fCachedSize_ == 0;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline float HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::load_factor () const
    {
        return static_cast<float> (fCachedSize_) / fBuckets_.size ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline float HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::max_load_factor () const
    {
        return fMaxLoadFactor_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::max_load_factor (float mlf)
    {
        Require (mlf > 0.0);
        fMaxLoadFactor_ = mlf;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::clear ()
    {
        if (fCachedSize_ != 0) {
            for (auto& bi : fBuckets_) {
                bi.fElements.clear ();
            }
            fCachedSize_ = 0;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::contains (ArgByValueType<key_type> key) const
    {
        size_t hashVal = Hash_ (key);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            for (auto i : fBuckets_[hashVal].fElements) {
                if (this->fKeyComparer_ (i.fKey, key)) {
                    return true;
                }
            }
        }
        return false;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    template <invocable<typename TRAITS::value_type> FUNCTION>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Apply (FUNCTION&& doToElement, Execution::SequencePolicy seq) const
    {
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            switch (seq) {
                case Execution::SequencePolicy::eSeq:
#if __cpp_lib_execution < 201603L
                default:
#endif
                    for (const auto& bi : fBuckets_) {
                        for (const auto& i : bi.fElements) {
                            forward<FUNCTION> (doToElement) (i);
                        }
                    }
                    break;
#if __cpp_lib_execution >= 201603L
                default:
                    std::for_each (execution::par, fBuckets_.begin (), fBuckets_.end (), [&] (const BucketType_& bi) {
                        std::for_each (execution::par, bi.fElements.begin (), bi.fElements.end (),
                                       [&] (const value_type& v) { forward<FUNCTION> (doToElement) (v); });
                    });
#endif
                    break;
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Find (ArgByValueType<key_type> key) const -> ForwardIterator
    {
        size_t hashVal = Hash_ (key);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            size_t idx{0};
            for (auto i : fBuckets_[hashVal].fElements) {
                if (this->fKeyComparer_ (i.fKey, key)) {
                    return ForwardIterator{this, make_tuple (hashVal, idx)};
                }
                ++idx;
            }
        }
        return ForwardIterator{};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    template <typename ARG_T>
    auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Find (ARG_T key) const -> ForwardIterator
        requires (not same_as<typename TRAITS::AlternateFindType, void> and same_as<remove_cvref_t<ARG_T>, typename TRAITS::AlternateFindType>)
    {
        size_t hashVal = Hash_ (key);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            size_t idx{0};
            for (auto i : fBuckets_[hashVal].fElements) {
                if (this->fKeyComparer_ (i.fKey, key)) {
                    return ForwardIterator{this, make_tuple (hashVal, idx)};
                }
                ++idx;
            }
        }
        return ForwardIterator{};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    template <predicate<typename TRAITS::key_type> FUNCTION>
    auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Find (FUNCTION&& firstThat) const -> ForwardIterator
    {
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            size_t hashVal{0};
            for (const auto& bi : fBuckets_) {
                size_t idx{0};
                for (const auto& i : bi.fElements) {
                    if (forward<FUNCTION> (firstThat) (i)) {
                        return ForwardIterator{this, make_tuple (hashVal, idx)};
                    }
                    ++idx;
                }
                ++hashVal;
            }
        }
        return ForwardIterator{};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::find (ArgByValueType<key_type> key) const -> ForwardIterator
    {
        return this->Find (key);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    template <typename ARG_T>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::find (ARG_T key) const -> ForwardIterator
        requires (not same_as<typename TRAITS::AlternateFindType, void> and same_as<remove_cvref_t<ARG_T>, typename TRAITS::AlternateFindType>)
    {
        return this->Find (key);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    template <typename CHECKED_T>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Update (const ForwardIterator& it, ArgByValueType<CHECKED_T> newValue)
        requires (not same_as<MAPPED_TYPE, void>)
    {
        size_t bucketIndex                                              = get<0> (it.GetUnderlyingIteratorRep ());
        size_t intraBucketIndex                                         = get<1> (it.GetUnderlyingIteratorRep ());
        this->fBuckets_[bucketIndex].fElements[intraBucketIndex].fValue = newValue;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Invariant () const noexcept
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Invariant_ ();
#endif
    }
#if qStroika_Foundation_Debug_AssertionsChecked
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Invariant_ () const noexcept
    {
    }
#endif

    /*
     ********************************************************************************
     ********* HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator ************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::ForwardIterator (const HashTable* data) noexcept
        : ForwardIterator{data, make_tuple (0, 0)}
    {
        AdvanceOverEmptyBuckets_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::ForwardIterator (const HashTable* data, UnderlyingIteratorRep startAt) noexcept
        : fData_{data}
        , fBucketIndex_{get<0> (startAt)}
        , fIntraBucketIndex_{get<1> (startAt)}
    {
        RequireNotNull (data);
    }
#if qStroika_Foundation_Debug_AssertionsChecked
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::~ForwardIterator ()
    {
        Invariant ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator bool () const
    {
        return not Done ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::Done () const noexcept
    {
        Assert (fData_ == nullptr or fBucketIndex_ <= fData_->bucket_count ());
        return fData_ == nullptr or fBucketIndex_ == fData_->bucket_count ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator* () const -> const value_type&
    {
        Require (not Done ());
        return fData_->fBuckets_[fBucketIndex_].fElements[fIntraBucketIndex_];
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator->() const -> const value_type*
    {
        Require (not Done ());
        return &fData_->fBuckets_[fBucketIndex_].fElements[fIntraBucketIndex_];
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator== (const ForwardIterator& rhs) const
    {
        Require (fData_ == rhs.fData_ or fData_ == nullptr or rhs.fData_ == nullptr); // nullptr used for sentinal end else must refer to same container
        bool done  = Done ();
        bool rDone = rhs.Done ();
        if (done and rDone) {
            return true;
        }
        if (done or rDone) {
            return false;
        }
        // neither is done, nor special sentinel value, so this case is easy
        return this->fBucketIndex_ == rhs.fBucketIndex_ and this->fIntraBucketIndex_ == rhs.fIntraBucketIndex_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        return make_tuple (fBucketIndex_, fIntraBucketIndex_);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::SetUnderlyingIteratorRep (const UnderlyingIteratorRep l)
    {
        fBucketIndex_      = get<0> (l);
        fIntraBucketIndex_ = get<1> (l);
        // @todo assert valid in range
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator++ () -> ForwardIterator&
    {
        Require (not Done ());
        RequireNotNull (fData_);
        ++fIntraBucketIndex_;
        Assert (fIntraBucketIndex_ <= fData_->bucket_size (fBucketIndex_));
        if (fIntraBucketIndex_ == fData_->bucket_size (fBucketIndex_)) {
            ++fBucketIndex_;
            fIntraBucketIndex_ = 0;
        }
        AdvanceOverEmptyBuckets_ ();
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator++ (int) -> ForwardIterator
    {
        ForwardIterator result = *this;
        this->operator++ ();
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::AdvanceOverEmptyBuckets_ ()
    {
        while (fBucketIndex_ < fData_->bucket_count () and fData_->bucket_size (fBucketIndex_) == 0) {
            ++fBucketIndex_;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::AssertDataMatches ([[maybe_unused]] const HashTable* data) const
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Require (data == fData_);
#endif
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::Invariant () const noexcept
    {
#if qStroika_Foundation_Debug_AssertionsChecked
        Invariant_ ();
#endif
    }
#if qStroika_Foundation_Debug_AssertionsChecked
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::Invariant_ () const noexcept
    {
    }
#endif
}
