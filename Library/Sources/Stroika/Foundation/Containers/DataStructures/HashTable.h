/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_HashTable_h_
#define _Stroika_Foundation_Containers_DataStructures_HashTable_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Cryptography/Digest/HashBase.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Memory/InlineBuffer.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Common::ArgByValueType;

    /**
     *  HashTable module design notes:
     *      o   use traits to pick between separate chaining, and linear probing (do sep chaining first since easiest)
     *      o   Sometimes want HashTable<T> and sometimes HashTable<KEY,MAPPED_VALUE> - later appears more natural due to
     *          ability to conditionally create overloads of things like Add (KEY) (when mapped_value=void).
     */
    namespace HashTable_Support {

        /**
         *  \brief used internally to select HashTable implementation strategies. Callers dont use directly, but use SeparateChainingOptions<>
         */
        struct SeparateChainingTag {};

        /**
         *  \brief used internally to select HashTable implementation strategies. NOT YET IMPLEMENTED OR THOUGHT OUT (see https://en.wikipedia.org/wiki/Linear_probing) - tricky part is delete
         */
        struct LinearProbingTag {};

        /**
         *  \brief use as LAYOUT_OPTIONS for HashTable DefaultTraits<> template
         */
        template <typename KEY_TYPE, typename MAPPED_TYPE, size_t INLINE_ELTS_PER_CHAIN = 2, size_t INLINE_BUCKETS = 5>
        struct SeparateChainingOptions : SeparateChainingTag {
            static constexpr size_t kBufferedElementsPerChain = INLINE_ELTS_PER_CHAIN;
            static constexpr size_t kBufferedBuckets          = INLINE_BUCKETS;
        };

        /**
         *  KEY_TYPE the type of the key element stored in the SkipList.
         *  KEY_COMPARER is nearly always as given
         *  AddOrExtendOrReplaceMode addOrExtendOrReplace defaults to eAddExtras, but here the caller may not want the default. There is no good default here.
         *  ALTERNATE_FIND_TYPE can often be omitted (default) - but allows Find () to be overloaded (argument comparer) on a different type (besides just KEY_TYPE).
         */
        template <typename KEY_TYPE, typename MAPPED_TYPE, Cryptography::Digest::IHashFunction<KEY_TYPE> HASHER = hash<KEY_TYPE>,
                  Common::IEqualsComparer<KEY_TYPE> EQUALS_COMPARER = equal_to<KEY_TYPE>, typename LAYOUT_OPTIONS = SeparateChainingOptions<KEY_TYPE, MAPPED_TYPE>,
                  AddOrExtendOrReplaceMode addOrExtendOrReplace = AddOrExtendOrReplaceMode::eAddExtras, typename ALTERNATE_FIND_TYPE = void>
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
            using KeyHasherType = HASHER;

            /**
             */
            using KeyEqualsComparerType = EQUALS_COMPARER;

            /**
             *  separate chaining (for now) - or some probing variation
             */
            using LayoutType = LAYOUT_OPTIONS;

            /**
             *  like is_transparent mechanism in C++14, except just adds one type (if not void) to the set of types you can find looking for)
             *  \note when using AlternateFindType != void, caller must ALSO provide a compare function which accepts combinations of T, and AlternateFindType
             */
            using AlternateFindType = ALTERNATE_FIND_TYPE;

            /**
             *  \see AddOrExtendOrReplaceMode
             */
            static constexpr AddOrExtendOrReplaceMode kAddOrExtendOrReplace = addOrExtendOrReplace;

            /**
             */
            static constexpr bool kAutoShrinkBucketCount = false;
        };

        /**
         *  \brief validated the HashTable provided TRAITS object looks healthy (for better compiler diagnostics and usage docs)
         */
        template <typename TRAITS, typename KEY_TYPE, typename MAPPED_TYPE>
        concept IValidTraits = requires (TRAITS traits) {
            { declval<typename TRAITS::KeyHasherType> () } -> Cryptography::Digest::IHashFunction<KEY_TYPE>;
            { declval<typename TRAITS::KeyEqualsComparerType> () } -> Common::IEqualsComparer<KEY_TYPE>;
            { declval<typename TRAITS::LayoutType> () };
            { TRAITS::kAddOrExtendOrReplace } -> convertible_to<AddOrExtendOrReplaceMode>;
            { TRAITS::kAutoShrinkBucketCount } -> convertible_to<bool>;
        } and (same_as<typename TRAITS::AlternateFindType, void> or requires (TRAITS traits) {
                                   { declval<typename TRAITS::AlternateFindType> () };
                                   {
                                       declval<typename TRAITS::KeyHasherType> ()
                                   } -> Cryptography::Digest::IHashFunction<typename TRAITS::AlternateFindType>;
#if 0
                    // @todo get this working!
                    { declval<typename TRAITS::KeyEqualsComparerType>() } -> Common::IEqualsComparer<typename TRAITS::AlternateFindType>;
#endif
                               });

    }

    /**
     *  \brief implement hash table support in a lightweight standard template library style. Use
     *         traits to describe various choices about hashtable layout (separate chaining vs linear probing) etc
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
        using KeyHasherType = typename TRAITS::KeyHasherType;

    public:
        /**
         */
        using KeyEqualsComparerType = typename TRAITS::KeyEqualsComparerType;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        using UnderlyingIteratorRep = tuple<size_t, size_t>;

    public:
        /**
         */
        using TraitsType = TRAITS;

    public:
        /**
         *  \note HashTable (const HashTable& src) (and move ctor) guarantee elements iteration order unchanged in copy
         *        (see https://stackoverflow.com/questions/79551148/how-to-copy-stdunordered-map-while-preserving-its-order?noredirect=1)
         */
        HashTable (const KeyHasherType& hashFunction = {}, const KeyEqualsComparerType& keyComparer = {});
        HashTable (size_t bucketCount, const KeyHasherType& hashFunction = {}, const KeyEqualsComparerType& keyComparer = {});
        HashTable (HashTable&& src) noexcept = default;
        HashTable (const HashTable& src)     = default;
        ~HashTable ()                        = default;

    public:
        /**
         */
        nonvirtual HashTable& operator= (const HashTable&) = default;
        nonvirtual HashTable& operator= (HashTable&&)      = default;

    public:
        /**
         * \note this name mimics the name used in https://en.cppreference.com/w/cpp/container/unordered_set/hash_function
         */
        nonvirtual KeyHasherType hash_function () const;

    public:
        /**
         * \note this name mimics the naming used in https://en.cppreference.com/w/cpp/container/unordered_map/key_eq
         * 
         * GetKeyEqualsComparerType
         */
        nonvirtual KeyEqualsComparerType key_eq () const;

    public:
        class ForwardIterator;

    public:
        /**
         */
        nonvirtual ForwardIterator begin ();

    public:
        /**
         */
        constexpr static ForwardIterator end ();

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
         *  \brief Add an item (key value pair typically, but the value can be void). Return true on list change; Respects TRAITS::kAddOrExtendOrReplace
         * 
         *  If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove (key) calls.
         *
         *  Returns true if the list was changed (if eAddReplaces, and key found, return true even if val same as value already there because we cannot generically compare values)
         * 
         *  \note Behavior of adding redundant keys (keys which are already present) depends on TRAITS::kAddOrExtendOrReplace.
         * 
         *  returns true iff container modified by this operation (so for add replaces mode no info if already was present)
         * 
         *  \note Runtime performance/complexity:   ??
         *      Average:    O(1)
         *      Worst:      N
         */
        nonvirtual bool Add (const value_type& t);
        nonvirtual bool Add (const key_type& t)
            requires (same_as<MAPPED_TYPE, void>);
        template <same_as<MAPPED_TYPE> MAPPED_TYPE2 = MAPPED_TYPE>
        nonvirtual bool Add (const key_type& t, const MAPPED_TYPE2& m)
            requires (not same_as<MAPPED_TYPE, void>);

    public:
        /**
         *  \brief somewhat stdlib-like names - that will do what is expected of someone from stdc++, except for the
         *         lack of return type that wouldn't make sense here since Stroika iterators not directly used for modification.
         * 
         *  \see https://en.cppreference.com/w/cpp/container/unordered_map/insert
         */
        nonvirtual void insert (const pair<KEY_TYPE, MAPPED_TYPE>& p);

    public:
        /**
         */
        nonvirtual optional<value_type> Lookup (const key_type& t);

    public:
        /**
         *  \req t present - use RemoveIf() to avoid that precondition
         */
        nonvirtual void Remove (const ForwardIterator& i, ForwardIterator* nextI = nullptr);
        nonvirtual void Remove (const key_type& t);

    public:
        /**
         */
        nonvirtual bool RemoveIf (const key_type& t);

    public:
        /**
         *  \brief stdlib like names and semantics (though may want to rethink the ForwardIterator vs UnderlyingIteratorRep thing)
         */
        nonvirtual ForwardIterator       erase (const ForwardIterator& i);
        nonvirtual UnderlyingIteratorRep erase (const UnderlyingIteratorRep& i);

    public:
        /**
         *  TBD...  NOT same as https://en.cppreference.com/w/cpp/container/unordered_set/rehash
         * 
         *  Use ROUGHLY the argument number of hash buckets. Call bucket_count() to find number actually used.
         * 
         *  bucket_count never goes below 1, but if you request a number too low, just goes to lowest allowed.
         * so ReHash(0) can be used to 'compact' as much as possible.
         */
        nonvirtual void ReHash (size_t newBucketCount);

    public:
        /**
         * This examines load_factor and max_load_factor(), and depending on relationship, makes
         * a guess as to best size to use in call to ReHash();
         */
        nonvirtual void ReHashIfNeeded ();

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual size_t bucket_count () const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual size_t bucket_size (size_t bucketIdx) const;

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
         *  \brief average number of elements per bucket
         * 
         *  \see https://en.cppreference.com/w/cpp/container/unordered_set/load_factor
         */
        nonvirtual float load_factor () const;

    public:
        /**
         *  \brief average number of elements per bucket
         * 
         *  \see https://en.cppreference.com/w/cpp/container/unordered_set/load_factor
         */
        nonvirtual float max_load_factor () const;
        nonvirtual void  max_load_factor (float mlf);

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: O(N) if ! trivial_t<T> and if trivial_t, O(N-BUCKETS)
         */
        nonvirtual void clear ();

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
         *      Always: O(N)
         */
        template <invocable<typename TRAITS::value_type> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement, Execution::SequencePolicy seq = Execution::SequencePolicy::eDEFAULT) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      overload: (key_type)
         *      Average/Worst:    O(1) ; N
         *  \note Runtime performance/complexity:
         *      overload: (FUNCTION&& f overload)
         *      Average/Worst:    O(N)
         * 
         *  \see also find()
         * 
         *  \note this is kind of like set<T>::find () - but not exactly, and find() doesn't really have a uniform API across the various stl containers...
         *        which is why we use Find(), instead of find() as a name
         */
        nonvirtual ForwardIterator Find (ArgByValueType<key_type> key) const;
        template <typename ARG_T = typename TRAITS::AlternateFindType>
        nonvirtual ForwardIterator Find (ARG_T key) const
            requires (not same_as<typename TRAITS::AlternateFindType, void> and same_as<remove_cvref_t<ARG_T>, typename TRAITS::AlternateFindType>);
        template <predicate<typename TRAITS::key_type> FUNCTION>
        nonvirtual ForwardIterator Find (FUNCTION&& firstThat) const;

    public:
        /**
         *  \brief stdlib-ish API for 'Find' - returns iterator for found object in hashtable
         * 
         *  \note Runtime performance/complexity:
         *      overload: (key_type)
         *      Average/Worst:    O(1) ; N
         * 
         *  \note closely resembles https://en.cppreference.com/w/cpp/container/unordered_set/find API, so use same name (except for const/non-const part).
         */
        nonvirtual ForwardIterator find (ArgByValueType<key_type> key) const;
        template <typename ARG_T = typename TRAITS::AlternateFindType>
        nonvirtual ForwardIterator find (ARG_T key) const
            requires (not same_as<typename TRAITS::AlternateFindType, void> and same_as<remove_cvref_t<ARG_T>, typename TRAITS::AlternateFindType>);

    public:
        /**
         *  \note - unlike other modifying operations, this doesn't invalidate any iterators (including the argument iterator).
         */
        template <typename CHECKED_T = MAPPED_TYPE>
        nonvirtual void Update (const ForwardIterator& it, ArgByValueType<CHECKED_T> newValue)
            requires (not same_as<MAPPED_TYPE, void>);

    public:
        constexpr void Invariant () const noexcept;

#if qStroika_Foundation_Debug_AssertionsChecked
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        using LayoutType_                                  = typename TraitsType::LayoutType;
        static constexpr size_t kBufferedElementsPerChain_ = LayoutType_::kBufferedElementsPerChain;
        static constexpr size_t kBufferedBuckets_          = LayoutType_::kBufferedBuckets;

        // this type depends MORE INTIMATELY on LayoutType (use concepts to select when we support more)
        //
        struct BucketType_ {
            // because we keep number of elements in a bucket low, often best to use array instead of linked list (performance)
            Memory::InlineBuffer<value_type, kBufferedElementsPerChain_> fElements;
        };

        Memory::InlineBuffer<BucketType_, kBufferedBuckets_> fBuckets_;

        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS KeyHasherType         fHasher_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS KeyEqualsComparerType fKeyComparer_;

        size_t fCachedSize_{0};

    private:
        nonvirtual size_t Hash_ (const key_type& v) const
        {
            Require (fBuckets_.size () > 0);
            return fHasher_ (v) % fBuckets_.size ();
        }
        template <typename AT = typename TRAITS::AlternateFindType>
            requires (not same_as<AT, void>)
        nonvirtual size_t Hash_ (const AT& v) const
        {
            Require (fBuckets_.size () > 0);
            return fHasher_ (v) % fBuckets_.size ();
        }

    private:
        // From https://en.cppreference.com/w/cpp/container/unordered_set/unordered_set - max_load_factor = 1 by default - not wedded to that, but not a crazy default...
        float fMaxLoadFactor_{1.0};
    };

    /**
     *      ForwardIterator allows you to iterate over a HashTable<KEY_TYPE, MAPPED_TYPE, TRAITS>. It is not safe to use a ForwardIterator after any
     *      update to the HashTable.
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, HashTable_Support::IValidTraits<KEY_TYPE, MAPPED_TYPE> TRAITS>
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

#if 0
    public:
        /**
         *  \note Runtime performance/complexity:
         *      Average/WorseCase:  O(N)        - super slow cuz have to traverse on average half the list
         * 
         *  \pre data == fData_ argument constructed with (or as adjusted by Move...); api takes extra param so release builds need not store fData_
         */
        nonvirtual size_t CurrentIndex (const HashTable* data) const;
#endif

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
        // to make == compares simpler
        nonvirtual void AdvanceOverEmptyBuckets_ ();

    private:
        const HashTable* fData_{nullptr}; // sentinel value indicating DONE
        size_t           fBucketIndex_{0};
        size_t           fIntraBucketIndex_{0};

    private:
        friend class HashTable;
    };
    static_assert (ranges::input_range<HashTable<int>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "HashTable.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_HashTable_h_ */
