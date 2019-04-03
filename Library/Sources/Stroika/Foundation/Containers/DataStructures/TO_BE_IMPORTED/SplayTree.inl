#pragma once

#include <algorithm>
#include <stack>

#if qDebug
#include <iostream>
#endif

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY, VALUE, TRAITS>::SplayTree ()
    : fHead (nullptr)
    , fLength (0)
    , fSplayType (::eDefaultSplayType)
#if qKeepADTStatistics
    , fCompares (0)
    , fRotations (0)
#endif
{
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY, VALUE, TRAITS>::SplayTree (const SplayTree& t)
    : fHead (nullptr)
    , fLength (t.fLength)
    , fSplayType (t.fSplayType)
#if qKeepADTStatistics
    , fCompares (t.fCompares)
    , fRotations (t.fRotations)
#endif
{
    fHead = DuplicateBranch (t.fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY, VALUE, TRAITS>& SplayTree<KEY, VALUE, TRAITS>::operator= (const SplayTree& t)
{
    RemoveAll ();
    fLength = t.fLength;
    fHead   = DuplicateBranch (t.fHead);

    return *this;
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY, VALUE, TRAITS>::~SplayTree ()
{
    RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
bool SplayTree<KEY, VALUE, TRAITS>::Find (const KeyType& key, ValueType* val)
{
    int    comparisonResult;
    size_t height;
    Node*  n = FindNode (key, &comparisonResult, &height);
    if (n != nullptr) {
        if (comparisonResult == 0) {
            if (val != nullptr) {
                *val = n->fEntry.GetValue ();
            }
            Splay (n, height);

            return true;
        }
        else {
            // maybe splay the one we found? Seems to work poorly in practice
            //Splay (n, height);
        }
    }
    return false;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY, VALUE, TRAITS>::Node* SplayTree<KEY, VALUE, TRAITS>::Rotate (Node* n, bool left)
{
    RequireNotNull (n);

#if qKeepADTStatistics
    ++fRotations;
#endif

    Node* newTop = (left) ? n->fRight : n->fLeft;
    RequireNotNull (newTop);

    if (n->fParent == nullptr) {
        Assert (n == fHead);
        fHead          = newTop;
        fHead->fParent = nullptr;
    }
    else {
        if (n->fParent->fLeft == n) {
            n->fParent->fLeft = newTop;
        }
        else {
            Assert (n->fParent->fRight == n);
            n->fParent->fRight = newTop;
        }
    }

    newTop->fParent = n->fParent;
    n->fParent      = newTop;

    if (left) {
        n->fRight = newTop->fLeft;
        if (n->fRight != nullptr) {
            n->fRight->fParent = n;
        }
        newTop->fLeft = n;
    }
    else {
        n->fLeft = newTop->fRight;
        if (n->fLeft != nullptr) {
            n->fLeft->fParent = n;
        }
        newTop->fRight = n;
    }
    return newTop;
}

template <typename KEY, typename VALUE, typename TRAITS>
const std::vector<size_t>& SplayTree<KEY, VALUE, TRAITS>::GetHeightWeights (SplayType st)
{
    switch (st) {
        case ::eAlwaysSplay:
            return sAlwaysSplayDistribution;
        case ::eUniformDistribution:
            return sUniformDistribution;
        case ::eNormalDistribution:
            return sNormalDistribution;
        case ::eZipfDistribution:
            return sZipfDistribution;
        case ::eCustomSplayType:
            return sCustomSplayTypeDistribution;
        default:
            AssertNotReached ();
    }
    AssertNotReached ();
    return sCustomSplayTypeDistribution;
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::SetCustomHeightWeights (const std::vector<size_t>& newHeightWeights)
{
    sCustomSplayTypeDistribution = newHeightWeights;
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::Splay (Node* n, size_t nodeHeight, bool forced)
{
    RequireNotNull (n);

    if (fSplayType == ::eNeverSplay) {
        return;
    }
    else if (fSplayType == ::eAlwaysSplay) {
        forced = true;
    }

    //  static  std::tr1::uniform_int<size_t> sDist (1, 10000);

    const std::vector<size_t>& kHeightBonus = GetHeightWeights (fSplayType);
    //  size_t  dieRoll = (forced) ? 1 : std::tr1::uniform_int<size_t> (1, 10000) (GetEngine ());
    size_t dieRoll = (forced) ? 1 : rand () % 10000;

    /*
        Move upwards in the tree. In classic splay tree, move all the way to the top.
        Splay trees do a slightly more complicated action than simple rotations. Whenever possible it does two rotations
        at once: rotating the parent node and the grandparent node, making the node the new grandparent.
        In cases where the node being played is on the same side of its parent as it's parent is to its grandparent,
        if first rotates the grandparent, then the parent. If on opposite sides, it does the normal rotation sequence (parent, then grandparent);
        It only does a single rotation if the node has no grandparent (its parent is the head)
     */
    while (n->fParent != nullptr) {
        Assert (n->fParent->fLeft == n or n->fParent->fRight == n);

        Node* ancestor = n->fParent->fParent;
        if (ancestor == nullptr) {
            if (not forced) {
                Assert (nodeHeight > 0);
                --nodeHeight;
                if ((nodeHeight < kHeightBonus.size ()) and (dieRoll > kHeightBonus[nodeHeight])) {
                    return;
                }
            }
            Rotate (n->fParent, (n->fParent->fRight == n));
        }
        else {
            if (not forced) {
                Assert (nodeHeight > 1);
                nodeHeight -= 2;
                size_t cutoff = (nodeHeight >= kHeightBonus.size ()) ? kHeightBonus[kHeightBonus.size () - 1] : kHeightBonus[nodeHeight];
                if (dieRoll >= cutoff) {
                    return;
                }
            }
            Node* parent = n->fParent;
            if ((parent->fLeft == n and ancestor->fLeft == parent) or (parent->fRight == n and ancestor->fRight == parent)) {
                // zig-zig
                bool left = (parent->fRight == n);
                Rotate (ancestor, left);
                Rotate (parent, left);
            }
            else {
                // zig-zag
                Rotate (n->fParent, (parent->fRight == n));
                Assert (ancestor->fLeft == n or ancestor->fRight == n);
                Rotate (ancestor, (ancestor->fRight == n));
            }
        }
    }
    Ensure ((n->fParent == nullptr) == (fHead == n));
    Ensure ((n->fParent == nullptr) or (n->fParent->fLeft == n) or (n->fParent->fRight == n));
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t SplayTree<KEY, VALUE, TRAITS>::ForceToBottom (Node* n)
{
    RequireNotNull (n);
    size_t rotations = 0;
    while (n->fLeft != nullptr or n->fRight != nullptr) {
        Rotate (n, (n->fLeft == nullptr) or (n->fRight != nullptr and FlipCoin ()));
        ++rotations;
    }

    Ensure (n->fLeft == nullptr and n->fRight == nullptr);
    Ensure (fHead->fParent == nullptr);

    return rotations;
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::Add (const KeyType& key, const ValueType& val)
{
    AddNode (new Node (key, val));
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::Add (const KeyType& keyAndValue)
{
    AddNode (new Node (keyAndValue, keyAndValue));
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::AddNode (Node* n)
{
    RequireNotNull (n);

    int    comp;
    size_t height;
    Node*  nearest = FindNode (n->fEntry.GetKey (), &comp, &height);
    if (nearest == nullptr) {
        Assert (fHead == nullptr);
        fHead = n;
    }
    else {
        n->fParent = nearest;
        if (comp == 0) {
            if (FlipCoin ()) {
                n->fLeft = nearest->fLeft;
                if (n->fLeft != nullptr) {
                    n->fLeft->fParent = n;
                }
                nearest->fLeft = n;
            }
            else {
                n->fRight = nearest->fRight;
                if (n->fRight != nullptr) {
                    n->fRight->fParent = n;
                }
                nearest->fRight = n;
            }
            height++;
        }
        else if (comp < 0) {
            Assert (nearest->fLeft == nullptr);
            nearest->fLeft = n;
        }
        else {
            Assert (nearest->fRight == nullptr);
            nearest->fRight = n;
        }

        Splay (n, height);
    }

    fLength++;
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::Remove (const KeyType& key)
{
    int   comp;
    Node* n = FindNode (key, &comp, nullptr);

    if ((n == nullptr) or (comp != 0)) {
        if (TRAITS::kPolicy & TreeTraits::eInvalidRemoveThrowException) {
            throw "attempt to remove missing item"; // need proper set of exceptions
        }
    }
    else {
        Assert (n->fEntry.GetKey () == key);
        RemoveNode (n);
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::ReplaceWithChild (Node* parent, Node* child)
{
    RequireNotNull (parent);

    Node* ancestor = parent->fParent;
    if (ancestor == nullptr) {
        Assert (fHead == parent);
        fHead = child;
    }
    else if (ancestor->fLeft == parent) {
        ancestor->fLeft = child;
    }
    else {
        ancestor->fRight = child;
    }
    if (child != nullptr) {
        child->fParent = ancestor;
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::RemoveNode (Node* n)
{
    RequireNotNull (n);

    Splay (n, 0, true);
    Assert (n == fHead);

    if (n->fLeft == nullptr and n->fRight == nullptr) {
        fHead = nullptr;
    }
    else if (n->fLeft == nullptr) {
        ReplaceWithChild (n, n->fRight);
    }
    else if (n->fRight == nullptr) {
        ReplaceWithChild (n, n->fLeft);
    }
    else {
        // quick cheat: GetFirst always looks from fHead, but we want to find the smallest node of our right branch
        fHead         = n->fRight;
        Node* minNode = GetFirst (nullptr);
        fHead         = n;

        AssertNotNull (minNode);
        if (minNode->fParent != n) {
            ReplaceWithChild (minNode, minNode->fRight);
            minNode->fRight = n->fRight;
            AssertNotNull (minNode->fRight);
            minNode->fRight->fParent = minNode;
        }
        ReplaceWithChild (n, minNode);
        minNode->fLeft = n->fLeft;
        AssertNotNull (minNode->fLeft);
        minNode->fLeft->fParent = minNode;
    }

    delete n;
    --fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::RemoveAll ()
{
    // iterate rather than natural tail recursive version because splay trees get deep
    std::stack<Node*> nodes;
    if (fHead != nullptr) {
        nodes.push (fHead);
        while (not nodes.empty ()) {
            Node* curNode = nodes.top ();
            nodes.pop ();

            AssertNotNull (curNode);
            if (curNode->fLeft != nullptr) {
                nodes.push (curNode->fLeft);
            }
            if (curNode->fRight != nullptr) {
                nodes.push (curNode->fRight);
            }
            delete curNode;
        }

        fHead = nullptr;
    }
    fLength = 0;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t SplayTree<KEY, VALUE, TRAITS>::GetLength () const
{
    Assert ((fLength == 0) == (fHead == nullptr));
    return fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY, VALUE, TRAITS>::Node* SplayTree<KEY, VALUE, TRAITS>::FindNode (const KeyType& key, int* comparisonResult, size_t* height) const
{
    RequireNotNull (comparisonResult);

    Node* n       = fHead;
    Node* nearest = n;
    if (height != nullptr) {
        *height = 0;
    }
    *comparisonResult = 0;
    while (n != nullptr) {
#if qKeepADTStatistics
        const_cast<SplayTree<KEY, VALUE, TRAITS>*> (this)->fCompares++;
#endif
        if (height != nullptr) {
            *height += 1;
        }
        nearest           = n;
        *comparisonResult = TRAITS::Comparer::Compare (key, n->fEntry.GetKey ());
        if (*comparisonResult == 0) {
            return n;
        }
        n = (*comparisonResult < 0) ? n->fLeft : n->fRight;
    }
    return nearest;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY, VALUE, TRAITS>::Node* SplayTree<KEY, VALUE, TRAITS>::GetFirst (size_t* height) const
{
    Node* n = fHead;
    if (height != nullptr) {
        *height = 0;
    }
    while (n->fLeft != nullptr) {
        if (height != nullptr) {
            *height += 1;
        }
        n = n->fLeft;
    }
    return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY, VALUE, TRAITS>::Node* SplayTree<KEY, VALUE, TRAITS>::GetLast (size_t* height) const
{
    Node* n = fHead;
    if (height != nullptr) {
        *height = 0;
    }
    while (n->fRight != nullptr) {
        if (height != nullptr) {
            *height += 1;
        }
        n = n->fRight;
    }
    return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayType SplayTree<KEY, VALUE, TRAITS>::GetSplayType () const
{
    return fSplayType;
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::SetSplayType (SplayType newSplayType)
{
    fSplayType = newSplayType;
}

template <typename KEY, typename VALUE, typename TRAITS>
bool SplayTree<KEY, VALUE, TRAITS>::FlipCoin ()
{
    static size_t sCounter = 0;
    return (++sCounter & 1);
}

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY, VALUE, TRAITS>::Node* SplayTree<KEY, VALUE, TRAITS>::DuplicateBranch (Node* n)
{
    // sadly, SplayTrees get very deep, so they hate recursion

    std::stack<Node*> nodes;
    std::stack<Node*> parents;
    std::stack<bool>  childIsLeft;

    Node* newTop = nullptr;

    nodes.push (n);
    parents.push (nullptr);
    childIsLeft.push (true);
    while (not nodes.empty ()) {
        Node* branchTop = nodes.top ();
        nodes.pop ();
        Node* newParent = parents.top ();
        parents.pop ();
        bool isLeft = childIsLeft.top ();
        childIsLeft.pop ();
        if (branchTop != nullptr) {
            Node* newNode    = new Node (*branchTop);
            newNode->fParent = newParent;
            if (newParent == nullptr) {
                Assert (newTop == nullptr);
                newTop = newNode;
            }
            else {
                if (isLeft) {
                    newParent->fLeft = newNode;
                }
                else {
                    newParent->fRight = newNode;
                }
            }
            if (branchTop->fLeft != nullptr) {
                nodes.push (branchTop->fLeft);
                parents.push (newNode);
                childIsLeft.push (true);
            }
            if (branchTop->fRight != nullptr) {
                nodes.push (branchTop->fRight);
                parents.push (newNode);
                childIsLeft.push (false);
            }
        }
    }
    return newTop;
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY, VALUE, TRAITS>::Node::Node (const KeyType& key, const ValueType& val)
    : fEntry (key, val)
    , fLeft (nullptr)
    , fRight (nullptr)
    , fParent (nullptr)
{
}

template <typename KEY, typename VALUE, typename TRAITS>
SplayTree<KEY, VALUE, TRAITS>::Node::Node (const Node& n)
    : fEntry (n.fEntry)
    , fLeft (nullptr)
    , fRight (nullptr)
    , fParent (nullptr)
{
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::ValidateBranch (Node* n, size_t& count)
{
    RequireNotNull (n);
    ++count;
    if (n->fLeft != nullptr) {
        Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->fLeft->fEntry.GetKey ()) >= 0);
        Assert (n->fLeft->fParent == n);
        ValidateBranch (n->fLeft, count);
    }
    if (n->fRight != nullptr) {
        // we cannot do strict < 0, because rotations can put on either side
        Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->fRight->fEntry.GetKey ()) <= 0);
        Assert (n->fRight->fParent == n);
        ValidateBranch (n->fRight, count);
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::ValidateAll () const
{
    size_t count = 0;

    if (fHead != nullptr) {
        ValidateBranch (fHead, count);
    }
    Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void SplayTree<KEY, VALUE, TRAITS>::ListAll () const
{
    std::function<void (Node*)> ListNode = [&ListNode] (Node* n) {
        if (n->fLeft != nullptr) {
            ListNode (n->fLeft);
        }
        std::cout << "(" << n->fEntry.GetKey () << "," << n->fPriority << ")";
        if (n->fRight != nullptr) {
            ListNode (n->fRight);
        }
    };

    std::cout << "[";
    if (fHead != nullptr) {
        ListNode (fHead);
    }
    std::cout << "]" << std::endl;
}

#endif

#if qKeepADTStatistics
template <typename KEY, typename VALUE, typename TRAITS>
size_t SplayTree<KEY, VALUE, TRAITS>::CalcHeight (size_t* totalHeight) const
{
    size_t maxHeight = 0;

    std::stack<Node*>  nodes;
    std::stack<size_t> heights;
    nodes.push (fHead);
    heights.push (0);
    while (not nodes.empty ()) {
        Node* curNode = nodes.top ();
        nodes.pop ();
        size_t height = heights.top ();
        heights.pop ();
        if (curNode == nullptr) {
            if (totalHeight != nullptr) {
                *totalHeight += height;
            }
            maxHeight = std::max (maxHeight, height);
        }
        else {
            heights.push (height + 1);
            nodes.push (curNode->fLeft);
            heights.push (height + 1);
            nodes.push (curNode->fRight);
        }
    }
    return maxHeight;
}
#endif

const size_t kAlwaysWeights[] = {10000};
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t> SplayTree<KEY, VALUE, TRAITS>::sAlwaysSplayDistribution (kAlwaysWeights, kAlwaysWeights + sizeof (kAlwaysWeights) / sizeof (kAlwaysWeights[0]));

const size_t kUniformWeights[] = {10, 50, 50, 100, 175, 350, 675};
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t> SplayTree<KEY, VALUE, TRAITS>::sUniformDistribution (kUniformWeights, kUniformWeights + sizeof (kUniformWeights) / sizeof (kUniformWeights[0]));

//const size_t kNormalWeights[] ={0, 0, 100, 100, 250, 250, 250, 250, 250, 250, 250};   //30.1401/30.0162/21.5254
const size_t kNormalWeights[] = {1, 3, 10, 12, 12, 12, 12, 12, 17, 17, 43};
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t> SplayTree<KEY, VALUE, TRAITS>::sNormalDistribution (kNormalWeights, kNormalWeights + sizeof (kNormalWeights) / sizeof (kNormalWeights[0]));

//const size_t kZifpWeights[] = {0, 5, 15, 30, 30, 60, 60, 125, 125, 250, 250};
const size_t kZifpWeights[] = {1, 1, 11, 30, 30, 60, 66, 62, 174, 261, 278, 278, 278, 278};
template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t> SplayTree<KEY, VALUE, TRAITS>::sZipfDistribution (kZifpWeights, kZifpWeights + sizeof (kZifpWeights) / sizeof (kZifpWeights[0]));

template <typename KEY, typename VALUE, typename TRAITS>
std::vector<size_t> SplayTree<KEY, VALUE, TRAITS>::sCustomSplayTypeDistribution (SplayTree<KEY, VALUE, TRAITS>::sUniformDistribution);

template <typename KEY, typename VALUE, typename TRAITS>
typename SplayTree<KEY, VALUE, TRAITS>::Engine& SplayTree<KEY, VALUE, TRAITS>::GetEngine ()
{
    static std::mt19937 sEngine;
    static bool         sFirstTime = true;
    if (sFirstTime) {
        sFirstTime = false;
        sEngine.seed (static_cast<unsigned int> (time (NULL)));
    }
    return sEngine;
}
