/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <random>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

namespace Stroika::Foundation::Containers::DataStructures {

    namespace Private_ {

        inline size_t RandomSize_t (size_t first, size_t last)
        {
            static thread_local std::mt19937 sRng_{std::random_device () ()};
            Assert (sRng_.min () <= first);
            //  Assert (eng.max () >= last);    // g++ has 8 byte size_t in 64 bit??
            std::uniform_int_distribution<size_t> unif (first, last);
            size_t                                result = unif (sRng_);
            Ensure (result >= first);
            Ensure (result <= last);
            return result;
        }

    }

    /*
     ********************************************************************************
     ************** SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Node_ ******************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Node_::Node_ (const key_type& key, const mapped_type& val)
        : fEntry{key, val}
    {
    }

    /*
     ********************************************************************************
     ******************* SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> ********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::SkipList (KeyComparerType keyComparer)
        : fKeysStrictInOrderComparer_{keyComparer}
    {
        GrowHeadLinksIfNeeded_ (1, nullptr);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::SkipList (const SkipList& s)
        : fKeysStrictInOrderComparer_{s.fKeysStrictInOrderComparer_}
    {
        operator= (s);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>& SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::operator= (const SkipList& t)
    {
        RemoveAll ();
        if (t.size () != 0) {
            Node_* prev = nullptr;
            Node_* n    = (t.fHead_.size () == 0) ? nullptr : t.fHead_[0];
            while (n != nullptr) {
                Node_* newNode = new Node_ (n->fEntry.fKey, n->fEntry.fValue);
                if (prev == nullptr) {
                    Assert (fHead_.size () == 1);
                    Assert (fHead_[0] == nullptr);
                    fHead_[0] = newNode;
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
            fLength_ = t.fLength_;
            ReBalance (); // this will give us a proper link structure
        }
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::~SkipList ()
    {
        RemoveAll ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline size_t SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetLinkHeightProbability ()
    {
        return 25;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetStats () const -> StatsType
    {
        return fStats_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline size_t SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::size () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fLength_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::begin () const -> ForwardIterator
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return ForwardIterator{this, GetFirst_ ()};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::end () const -> ForwardIterator
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return ForwardIterator{this, nullptr};
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::FindNode_ (const key_type& key) const -> Node_*
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Assert (fHead_.size () > 0);
        vector<Node_*> const* startV = &fHead_;
        for (size_t linkHeight = fHead_.size (); linkHeight > 0; linkHeight--) {
            Node_* n = (*startV)[linkHeight - 1];
            // tweak to use pointer comparisons rather than key field compares. We know any link heigher than the current link being
            // tested must point past the key we are looking for, so we can compare our current node with that one and skip the
            // test if they are the same. In practice, seems to avoid 3-10% of all compares
            Node_* overShotNode = (startV->size () <= linkHeight) ? nullptr : (*startV)[linkHeight];
            while (n != overShotNode) {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    fStats_.fCompares++;
                }
                if (this->fKeysStrictInOrderComparer_ (n->fEntry.fKey, key)) {
                    startV = &n->fNext;
                    n      = n->fNext[linkHeight - 1];
                }
                else if (not fKeysStrictInOrderComparer_ (key, n->fEntry.fKey)) {
                    return n;
                }
                else {
                    break; // overshot
                }
            }
        }
        return nullptr;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Find (const key_type& key, mapped_type* val) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Node_*                                         n = FindNode_ (key);
        if (n != nullptr) {
            if (val != nullptr) {
                *val = n->fEntry.fValue;
            }
            return true;
        }
        return false;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::contains (const key_type& key) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return FindNode_ (key) != nullptr;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const key_type& key, const mapped_type& val, AddOrExtendOrReplaceMode addOrReplaceMode)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        vector<Node_*>                                  links;
        Node_*                                          n = FindNearest_ (key, links);
        if (n == nullptr) {
            AddNode_ (new Node_ (key, val), links);
            return true;
        }
        else {
            switch (addOrReplaceMode) {
                case AddOrExtendOrReplaceMode::eAddIfMissing:
                    return false;
                case AddOrExtendOrReplaceMode::eAddReplaces:
                    n->fEntry.fValue = val;
                    return true;
                case AddOrExtendOrReplaceMode::eAddExtras:
                    AddNode_ (new Node_ (key, val), links);
                    return true;
                case AddOrExtendOrReplaceMode::eDuplicatesRejected:
                    static const auto kExcept_ = Execution::RuntimeErrorException<logic_error>{"Duplicates not allowed"sv};
                    Execution::Throw (kExcept_);
            }
            AssertNotReached ();
            return false;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const value_type& v, AddOrExtendOrReplaceMode addOrReplaceMode)
    {
        return Add (v.fKey, v.fValue, addOrReplaceMode);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::AddNode_ (Node_* node, const vector<Node_*>& links)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        RequireNotNull (node);
        size_t newLinkHeight = DetermineLinkHeight_ ();
        node->fNext.resize (newLinkHeight);
        size_t linksToPatch = min (fHead_.size (), newLinkHeight);
        for (size_t i = 0; i < linksToPatch; i++) {
            Node_* nextL = nullptr;
            if (links[i] == nullptr) {
                nextL     = fHead_[i];
                fHead_[i] = node;
            }
            else {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    fStats_.fRotations++;
                }
                Node_* oldLink = links[i];
                AssertNotNull (oldLink);
                nextL             = oldLink->fNext[i];
                oldLink->fNext[i] = node;
            }
            node->fNext[i] = nextL;
        }
        GrowHeadLinksIfNeeded_ (newLinkHeight, node);
        fLength_++;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Remove (const key_type& key)
    {
        Verify (RemoveIf (key));
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveIf (const key_type& key)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        vector<Node_*>                                  links;
        Node_*                                          n = FindNearest_ (key, links);
        if (n != nullptr) {
            RemoveNode_ (n, links);
            return true;
        }
        else {
            return false;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveNode_ (Node_* n, const vector<Node_*>& links)
    {
        for (auto it = links.begin (); it != links.end (); it++) {
            size_t  index     = it - links.begin ();
            Node_** patchNode = (*it == nullptr) ? &fHead_[index] : &(*it)->fNext[index];
            if (*patchNode == n) {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    fStats_.fRotations++;
                }
                *patchNode = n->fNext[index];
            }
            else {
                break;
            }
        }
        if (n->fNext.size () == fHead_.size ()) {
            ShrinkHeadLinksIfNeeded_ ();
        }
        delete n;
        fLength_--;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    size_t SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::DetermineLinkHeight_ () const
    {
        constexpr size_t kMaxNewGrowth = 1;
        size_t           linkHeight    = 1;
        size_t           maxHeight     = min (fHead_.size () + kMaxNewGrowth, size_t (kMaxLinkHeight_));
        while ((linkHeight < maxHeight) and (Private_::RandomSize_t (1, 100) <= GetLinkHeightProbability ())) {
            linkHeight++;
        }
        return linkHeight;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GrowHeadLinksIfNeeded_ (size_t newSize, Node_* nodeToPointTo)
    {
        if (newSize > fHead_.size ()) {
            fHead_.resize (newSize, nodeToPointTo);
            Assert (fHead_[newSize - 1] == nodeToPointTo);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ShrinkHeadLinksIfNeeded_ ()
    {
        if (fHead_.size () <= 1)
            return;
        for (size_t i = fHead_.size () - 1; i > 1; i--) {
            if (fHead_[i] == nullptr) {
                fHead_.pop_back ();
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveAll ()
    {
        Node_* link = (fHead_.size () == 0) ? nullptr : fHead_[0];
        while (link != nullptr) {
            Node_* nextLink = link->fNext[0];
            delete link;
            link = nextLink;
        }
        fHead_.resize (1);
        fHead_[0] = nullptr;
        fLength_  = 0;
        Ensure (size () == 0);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    typename SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Node_* SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::FindNearest_ (const key_type& key,
                                                                                                                    vector<Node_*>& links) const
    {
        Require (links.size () == 0); // we want to be passed in a totally empty vector
        Assert (fHead_.size () > 0);
        links                  = fHead_;
        Node_* newOverShotNode = nullptr;
        Node_* foundNode       = nullptr;
        size_t linkIndex       = links.size () - 1;
        do {
            Node_* n = links[linkIndex];
            // tweak to use pointer comparisons rather than key field compares. We know any link higher than the current link being
            // tested must point past the key we are looking for, so we can compare our current node with that one and skip the
            // test if they are the same. In practice, seems to avoid 3-10% of all compares
            Node_* overShotNode = newOverShotNode;
            Assert (n == nullptr or overShotNode == nullptr or (not fKeysStrictInOrderComparer_ (overShotNode->fEntry.fKey, n->fEntry.fKey)));

            links[linkIndex] = nullptr;
            while (n != overShotNode) {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    fStats_.fCompares++;
                }
                if (fKeysStrictInOrderComparer_ (n->fEntry.fKey, key)) {
                    links[linkIndex] = n;
                    n                = n->fNext[linkIndex];
                    newOverShotNode  = n;
                }
                else if (not fKeysStrictInOrderComparer_ (key, n->fEntry.fKey)) {
                    foundNode       = n;
                    newOverShotNode = foundNode;
                    break;
                }
                else {
                    break;
                }
            }
            if (linkIndex > 0 and links[linkIndex] != nullptr) {
                links[linkIndex - 1] = links[linkIndex];
            }

        } while (linkIndex-- != 0);

        return foundNode;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetFirst_ () const -> Node_*
    {
        if (fHead_.size () == 0) {
            return nullptr;
        }
        return fHead_[0];
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetLast_ () const -> Node_*
    {
        if (fHead_.size () == 0) {
            return nullptr;
        }
        size_t linkIndex = fHead_.size () - 1;
        Node_* n         = fHead_[linkIndex];
        if (n != nullptr) {
            Node_* prev = n;
            while (true) {
                while (n != nullptr) {
                    prev = n;
                    n    = n->fNext[linkIndex];
                }
                n = prev;
                if (linkIndex == 0) {
                    break;
                }
                linkIndex--;
            }
        }
        return n;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Prioritize (const key_type& key)
    {
        vector<Node_*> links;
        Node_*         node = FindNearest_ (key, links);
        if (node != nullptr and node->fNext.size () <= fHead_.size ()) {
            if (node->fNext.size () == fHead_.size ()) {
                GrowHeadLinksIfNeeded_ (fHead_.size () + 1, node);
                links.resize (fHead_.size (), node);
            }
            size_t oldLinkHeight = node->fNext.size ();
            node->fNext.resize (fHead_.size (), nullptr);
            size_t newLinkHeight = node->fNext.size ();
            Assert (oldLinkHeight < newLinkHeight);
            for (size_t i = oldLinkHeight; i <= newLinkHeight - 1; i++) {
                if (links[i] == nullptr) {
                    fHead_[i] = node;
                }
                else if (links[i] == node) {
                    break;
                }
                else {
                    if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                        fStats_.fRotations++;
                    }
                    Node_* oldLink = links[i];
                    AssertNotNull (oldLink);
                    Assert (oldLink->fNext.size () > i);
                    Node_* nextL      = oldLink->fNext[i];
                    oldLink->fNext[i] = node;
                    node->fNext[i]    = nextL;
                }
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ReBalance ()
    {
        if (size () == 0) [[unlikely]] {
            return;
        }
        // precompute table of indices height
        // idea is to have a link for every log power of the probability at a particular index
        // for example, for a 1/2 chance, have heights start as 1 2 1 3 1 2 1 4
        double indexBase = (GetLinkHeightProbability () == 0) ? 0 : 1 / (GetLinkHeightProbability () / 100.0);
        size_t height[kMaxLinkHeight_];
        size_t lastValidHeight = 0;
        for (size_t i = 0; i < sizeof (height) / sizeof (size_t); i++) {
            height[i] = size_t (pow (indexBase, double (i)));

            if (height[i] == 0 or height[i] > size ()) {
                Assert (i > 0); // else have no valid heights
                break;
            }
            lastValidHeight = i;
        }
        // wipe out everything, keeping only a link to the first item in list
        Node_* node = fHead_[0];
        fHead_.clear ();
        fHead_.resize (kMaxLinkHeight_);

        Node_** patchNodes[kMaxLinkHeight_];
        for (size_t i = 0; i < sizeof (patchNodes) / sizeof (size_t); i++) {
            patchNodes[i] = &fHead_[i];
        }

        size_t index = 1;
        while (node != nullptr) {
            Node_* next = node->fNext[0];
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
#if qDebug
            Assert (patched);
#endif

            index++;
            node = next;
        }
        Assert (index == size () + 1);
        ShrinkHeadLinksIfNeeded_ ();
    }
#if qDebug
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ListAll () const
    {
        // replace with ToString() overload???? - enable-if ToString(KEY_TYPE and MAPPED_TYPE)
        cout << "[";
        for (size_t i = 0; i < fHead_.size (); i++) {
            if (fHead_[i] == nullptr) {
                cout << "*"
                     << ", ";
            }
            else {
                cout << fHead_[i]->fEntry.GetValue () << ", ";
            }
        }
        cout << "]  ";

        Node_* n = fHead_[0];
        while (n != nullptr) {
            cout << n->fEntry.GetValue () << " (" << n->fNext.size () << "), ";
            n = n->fNext[0];
        }
        cout << endl << flush;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ValidateAll () const
    {
        Node_* n = fHead_[0];

        while (n != nullptr) {
            KEY_TYPE oldKey = n->fEntry.fKey;
            for (size_t i = 1; i < n->fNext.size (); i++) {
                Node_* newN = n->fNext[i];
                if (n == nullptr) {
                    Assert (newN == nullptr);
                }
                else {
                    Assert (newN != n);
                    Assert (newN == nullptr or (TRAITS::Comparer::Compare (oldKey, newN->fEntry.fKey) <= 0));
                }
            }
            n = n->fNext[0];
            Assert (n == nullptr or (TRAITS::Comparer::Compare (n->fEntry.fKey, oldKey) >= 0));
        }
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    size_t SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::CalcHeight (size_t* totalHeight) const
    {
        if (totalHeight != nullptr) {
            *totalHeight         = 0;
            size_t maxLinkHeight = 0;
            Node_* n             = fHead_[0];
            while (n != nullptr) {
                maxLinkHeight = max (maxLinkHeight, n->fNext.size ());
                *totalHeight += n->fNext.size ();
                n = n->fNext[0];
            }
            Assert (maxLinkHeight == fHead_.size ());
        }
        return fHead_.size ();
    }

    /*
     ********************************************************************************
     *********** SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::ForwardIterator (const SkipList* data, const Node_* n)
        : fData_{data}
        , fCurrent_{n}
    {
    }
#if qDebug
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::~ForwardIterator ()
    {
        Invariant ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::Current () const -> value_type
    {
        RequireNotNull (fCurrent_);
        return fCurrent_->fEntry;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline constexpr bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator== (const ForwardIterator& rhs) const
    {
        Require (fData_ == nullptr or rhs.fData_ == nullptr or fData_ == rhs.fData_);
        return fCurrent_ == rhs.fCurrent_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::GetUnderlyingData () const -> const SkipList*
    {
        return fData_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator++ () -> ForwardIterator&
    {
        fCurrent_ = fCurrent_->fNext[0]; //tmphack...
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::Invariant_ () const noexcept
    {
        // @todo
    }
#endif

}
