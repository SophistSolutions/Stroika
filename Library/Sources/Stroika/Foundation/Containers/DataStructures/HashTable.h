/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_HashTable_h_
#define _Stroika_Foundation_Containers_DataStructures_HashTable_h_

#include "Stroika/Foundation/StroikaPreComp.h"

// for now uses std::vector...
//#include <vector>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Common::ArgByValueType;

    /**
     *  HashTable module design notes:
     *      o   use traits to pick between separate chaining, and linear probing (do sep chaining first since easiest)
     *      o   Sometimes want HashTable<T> and sometimes HashTable<KEY,MAPPED_VALUE> - could go either way (with mapped_value==void).
     *          Did other way for skipList - not sure what is best. For now - do HashTable<T>, and use Hash<KVP<KEY,MAPPED_VALUE>> in usage, and provide 
     *          wrapping hash/compare functions
     */
    namespace HashTable_Support {

        struct SeparateChainingTag {};

        template <typename KEY_TYPE, typename MAPPED_TYPE, size_t INLINE_ELTS_PER_CHAIN = 2, size_t INLINE_BUCKETS = 5>
        struct SeparateChainingOptions : SeparateChainingTag {
            // for now no options

            static constexpr size_t kBufferedElementsPerChain = INLINE_ELTS_PER_CHAIN;

            static constexpr size_t kBufferedBuckets = INLINE_BUCKETS;
        };

        /**
         *  KEY_TYPE the type of the key element stored in the SkipList.
         *  KEY_COMPARER is nearly always as given
         *  AddOrExtendOrReplaceMode addOrExtendOrReplace defaults to eAddExtras, but here the caller may not want the default. There is no good default here.
         *  ALTERNATE_FIND_TYPE can often be omitted (default) - but allows Find () to be overloaded (argument comparer) on a different type (besides just KEY_TYPE).
         */
        template <typename KEY_TYPE, typename MAPPED_TYPE, typename HASHER = std::hash<KEY_TYPE>, Common::IEqualsComparer<KEY_TYPE> EQUALS_COMPARER = equal_to<KEY_TYPE>,
                  typename LAYOUT_OPTIONS = SeparateChainingOptions<KEY_TYPE, MAPPED_TYPE>, typename ALTERNATE_FIND_TYPE = void>
        struct DefaultTraits {
            /**
         */
            using key_type = KEY_TYPE;

            /**
         */
            using mapped_type = MAPPED_TYPE;

            /**
             */
            using value_type = Common::KeyValuePair<key_type, mapped_type>;

            /**
             */
            using ValueHasherType = HASHER;

            /**
             */
            using EqualsComparerType = EQUALS_COMPARER;

            /**
            *  separate chaining (for now) - or some probing variation
             */
            using LayoutType = LAYOUT_OPTIONS;

            /**
             *  like is_transparent mechanism in C++14, except just adds one type (if not void) to the set of types you can find looking for)
             *  \note when using AlternateFindType != void, caller must ALSO provide a compare function which accepts combinations of T, and AlternateFindType
             */
            using AlternateFindType = ALTERNATE_FIND_TYPE;
        };

        /**
         */
        template <typename TRAITS, typename KEY_TYPE, typename MAPPED_TYPE>
        concept IValidTraits = Common::IEqualsComparer<typename TRAITS::EqualsComparerType, KEY_TYPE>
#if 0
            and requires (TRAITS) {
            {  TRAITS::ValueHasherType } -> invocable<typename TRAITS::value_type>;
            /*{ TRAITS::ValueHasherType };
            { TRAITS::EqualsComparerType };
            { TRAITS::LayoutType };
            { TRAITS::AlternateFindType };*/
        }
#endif
            ;

    }

    /**
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE = void, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS = HashTable_Support::DefaultTraits<KEY_TYPE, MAPPED_TYPE>>
    class HashTable : public Debug::AssertExternallySynchronizedMutex {
    public:
        /**
         */
        using key_type = typename TRAITS::key_type;

    public:
        /**
         */
        using mapped_type = typename TRAITS::mapped_type;

    public:
        /**
         */
        using value_type = typename TRAITS::value_type;

    public:
        /**
         */
        using ValueHasherType = typename TRAITS::ValueHasherType;

    public:
        /**
         */
        using EqualsComparerType = typename TRAITS::EqualsComparerType;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        //using UnderlyingIteratorRep = const Link_*;

    public:
        /**
         */
        using TraitsType = TRAITS;

    public:
        /**
         */
        HashTable ()
            : HashTable{kBufferedBuckets_}
        {
        }
        HashTable (size_t bucketCount, const ValueHasherType& hashFunction = {}, const EqualsComparerType& keyComparer = {})
            : fHasher_{hashFunction}
            , fKeyComparer_{keyComparer}
        {
            ReHash (bucketCount);
        }
        HashTable (HashTable&& src) noexcept;
        HashTable (const HashTable& src);
        ~HashTable () = default;

    public:
        nonvirtual HashTable& operator= (const HashTable& rhs) = default;

        //public:
        //    /**
        //     */
        //    constexpr KeyComparerType key_comp () const;

    public:
        class ForwardIterator;

    private:
        using LayoutType_                                  = TraitsType::LayoutType;
        static constexpr size_t kBufferedElementsPerChain_ = TraitsType::LayoutType::kBufferedElementsPerChain;
        static constexpr size_t kBufferedBuckets_          = TraitsType::LayoutType::kBufferedBuckets;

        // this type depends MORE INTIMATELY on LayoutType (use concepts to select when we support more)
        //
        struct BucketType_ {
            // because we keep number of elements in a bucket low, often best to use array instead of linked list (performance)
            Memory::InlineBuffer<value_type, kBufferedElementsPerChain_> fElements;
        };

        Memory::InlineBuffer<BucketType_, kBufferedBuckets_> fBuckets_;

        [[no_unique_address]] ValueHasherType    fHasher_;
        [[no_unique_address]] EqualsComparerType fKeyComparer_;

        size_t fCachedSize_{0};

    private:
        nonvirtual size_t Hash_ (const key_type& v, size_t useBucketSize)
        {
            Require (useBucketSize > 0);
            return fHasher_ (v) % useBucketSize;
        }
        nonvirtual size_t Hash_ (const key_type& v)
        {
            return Hash_ (v, fBuckets_.size ());
        }

    public:
        nonvirtual void Add (const value_type& t)
        {
            size_t hashVal = Hash_ (t.fKey);
            if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
                fBuckets_[hashVal].fElements.push_back (t);
                ++fCachedSize_;
                ReHashIfNeeded ();
            }
        }
        nonvirtual void Add (const key_type& t)
            requires (same_as<mapped_type, void>)
        {
            Add (Common::KeyValuePair<key_type, void>{t});
        }
        template <same_as<MAPPED_TYPE> MAPPED_TYPE2 = MAPPED_TYPE>
        nonvirtual void Add (const key_type& t, const MAPPED_TYPE2& m)
            requires (not same_as<MAPPED_TYPE, void>)
        {
            Add (Common::KeyValuePair<key_type, mapped_type>{t, m});
        }

    public:
        optional<value_type> Lookup (const key_type& t)
        {
            size_t hashVal = Hash_ (t);
            if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
                for (auto i : fBuckets_[hashVal].fElements) {
                    if (i.fKey == t) {
                        return i;
                    }
                }
            }
            return nullopt;
        }

    public:
        void Remove (const key_type& t)
        {
            (void)RemoveIf (t);
        }

    public:
        bool RemoveIf (const key_type& t)
        {
            size_t hashVal = Hash_ (t);
            if constexpr (derived_from<LayoutType_, HashTable_Support::SeparateChainingTag>) {
                for (auto i : fBuckets_[hashVal].fElements) {
                    if (i.fKey == t) {
                        /// fBuckets_.erase  NYI
                    }
                }
            }
            return nullopt;
        }

    public:
        /**
         *  TBD...  NOT same as https://en.cppreference.com/w/cpp/container/unordered_set/rehash
         * 
         *  Use ROUGHLY the argument number of hash buckets. Call bucket_count() to find number actually used.
         * 
         *  bucket_count never goes below 1, but if you request a number too low, just goes to lowest allowed.
         * so ReHash(0) can be used to 'compact' as much as possible.
         */
        void ReHash (size_t newBucketCount)
        {
            size_t useBucketCount = Math::AtLeast (Math::PrimeAtLeastThisBig (newBucketCount), kBufferedBuckets_);
            if (useBucketCount != fBuckets_.size ()) {
                Memory::InlineBuffer<BucketType_, kBufferedBuckets_> newBuckets;
                // fill in by iterating, but for now quick tmphack
                newBuckets = fBuckets_;
                newBuckets.resize (newBucketCount);

                // this move is expensive - perhaps better to indirect buckets_ into HEAP object so this is cheaper
                fBuckets_ = move (newBuckets);
            }
        }

    public:
        /**
        * This examines load_factor and max_load_factor(), and depending on relationship, makes
        * a guess as to best size to use in call to ReHash();
         */
        void ReHashIfNeeded ()
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

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual size_t size () const
        {
            return fCachedSize_;
        }

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual bool empty () const
        {
            return fCachedSize_ == 0;
        }

    public:
        /**
         *  \brief average number of elements per bucket
         * 
         *  \see https://en.cppreference.com/w/cpp/container/unordered_set/load_factor
         */
        float load_factor () const
        {
            return static_cast<float> (fCachedSize_) / fBuckets_.size ();
        }

    private:
        float fMaxLoadFactor_{1.0};

    public:
        /**
         *  \brief average number of elements per bucket
         * 
         *  \see https://en.cppreference.com/w/cpp/container/unordered_set/load_factor
         */
        float max_load_factor () const
        {
            return fMaxLoadFactor_;
        }
        void max_load_factor (float mlf)
        {
            Require (mlf > 0.0);
            fMaxLoadFactor_ = mlf;
        }
#if 0
    public:
        /**
         *  You can add more than one item with the same key. If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove calls.
         *
         *  Returns true if the list was changed (if eAddReplaces, and key found, return true even if val same as value already there because we cannot generically compare values)
         * 
         *  \note Runtime performance/complexity:   ??
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual bool Add (ArgByValueType<key_type> key, ForwardIterator* oAddedI = nullptr)
            requires (same_as<mapped_type, void>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        {
            return Add1_ (key, oAddedI);
        }
#else
        ;
#endif
        template <typename CHECK_T = MAPPED_TYPE>
        nonvirtual bool Add (ArgByValueType<key_type> key, ArgByValueType<CHECK_T> val, ForwardIterator* oAddedI = nullptr)
            requires (not same_as<mapped_type, void>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        {
            return Add2_ (key, val, oAddedI);
        }
#else
        ;
#endif
        nonvirtual bool Add (const value_type& v, ForwardIterator* oAddedI = nullptr);

    public:
        /**
         *  \brief Remove an item with the given key (require it exists)
         * 
         *  \pre contains (key)
         * 
         *  \note same as Verify (RemoveIf (key))
         * 
         *  \note Runtime performance/complexity:
         *      Average:    log(N)
         *      Worst:      N
         * 
         *  \see also erase()
         */
        nonvirtual void Remove (ArgByValueType<key_type> key);
        nonvirtual void Remove (const ForwardIterator& it);

    public:
        /**
         *  \brief remove the element at i, and return valid iterator to the element that was following it (which can be empty iterator)
         * 
         *  \pre i != end ()
         * 
         *  \brief see https://en.cppreference.com/w/cpp/container/vector/erase
         * 
         *  \note Runtime performance/complexity:
         *      Average:    log(N)
         *      Worst:      N
         * 
         *  \see also Remove()
         */
        nonvirtual ForwardIterator erase (const ForwardIterator& i);

    public:
        /**
         * \brief Remove the first item with the given key, if any. Return true if a value found and removed, false if no such key found.
         * 
         *  \note Runtime performance/complexity:
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual bool RemoveIf (ArgByValueType<key_type> key);

    public:
        /**
        * @todo discuss with sterl - if we allow multiple values with same key, add RemoveAll overload taking key_type, and maybe returning count removed? RemoveAllIf
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual bool empty () const;

    public:
        /**
         */
        nonvirtual ForwardIterator begin () const;

    public:
        /**
         */
        constexpr ForwardIterator end () const noexcept;

    public:
        /*
         *  Support for COW (CopyOnWrite):
         *
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an equivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const HashTable* movedFrom) const;

    public:
        /**
         *  \see https://en.cppreference.com/w/cpp/container/map/contains
         *
         *  \note Runtime performance/complexity:   ??
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual bool contains (ArgByValueType<key_type> key) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      overload: (key_type)
         *      Average/Worst:    log(N) ; N
         *  \note Runtime performance/complexity:
         *      overload: (FUNCTION&& f overload)
         *      Average/Worst:    O(N)
         * 
         *  \note this is kind of like set<T>::find () - but not exactly, and find() doesn't really have a uniform API across the various stl containers...
         *        which is why we use Find(), instead of find() as a name
         */
        nonvirtual ForwardIterator Find (ArgByValueType<key_type> key) const;
        template <typename ARG_T = typename TRAITS::AlternateFindType>
        nonvirtual ForwardIterator Find (ARG_T key) const
            requires (not same_as<typename TRAITS::AlternateFindType, void> and same_as<remove_cvref_t<ARG_T>, typename TRAITS::AlternateFindType>);
        template <predicate<typename HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type> FUNCTION>
        nonvirtual ForwardIterator Find (FUNCTION&& firstThat) const
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        {
            for (auto i = begin (); i; ++i) {
                if (firstThat (*i)) {
                    return i;
                }
            }
            return end ();
        }
#else
            ;
#endif

    public:
        /**
         *  \par Example Usage:
         *      \code
         *          EXPECT_EQ (t.First (key), i);
         *      \endcode
         * 
         *  \par Example Usage:
         *      \code
         *          if (auto o = t.First (key)) {
         *              useO = *o;
         *          }
         *      \endcode
         * 
         *  \par Example Usage:
         *      \code
         *          // find value of first odd key
         *          if (auto o = t.First ([] (auto kvp) { return kvp.fKey & 1; }) {
         *              useO = *o;
         *          }
         *      \endcode
         * 
         *  \note Complexity (key_type):   ??
         *      Average:    log(N)
         *      Worst:      N
         *  \note Complexity (FUNCTION&& f overload):
         *      Average/Worst:    O(N)
         */
        nonvirtual optional<mapped_type> First (ArgByValueType<key_type> key) const;
        template <qCompilerAndStdLib_RequiresNotMatchXXXDefined_1_BWA (predicate<typename HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type>) FUNCTION>
        nonvirtual optional<mapped_type> First (FUNCTION&& firstThat) const;

    public:
        /**
         *  \note - unlike modifying operations, this doesn't invalidate any iterators (including the argument iterator).
         */
        template <typename CHECKED_T = MAPPED_TYPE>
        nonvirtual void Update (const ForwardIterator& it, ArgByValueType<CHECKED_T> newValue)
            requires (not same_as<MAPPED_TYPE, void>);

    public:
        /**
         *  \brief optimize the memory layout of the HashTable
         * 
         * calling this will result in maximal search performance until further adds or removes
         * call when list is relatively stable in size, and it will set links to near classic log(n/2) search time
         * relatively fast to call, as is order N (single list traversal)
         * 
         *  @aliases Optimize
         * 
         *  \note Runtime performance/complexity:
         *      Average/WorseCase???
         */
        nonvirtual void ReBalance ();

    public:
        /**
         * make the key faster on finds, possibly slowing other key searches down
         * 
         *  \note Runtime performance/complexity:
         *      Average/WorseCase???
         */
        nonvirtual void Prioritize (ArgByValueType<key_type> key);

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: O(N)
         */
        template <qCompilerAndStdLib_RequiresNotMatchXXXDefined_1_BWA (invocable<typename HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type>) FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;

    public:
        constexpr void Invariant () const noexcept;

    public:
        /**
        * @todo doc api just for debugging? And not generally useful. And maybe have return tuple, not take var param?
        // height is highest link height, also counts total links if pass in non-null totalHeight
            @todo ask sterl about this?
         */
        nonvirtual size_t CalcHeight (size_t* totalHeight = nullptr) const;

    public:
        /**
         *  @todo DOC MENAING - CONTROLS - (TRAITS) and maybe range (type special) - 0..100?)
         */
        static size_t GetLinkHeightProbability (); // percent chance. We use 25%, which appears optimal

    public:
        /**
         *  Instantiate with TRAITS::kKeepStatistics==true to get useful stats.
         */
        nonvirtual StatsType GetStats () const;

    private:
        /*
         *  These return the first and last entries in the tree (defined as the first and last entries that would be returned via
         *  iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
         */
        nonvirtual Link_* GetFirst_ () const; // synonym for begin (), MakeIterator ()

    private:
        nonvirtual Link_* GetLast_ () const; // returns iterator to largest key

    private:
        // @todo maybe make part of traits??? and use in InlineBuffer somehow? instead of vector
        // maybe no need for MAX - just optimized-for-max - size of inline buffer - not sure why we need any other max (can use stackbuffer for that)
        static constexpr size_t kMaxLinkHeight_ = sizeof (size_t) * 8;

    private:
        // @todo consider using Memory::InlineBuffer<> - so fewer memory allocations for some small buffer size???, and tune impl to prefer this size or take param in traits used for this
        using LinkVector_ = vector<Link_*>;

    private:
        // Fundamentally a linked-list, but with a quirky 'next' pointer(s)
        struct Link_ : public Memory::UseBlockAllocationIfAppropriate<Link_, sizeof (value_type) <= 128> {
            template <typename MAPPED_TYPE2 = MAPPED_TYPE>
            constexpr Link_ (ArgByValueType<key_type> key, ArgByValueType<MAPPED_TYPE2> val)
                requires (not same_as<MAPPED_TYPE2, void>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
                : fEntry{key, val} {}
#else
            ;
#endif
                constexpr Link_ (ArgByValueType<key_type> key)
                    requires (same_as<MAPPED_TYPE, void>)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
                : fEntry{key} {}
#else
            ;
#endif
                constexpr Link_ (ArgByValueType<value_type> v);

            value_type  fEntry;
            LinkVector_ fNext; // for a HashTable, you have an array of next pointers, rather than just one
        };
        LinkVector_ fHead_{};

    private:
        /*
         * Find Link for key in HashTable, else nullptr. In cases of duplicate values, return first found.
         */
        template <Common::IAnyOf<KEY_TYPE, typename TRAITS::AlternateFindType> KEYISH_T>
        nonvirtual Link_* FindLink_ (const KEYISH_T& key) const;

    private:
        /*
         *  This searches the list for the given key. If found exactly, it is returned. If it occurs multiple times a random one is selected.
         *
         * this is specialized for the case of adding or removing elements, as it also returns
         * all links that will need to be updated for the new element or the element to be removed
         *
         *      \post (result == nullptr or fKeyThreeWayComparer_ (result->fEntry.fKey, key) == strong_ordering::equal);
         * 
///??? MAYBE NOT         *  \post all links in linksPointingToReturnedLink are non-null, and valid Link_* pointers
                @todo CONSIDER if LinkVector sb replaced with set<Link*>
         */
        struct LinkAndInfoAboutBackPointers {
            Link_* fLink;
            /**
             *  This is a vector, not a set, because it must reproduce the 'heights' of the linked tree structure.
             *  and nullptr entries in the list are 'sentinel values' indicating start of list (@Sterl why not just inert &fHead directly)
             */
            LinkVector_ fLinksPointingToReturnedLink; // @todo consider using set, and unclear what nullptr means in this vector, nor duplicates?
        };
        nonvirtual LinkAndInfoAboutBackPointers FindNearest_ (const variant<key_type, ForwardIterator>& keyOrI) const;

    private:
        // @todo ASK STERL MEANING OF LinkVector_ argument? Is it links to patch, or a starter on links for 'n'
        // and why not have PatchLinks method?
        nonvirtual void AddLink_ (Link_* n, const LinkVector_& linksToPatch);

    private:
        // @todo ask sterl meaning of LinkVector_ argument here? Why not have PatchLinks_ method?
        nonvirtual void RemoveLink_ (Link_* n, const LinkVector_& linksToPatch);

#if qStroika_Foundation_Debug_AssertionsChecked
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        nonvirtual void ShrinkHeadLinksIfNeeded_ ();

    private:
        nonvirtual void GrowHeadLinksIfNeeded_ (size_t newSize, Link_* linkToPointTo);

    private:
        nonvirtual size_t DetermineLinkHeight_ () const;

#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    private:
        bool Add1_ (ArgByValueType<key_type> key, ForwardIterator* oAddedI);
        template <typename CHECK_T = MAPPED_TYPE>
        bool Add2_ (ArgByValueType<key_type> key, ArgByValueType<CHECK_T> val, ForwardIterator* oAddedI);
#endif

    private:
        [[no_unique_address]] KeyComparerType   fKeyThreeWayComparer_{};
        size_t                                  fLength_{0};
        [[no_unique_address]] mutable StatsType fStats_{};

#endif
    };

#if 0
    /*
     *      ForwardIterator allows you to iterate over a HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>. It is not safe to use a ForwardIterator after any
     *      update to the HashTable.
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE> TRAITS>
    class HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator {
    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = forward_iterator_tag;
        using value_type        = HashTable::value_type;
        using difference_type   = ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;

    public:
        /**
         *  /0 overload: sets iterator to 'end' - sentinel
         *  /1 (data) overload: sets iterator to begin
         *  /2 (data,startAt) overload: sets iterator to startAt
         */
        constexpr ForwardIterator () noexcept = default;
        explicit constexpr ForwardIterator (const HashTable* data) noexcept;
        explicit constexpr ForwardIterator (const HashTable* data, UnderlyingIteratorRep startAt) noexcept;
        constexpr ForwardIterator (const ForwardIterator&) noexcept = default;
        constexpr ForwardIterator (ForwardIterator&&) noexcept      = default;

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator&)     = default;
        nonvirtual ForwardIterator& operator= (ForwardIterator&&) noexcept = default;

#if qStroika_Foundation_Debug_AssertionsChecked
    public:
        ~ForwardIterator ();
#endif

    public:
        /**
         *  return true if iterator not Done
         */
        explicit operator bool () const;

    public:
        nonvirtual bool Done () const noexcept;

    public:
        nonvirtual const value_type& operator* () const; //  Error to call if Done (), otherwise OK

    public:
        nonvirtual const value_type* operator->() const; //  Error to call if Done (), otherwise OK

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Average/WorseCase:  O(N)        - super slow cuz have to traverse on average half the list
         * 
         *  \pre data == fData_ argument constructed with (or as adjusted by Move...); api takes extra param so release builds need not store fData_
         */
        nonvirtual size_t CurrentIndex (const HashTable* data) const;

    public:
        /**
         *  \pre GetUnderlyingData() == rhs.GetUnderlyingData (), or special case of one or the other is nullptr
         */
        constexpr bool operator== (const ForwardIterator& rhs) const;

    public:
        constexpr UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (const UnderlyingIteratorRep l);

    public:
        nonvirtual ForwardIterator& operator++ ();
        nonvirtual ForwardIterator  operator++ (int);

    public:
        // safe to update in place (doesn't change iterators) since doesn't change order of list (since not updating key)
        template <typename CHECKED_T = MAPPED_TYPE>
        nonvirtual void UpdateValue (ArgByValueType<CHECKED_T> newValue)
            requires (not same_as<MAPPED_TYPE, void>);

    public:
        /**
         *  For debugging, assert the iterator data matches argument data
         */
        constexpr void AssertDataMatches (const HashTable* data) const;

    public:
        constexpr void Invariant () const noexcept;

#if qStroika_Foundation_Debug_AssertionsChecked
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        const Link_* fCurrent_{nullptr};
#if qStroika_Foundation_Debug_AssertionsChecked
        const HashTable* fData_{nullptr};
#endif

    private:
        friend class HashTable;
    };

    static_assert (ranges::input_range<HashTable<int, int>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "HashTable.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_HashTable_h_ */
