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
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::DataStructures {

    namespace SkipList_Support {

        template <typename KEY_TYPE, Common::IThreeWayComparer<KEY_TYPE> KEY_COMPARER = compare_three_way, AddOrExtendOrReplaceMode addOrExtendOrReplace = AddOrExtendOrReplaceMode::eAddExtras>
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
     *      This is an ORDERED linked list. To get extra speed on lookups, besides a 'next' pointer,
     *      each node may have a list of additional pointers that go (each progressively) further into the linked
     *      list. Ideally, these 'jumps' deeper into the linked list would be 'well spaced' so that you approach
     *      log(N) lookup times trying to find a Node.
     * 
     *      For each node, the fLinks[0] is always == NEXT link.
     * 
     * \see http://en.wikipedia.org/wiki/Skip_list:
     *      A skip list is a data structure for storing a sorted list of items using a hierarchy of linked lists that connect
     *      increasingly sparse subsequences of the items. These auxiliary lists allow item lookup with efficiency comparable 
     *      to balanced binary search trees (that is, with number of probes proportional to log n instead of n).
     * 
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     * 
     *  \note   TODOS:
     *              @todo assure works with MAPPED_TYPE=void
     *              @todo - should we use shared_ptr for Node*? at last use blockallocation - must be more carefula bout leaks if not using shared_ptr
     *              @todo Cleanup docs
     *              @todo Integrate into concrete containers
     *              @todo use InlineBuffer instead of vector, and make size of pre-allocated guy fixed in TRAITS (@todo discuss with sterl)

    // OLD DOCS to lift from (from SSW impl)
    In principle you can use different probabilies for having more than one link. The optimal probability for finds is 1/4, and that also produces a list
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
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS = SkipList_Support::DefaultTraits<KEY_TYPE>>
    class SkipList : public Debug::AssertExternallySynchronizedMutex {
    public:
        /**
         *  This is the key for the associative container. It need not be unique (see various Add / Lookup APIs).
         */
        using key_type = KEY_TYPE;

    public:
        /**
         *  This is typically NOT void, but may be void (NYI).
         */
        using mapped_type = MAPPED_TYPE;

    public:
        /**
         *  SkipLists intrinsically require knowing the in-order comparison function applied to key data.
         */
        using KeyComparerType = TRAITS::KeyComparerType;

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
         */
        SkipList (KeyComparerType keyComparer = {});
        SkipList (const SkipList& s);
        ~SkipList ();

    public:
        nonvirtual SkipList& operator= (const SkipList& t);

    public:
        /**
         */
        nonvirtual KeyComparerType GetComparer () const;

    public:
        /**
         *  You can add more than one item with the same key. If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove calls.
         *
         *  Returns true if the list was changed (if eAddReplaces, and key found, return true even if val same as value already there because we cannot generically compare values)
         * 
         *  \note Complexity:   ??
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual bool Add (const key_type& key, const mapped_type& val);
        nonvirtual bool Add (const value_type& v);

    public:
        /**
         *  \brief Remove an item with the given key (require it exists)
         * 
         *  \req contains (key)
         * 
         *  \note same as Verify (RemoveIf (key))
         * 
         *  \note Complexity:   ??
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual void Remove (const key_type& key);

    public:
        /**
         * \brief Remove the first item with the given key, if any. Return true if a value found and removed.
         * 
         *  \note Complexity:   ??
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual bool RemoveIf (const key_type& key);

    public:
        /**
        * @todo discuss with sterl - if we allow multiple values with same key, add removeall overload taking key_type, and maybe returning count removed? removeallif
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual bool empty () const;

    public:
        class ForwardIterator;

    public:
        /**
         */
        nonvirtual ForwardIterator begin () const;

    public:
        /**
         */
        nonvirtual ForwardIterator end () const;

    public:
        /**
         *  \see https://en.cppreference.com/w/cpp/container/map/contains
         *
         *  \note Complexity:   ??
         *      Average:    log(N)
         *      Worst:      N
         */
        nonvirtual bool contains (const key_type& key) const;

    public:
        /*
        * @todo cleanup API - should use optional? Overloading?
        * 
                        Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
                        In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
                        how to do this.

                    \see contains()


                    maybe replace with LookupOne (returns optional) - maybe FindOne? See names used elsewhere
                    Find (returns Iterator<> with promise all 'equal value' items sorted together.
                    */
        nonvirtual bool Find (const key_type& key, mapped_type* val = nullptr) const;

    public:
        // returns the first entry equal to, or the smallest entry with key larger than the passed in key
        // // probably call this Find ()
        //  nonvirtual ForwardIterator MakeIterator (const key_type& key) const;

    public:
        //    nonvirtual void Update (const ForwardIterator& it, const mapped_type& newValue);

    public:
        //   nonvirtual void Remove (const ForwardIterator& it);

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
         *  \note Complexity:
         *      Average/WorseCase???
         */
        nonvirtual void ReBalance ();

    public:
        /**
         * make the node faster on finds, possibly slowing other node searches down
         * 
         *  \note Complexity:
         *      Average/WorseCase???
         */
        nonvirtual void Prioritize (const key_type& key);

    public:
        constexpr void Invariant () const noexcept;

#if qDebug
        // @todo maybe lose these APIs?? Can do with iterator easily enuf, and validateall part of invariant???
    public:
        nonvirtual void ListAll () const;
#endif

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
        struct Node_;

    private:
        /*
         *  These return the first and last entries in the tree (defined as the first and last entries that would be returned via
         *  iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
         */
        nonvirtual Node_* GetFirst_ () const; // synonym for begin (), MakeIterator ()
    private:
        nonvirtual Node_* GetLast_ () const; // returns iterator to largest key

    private:
        // @todo maybe make part of traits??? and use in InlineBuffer somehow? instead of vector
        // maybe no need for MAX - just optimized-for-max - size of inline buffer - not sure why we need any other max (can use stackbuffer for that)
        static constexpr size_t kMaxLinkHeight_ = sizeof (size_t) * 8;

    private:
        struct Node_ : public Memory::UseBlockAllocationIfAppropriate<Node_> {
            constexpr Node_ (const key_type& key, const mapped_type& val);

            value_type     fEntry;
            vector<Node_*> fNext; // for a skiplist, you have an array of next pointers, rather than just one
            // @todo consider using Memory::InlineBuffer<> - so fewer memory allocations for some small buffer size???, and tune impl to prefer this size or take param in traits used for this
        };
        vector<Node_*> fHead_;

    private:
        /*
         * Find node for key in skiplist, else nullptr. In cases of duplicate values, return first found
         */
        nonvirtual Node_* FindNode_ (const key_type& key) const;

    private:
        /*
         *  This searches the list for the given key. If found exactly, it is returned.
         *
         * this is specialized for the case of adding or removing elements, as it caches
         * all links that will need to be updated for the new element or the element to be removed
         *
         *      \ens (result == nullptr or fKeyThreeWayComparer_ (result->fEntry.fKey, key) == strong_ordering::equal);
         */
        nonvirtual Node_* FindNearest_ (const key_type& key, vector<Node_*>& links) const;

    private:
        nonvirtual void AddNode_ (Node_* n, const vector<Node_*>& linksToPatch);

    private:
        nonvirtual void RemoveNode_ (Node_* n, const vector<Node_*>& linksToPatch);

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        nonvirtual void ShrinkHeadLinksIfNeeded_ ();

    private:
        nonvirtual void GrowHeadLinksIfNeeded_ (size_t newSize, Node_* nodeToPointTo);

    private:
        nonvirtual size_t DetermineLinkHeight_ () const;

    private:
        [[no_unique_address]] KeyComparerType   fKeyThreeWayComparer_;
        size_t                                  fLength_{0};
        [[no_unique_address]] mutable StatsType fStats_;
    };

    /**
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    class SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator {
    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = forward_iterator_tag;
        using value_type        = SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type;
        using difference_type   = ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;

    public:
        /**
         */
        ForwardIterator () = delete;
        constexpr ForwardIterator (const SkipList* data, const Node_* n = nullptr);
        ForwardIterator (const ForwardIterator& src) = default;

#if qDebug
        ~ForwardIterator ();
#endif

    public:
        nonvirtual value_type Current () const; //  Error to call if Done (), otherwise OK

    public:
        /**
         *  \req GetUnderlyingData() == rhs.GetUnderlyingData (), or special case of one or the other is nullptr
         */
        constexpr bool operator== (const ForwardIterator& rhs) const;

    public:
        constexpr const SkipList* GetUnderlyingData () const;

    public:
        nonvirtual ForwardIterator& operator++ ();

    public:
        constexpr void Invariant () const noexcept;

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
#if qDebug
        const SkipList* fData_{nullptr}; // @todo - maybe only keep this for DEBUG case? Can we always navigate by Node_?
#endif
        const Node_* fCurrent_{nullptr};

    private:
        friend class SkipList;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SkipList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_SkipList_h_ */
