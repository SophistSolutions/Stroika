#pragma once

#include <vector>

#include "../Shared/Headers/BlockAllocated.h"
#include "../Shared/Headers/TreeTraits.h"

/*
    From Wikipedia:
    A skip list is a data structure for storing a sorted list of items using a hierarchy of linked lists that connect increasingly sparse subsequences of the items.
    These auxiliary lists allow item lookup with efficiency comparable to balanced binary search trees (that is, with number of probes proportional to log n instead of n).
    see http://en.wikipedia.org/wiki/Skip_list

    SkipLists have the following desireable features
    fast find, reasonably fast add and remove (about 20-30% as many comparisons as finds)

    robust about order of additions

    space efficient (1.33 links per value, as opposed to tree structures requiring 2 links per value)

    ability to add more than one entry with the same key

    ability to reorder links into nearly optimal configuration. You can optimize the node structure in a skip list in a single pass (order N).
    This reduces the total comparisons in a search by between 20 and 40%.

    possible to efficiently parallelize (not yet attempted -- see http://www.1024cores.net/home/parallel-computing/concurrent-skip-list)

    SkipLists support fast forward iteration (linked list traversal). They do not support backwards iteration.

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

template <typename KEY,
          typename VALUE,
          typename TRAITS = TreeTraits::Traits<
              KeyValue<KEY, VALUE>,
              TreeTraits::DefaultComp<KEY>>>
class SkipList {
public:
    typedef KEY   KeyType;
    typedef VALUE ValueType;

public:
    SkipList ();
    SkipList (const SkipList& s);
    ~SkipList ();

    SkipList& operator= (const SkipList& t);

    /*
        Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
        In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
        how to do this.
    */
    bool Find (const KeyType& key, ValueType* val = nullptr) const;

    /*
        You can add more than one item with the same key. If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove calls.
    */
    void Add (const KeyType& key, const ValueType& val);

    void Remove (const KeyType& key);
    void RemoveAll ();

    size_t GetLength () const; // always equal to total Add minus total Remove

    static size_t GetLinkHeightProbability (); // percent chance. We use 25%, which appears optimal

    // calling this will result in maximal search performance until further adds or removes
    // call when list is relatively stable in size, and it will set links to near classic log(n/2) search time
    // relatively fast to call, as is order N (single list traversal)
    void Optimize ();

#if qDebug
    void ListAll () const;
    void ValidateAll () const;
#endif

#if qKeepADTStatistics
    // height is highest link height, also counts total links if pass in non-null totalHeight
    size_t CalcHeight (size_t* totalHeight = nullptr) const;
#endif

public:
    enum {
        kMaxLinkHeight = sizeof (size_t) * 8,
    };

    struct Node {
        Node (const KeyType& key, const ValueType& val);

        DECLARE_USE_BLOCK_ALLOCATION (Node);

        typename TRAITS::KeyValue fEntry;
        std::vector<Node*>        fNext; // for a skiplist, you have an array of next pointers, rather than just one
    };
    std::vector<Node*> fHead;

    /*
        Find node for key in skiplist, else nullptr. In cases of duplicate values, return first found
     */
    Node* FindNode (const KeyType& key) const;

    void AddNode (Node* n, const std::vector<Node*>& linksToPatch);
    void RemoveNode (Node* n, const std::vector<Node*>& linksToPatch);

    // this is specialized for the case of adding or removing elements, as it caches
    // all links that will need to be updated for the new element or the element to be removed
    Node* FindNearest (KeyType key, std::vector<Node*>& links) const;

private:
    void   ShrinkHeadLinksIfNeeded ();
    void   GrowHeadLinksIfNeeded (size_t newSize, Node* nodeToPointTo);
    size_t DetermineLinkHeight () const;

    size_t fLength;

#if qKeepADTStatistics
public:
    mutable size_t fCompares;
    mutable size_t fRotations;
#endif
};

#include "SkipList.inl"
