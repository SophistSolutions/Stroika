/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include <random>

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

namespace Stroika::Foundation::Containers::DataStructures {

    // a few smoke checks to assure SkipList defined properly...
    static_assert (constructible_from<SkipList<int, int>>);
    static_assert (constructible_from<SkipList<int, void>>);
    static_assert (input_or_output_iterator<SkipList<int, int>::ForwardIterator>);

    namespace Private_ {

        inline size_t RandomSize_t (size_t first, size_t last)
        {
            static thread_local std::mt19937 sRng_{std::random_device () ()};
            Assert (sRng_.min () <= first);
            //  Assert (eng.max () >= last);    // g++ has 8 byte size_t in 64 bit??
            std::uniform_int_distribution<size_t> unif{first, last};
            size_t                                result = unif (sRng_);
            Ensure (result >= first);
            Ensure (result <= last);
            return result;
        }

    }

    /*
     ********************************************************************************
     ************** SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Link_ ******************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Link_::Link_ (ArgByValueType<key_type> key, ArgByValueType<mapped_type> val)
        requires (not same_as<mapped_type, void>)
        : fEntry{key, val}
    {
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Link_::Link_ (ArgByValueType<key_type> key)
        requires (same_as<mapped_type, void>)
        : fEntry{key}
    {
    }

    /*
     ********************************************************************************
     ******************* SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS> ********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::SkipList (KeyComparerType keyComparer)
        : fKeyThreeWayComparer_{keyComparer}
    {
        GrowHeadLinksIfNeeded_ (1, nullptr);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::SkipList (const SkipList& s)
        : fKeyThreeWayComparer_{s.fKeyThreeWayComparer_}
    {
        operator= (s);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>& SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::operator= (const SkipList& t)
    {
        RemoveAll ();
        if (t.size () != 0) {
            Link_* prev = nullptr;
            Link_* n    = t.fHead_[0];
            while (n != nullptr) {
                Link_* newNode = new Link_{n->fEntry.fKey, n->fEntry.fValue};
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
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::empty () const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return fLength_ == 0;
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
    inline void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::MoveIteratorHereAfterClone (ForwardIterator* pi, const SkipList* movedFrom) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        RequireNotNull (pi);
        RequireNotNull (movedFrom);
        Require (pi->fData_ == movedFrom);
        // TRICKY TODO - BUT MUST DO - MUST MOVE FROM OLD ITER TO NEW
        // only way
        //
        // For STL containers, not sure how to find an equiv new iterator for an old one, but my best guess is to iterate through
        // old for old, and when I match, stop on new
        Link_*                  newI = this->fHead_[0];
        [[maybe_unused]] Link_* newE = nullptr;
        Link_*                  oldI = movedFrom->fHead_[0];
        [[maybe_unused]] Link_* oldE = nullptr;
        while (oldI != pi->fCurrent_) {
            Assert (newI != newE);
            Assert (oldI != oldE);
            newI = newI->fNext[0];
            oldI = oldI->fNext[0];
            Assert (newI != newE);
            Assert (oldI != oldE);
        }
        Assert (oldI == pi->fCurrent_);
        pi->fCurrent_ = newI;
        pi->fData_    = this;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::FindNode_ (ArgByValueType<key_type> key) const -> Link_*
    {
        using Common::ToInt;
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        Assert (fHead_.size () > 0);
        LinkVector_ const* startV = &fHead_;
        for (size_t linkHeight = fHead_.size (); linkHeight > 0; --linkHeight) {
            Link_* n = (*startV)[linkHeight - 1];
            // tweak to use pointer comparisons rather than key field compares. We know any link higher than the current link being
            // tested must point past the key we are looking for, so we can compare our current node with that one and skip the
            // test if they are the same. In practice, seems to avoid 3-10% of all compares
            Link_* overShotNode = (startV->size () <= linkHeight) ? nullptr : (*startV)[linkHeight];
            while (n != overShotNode) {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    ++fStats_.fCompares;
                }
                switch (ToInt (fKeyThreeWayComparer_ (n->fEntry.fKey, key))) {
                    case ToInt (strong_ordering::equal):
                        return n;
                    case ToInt (strong_ordering::less):
                        startV = &n->fNext;
                        n      = n->fNext[linkHeight - 1];
                        break;
                    case ToInt (strong_ordering::greater):
                        goto overshoot;
                }
            }
        overshoot:;
        }
        return nullptr;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Find (ArgByValueType<key_type> key) const -> ForwardIterator
    {
        return ForwardIterator{this, FindNode_ (key)};
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    template <predicate<typename SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type> FUNCTION>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Find (FUNCTION&& firstThat) const -> ForwardIterator
    {
        for (auto i = begin (); i; ++i) {
            if (firstThat (*i)) {
                return i;
            }
        }
        return end ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::First (ArgByValueType<key_type> key) const -> optional<mapped_type>
    {
        if (auto o = FindNode_ (key)) {
            return o->fEntry.fValue;
        }
        return nullopt;
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    template <predicate<typename SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::value_type> FUNCTION>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::First (FUNCTION&& firstThat) const -> optional<mapped_type>
    {
        for (auto i : *this) {
            if (firstThat (*i)) {
                return i->fValue;
            }
        }
        return nullopt;
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::contains (ArgByValueType<key_type> key) const
    {
        AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        return FindNode_ (key) != nullptr;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetComparer () const -> KeyComparerType
    {
        return this->fKeyThreeWayComparer_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    template <typename CHECK_T>
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (ArgByValueType<key_type> key, ArgByValueType<CHECK_T> val)
        requires (not same_as<mapped_type, void>)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        if constexpr (TRAITS::kCostlyInvariants) {
            Invariant ();
        }
        LinkVector_ links;
        Link_*      n = FindNearest_ (key, links);
        if (n == nullptr) {
            AddNode_ (new Link_{key, val}, links);
            if constexpr (TRAITS::kCostlyInvariants) {
                Invariant ();
            }
            return true;
        }
        else {
            switch (TRAITS::kAddOrExtendOrReplaceMode) {
                case AddOrExtendOrReplaceMode::eAddIfMissing:
                    return false;
                case AddOrExtendOrReplaceMode::eAddReplaces:
                    n->fEntry.fKey   = key; // two 'different' objects can compare equal, and this updates the value (e.g. stroika set)
                    n->fEntry.fValue = val;
                    if constexpr (TRAITS::kCostlyInvariants) {
                        Invariant ();
                    }
                    return true;
                case AddOrExtendOrReplaceMode::eAddExtras:
                    AddNode_ (new Link_{key, val}, links);
                    if constexpr (TRAITS::kCostlyInvariants) {
                        Invariant ();
                    }
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
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (ArgByValueType<key_type> key)
        requires (same_as<mapped_type, void>)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        if constexpr (TRAITS::kCostlyInvariants) {
            Invariant ();
        }
        LinkVector_ links;
        Link_*      n = FindNearest_ (key, links);
        if (n == nullptr) {
            AddNode_ (new Link_{key}, links);
            if constexpr (TRAITS::kCostlyInvariants) {
                Invariant ();
            }
            return true;
        }
        else {
            switch (TRAITS::kAddOrExtendOrReplaceMode) {
                case AddOrExtendOrReplaceMode::eAddIfMissing:
                    return false;
                case AddOrExtendOrReplaceMode::eAddReplaces:
                    n->fEntry.fKey = key; // two 'different' objects can compare equal, and this updates the value (e.g. stroika set)
                    if constexpr (TRAITS::kCostlyInvariants) {
                        Invariant ();
                    }
                    return true;
                case AddOrExtendOrReplaceMode::eAddExtras:
                    AddNode_ (new Link_{key}, links);
                    if constexpr (TRAITS::kCostlyInvariants) {
                        Invariant ();
                    }
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
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Add (const value_type& v)
    {
        return Add (v.fKey, v.fValue);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::AddNode_ (Link_* node, const LinkVector_& links)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        RequireNotNull (node);
        size_t newLinkHeight = DetermineLinkHeight_ ();
        node->fNext.resize (newLinkHeight);
        size_t linksToPatch = min (fHead_.size (), newLinkHeight);
        for (size_t i = 0; i < linksToPatch; ++i) {
            Link_* nextL = nullptr;
            if (links[i] == nullptr) {
                nextL     = fHead_[i];
                fHead_[i] = node;
            }
            else {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    ++fStats_.fRotations;
                }
                Link_* oldLink = links[i];
                AssertNotNull (oldLink);
                nextL             = oldLink->fNext[i];
                oldLink->fNext[i] = node;
            }
            node->fNext[i] = nextL;
        }
        GrowHeadLinksIfNeeded_ (newLinkHeight, node);
        ++fLength_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Remove (ArgByValueType<key_type> key)
    {
        Verify (RemoveIf (key));
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveIf (ArgByValueType<key_type> key)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        if constexpr (TRAITS::kCostlyInvariants) {
            Invariant ();
        }
        LinkVector_ links;
        Link_*      n = FindNearest_ (key, links);
        if (n != nullptr) {
            RemoveNode_ (n, links);
            if constexpr (TRAITS::kCostlyInvariants) {
                Invariant ();
            }
            return true;
        }
        else {
            return false;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveNode_ (Link_* n, const LinkVector_& links)
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        for (auto it = links.begin (); it != links.end (); ++it) {
            size_t  index     = it - links.begin ();
            Link_** patchNode = (*it == nullptr) ? &fHead_[index] : &(*it)->fNext[index];
            if (*patchNode == n) {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    ++fStats_.fRotations;
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
        --fLength_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    size_t SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::DetermineLinkHeight_ () const
    {
        constexpr size_t kMaxNewGrowth = 1;
        size_t           linkHeight    = 1;
        size_t           maxHeight     = min (fHead_.size () + kMaxNewGrowth, size_t (kMaxLinkHeight_));
        while ((linkHeight < maxHeight) and (Private_::RandomSize_t (1, 100) <= GetLinkHeightProbability ())) {
            ++linkHeight;
        }
        return linkHeight;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GrowHeadLinksIfNeeded_ (size_t newSize, Link_* nodeToPointTo)
    {
        if (newSize > fHead_.size ()) {
            fHead_.resize (newSize, nodeToPointTo);
            Assert (fHead_[newSize - 1] == nodeToPointTo);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ShrinkHeadLinksIfNeeded_ ()
    {
        Require (fHead_.size () >= 1);
        for (size_t i = fHead_.size () - 1; i >= 1; --i) {
            if (fHead_[i] == nullptr) {
                fHead_.pop_back ();
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::RemoveAll ()
    {
        AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
        Link_*                                          link = (fHead_.size () == 0) ? nullptr : fHead_[0];
        while (link != nullptr) {
            Link_* nextLink = link->fNext[0];
            delete link;
            link = nextLink;
        }
        fHead_.resize (1);
        fHead_[0] = nullptr;
        fLength_  = 0;
        Ensure (size () == 0);
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::FindNearest_ (ArgByValueType<key_type> key, LinkVector_& links) const -> Link_*
    {
        using Common::ToInt;
        Require (links.size () == 0); // we want to be passed in a totally empty vector
        Assert (fHead_.size () > 0);
        links                  = fHead_;
        Link_* newOverShotNode = nullptr;
        Link_* foundNode       = nullptr;
        Assert (not links.empty ()); // now
        size_t linkIndex = links.size () - 1;
        do {
            Link_* n = links[linkIndex];
            // tweak to use pointer comparisons rather than key field compares. We know any link higher than the current link being
            // tested must point past the key we are looking for, so we can compare our current node with that one and skip the
            // test if they are the same. In practice, seems to avoid 3-10% of all compares
            Link_* overShotNode = newOverShotNode;
            Assert (n == nullptr or overShotNode == nullptr or
                    (fKeyThreeWayComparer_ (n->fEntry.fKey, overShotNode->fEntry.fKey) != strong_ordering::greater));

            links[linkIndex] = nullptr;
            while (n != overShotNode) {
                if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                    ++fStats_.fCompares;
                }
                switch (ToInt (fKeyThreeWayComparer_ (n->fEntry.fKey, key))) {
                    case ToInt (strong_ordering::equal):
                        foundNode       = n;
                        newOverShotNode = foundNode;
                        goto finished;
                    case ToInt (strong_ordering::less):
                        links[linkIndex] = n;
                        n                = n->fNext[linkIndex];
                        newOverShotNode  = n;
                        break;
                    case ToInt (strong_ordering::greater):
                        goto finished;
                }
            }
        finished:;
            if (linkIndex > 0 and links[linkIndex] != nullptr) {
                links[linkIndex - 1] = links[linkIndex];
            }

        } while (linkIndex-- != 0);

        Ensure (foundNode == nullptr or fKeyThreeWayComparer_ (foundNode->fEntry.fKey, key) == strong_ordering::equal);
        return foundNode;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetFirst_ () const -> Link_*
    {
        Require (fHead_.size () >= 1);
        return fHead_[0];
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::GetLast_ () const -> Link_*
    {
        Require (fHead_.size () >= 1);
        size_t linkIndex = fHead_.size () - 1;
        Link_* n         = fHead_[linkIndex];
        if (n != nullptr) {
            Link_* prev = n;
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
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    template <typename FUNCTION>
    inline void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Apply (FUNCTION&& doToElement) const
    {
        Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
        std::for_each (begin (), end (), forward<FUNCTION> (doToElement));
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Prioritize (ArgByValueType<key_type> key)
    {
        LinkVector_ links;
        Link_*      node = FindNearest_ (key, links);
        if (node != nullptr and node->fNext.size () <= fHead_.size ()) {
            if (node->fNext.size () == fHead_.size ()) {
                GrowHeadLinksIfNeeded_ (fHead_.size () + 1, node);
                links.resize (fHead_.size (), node);
            }
            size_t oldLinkHeight = node->fNext.size ();
            node->fNext.resize (fHead_.size (), nullptr);
            size_t newLinkHeight = node->fNext.size ();
            Assert (oldLinkHeight < newLinkHeight);
            for (size_t i = oldLinkHeight; i <= newLinkHeight - 1; ++i) {
                if (links[i] == nullptr) {
                    fHead_[i] = node;
                }
                else if (links[i] == node) {
                    break;
                }
                else {
                    if constexpr (same_as<SkipList_Support::Stats_Basic, StatsType>) {
                        ++fStats_.fRotations;
                    }
                    Link_* oldLink = links[i];
                    AssertNotNull (oldLink);
                    Assert (oldLink->fNext.size () > i);
                    Link_* nextL      = oldLink->fNext[i];
                    oldLink->fNext[i] = node;
                    node->fNext[i]    = nextL;
                }
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ReBalance ()
    {
        if (empty ()) [[unlikely]] {
            return;
        }
        // precompute table of indices height
        // idea is to have a link for every log power of the probability at a particular index
        // for example, for a 1/2 chance, have heights start as 1 2 1 3 1 2 1 4
        double indexBase = (GetLinkHeightProbability () == 0) ? 0 : 1 / (GetLinkHeightProbability () / 100.0);
        size_t height[kMaxLinkHeight_];
        size_t lastValidHeight = 0;
        for (size_t i = 0; i < sizeof (height) / sizeof (size_t); ++i) {
            height[i] = size_t (pow (indexBase, double (i)));

            if (height[i] == 0 or height[i] > size ()) {
                Assert (i > 0); // else have no valid heights
                break;
            }
            lastValidHeight = i;
        }
        // wipe out everything, keeping only a link to the first item in list
        Link_* node = fHead_[0];
        fHead_.clear ();
        fHead_.resize (kMaxLinkHeight_);

        Link_** patchNodes[kMaxLinkHeight_];
        for (size_t i = 0; i < sizeof (patchNodes) / sizeof (size_t); ++i) {
            patchNodes[i] = &fHead_[i];
        }

        size_t index = 1;
        while (node != nullptr) {
            Link_* next = node->fNext[0];
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

            ++index;
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
        for (size_t i = 0; i < fHead_.size (); ++i) {
            if (fHead_[i] == nullptr) {
                cout << "*"
                     << ", ";
            }
            else {
                cout << fHead_[i]->fEntry.GetValue () << ", ";
            }
        }
        cout << "]  ";

        Link_* n = fHead_[0];
        while (n != nullptr) {
            cout << n->fEntry.GetValue () << " (" << n->fNext.size () << "), ";
            n = n->fNext[0];
        }
        cout << endl << flush;
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    size_t SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::CalcHeight (size_t* totalHeight) const
    {
        if (totalHeight != nullptr) {
            *totalHeight         = 0;
            size_t maxLinkHeight = 0;
            Link_* n             = fHead_[0];
            while (n != nullptr) {
                maxLinkHeight = max (maxLinkHeight, n->fNext.size ());
                *totalHeight += n->fNext.size ();
                n = n->fNext[0];
            }
            Assert (maxLinkHeight == fHead_.size ());
        }
        return fHead_.size ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Invariant () const noexcept
    {
#if qDebug
        Invariant_ ();
#endif
    }
#if qDebug
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::Invariant_ () const noexcept
    {
        Link_* n = fHead_[0];
        while (n != nullptr) {
            KEY_TYPE oldKey = n->fEntry.fKey;
            for (size_t i = 1; i < n->fNext.size (); ++i) {
                Link_* newN = n->fNext[i];
                if (n == nullptr) {
                    Assert (newN == nullptr);
                }
                else {
                    Assert (newN != n);
                    Assert (newN == nullptr or fKeyThreeWayComparer_ (oldKey, newN->fEntry.fKey) != strong_ordering::greater);
                }
            }
            n = n->fNext[0];
            Assert (n == nullptr or (fKeyThreeWayComparer_ (n->fEntry.fKey, oldKey) != strong_ordering::less));
        }
    }
#endif

    /*
     ********************************************************************************
     *********** SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::ForwardIterator (const SkipList* data, UnderlyingIteratorRep startAt)
        : fData_{data}
        , fCurrent_{startAt}
    {
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::ForwardIterator (const SkipList* data)
        : ForwardIterator{data, data->fHead_[0]}
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
    inline SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator bool () const
    {
        return not Done ();
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::Done () const noexcept -> bool
    {
        return fCurrent_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator* () const -> value_type
    {
        RequireNotNull (fCurrent_);
        return fCurrent_->fEntry;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator->() const -> const value_type*
    {
        RequireNotNull (fCurrent_);
        return &fCurrent_->fEntry;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::CurrentIndex () const -> size_t
    {
        Require (not Done ());
        RequireNotNull (fData_);
        RequireNotNull (this->fCurrent_);
        size_t i = 0;
        for (const Link_* l = fData_->fHead_; l != fCurrent_; l = l->fNext[0], ++i) {
            AssertNotNull (l);
        }
        return i;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline constexpr bool SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator== (const ForwardIterator& rhs) const
    {
        Require (fData_ == nullptr or rhs.fData_ == nullptr or fData_ == rhs.fData_);
        return fCurrent_ == rhs.fCurrent_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    constexpr auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::GetUnderlyingIteratorRep () const -> UnderlyingIteratorRep
    {
        return fCurrent_;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    inline void SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::SetUnderlyingIteratorRep (const UnderlyingIteratorRep l)
    {
        fCurrent_ = l;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator++ () -> ForwardIterator&
    {
        fCurrent_ = fCurrent_->fNext[0];
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_TYPE, SkipList_Support::IValidTraits<KEY_TYPE> TRAITS>
    auto SkipList<KEY_TYPE, MAPPED_TYPE, TRAITS>::ForwardIterator::operator++ (int) -> ForwardIterator
    {
        ForwardIterator result = *this;
        this->operator++ ();
        return result;
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
        fData_->Invariant (); // @todo more... fNode from somewhere inside fData_....
    }
#endif

}
