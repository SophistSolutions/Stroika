#pragma once

#include <random>
#include <vector>

#include "../Shared/Headers/BlockAllocated.h"
#include "../Shared/Headers/TreeTraits.h"

/*
    SplayTree is an aggressively self-adjusting binary tree. A classic splay tree always moves any found node to the top after a find or an add. This produces
    a larger number or rotations, but with non-uniform requests, the reduction in average node lookup distance can make up the lost time.

    SplayTree have the following desireable features
    fast find, add and remove.

    ability to add more than one entry with the same key

    ability to tune splay characteristics to match usage

    support backwards and forwards iteration (technically support in-order, pre-order and post-order, plus backward iteration which is reverse in-order)

    Drawbacks:
    difficult to efficiently parallelize, because of aggresive restructuring

    somewhat sensitive to order of additions. This can temporarily make the height of a splay tree very large. But find will very quickly reduce the height, and its
    average behavior is not particularly sensitive to addition order.

    SplayTrees have less overhead than many tree structures. They require at least 2 extra links per node for children, and often additional pointer to parent
    (not necessary but  makes some routines simpler and easier to parallelize). Overhead is thus either 2 or 3 links per node, considerably higher than SkipList (1.33),
    but less than other tree structures that record additional information with every node.

    Splaying (restructuring to make a node be the top node) is controlled by SplayType, and can very from never (simple binary search tree) to always (classic splaytree).
    In between SplayTree supports 3 built-in request distributions: uniform, normal, and Zipf. SplayTrees are usually not an optimal choice for uniform requests, but
    they do better with tighter request frequencies, such as normal and especially zipf. You can also set custom weights tuned to your own user's characteristics.
*/

typedef enum SplayType {
    eNeverSplay  = -1,
    eAlwaysSplay = 0,
    eUniformDistribution,
    eNormalDistribution,
    eZipfDistribution,
    eCustomSplayType, // set to eUniformDistribution unless user sets custom weights

    eDefaultSplayType = eUniformDistribution
} SplayType;

template <typename KEY,
          typename VALUE,
          typename TRAITS = TreeTraits::Traits<
              KeyValue<KEY, VALUE>,
              TreeTraits::DefaultComp<KEY>>>
class SplayTree {
public:
    typedef KEY   KeyType;
    typedef VALUE ValueType;

public:
    SplayTree ();
    SplayTree (const SplayTree& t);
    ~SplayTree ();

    nonvirtual SplayTree& operator= (const SplayTree& t);

    /*
        Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
        In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
        how to do this.
        Note that for a splay tree, unlike most containers, Find is not a const method
    */
    nonvirtual bool Find (const KeyType& key, ValueType* val = nullptr);

    /*
        You can add more than one item with the same key. If you add different values with the same key, but it is unspecified which item will be returned on subsequent Find or Remove calls.
    */
    nonvirtual void Add (const KeyType& key, const ValueType& val);
    nonvirtual void Add (const KeyType& keyAndValue); // convenient when key and value are the same, like a sorted list of names

    nonvirtual void Remove (const KeyType& key);
    nonvirtual void RemoveAll ();

    nonvirtual size_t GetLength () const; // always equal to total Add minus total Remove

    /*
        The chance that a node will splay to near the top of the tree.
        100 is a classic splay tree, while zero is a sorted binary tree. Numbers in between
        are used, along with the current height of the node, to decide whether to keep splaying.
        Which number works best depends on users usage, in particular who normalized the key requests are.
        A splay chance of 10% does a good job or guarding against randomly distributed requests. Very tightly bounded
        requests may want to reduce the splay chance further, to avoid unnecessary rotations near the top of the tree.
    */
    nonvirtual SplayType GetSplayType () const;
    nonvirtual void      SetSplayType (SplayType newSplayType);

    // for expert users. If you know the details of your distribution, you can set custom height weights optimal to usage. The weights
    // represent the chance in 10000 that a node at a height one greater than the weight index will splay. The last weight
    // is also used for any nodes with heights greater than the weight lists size.
    static const std::vector<size_t>& GetHeightWeights (SplayType st);
    static void                       SetCustomHeightWeights (const std::vector<size_t>& newHeightWeights);

public:
    struct Node {
        Node (const KeyType& key, const ValueType& val);
        Node (const Node& n);

        DECLARE_USE_BLOCK_ALLOCATION (Node);

        typename TRAITS::KeyValue fEntry;
        Node*                     fLeft;
        Node*                     fRight;
        Node*                     fParent;
    };
    Node* fHead;

    /*
        Find node with matching key in treap. In cases of duplicate values, return first found.
        Note that FindNode is const: it does not alter the tree structure. You can optionally pass in the height parameter, which returns
        the number of parent nodes of the found node. This can then be passed on to Splay if you wish to manually realter the tree.
     */
    nonvirtual Node* FindNode (const KeyType& key, int* comparisonResult, size_t* height = nullptr) const;

    /*
        These return the first and last entries in the tree (defined as the first and last entries that would be returned via
        iteration, assuming other users did not alter the tree. Similar to FindNode in that they do not alter the tree, and
        support an optional height argument. Note that these routines require no key compares, and are thus very fast.
    */
    nonvirtual Node* GetFirst (size_t* height = nullptr) const;
    nonvirtual Node* GetLast (size_t* height = nullptr) const;

    /*
        Potentially move the node nearer to the top of the tree. If specify forced always bring to top of tree. Otherwise movement is
        determined by SplayType, with eNeverSplay never moving the node, eAlwaysSplay always moving to the top, and the others rolling
        dice to decide how far, if any, to move up the node. Usually deeply nested nodes are more likely to move up than those that are
        already near the top of the tree.
     */
    nonvirtual void Splay (Node* n, size_t nodeHeight, bool forced = false);

    // force a node to be a leaf node via rotations, useful before delete
    nonvirtual size_t ForceToBottom (Node* n);

    // swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
    nonvirtual Node* Rotate (Node* n, bool left);

    nonvirtual void AddNode (Node* n);
    nonvirtual void RemoveNode (Node* n);

    nonvirtual void ReplaceWithChild (Node* parent, Node* child);

public:
#if qDebug
    nonvirtual void ListAll () const;
    static void     ValidateBranch (Node* n, size_t& count);
    nonvirtual void ValidateAll () const;
#endif

private:
    size_t    fLength;
    SplayType fSplayType;

#if qKeepADTStatistics
public:
    mutable size_t fCompares;
    mutable size_t fRotations;

    size_t CalcHeight (size_t* totalHeight = nullptr) const;
#endif

    static bool FlipCoin ();

    static Node* DuplicateBranch (Node* branchTop);

    static std::vector<size_t> sAlwaysSplayDistribution;
    static std::vector<size_t> sUniformDistribution;
    static std::vector<size_t> sNormalDistribution;
    static std::vector<size_t> sZipfDistribution;
    static std::vector<size_t> sCustomSplayTypeDistribution;

private:
    typedef std::mt19937 Engine;
    static Engine&       GetEngine ();
};

#include "SplayTree.inl"
