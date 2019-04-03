#pragma once

#include <algorithm>
#include <math.h>

#if qDebug
#include <iostream>
#endif

#include "../Shared/Headers/Utils.h"

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY, VALUE, TRAITS>::Treap ()
    : fHead (nullptr)
    , fLength (0)
#if qKeepADTStatistics
    , fCompares (0)
    , fRotations (0)
#endif
{
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY, VALUE, TRAITS>::Treap (const Treap& t)
    : fHead (nullptr)
    , fLength (t.fLength)
#if qKeepADTStatistics
    , fCompares (t.fCompares)
    , fRotations (t.fRotations)
#endif
{
    Require (TRAITS::kOptimizeOnFindChance >= 0);   // is there a way to catch at compile time?
    Require (TRAITS::kOptimizeOnFindChance <= 100); // is there a way to catch at compile time?

    fHead = DuplicateBranch (t.fHead);
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY, VALUE, TRAITS>::~Treap ()
{
    RemoveAll ();
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY, VALUE, TRAITS>& Treap<KEY, VALUE, TRAITS>::operator= (const Treap& t)
{
    RemoveAll ();
    fHead   = DuplicateBranch (t.fHead);
    fLength = t.fLength;
    return *this;
}

template <typename KEY, typename VALUE, typename TRAITS>
bool Treap<KEY, VALUE, TRAITS>::Find (const KeyType& key, ValueType* val) const
{
    int   comp;
    Node* n = FindNode (key, &comp);
    if (n != nullptr and (comp == 0)) {
        if (val != nullptr) {
            *val = n->fEntry.GetValue ();
        }

        if ((TRAITS::kOptimizeOnFindChance > 0) and (RandomSize_t (1, 100) <= TRAITS::kOptimizeOnFindChance)) {
            // still only move if get higher priority
            size_t newPriority = RandomSize_t ();
            if (newPriority > n->fPriority) {
                n->fPriority = newPriority;
                const_cast<Treap<KEY, VALUE, TRAITS>*> (this)->Prioritize (n);
            }
        }
        return true;
    }
    return false;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY, VALUE, TRAITS>::Node* Treap<KEY, VALUE, TRAITS>::Rotate (Node* n, bool left)
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
void Treap<KEY, VALUE, TRAITS>::Prioritize (Node* n)
{
    RequireNotNull (n);

#define qUseSplayStyleZigZigWhenCan 0

#if qUseSplayStyleZigZigWhenCan
    auto OtherChild = [] (Node* parent, Node* child) -> Node* {
        RequireNotNull (parent);
        RequireNotNull (child);
        Require (parent->fLeft == child or parent->fRight == child);
        return ((parent->fLeft == child) ? parent->fRight : parent->fLeft);
    };
#endif

    while (n->fParent != nullptr and (n->fParent->fPriority < n->fPriority)) {
#if qUseSplayStyleZigZigWhenCan
        Node* ancestor = n->fParent->fParent;
        if (ancestor != nullptr and (ancestor->fPriority < n->fPriority)) {
            Node* parent = n->fParent;
            if ((parent->fLeft == n and ancestor->fLeft == parent) or (parent->fRight == n and ancestor->fRight == parent)) {
                Node* otherChild = OtherChild (ancestor, parent);
                if (otherChild != nullptr and parent->fPriority >= otherChild->fPriority) {
                    // zig-zig
                    std::swap (parent->fPriority, ancestor->fPriority);
                    bool left = (parent->fRight == n);
                    Rotate (ancestor, left);
                    Rotate (parent, left);
                    continue;
                }
            }
        }
#endif

        Assert (n->fParent->fLeft == n or n->fParent->fRight == n);
        Rotate (n->fParent, (n->fParent->fRight == n));
    }
    Ensure ((n->fParent == nullptr) == (fHead == n));
    Ensure ((n->fParent == nullptr) or (n->fParent->fLeft == n) or (n->fParent->fRight == n));
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::Add (const KeyType& key, ValueType val)
{
    Node* n      = new Node (key, val);
    n->fPriority = RandomSize_t ();
    AddNode (n);
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::AddNode (Node* n)
{
    RequireNotNull (n);

    int   comp;
    Node* nearest = FindNode (n->fEntry.GetKey (), &comp);
    if (nearest == nullptr) {
        Assert (fHead == nullptr);
        fHead = n;
    }
    else {
        n->fParent = nearest;
        if (comp == 0) {
            n->fLeft       = nearest->fLeft;
            nearest->fLeft = n;
            ForceToBottom (n);
        }
        else if (comp < 0) {
            Assert (nearest->fLeft == nullptr);
            nearest->fLeft = n;
        }
        else {
            Assert (nearest->fRight == nullptr);
            nearest->fRight = n;
        }
        Prioritize (n);
    }

    fLength++;
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::ForceToBottom (Node* n)
{
    RequireNotNull (n);
    while (n->fLeft != nullptr or n->fRight != nullptr) {
        Rotate (n, (n->fLeft == nullptr) or ((n->fRight != nullptr) and (n->fLeft->fPriority <= n->fRight->fPriority)));
    }

    Ensure (fHead->fParent == nullptr);
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::Remove (const KeyType& key)
{
    int   comp;
    Node* n = FindNode (key, &comp);

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
void Treap<KEY, VALUE, TRAITS>::RemoveNode (Node* n)
{
    RequireNotNull (n);
    // we need to move it to the bottom of the tree, and only then remove it. Thus we keep
    // priorities in proper order
    ForceToBottom (n);
    if (n->fParent != nullptr) {
        if (n->fParent->fLeft == n) {
            n->fParent->fLeft = nullptr;
        }
        else {
            Assert (n->fParent->fRight == n);
            n->fParent->fRight = nullptr;
        }
    }
    else {
        Assert (fHead == n);
        Assert (fLength == 1);
        fHead = nullptr;
    }
    delete n;
    --fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::RemoveAll ()
{
    std::function<void (Node*)> DeleteANode = [&DeleteANode] (Node* n) {
        if (n != nullptr) {
            DeleteANode (n->fLeft);
            DeleteANode (n->fRight);
            delete n;
        }
    };

    DeleteANode (fHead);

    fHead   = nullptr;
    fLength = 0;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t Treap<KEY, VALUE, TRAITS>::GetLength () const
{
    Assert ((fLength == 0) == (fHead == nullptr));
    return fLength;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY, VALUE, TRAITS>::Node* Treap<KEY, VALUE, TRAITS>::FindNode (const KeyType& key, int* comparisonResult) const
{
    RequireNotNull (comparisonResult);

    Node* n       = fHead;
    Node* nearest = n;
    while (n != nullptr) {
#if qKeepADTStatistics
        const_cast<Treap<KEY, VALUE, TRAITS>*> (this)->fCompares++;
#endif
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
typename Treap<KEY, VALUE, TRAITS>::Node* Treap<KEY, VALUE, TRAITS>::GetFirst () const
{
    Node* n = fHead;
    while (n->fLeft != nullptr) {
        n = n->fLeft;
    }
    return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY, VALUE, TRAITS>::Node* Treap<KEY, VALUE, TRAITS>::GetLast () const
{
    Node* n = fHead;
    while (n->fRight != nullptr) {
        n = n->fRight;
    }
    return n;
}

template <typename KEY, typename VALUE, typename TRAITS>
typename Treap<KEY, VALUE, TRAITS>::Node* Treap<KEY, VALUE, TRAITS>::DuplicateBranch (Node* branchTop)
{
    Node* newNode = nullptr;
    if (branchTop != nullptr) {
        newNode        = new Node (*branchTop);
        newNode->fLeft = DuplicateBranch (branchTop->fLeft);
        if (newNode->fLeft != nullptr) {
            newNode->fLeft->fParent = newNode;
        }
        newNode->fRight = DuplicateBranch (branchTop->fRight);
        if (newNode->fRight != nullptr) {
            newNode->fRight->fParent = newNode;
        }
    }
    return newNode;
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::Optimize ()
{
    // better to build on the stack
    Node** nodeList = new Node*[GetLength ()];
    int    curIndex = 0;

    // stuff the array with the nodes. Better if have iterator support
    std::function<void (Node*)> AssignNodeToArray = [&AssignNodeToArray, &nodeList, &curIndex] (Node* n) {
        if (n->fLeft != nullptr) {
            AssignNodeToArray (n->fLeft);
        }
        nodeList[curIndex++] = n;
        if (n->fRight != nullptr) {
            AssignNodeToArray (n->fRight);
        }
    };

    AssignNodeToArray (fHead);

    // from now on, working with an array (nodeList) that has all the tree nodes in sorted order
    size_t kMaxPriority = size_t (-1);
    size_t maxHeight    = size_t (log (double (GetLength ())) / log (2.0) + .5) + 1;
    size_t bucketSize   = kMaxPriority / maxHeight;

    std::function<Node*(int startIndex, int endIndex)> Balance = [&Balance, &nodeList, &bucketSize, &maxHeight] (int startIndex, int endIndex) -> Node* {
        Require (startIndex <= endIndex);
        if (startIndex == endIndex) {
            Node* n      = nodeList[startIndex];
            n->fLeft     = nullptr;
            n->fRight    = nullptr;
            n->fPriority = 1;
            return n;
        }

        int curIdx = startIndex + (endIndex - startIndex) / 2;
        Assert (curIdx <= endIndex);
        Assert (curIdx >= startIndex);

        Node* n = nodeList[curIdx];
        AssertNotNull (n);

        size_t maxPriority = 0;
        if (curIdx == startIndex) {
            n->fLeft = nullptr;
        }
        else {
            n->fLeft    = Balance (startIndex, curIdx - 1);
            maxPriority = n->fLeft->fPriority;
            Assert (maxHeight > maxPriority);
            n->fLeft->fPriority = RandomSize_t (bucketSize * (n->fLeft->fPriority - 1), bucketSize * (n->fLeft->fPriority) - 1);
            n->fLeft->fParent   = n;
            Assert (n->fLeft->fLeft == nullptr or n->fLeft->fPriority > n->fLeft->fLeft->fPriority);
            Assert (n->fLeft->fRight == nullptr or n->fLeft->fPriority > n->fLeft->fRight->fPriority);
        }
        if (curIdx == endIndex) {
            n->fRight = nullptr;
        }
        else {
            n->fRight   = Balance (curIdx + 1, endIndex);
            maxPriority = std::max (maxPriority, n->fRight->fPriority);
            Assert (maxHeight > maxPriority);
            n->fRight->fPriority = RandomSize_t (bucketSize * (n->fRight->fPriority - 1), bucketSize * (n->fRight->fPriority) - 1);
            n->fRight->fParent   = n;
            Assert (n->fRight->fLeft == nullptr or n->fRight->fPriority > n->fRight->fLeft->fPriority);
            Assert (n->fRight->fRight == nullptr or n->fRight->fPriority > n->fRight->fRight->fPriority);
        }

        n->fPriority = 1 + maxPriority; // use priority to track height while building

        return n;
    };
    if (fHead != nullptr) {
        fHead            = Balance (0, GetLength () - 1);
        fHead->fPriority = RandomSize_t (bucketSize * (maxHeight - 1));
        Assert (fHead->fLeft == nullptr or fHead->fPriority > fHead->fLeft->fPriority);
        Assert (fHead->fRight == nullptr or fHead->fPriority > fHead->fRight->fPriority);
        fHead->fParent = nullptr;
    }

    delete[] nodeList;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t Treap<KEY, VALUE, TRAITS>::GetFindOptimizeChance () const
{
    return TRAITS::kOptimizeOnFindChance;
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY, VALUE, TRAITS>::Node::Node (const KeyType& key, const ValueType& val)
    : fPriority (0)
    , fEntry (key, val)
    , fLeft (nullptr)
    , fRight (nullptr)
    , fParent (nullptr)
{
}

template <typename KEY, typename VALUE, typename TRAITS>
Treap<KEY, VALUE, TRAITS>::Node::Node (const Node& n)
    : fPriority (n.fPriority)
    , fEntry (n.fEntry)
    , fLeft (nullptr)
    , fRight (nullptr)
    , fParent (nullptr)
{
}

#if qDebug

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::ValidateBranch (Node* n, size_t& count)
{
    RequireNotNull (n);
    ++count;
    if (n->fLeft != nullptr) {
        Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->fLeft->fEntry.GetKey ()) >= 0);
        Assert (n->fPriority >= n->fLeft->fPriority);
        Assert (n->fLeft->fParent == n);
        ValidateBranch (n->fLeft, count);
    }
    if (n->fRight != nullptr) {
        // we cannot do strict < 0, because rotations can put on either side
        Assert (TRAITS::Comparer::Compare (n->fEntry.GetKey (), n->fRight->fEntry.GetKey ()) <= 0);
        Assert (n->fPriority >= n->fRight->fPriority);
        Assert (n->fRight->fParent == n);
        ValidateBranch (n->fRight, count);
    }
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::ValidateAll () const
{
    size_t count = 0;

    if (fHead != nullptr) {
        ValidateBranch (fHead, count);
    }
    Assert (count == fLength);
}

template <typename KEY, typename VALUE, typename TRAITS>
void Treap<KEY, VALUE, TRAITS>::ListAll () const
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
size_t Treap<KEY, VALUE, TRAITS>::CalcNodeHeight (Node* n, size_t height, size_t* totalHeight)
{
    if (n == nullptr) {
        if (totalHeight != nullptr) {
            *totalHeight += height;
        }
        return height;
    }

    size_t newHeight = std::max (
        CalcNodeHeight (n->fLeft, height + 1, totalHeight),
        CalcNodeHeight (n->fRight, height + 1, totalHeight));

    return newHeight;
}

template <typename KEY, typename VALUE, typename TRAITS>
size_t Treap<KEY, VALUE, TRAITS>::CalcHeight (size_t* totalHeight) const
{
    return CalcNodeHeight (fHead, 0, totalHeight);
}

#endif
