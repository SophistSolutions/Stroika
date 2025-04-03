/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include <random>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

namespace Stroika::Foundation::Containers::DataStructures {

    /*
     ********************************************************************************
     ****************** HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS> ********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::HashTable ()
        : HashTable{kBufferedBuckets_}
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
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetKeyHasherType () const -> KeyHasherType
    {
        return fHasher_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetKeyEqualsComparerType () const -> KeyEqualsComparerType
    {
        return fKeyComparer_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::begin () -> ForwardIterator
    {
        return ForwardIterator{this};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    constexpr auto HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::end () -> ForwardIterator
    {
        return ForwardIterator{};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const value_type& t)
    {
        size_t hashVal = Hash_ (t.fKey);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            fBuckets_[hashVal].fElements.push_back (t);
            ++fCachedSize_;
            ReHashIfNeeded ();
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const key_type& t)
        requires (same_as<MAPPED_TYPE, void>)
    {
        Add (Common::KeyValuePair<key_type, void>{t});
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    template <same_as<MAPPED_TYPE> MAPPED_TYPE2>
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const key_type& t, const MAPPED_TYPE2& m)
        requires (not same_as<MAPPED_TYPE, void>)
    {
        Add (Common::KeyValuePair<key_type, mapped_type>{t, m});
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
    inline void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::Remove (const key_type& t)
    {
        Verify (RemoveIf (t));
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    bool HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveIf (const key_type& t)
    {
        size_t hashVal = Hash_ (t);
        if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
            for (auto i : fBuckets_[hashVal].fElements) {
                if (this->fKeyComparer_ (i.fKey, t)) {
                    /// fBuckets_.erase  NYI
                    AssertNotReached ();
                }
            }
        }
        return false;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ReHash (size_t newBucketCount)
    {
        size_t useBucketCount = Math::AtLeast (Math::PrimeAtLeastThisBig (newBucketCount), kBufferedBuckets_);
        if (useBucketCount != fBuckets_.size ()) {
            Memory::InlineBuffer<BucketType_, kBufferedBuckets_> newBuckets;
            // fill in new by iterating, so basically cost of a whole new copy of all the data
            HashTable n{newBucketCount, fHasher_, fKeyComparer_};
            for (auto i : *this) {
                n.Add (i);
            }
            // this move is expensive - perhaps better to indirect buckets_ into HEAP object so this is cheaper
            fBuckets_ = move (n.fBuckets_);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
    void HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ReHashIfNeeded ()
    {
        // @todo redo so less finicky.. - this logic is WRONG/poor
        float lf = load_factor ();
        float thresholdBelowWhichWeShouldShrink = fMaxLoadFactor_ / 4;
        if (lf < thresholdBelowWhichWeShouldShrink) {
        }
        else if (lf > fMaxLoadFactor_) {
            float  targetLoadFactor  = fMaxLoadFactor_ / 2; // VERY roughly
            size_t targetBucketCount = static_cast<size_t> (targetLoadFactor * fCachedSize_);
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
