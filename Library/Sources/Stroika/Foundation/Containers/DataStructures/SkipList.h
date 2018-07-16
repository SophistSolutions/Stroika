/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_SkipList_h_
#define _Stroika_Foundation_Containers_DataStructures_SkipList_h_

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"
#include "../../Memory/BlockAllocated.h"

#include "../Common.h"

/**
 *
 *      SkipList<> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
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

    /*

                ADT::Traits<
                KeyValue<KEY,VALUE>,
                            ADT::DefaultComp<KEY>,
                            ADT::eDefaultPolicy >

                                 */
    template <typename KEY,
              typename VALUE,
              typename TRAITS>
    class SkipList : public Debug::AssertExternallySynchronizedLock {
    public:
        using KeyType   = KEY;
        using ValueType = VALUE;
        using KeyValue  = typename TRAITS::KeyValue;

    public:
        using value_type = pair<KEY, VALUE>; /// gonna have to work on this to get working...

    public:
        SkipList ();
        SkipList (const SkipList& s);
        ~SkipList ();

    public:
        nonvirtual SkipList& operator= (const SkipList& t);

    public:
        /*
                        Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
                        In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
                        how to do this.
                    */
        nonvirtual bool Find (const KeyType& key, ValueType* val = nullptr) const;

    public:
        /*
                        You can add more than one item with the same key. If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove calls.
                    */
        nonvirtual void Add (const KeyType& key, const ValueType& val);
        nonvirtual void Add (const KeyType& keyAndValue); // convenient when key and value are the same, like a sorted list of names

    public:
        nonvirtual void Remove (const KeyType& key);

    public:
        nonvirtual void RemoveAll ();

    public:
        nonvirtual size_t GetLength () const; // always equal to total Add minus total Remove

    public:
        class ForwardIterator;

    public:
        nonvirtual ForwardIterator MakeIterator () const;

        // returns the first entry equal to, or the smallest entry with key larger than the passed in key
        nonvirtual ForwardIterator MakeIterator (const KeyType& key) const;

    public:
        nonvirtual ForwardIterator GetFirst () const; // synonym for begin (), MakeIterator ()
        nonvirtual ForwardIterator GetLast () const;  // returns iterator to largest key

    public:
        nonvirtual void Update (const ForwardIterator& it, const ValueType& newValue);

    public:
        nonvirtual void Remove (const ForwardIterator& it);

    public:
        // calling this will result in maximal search performance until further adds or removes
        // call when list is relatively stable in size, and it will set links to near classic log(n/2) search time
        // relatively fast to call, as is order N (single list traversal)
        nonvirtual void ReBalance ();

    public:
        // make the node faster on finds, possibly slowing other node searches down
        nonvirtual void Prioritize (const KeyType& key);

    public:
        nonvirtual void Invariant () const;

#if qDebug
    public:
        nonvirtual void ListAll () const;
        nonvirtual void ValidateAll () const;
#endif

#if qKeepADTStatistics
        // height is highest link height, also counts total links if pass in non-null totalHeight
        nonvirtual size_t CalcHeight (size_t* totalHeight = nullptr) const;
#endif

    public:
        static size_t GetLinkHeightProbability (); // percent chance. We use 25%, which appears optimal

    protected:
        enum {
            kMaxLinkHeight = sizeof (size_t) * 8,
        };

    protected:
        struct Node : public Memory::UseBlockAllocationIfAppropriate<Node> {
            Node (const KeyType& key, const ValueType& val);

            KeyValue      fEntry;
            vector<Node*> fNext; // for a skiplist, you have an array of next pointers, rather than just one
        };
        vector<Node*> fHead;

    protected:
        /*
         *  These return the first and last entries in the tree (defined as the first and last entries that would be returned via
         *  iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
         */
        nonvirtual Node* FirstNode () const;
        nonvirtual Node* LastNode () const;

        /*
         * Find node for key in skiplist, else nullptr. In cases of duplicate values, return first found
         */
        nonvirtual Node* FindNode (const KeyType& key) const;

        // this is specialized for the case of adding or removing elements, as it caches
        // all links that will need to be updated for the new element or the element to be removed
        nonvirtual Node* FindNearest (KeyType key, vector<Node*>& links) const;

        nonvirtual void AddNode (Node* n, const vector<Node*>& linksToPatch);
        nonvirtual void RemoveNode (Node* n, const vector<Node*>& linksToPatch);

#if qDebug
    protected:
        nonvirtual void Invariant_ () const;
#endif

    private:
        nonvirtual void ShrinkHeadLinksIfNeeded ();
        nonvirtual void GrowHeadLinksIfNeeded (size_t newSize, Node* nodeToPointTo);
        nonvirtual size_t DetermineLinkHeight () const;

        size_t fLength;

#if qKeepADTStatistics
    public:
        mutable size_t fCompares;
        mutable size_t fRotations; // skiplists don't really do rotations, but we treak link patching as same thing
#endif

        friend class SkipListIterator<KEY, VALUE, TRAITS>;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SkipList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_SkipList_h_ */
