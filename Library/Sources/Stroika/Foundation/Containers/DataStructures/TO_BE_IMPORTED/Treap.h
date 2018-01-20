#pragma once

#include <vector>

#include "../Shared/Headers/BlockAllocated.h"
#include "../Shared/Headers/Config.h"

#include "../Shared/Headers/TreeTraits.h"

/*
    A treap is a sorted binary tree that is further constrained by assigning a (random) priority to each node and applying the heap constraint (parent nodes are always
    of higher priority than their children).

    Treap have the following desireable features
    fast find, add and remove. Add and remove require no more comparisons than find, although they do require additional tree rotations to maintain heap balance.

    robust about order of additions. There are possible pathological trees, but in practice no series of inputs would reliably reproduce (tree order not dependent on
    keys), and changes or worst case is statistically extremely unlikely.

    ability to add more than one entry with the same key

    ability to reorder links into  optimal configuration. Optimize call requires no compares and is done in O(n). It roughly halves the tree height, and reduces
    compares during find by about 20%. It reduces compares for cases of add and remove as well.

    ability to efficiently implement set interface (intersection, union and difference)

    possible to efficiently parallelize (see http://www.cs.cmu.edu/~scandal/papers/treaps-spaa98.pdf)

    ability to reorder tree based on find usage, so that frequently accessed items are found first (calling optimize will destroy this characteristic).

    support backwards and forwards iteration (technically support in-order, pre-order and post-order, plus backward iteration which is reverse in-order)

    Drawbacks:
    Treaps have a fair amount of overhead. They require at least 2 extra links per node for children, and often additional pointer to parent (not necessary but
    makes some routines simpler and easier to parallelize). In addition they have a priority value, also of size_t, so equivalent to a node link. Overhead is thus
    either 3 or 4 links per node, considerably higher than SkipList (1.33) or some other trees (Red-Black has only 2-3 links plus a bit or byte for color information)

    Red-Black tree should have better default find, and also less variance in time.

    Optimize On Find:
        If you know that your users are going to not do randomly distributed finds, setting this can produce find results that
        exceed a perfectly balanced tree. You specify a percentage chance that after a successful find we try to readjust
        the tree to make the found entry nearer the top (even if the decision is to modify the tree it will only do it some of the
        time, with nodes nearer the top already being less likely to move than leaf nodes).
        Empirical testing with some non-uniform finds showed fairly small degradation of find speed when given worst case, evenly
        distributed finds, and significant speedups in cases of tightly distributed finds (all finds on only 10% of the data);
        Note that using non-zero find optimization makes parallelizing much worse, as finds can modify the tree
 */

enum {
    kNoOptimizeFinds       = 0,
    kStandardOptimizeFinds = 5,
    kDefaultOptimizeFinds  = kNoOptimizeFinds,
};

template <typename KEYVALUE, typename COMP, int POLICY, int OPTFINDCHANCE>
struct TreapTraits : TreeTraits::Traits<KEYVALUE, COMP, POLICY> {
    static const size_t kOptimizeOnFindChance = OPTFINDCHANCE;
};

template <typename KEY,
          typename VALUE,
          typename TRAITS = TreapTraits<
              KeyValue<KEY, VALUE>,
              TreeTraits::DefaultComp<KEY>,
              TreeTraits::eInvalidRemoveThrowException,
              kDefaultOptimizeFinds>>
class Treap {
public:
    typedef KEY   KeyType;
    typedef VALUE ValueType;

public:
    Treap ();
    Treap (const Treap& t);
    ~Treap ();

    Treap& operator= (const Treap& t);

    /*
        Basic find operation. If pass in nullptr for val then only tests inclusion, otherwise fills val with value linked to key.
        In some cases (such as using a counter) you want full Node information rather than just the value -- see FindNode below for
        how to do this.
    */
    bool Find (const KeyType& key, ValueType* val = nullptr) const;

    /*
        You can add more than one item with the same key. If you add different values with the same key, it is unspecified which item will be returned on subsequent Find or Remove calls.
    */
    void Add (const KeyType& key, ValueType val);

    void Remove (const KeyType& key);
    void RemoveAll ();

    size_t GetLength () const; // always equal to total Add minus total Remove

    void Optimize ();

    size_t GetFindOptimizeChance () const; // percent change zero-100

public:
    struct Node {
        Node (const KeyType& key, const ValueType& val);
        Node (const Node& n);

        DECLARE_USE_BLOCK_ALLOCATION (Node);

        size_t                    fPriority;
        typename TRAITS::KeyValue fEntry;
        Node*                     fLeft;
        Node*                     fRight;
        Node*                     fParent;
    };
    Node* fHead;

    /*
        Find closest node for key in treap. In cases of duplicate values, return first found.
        If not found return the node that is smaller than key by the least amount. For cases that did
        not match, the returned node will always an external node (at least one nullptr leaf). Returns
        nullptr if tree is empty.
     */
    Node* FindNode (const KeyType& key, int* comparisonResult) const;

    /*
        These return the first and last entries in the tree (defined as the first and last entries that would be returned via
        iteration, assuming other users did not alter the tree.  Note that these routines require no key compares, and are thus very fast.
    */
    nonvirtual Node* GetFirst () const;
    nonvirtual Node* GetLast () const;

    /*
        After insertions, a node will be in correct sort order, but not in correct heap order (parents must have
        higher priority than their children. This routine will do node rotations upwards until the tree is in proper heap order
    */
    void Prioritize (Node* n);

    // the symmetrical case for removal, here we force to bottom of tree before removing, using rotations to keep tree in proper heap order
    void ForceToBottom (Node* n);

    // swap places of left or right child with n. A left rotation makes the right child the new parent, and a right rotation makes the left child the new parent
    Node* Rotate (Node* n, bool left);

    static Node* DuplicateBranch (Node* branchTop);

    void AddNode (Node* n);
    void RemoveNode (Node* n);

public:
#if qDebug
    void        ListAll () const;
    static void ValidateBranch (Node* n, size_t& count);
    void        ValidateAll () const;
#endif

private:
    size_t fLength;
    //      size_t  fFindOptimizeChance;

#if qKeepADTStatistics
public:
    mutable size_t fCompares;
    mutable size_t fRotations;

    size_t        CalcHeight (size_t* totalHeight = nullptr) const;
    static size_t CalcNodeHeight (Node* n, size_t height, size_t* totalHeight);
#endif
};

#include "Treap.inl"
