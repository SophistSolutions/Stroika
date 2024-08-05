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
 *
 * TODO:
 *      STATUS _ FAR FROM COMPILING - JUST STARTED INTEGRATING VERSION OF CODE FROM STERL 2013-06-17
 *
 *
 *      @todo
 *
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    namespace SkipList_Support {
        enum {
            // @todo lose these in favor of AddIf, RemoveIf() APIs
            eDefaultPolicy              = 0x0000,
            eInvalidRemoveIgnored       = 0x0001,
            eDuplicateAddThrowException = 0x0010,
        };

        template <typename KEY_TYPE, Common::IInOrderComparer<KEY_TYPE> KEY_COMPARER = less<KEY_TYPE>, int POLICY = eDefaultPolicy>
        struct DefaultTraits {
            using KeyComparerType = KEY_COMPARER;

            // @todo LOSE THIS PROBABLY BUT DISCUSS WITH STERL
            static const int kPolicy = POLICY; // bit field for now

            /**
             *  Store stats about performance of skiplist, for tuning purposes
             */
            static constexpr bool kKeepStatistics{false};
        };

        /**
        * @todo add cjeck for kKeepStatistics static bool member
         */
        template <typename TRAITS, typename KEY_TYPE>
        concept IValidTraits = Common::IInOrderComparer<typename TRAITS::KeyComparerType, KEY_TYPE>;

        struct Stats_Basic {

            size_t fCompares{0};
            size_t fRotations{0}; // skiplists don't really do rotations, but we treak link patching as same thing

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
     *
     *      SkipList<> is a (low level) data structure class. It is not intended to be directly
     *  used by programmers, except in implementing concrete container reps, but of course can be
     *  where extra performance maybe needed, over convenience and flexibility.
     *
     @todo DISCUSS WITH STERL

        o   I THINK IDEA IS ROUGHLY - std::map<KEY_TYPE,VALUE> - that compare is generally on KEY_TYPE (not on value), and can use where we would use std::map
        o   NOT sure we need traits. CAN probably handle policies the way we do other such situations in stroika containers - with overloads.
        o   Instead - just have as parameter to type - KEY_COMPARER.
        o   OR - maybe KEEP traits for FLAG indicating 'keep statistics'; define StatisticsType record...

        Key Features (compared with balanced binary tree)
            o   Explicit Rebalance () call
            o   (optional)Prioritize() call


            @todo assure works with MAPPED_TYPE=void

            @todo - should we use shared_ptr for Node*? at last use blockallocation - must be more carefula bout leaks if not using shared_ptr

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
        /*
        * @todo cleanup API - should use optional? Overloading?
        * 
                        Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
                        In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
                        how to do this.

                    \see contains()
                    */
        nonvirtual bool Find (const key_type& key, mapped_type* val = nullptr) const;

    public:
        /**
         *  \see https://en.cppreference.com/w/cpp/container/map/contains
         *
         *  \note Complexity:
         *      Average/WorseCase??? - I think ave log(N), worst N, but probably not quite - depends on max keys etc...
         */
        nonvirtual bool contains (const key_type& key) const;

    public:
        /**
        *  @todo maybe add AddIf() API, return bool if added (not sure how to handle cases of conflict - discuss with sterl) try to lose policy
        * 
         *  You can add more than one item with the same key. If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove calls.
         *
         *  \note Complexity:
         *      Average/WorseCase???
         */
        nonvirtual void Add (const key_type& key, const mapped_type& val);
        nonvirtual void Add (const value_type& v);

    public:
        /**
         *  @todo overload for If() case... 
         * 
         *  \note Complexity:
         *      Average/WorseCase???
         */
        nonvirtual void Remove (const key_type& key);

    public:
        /**
        * @todo discuss with sterl - if we allow multiple values with same key, add removeall overload taking key_type, and maybe returning count removed? removeallif
         */
        nonvirtual void RemoveAll ();

    public:
        /**
        * @todo better document - cost and means same as length - ; unclear rule about DUPS (DOC IN CLASS DECLARATION)
        * 
         */
        nonvirtual size_t size () const; // always equal to total Add minus total Remove

    public:
        class ForwardIterator;

    public:
        /**
         */
        //   nonvirtual ForwardIterator MakeIterator () const;
        nonvirtual ForwardIterator begin () const;

    public:
        nonvirtual ForwardIterator end () const;
        // returns the first entry equal to, or the smallest entry with key larger than the passed in key
        //  nonvirtual ForwardIterator MakeIterator (const key_type& key) const;

    public:
        //    nonvirtual void Update (const ForwardIterator& it, const mapped_type& newValue);

    public:
        //   nonvirtual void Remove (const ForwardIterator& it);

    public:
        /**
        // calling this will result in maximal search performance until further adds or removes
        // call when list is relatively stable in size, and it will set links to near classic log(n/2) search time
        // relatively fast to call, as is order N (single list traversal)
         */
        nonvirtual void ReBalance ();

    public:
        // make the node faster on finds, possibly slowing other node searches down
        nonvirtual void Prioritize (const key_type& key);

    public:
        nonvirtual void Invariant () const noexcept;

#if qDebug
        // @todo maybe lose these APIs?? Can do with iterator easily enuf, and validateall part of invariant???
    public:
        nonvirtual void ListAll () const;
        nonvirtual void ValidateAll () const;
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
         */
        nonvirtual StatsType GetStats () const;

    private:
        struct Node_;

    private:
        nonvirtual Node_* GetFirst_ () const; // synonym for begin (), MakeIterator ()
            //        nonvirtual ForwardIterator GetFirst () const; // synonym for begin (), MakeIterator ()
    private:
        nonvirtual Node_* GetLast_ () const; // returns iterator to largest key
                                             //        nonvirtual ForwardIterator GetLast () const;  // returns iterator to largest key

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
        vector<Node_*> fHead;

    private:
        /*
         *  These return the first and last entries in the tree (defined as the first and last entries that would be returned via
         *  iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
         */
        nonvirtual Node_* FirstNode_ () const;

    private:
        nonvirtual Node_* LastNode_ () const;

    private:
        /*
         * Find node for key in skiplist, else nullptr. In cases of duplicate values, return first found
         */
        nonvirtual Node_* FindNode_ (const key_type& key) const;

    private:
        // this is specialized for the case of adding or removing elements, as it caches
        // all links that will need to be updated for the new element or the element to be removed
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
        [[no_unique_address]] KeyComparerType fKeysStrictInOrderComparer_;
        size_t                                fLength_{0};

    private:
        [[no_unique_address]] mutable StatsType fStats_;
    };

    /**
    * DRAFT - INCOMPLETE... For example, no ++ etc...
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
        const SkipList* fData_{nullptr}; // @todo - maybe only keep this for DEBUG case? Can we always navigate by Node_?
        const Node_*    fCurrent_{nullptr};

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
