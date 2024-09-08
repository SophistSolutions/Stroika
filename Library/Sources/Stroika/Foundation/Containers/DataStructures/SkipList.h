/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_SkipList_h_
#define _Stroika_Foundation_Containers_DataStructures_SkipList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

// for now uses std::vector...
#include <vector>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/Empty.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    namespace SkipList_Support {

        template <typename KEY_TYPE, Common::IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way,
                  AddOrExtendOrReplaceMode addOrExtendOrReplace = AddOrExtendOrReplaceMode::eAddExtras, typename ALTERNATE_FIND_TYPE = void>
        struct DefaultTraits {
            /**
             */
            using KeyComparerType = KEY_COMPARER;

            /**
             *  no idea what good default is here... Probably isn't one...
             */
            static constexpr AddOrExtendOrReplaceMode kAddOrExtendOrReplaceMode{addOrExtendOrReplace};

            /**
             *  Store stats about performance of SkipList, for tuning purposes
             */
            static constexpr bool kKeepStatistics{false};

            /**
             */
            static constexpr bool kCostlyInvariants{false};

            /**
             *  like is_transparent mechanism in C++14, except just adds one type (if not void) to the set of types you can find looking for)
             *  \note when using AlternateFindType != void, caller must ALSO provide a compare function which accepts combinations of T, and AlternateFindType
             */
            using AlternateFindType = ALTERNATE_FIND_TYPE;
        };

        /**
         */
        template <typename TRAITS, typename KEY_TYPE>
        concept IValidTraits = Common::IThreeWayComparer<typename TRAITS::KeyComparerType, KEY_TYPE> and requires (TRAITS a) {
            {
                TRAITS::kKeepStatistics
            } -> std::convertible_to<bool>;
            {
                TRAITS::kAddOrExtendOrReplaceMode
            } -> std::convertible_to<AddOrExtendOrReplaceMode>;
        };

        struct Stats_Basic {
            size_t fCompares{0};
            // skiplists don't really do rotations, but we treat link patching as same thing
            // @todo rename so more appropriate - and looking at code - not clear what this is actually counting
            size_t fRotations{0};

            /**
             *  @see Characters::ToString ();
             */
            //nonvirtual Characters::String ToString () const;
        };

        /**
         */
        template <typename KEY_TYPE, IValidTraits<KEY_TYPE> TRAITS>
        using StatsType = conditional_t<TRAITS::kKeepStatistics, Stats_Basic, Configuration::Empty>;
    }

    /*
     *      SkipList<> is a (low level) data structure class. It is not intended to be directly
     *  used by programmers, except in implementing concrete container reps, but of course can be
     *  where extra performance maybe needed, over convenience and flexibility.
     *
     *  Key Features (compared with balanced binary tree)
     *      o   Explicit Rebalance () call
     *      o   (optional)Prioritize() call - can optimize lookup of any key
     *      o   fast find, reasonably fast add and remove (about 20-30% as many comparisons as finds)
     *      o   robust about order of additions
     *      o   space efficient (1.33 links per value, as opposed to tree structures requiring 2 links per value)
     *      o   ability to add more than one entry with the same key
     *      o   ability to reorder links into nearly optimal configuration. You can optimize the node structure in a skip list in a single pass (order N).
     *          This reduces the total comparisons in a search by between 20 and 40%.
     *      o   possible to efficiently parallelize (not yet attempted -- see http://www.1024cores.net/home/parallel-computing/concurrent-skip-list)
     *      o   SkipLists support fast forward iteration (linked list traversal). They do not support backwards iteration.
     *
     *  Design Overview:
     *      This is an ORDERED linked list (so no push_front, cuz order implied by key value and comparer). 
     *      To get extra speed on lookups, besides a 'next' pointer, each node may have a list of additional
     *      pointers that go (each progressively) further into the linked list. Ideally, these 'jumps' deeper 
     *      into the linked list would be 'well spaced' so that you approach log(N) lookup times trying to find a Node.
     * 
     *      For each node, the fLinks[0] is always == NEXT link.
     * 
     * \see http://en.wikipedia.org/wiki/Skip_list:
     *      A skip list is a data structure for storing a sorted list of items using a hierarchy of linked lists that connect
     *      increasingly sparse subsequences of the items. These auxiliary lists allow item lookup with efficiency comparable 
     *      to balanced binary search trees (that is, with number of probes proportional to log n instead of n).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     * 
     *  \note   TODOS:
     *              @todo - should we use shared_ptr  - must be more careful bout leaks if not using shared_ptr
     *              @todo Cleanup docs
     *              @todo use InlineBuffer instead of vector, and make size of pre-allocated guy fixed in TRAITS (@todo discuss with sterl)
     *              @todo   https://stroika.atlassian.net/browse/STK-1016 - ranges/sentinel support
     * 
    // OLD DOCS to lift from (from SSW impl)
    In principle you can use different probabilities for having more than one link. The optimal probability for finds is 1/4, and that also produces a list
    that is more space efficient than a traditional binary tree, as it has only 1.33 nodes per entry, compared with a binary tree using 2.

    Testing SkipList of 100000 entries, sorted add, link creation probability of 0.25
      Unoptimized SkipList
      total links = 133298; avg link height = 1.33298; max link height= 9
      find avg comparisons = 28.8035; expected = 33.2193
      add  avg comparisons = 37.5224; expected = 35.5526
      remove  avg comparisons = 38.1671; expected = 35.5526

      After optimizing links
      total links = 133330; avg link height = 1.3333; max link height= 9
      find avg comparisons = 18.1852; expected = 33.2193
      find reduction = 36.8646%

   The "expected" above is from wikipedia, and is calculated as (log base 1/p n)/p.

     *  \original Author: Sterling Wight
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS = SkipList_Support::DefaultTraits<KEY_TYPE>>
    class SkipList : public Debug::AssertExternallySynchronizedMutex {
    private:
        struct Link_;

    public:
        /**
         *  This is the key for the associative container. It need not be unique (see various Add / Lookup APIs).
         */
        using key_type = KEY_TYPE;

    public:
        /**
         *  This is typically NOT void, but may be void
         */
        using mapped_type = MAPPED_TYPE;

    public:
        /**
         *  SkipLists intrinsically require knowing the in-order comparison function applied to key data.
         */
        using KeyComparerType = typename TRAITS::KeyComparerType;

    public:
        /**
         *  This is typically Configuration::Empty, but can contain real stats used to debug/tune parameters,
         *  if you construct the SkipList with TRAITS having kKeepStatistics true.
         */
        using StatsType = SkipList_Support::StatsType<KEY_TYPE, TRAITS>;

    public:
        /**
         *  KeyValuePair of KEY_TYPE and MAPPED_TYPE. This is what you iterate over, and the SkipList is a container of these.
         */
        using value_type = Common::KeyValuePair<KEY_TYPE, MAPPED_TYPE>;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        using UnderlyingIteratorRep = const Link_*;

    public:
        /**
         */
        using TraitsType = TRAITS;

    public:
        /**
         */
        SkipList (KeyComparerType keyComparer = {});
        SkipList (SkipList&& src) noexcept;
        SkipList (const SkipList& src);
        ~SkipList ();

    public:
        nonvirtual SkipList& operator= (const SkipList& t);

    public:
        /**
         */
        constexpr KeyComparerType key_comp () const;

    public:
        class ForwardIterator;

#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    private:
        bool Add1_ (ArgByValueType<key_type> key, ForwardIterator* oAddedI);
        template <typename CHECK_T = MAPPED_TYPE>
        bool Add2_ (ArgByValueType<key_type> key, ArgByValueType<CHECK_T> val, ForwardIterator* oAddedI);
#endif

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
         *  \req contains (key)
         * 
         *  \note same as Verify (RemoveIf (key))
         * 
         *  \note Runtime performance/complexity:   ??
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
         *  \req i != end ()
         * 
         *  \brief see https://en.cppreference.com/w/cpp/container/vector/erase
         * 
         *  \see also Remove()
         */
        nonvirtual ForwardIterator erase (const ForwardIterator& i);

    public:
        /**
         * \brief Remove the first item with the given key, if any. Return true if a value found and removed.
         * 
         *  \note Runtime performance/complexity:   ??
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual bool RemoveIf (ArgByValueType<key_type> key);

    public:
        /**
        * @todo discuss with sterl - if we allow multiple values with same key, add removeall overload taking key_type, and maybe returning count removed? removeallif
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
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an equivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const SkipList* movedFrom) const;

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
        template <predicate<typename SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type> FUNCTION>
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
        template <predicate<typename SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type> FUNCTION>
        nonvirtual optional<mapped_type> First (FUNCTION&& firstThat) const
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        {
            for (auto i : *this) {
                if (firstThat (*i)) {
                    return i->fValue;
                }
            }
            return nullopt;
        }
#else
            ;
#endif

    public:
        template <typename CHECKED_T = MAPPED_TYPE>
        nonvirtual void Update (const ForwardIterator& it, ArgByValueType<CHECKED_T> newValue)
            requires (not same_as<MAPPED_TYPE, void>);

    public:
        /**
         *  \brief optimize the memory layout of the SkipList
         * 
         * calling this will result in maximal search performance until further adds or removes
         * call when list is relatively stable in size, and it will set links to near classic log(n/2) search time
         * relatively fast to call, as is order N (single list traversal)
         * 
         *  \note alias: Could be called 'Optimize'
         * 
         *  \note Runtime performance/complexity:
         *      Average/WorseCase???
         */
        nonvirtual void ReBalance ();

    public:
        /**
         * make the node faster on finds, possibly slowing other node searches down
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

#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
        template <typename FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;
#else
        template <invocable<typename SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;
#endif

    public:
        constexpr void Invariant () const noexcept;

    public:
        /**
        * @todo doc api just for debugging? And not generally useful. And maybe have return tuple, not take var param?
        // height is highest link height, also counts total links if pass in non-null totalHeight
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
            LinkVector_ fNext; // for a SkipList, you have an array of next pointers, rather than just one
        };
        LinkVector_ fHead_{};

    private:
        /*
         * Find node for key in SkipList, else nullptr. In cases of duplicate values, return first found.
         */
        template <Configuration::IAnyOf<KEY_TYPE, typename TRAITS::AlternateFindType> KEYISH_T>
        nonvirtual Link_* FindNode_ (const KEYISH_T& key) const;

    private:
        /*
         *  This searches the list for the given key. If found exactly, it is returned.
         *
         * this is specialized for the case of adding or removing elements, as it also returns
         * all links that will need to be updated for the new element or the element to be removed
         *
         *      \ens (result == nullptr or fKeyThreeWayComparer_ (result->fEntry.fKey, key) == strong_ordering::equal);
         */
        nonvirtual Link_* FindNearest_ (ArgByValueType<key_type> key, LinkVector_& links) const;
        nonvirtual Link_* FindNearest_ (const ForwardIterator& i, LinkVector_& links) const;

    private:
        nonvirtual void AddNode_ (Link_* n, const LinkVector_& linksToPatch);

    private:
        nonvirtual void RemoveNode_ (Link_* n, const LinkVector_& linksToPatch);

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        nonvirtual void ShrinkHeadLinksIfNeeded_ ();

    private:
        nonvirtual void GrowHeadLinksIfNeeded_ (size_t newSize, Link_* nodeToPointTo);

    private:
        nonvirtual size_t DetermineLinkHeight_ () const;

    private:
        [[no_unique_address]] KeyComparerType   fKeyThreeWayComparer_{};
        size_t                                  fLength_{0};
        [[no_unique_address]] mutable StatsType fStats_{};
    };

    /*
     *      ForwardIterator allows you to iterate over a SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>. It is not safe to use a ForwardIterator after any
     *      update to the SkipList.
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    class SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator {
    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = forward_iterator_tag;
        using value_type        = SkipList::value_type;
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
        explicit constexpr ForwardIterator (const SkipList* data) noexcept;
        explicit constexpr ForwardIterator (const SkipList* data, UnderlyingIteratorRep startAt) noexcept;
        constexpr ForwardIterator (const ForwardIterator&) noexcept = default;
        constexpr ForwardIterator (ForwardIterator&&) noexcept      = default;

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator&)     = default;
        nonvirtual ForwardIterator& operator= (ForwardIterator&&) noexcept = default;

#if qDebug
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
         *  \req data == fData_ argument constructed with (or as adjusted by Move...); api takes extra param so release builds need not store fData_
         */
        nonvirtual size_t CurrentIndex (const SkipList* data) const;

    public:
        /**
         *  \req GetUnderlyingData() == rhs.GetUnderlyingData (), or special case of one or the other is nullptr
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
        // safe to update in place since doesn't change order of list (since not updating key)
        template <typename CHECKED_T = MAPPED_TYPE>
        nonvirtual void UpdateValue (ArgByValueType<CHECKED_T> newValue)
            requires (not same_as<MAPPED_TYPE, void>);

    public:
        /**
         *  For debugging, assert the iterator data matches argument data
         */
        constexpr void AssertDataMatches (const SkipList* data) const;

    public:
        constexpr void Invariant () const noexcept;

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        const Link_* fCurrent_{nullptr};
#if qDebug
        const SkipList* fData_{nullptr};
#endif

    private:
        friend class SkipList;
    };

    static_assert (ranges::input_range<SkipList<int, int>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SkipList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_SkipList_h_ */
