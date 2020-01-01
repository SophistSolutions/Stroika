/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_SkipList_inl_
#define _Stroika_Foundation_Containers_DataStructures_SkipList_inl_ 1

#include "../../Debug/Assertions.h"

namespace Stroika::Foundation::Containers::DataStructures {

    template <typename KEY, typename VALUE, typename TRAITS>
    SkipList<KEY, VALUE, TRAITS>::Node::Node (const KeyType& key, const ValueType& val)
        : fEntry (key, val)
    {
    }

    template <typename KEY, typename VALUE, typename TRAITS>
    SkipList<KEY, VALUE, TRAITS>::SkipList ()
        : fLength (0)
#if qKeepADTStatistics
        , fCompares (0)
        , fRotations (0)
#endif
    {
        GrowHeadLinksIfNeeded (1, nullptr);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    SkipList<KEY, VALUE, TRAITS>::SkipList (const SkipList& s)
        : fLength (0)
#if qKeepADTStatistics
        , fCompares (0)
        , fRotations (0)
#endif
    {
        operator= (s);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    SkipList<KEY, VALUE, TRAITS>& SkipList<KEY, VALUE, TRAITS>::operator= (const SkipList& t)
    {
        RemoveAll ();
        if (t.GetLength () != 0) {
            Node* prev = nullptr;
            Node* n    = (t.fHead.size () == 0) ? nullptr : t.fHead[0];
            while (n != nullptr) {
                Node* newNode = new Node (n->fEntry.GetKey (), n->fEntry.GetValue ());
                if (prev == nullptr) {
                    Assert (fHead.size () == 1);
                    Assert (fHead[0] == nullptr);
                    fHead[0] = newNode;
                }
                else {
                    prev->fNext.push_back (newNode);
                }
                prev = newNode;
                n    = n->fNext[0];
            }
            AssertNotNull (prev);
            Assert (prev->fNext.size () == 0);
            prev->fNext.push_back (nullptr);

            fLength = t.fLength;
            ReBalance (); // this will give us a proper link structure
        }
        return *this;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    SkipList<KEY, VALUE, TRAITS>::~SkipList ()
    {
        RemoveAll ();
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline size_t SkipList<KEY, VALUE, TRAITS>::GetLinkHeightProbability ()
    {
        return 25;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    inline size_t SkipList<KEY, VALUE, TRAITS>::GetLength () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fLength;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    typename SkipList<KEY, VALUE, TRAITS>::Node* SkipList<KEY, VALUE, TRAITS>::FindNode (const KeyType& key) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Assert (fHead.size () > 0);

        vector<Node*> const* startV = &fHead;
        for (size_t linkHeight = fHead.size (); linkHeight > 0; --linkHeight) {
            Node* n = (*startV)[linkHeight - 1];

            // tweak to use pointer comparisons rather than key field compares. We know any link heigher than the current link being
            // tested must point past the key we are looking for, so we can compare our current node with that one and skip the
            // test if they are the same. In practice, seems to avoid 3-10% of all compares
            Node* overShotNode = (startV->size () <= linkHeight) ? nullptr : (*startV)[linkHeight];
            while (n != overShotNode) {
#if qKeepADTStatistics
                const_cast<SkipList<KEY, VALUE, TRAITS>*> (this)->fCompares++;
#endif

                int comp = TRAITS::Comparer::Compare (n->fEntry.GetKey (), key);
                if (comp == 0) {
                    return n;
                }
                else if (comp < 0) {
                    startV = &n->fNext;
                    n      = n->fNext[linkHeight - 1];
                }
                else {
                    break; // overshot
                }
            }
        }

        return nullptr;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    bool SkipList<KEY, VALUE, TRAITS>::Find (const KeyType& key, ValueType* val) const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        Node*                                               n = FindNode (key);
        if (n != nullptr) {
            if (val != nullptr) {
                *val = n->fEntry.GetValue ();
            }
            return true;
        }
        return false;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::Add (const KeyType& key, const ValueType& val)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        vector<Node*>                                      links;

        Node* n = FindNearest (key, links);
        if ((n != nullptr) and (TRAITS::kPolicy & ADT::eDuplicateAddThrowException)) {
            throw DuplicateAddException ();
        }
        AddNode (new Node (key, val), links);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::Add (const KeyType& keyAndValue)
    {
        Add (keyAndValue, keyAndValue);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::AddNode (Node* node, const vector<Node*>& links)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (node);
        size_t newLinkHeight = DetermineLinkHeight ();
        node->fNext.resize (newLinkHeight);

        size_t linksToPatch = min (fHead.size (), newLinkHeight);
        for (size_t i = 0; i < linksToPatch; ++i) {
            Node* nextL = nullptr;
            if (links[i] == nullptr) {
                nextL    = fHead[i];
                fHead[i] = node;
            }
            else {
#if qKeepADTStatistics
                const_cast<SkipList<KEY, VALUE, TRAITS>*> (this)->fRotations++;
#endif
                Node* oldLink = links[i];
                AssertNotNull (oldLink);
                nextL             = oldLink->fNext[i];
                oldLink->fNext[i] = node;
            }

            node->fNext[i] = nextL;
        }

        GrowHeadLinksIfNeeded (newLinkHeight, node);
        ++fLength;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::Remove (const KeyType& key)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        vector<Node*>                                      links;
        Node*                                              n = FindNearest (key, links);
        if (n != nullptr) {
            RemoveNode (n, links);
        }
        else {
            if (not(TRAITS::kPolicy & ADT::eInvalidRemoveIgnored)) {
                throw InvalidRemovalException ();
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::RemoveNode (Node* n, const vector<Node*>& links)
    {
        for (auto it = links.begin (); it != links.end (); ++it) {
            size_t index     = it - links.begin ();
            Node** patchNode = (*it == nullptr) ? &fHead[index] : &(*it)->fNext[index];
            if (*patchNode == n) {
#if qKeepADTStatistics
                const_cast<SkipList<KEY, VALUE, TRAITS>*> (this)->fRotations++;
#endif
                *patchNode = n->fNext[index];
            }
            else {
                break;
            }
        }
        if (n->fNext.size () == fHead.size ()) {
            ShrinkHeadLinksIfNeeded ();
        }
        delete n;

        --fLength;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    size_t SkipList<KEY, VALUE, TRAITS>::DetermineLinkHeight () const
    {
        enum {
            kMaxNewGrowth = 1
        };

        int linkHeight = 1;
        int maxHeight  = min (fHead.size () + kMaxNewGrowth, size_t (kMaxLinkHeight));
        while ((linkHeight < maxHeight) and (RandomSize_t (1, 100) <= GetLinkHeightProbability ())) {
            ++linkHeight;
        }
        return linkHeight;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::GrowHeadLinksIfNeeded (size_t newSize, Node* nodeToPointTo)
    {
        if (newSize > fHead.size ()) {
            fHead.resize (newSize, nodeToPointTo);
            Assert (fHead[newSize - 1] == nodeToPointTo);
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::ShrinkHeadLinksIfNeeded ()
    {
        if (fHead.size () <= 1)
            return;

        for (size_t i = fHead.size () - 1; i > 1; --i) {
            if (fHead[i] == nullptr) {
                fHead.pop_back ();
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::RemoveAll ()
    {
        Node* link = (fHead.size () == 0) ? nullptr : fHead[0];
        while (link != nullptr) {
            Node* nextLink = link->fNext[0];
            delete link;
            link = nextLink;
        }
        fHead.resize (1);
        fHead[0] = nullptr;
        fLength  = 0;

        Ensure (GetLength () == 0);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    typename SkipList<KEY, VALUE, TRAITS>::Node* SkipList<KEY, VALUE, TRAITS>::FindNearest (KeyType key, vector<Node*>& links) const
    {
        Require (links.size () == 0); // we want to be passed in a totally empty vector
        Assert (fHead.size () > 0);

        links = fHead;

        Node*  newOverShotNode = nullptr;
        Node*  foundNode       = nullptr;
        size_t linkIndex       = links.size () - 1;
        do {
            Node* n = links[linkIndex];

            // tweak to use pointer comparisons rather than key field compares. We know any link heigher than the current link being
            // tested must point past the key we are looking for, so we can compare our current node with that one and skip the
            // test if they are the same. In practice, seems to avoid 3-10% of all compares
            Node* overShotNode = newOverShotNode;
            Assert (n == nullptr or overShotNode == nullptr or (TRAITS::Comparer::Compare (n->fEntry.GetKey (), overShotNode->fEntry.GetKey ())) <= 0);

            links[linkIndex] = nullptr;
            while (n != overShotNode) {
#if qKeepADTStatistics
                const_cast<SkipList<KEY, VALUE, TRAITS>*> (this)->fCompares++;
#endif

                int comp = TRAITS::Comparer::Compare (n->fEntry.GetKey (), key);
                if (comp == 0) {
                    foundNode       = n;
                    newOverShotNode = foundNode;
                    break;
                }
                else if (comp < 0) {
                    links[linkIndex] = n;
                    n                = n->fNext[linkIndex];
                    newOverShotNode  = n;
                }
                else {
                    break;
                }
            }
            if ((linkIndex > 0) and (links[linkIndex] != nullptr)) {
                links[linkIndex - 1] = links[linkIndex];
            }

        } while (linkIndex-- != 0);

        return foundNode;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    typename SkipList<KEY, VALUE, TRAITS>::Node* SkipList<KEY, VALUE, TRAITS>::GetFirst () const
    {
        if (fHead.size () == 0) {
            return nullptr;
        }
        return (fHead[0]);
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    typename SkipList<KEY, VALUE, TRAITS>::Node* SkipList<KEY, VALUE, TRAITS>::GetLast () const
    {
        if (fHead.size () == 0) {
            return nullptr;
        }

        size_t linkIndex = fHead.size () - 1;
        Node*  n         = fHead[linkIndex];
        if (n != nullptr) {
            Node* prev = n;
            while (true) {
                while (n != nullptr) {
                    prev = n;
                    n    = n->fNext[linkIndex];
                }
                n = prev;
                if (linkIndex == 0) {
                    break;
                }
                --linkIndex;
            }
        }
        return n;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::Prioritize (const KeyType& key)
    {
        vector<Node*> links;

        Node* node = FindNearest (key, links);
        if (node != nullptr and node->fNext.size () <= fHead.size ()) {
            if (node->fNext.size () == fHead.size ()) {
                GrowHeadLinksIfNeeded (fHead.size () + 1, node);
                links.resize (fHead.size (), node);
            }

            size_t oldLinkHeight = node->fNext.size ();
            node->fNext.resize (fHead.size (), nullptr);
            size_t newLinkHeight = node->fNext.size ();
            Assert (oldLinkHeight < newLinkHeight);

            for (size_t i = oldLinkHeight; i <= newLinkHeight - 1; ++i) {
                if (links[i] == nullptr) {
                    fHead[i] = node;
                }
                else if (links[i] == node) {
                    break;
                }
                else {
#if qKeepADTStatistics
                    const_cast<SkipList<KEY, VALUE, TRAITS>*> (this)->fRotations++;
#endif
                    Node* oldLink = links[i];
                    AssertNotNull (oldLink);
                    Assert (oldLink->fNext.size () > i);
                    Node* nextL       = oldLink->fNext[i];
                    oldLink->fNext[i] = node;
                    node->fNext[i]    = nextL;
                }
            }
        }
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::ReBalance ()
    {
        if (GetLength () == 0)
            return;

        // precompute table of indices height
        // idea is to have a link for every log power of the probability at a particular index
        // for example, for a 1/2 chance, have heights start as 1 2 1 3 1 2 1 4
        double indexBase = (GetLinkHeightProbability () == 0) ? 0 : 1 / (GetLinkHeightProbability () / 100.0);
        size_t height[kMaxLinkHeight];
        size_t lastValidHeight = 0;
        for (size_t i = 0; i < sizeof (height) / sizeof (size_t); ++i) {
            height[i] = size_t (pow (indexBase, double (i)));

            if (height[i] == 0 or height[i] > GetLength ()) {
                Assert (i > 0); // else have no valid heights
                break;
            }
            lastValidHeight = i;
        }

        // wipe out everything, keeping only a link to the first item in list
        Node* node = fHead[0];
        fHead.clear ();
        fHead.resize (kMaxLinkHeight);

        Node** patchNodes[kMaxLinkHeight];
        for (size_t i = 0; i < sizeof (patchNodes) / sizeof (size_t); ++i) {
            patchNodes[i] = &fHead[i];
        }

        size_t index = 1;
        while (node != nullptr) {
            Node* next = node->fNext[0];
            node->fNext.clear ();

#if qDebug
            bool patched = false;
#endif
            for (size_t hIndex = lastValidHeight + 1; hIndex-- > 0;) {
                if (index >= height[hIndex] and (index % height[hIndex] == 0)) {
                    node->fNext.resize (hIndex + 1, nullptr);
                    for (size_t patchIndex = node->fNext.size (); patchIndex-- > 0;) {
                        *patchNodes[patchIndex] = node;
                        patchNodes[patchIndex]  = &node->fNext[patchIndex];
                    }
#if qDebug
                    patched = true;
#endif
                    break;
                }
            }
            Assert (patched);

            ++index;
            node = next;
        }
        Assert (index == GetLength () + 1);

        ShrinkHeadLinksIfNeeded ();
    }
#if qDebug
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::ListAll () const
    {
        cout << "[";
        for (size_t i = 0; i < fHead.size (); ++i) {
            if (fHead[i] == nullptr) {
                cout << "*"
                     << ", ";
            }
            else {
                cout << fHead[i]->fEntry.GetValue () << ", ";
            }
        }
        cout << "]  ";

        Node* n = fHead[0];
        while (n != nullptr) {
            cout << n->fEntry.GetValue () << " (" << n->fNext.size () << "), ";
            n = n->fNext[0];
        }
        cout << endl
             << flush;
    }
    template <typename KEY, typename VALUE, typename TRAITS>
    void SkipList<KEY, VALUE, TRAITS>::ValidateAll () const
    {
        Node* n = fHead[0];

        while (n != nullptr) {
            KEY oldKey = n->fEntry.GetKey ();
            for (size_t i = 1; i < n->fNext.size (); ++i) {
                Node* newN = n->fNext[i];
                if (n == nullptr) {
                    Assert (newN == nullptr);
                }
                else {
                    Assert (newN != n);
                    Assert (newN == nullptr or (TRAITS::Comparer::Compare (oldKey, newN->fEntry.GetKey ()) <= 0));
                }
            }
            n = n->fNext[0];
            Assert (n == nullptr or (TRAITS::Comparer::Compare (n->fEntry.GetKey (), oldKey) >= 0));
        }
    }
#endif
#if qKeepADTStatistics
    template <typename KEY, typename VALUE, typename TRAITS>
    size_t SkipList<KEY, VALUE, TRAITS>::CalcHeight (size_t* totalHeight) const
    {
        if (totalHeight != nullptr) {
            *totalHeight         = 0;
            size_t maxLinkHeight = 0;

            Node* n = fHead[0];
            while (n != nullptr) {
                maxLinkHeight = max (maxLinkHeight, n->fNext.size ());
                *totalHeight += n->fNext.size ();
                n = n->fNext[0];
            }
            Assert (maxLinkHeight == fHead.size ());
        }

        return fHead.size ();
    }
#endif
}
#endif /* _Stroika_Foundation_Containers_DataStructures_SkipList_inl_ */
