/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_inl_
#define _Stroika_Foundation_Traversal_Iterable_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <bitset>
#include <set>

#include "../Debug/Assertions.h"

#include "Generator.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ********************************* IterableBase *********************************
     ********************************************************************************
     */
    template <typename SHARED_T, typename... ARGS_TYPE>
    inline auto IterableBase::MakeSmartPtr (ARGS_TYPE&&... args) -> PtrImplementationTemplate<SHARED_T>
    {
        if constexpr (kIterableUsesStroikaSharedPtr) {
            return Memory::MakeSharedPtr<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
        else {
            return make_shared<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
    }

    /*
     ********************************************************************************
     *************************** Iterable<T>::_IRep *********************************
     ********************************************************************************
     */
    template <typename T>
    inline bool Iterable<T>::_IRep::_IsEmpty () const
    {
        return GetLength () == 0;
    }
    template <typename T>
    inline void Iterable<T>::_IRep::_Apply (_APPLY_ARGTYPE doToElement) const
    {
        RequireNotNull (doToElement);
        for (Iterator<T> i = MakeIterator (this); i != Iterable<T>::end (); ++i) {
            (doToElement) (*i);
        }
    }
    template <typename T>
    inline Iterator<T> Iterable<T>::_IRep::_FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
    {
        RequireNotNull (doToElement);
        for (Iterator<T> i = MakeIterator (suggestedOwner); i != Iterable<T>::end (); ++i) {
            if ((doToElement) (*i)) {
                return i;
            }
        }
        return end ();
    }

    /*
     ********************************************************************************
     ******************* Iterable<T>::_SafeReadRepAccessor **************************
     ********************************************************************************
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const Iterable<T>* it) noexcept
        : shared_lock<const Debug::AssertExternallySynchronizedLock> (*it)
        , fConstRef_ (static_cast<const REP_SUB_TYPE*> (it->_fRep.cget ()))
#if qDebug
        , fIterableEnvelope_ (it)
#endif
    {
        RequireNotNull (it);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const _SafeReadRepAccessor& src) noexcept
#if qDebug
        : shared_lock<const Debug::AssertExternallySynchronizedLock> (*src.fIterableEnvelope_)
#else
        : shared_lock<const Debug::AssertExternallySynchronizedLock> (*(const Iterable<T>*)nullptr)
#endif
        , fConstRef_ (src.fConstRef_)
#if qDebug
        , fIterableEnvelope_ (src.fIterableEnvelope_)
#endif
    {
        RequireNotNull (fConstRef_);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (_SafeReadRepAccessor&& src) noexcept
        : shared_lock<const Debug::AssertExternallySynchronizedLock> (move<const Debug::AssertExternallySynchronizedLock> (src))
        , fConstRef_ (src.fConstRef_)
#if qDebug
        , fIterableEnvelope_ (src.fIterableEnvelope_)
#endif
    {
        RequireNotNull (fConstRef_);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
        src.fConstRef_ = nullptr;
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline auto Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::operator= (const _SafeReadRepAccessor& rhs) noexcept -> _SafeReadRepAccessor&
    {
        shared_lock<const Debug::AssertExternallySynchronizedLock>::operator= (rhs);
        fConstRef_                                                          = rhs.fConstRef_;
#if qDebug
        fIterableEnvelope_ = rhs.fIterableEnvelope_;
#endif
        return *this;
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline const REP_SUB_TYPE& Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const noexcept
    {
        EnsureMember (fConstRef_, REP_SUB_TYPE);
        return *fConstRef_;
    }

    /*
     ********************************************************************************
     ************* Iterable<CONTAINER_OF_T, T>::_SafeReadWriteRepAccessor ***********
     ********************************************************************************
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (Iterable<T>* iterableEnvelope)
        : lock_guard<const Debug::AssertExternallySynchronizedLock> (*iterableEnvelope)
        , fIterableEnvelope_ (iterableEnvelope)
        , fRepReference_ (static_cast<REP_SUB_TYPE*> (iterableEnvelope->_fRep.get (iterableEnvelope)))
    {
        RequireNotNull (iterableEnvelope);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (_SafeReadWriteRepAccessor&& from)
        : lock_guard<const Debug::AssertExternallySynchronizedLock> (move<const Debug::AssertExternallySynchronizedLock> (from))
        , fIterableEnvelope_ (from.fIterableEnvelope_)
        , fRepReference_ (from.fRepReference_)
    {
        RequireNotNull (fRepReference_);
        EnsureMember (fRepReference_, REP_SUB_TYPE);
        from.fIterableEnvelope_ = nullptr;
        from.fRepReference_     = nullptr;
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline const REP_SUB_TYPE& Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
    {
        EnsureNotNull (fRepReference_);
        return *fRepReference_;
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline REP_SUB_TYPE& Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_GetWriteableRep ()
    {
        EnsureNotNull (fRepReference_);
        return *fRepReference_;
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline void Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_UpdateRep (const typename _SharedByValueRepType::shared_ptr_type& sp)
    {
        RequireNotNull (sp);
        EnsureNotNull (fIterableEnvelope_);
        fIterableEnvelope_->_fRep = sp;
    }

    /*
     ********************************************************************************
     ********************************** Iterable<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    inline Iterable<T>::Iterable (const _IterableRepSharedPtr& rep) noexcept
        : _fRep (rep)
    {
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
    }
    template <typename T>
    inline Iterable<T>::Iterable (_IterableRepSharedPtr&& rep) noexcept
        : _fRep (move (rep))
    {
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
        if constexpr (!kIterableUsesStroikaSharedPtr) {
            Require (rep == nullptr); // after move (see https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr "After the construction, ... r is empty and its stored pointer is null"
        }
    }
    template <typename T>
    inline Iterable<T>::Iterable (const Iterable& from) noexcept
        : _fRep (from._fRep)
    {
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
    }
    template <typename T>
    inline Iterable<T>::Iterable (Iterable&& from) noexcept
        : _fRep (from._fRep)
    {
        /*
         *  SEE DESIGN NOTE in class (and https://stroika.atlassian.net/browse/STK-541) - why we intentionally use copy of rep not MOVE
         */
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
    }
    template <typename T>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_base_of_v<Iterable<T>, Configuration::remove_cvref_t<CONTAINER_OF_T>>>*>
    Iterable<T>::Iterable (CONTAINER_OF_T&& from)
        : _fRep (mk_ (forward<CONTAINER_OF_T> (from))._fRep)
    {
    }
    template <typename T>
    Iterable<T>::Iterable (const initializer_list<T>& from)
        : _fRep (mk_ (from)._fRep)
    {
    }
    template <typename T>
    inline Iterable<T>& Iterable<T>::operator= (const Iterable& rhs)
    {
        RequireNotNull (rhs._fRep);
        _fRep = rhs._fRep;
        return *this;
    }
    template <typename T>
    inline Iterable<T>& Iterable<T>::operator= (Iterable&& rhs) noexcept
    {
        RequireNotNull (rhs._fRep);
        _fRep = move (rhs._fRep);
        return *this;
    }
    template <typename T>
    inline typename Iterable<T>::_IterableRepSharedPtr Iterable<T>::Clone_ (const _IRep& rep, IteratorOwnerID forIterableEnvelope)
    {
        return rep.Clone (forIterableEnvelope);
    }
    template <typename T>
    template <typename CONTAINER_OF_T>
    Iterable<T> Iterable<T>::mk_ (CONTAINER_OF_T&& from)
    {
        vector<T>                tmp (from.begin (), from.end ()); // Somewhat simplistic / inefficient implementation
        size_t                   idx{0};
        function<optional<T> ()> getNext = [tmp, idx] () mutable -> optional<T> {
            if (idx < tmp.size ())
                [[LIKELY_ATTR]]
                {
                    return tmp[idx++];
                }
            else {
                return nullopt;
            }
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    inline Memory::SharedByValue_State Iterable<T>::_GetSharingState () const
    {
        return _fRep.GetSharingState ();
    }
    template <typename T>
    inline Iterator<T> Iterable<T>::MakeIterator () const
    {
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().MakeIterator (this);
    }
    template <typename T>
    inline size_t Iterable<T>::GetLength () const
    {
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().GetLength ();
    }
    template <typename T>
    inline bool Iterable<T>::IsEmpty () const
    {
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().IsEmpty ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Iterable<T>::Contains (ArgByValueType<T> element, const EQUALS_COMPARER& equalsComparer) const
    {
        // grab iterator to first matching item, and contains if not at end; this is faster than using iterators
        return static_cast<bool> (this->FindFirstThat ([&element, &equalsComparer] (T i) -> bool {
            return equalsComparer (i, element);
        }));
    }
    template <typename T>
    template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    bool Iterable<T>::SetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer)
    {
        /*
         *  An extremely inefficient but space-constant implementation. N^2 and check
         *  a contains b and b contains a
         */
        for (auto&& ti : lhs) {
            bool contained = false;
            for (auto ri : rhs) {
                if (equalsComparer (ti, ri)) {
                    contained = true;
                    break;
                }
            }
            if (not contained) {
                return false;
            }
        }
        for (auto&& ri : rhs) {
            bool contained = false;
            for (auto ti : lhs) {
                if (equalsComparer (ti, ri)) {
                    contained = true;
                    break;
                }
            }
            if (not contained) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    inline bool Iterable<T>::SetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer) const
    {
        return SetEquals (*this, rhs, equalsComparer);
    }
    template <typename T>
    template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    bool Iterable<T>::MultiSetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer)
    {
        auto tallyOf = [&equalsComparer] (const auto& c, T item) -> size_t {
            size_t total = 0;
            for (auto ti : c) {
                if (equalsComparer (ti, item)) {
                    total++;
                }
            }
            return total;
        };
        /*
         *  An extremely in-efficient but space-constant implementation. N^3 and check
         *  a contains b and b contains a
         */
        for (auto&& ti : lhs) {
            if (tallyOf (lhs, ti) != tallyOf (rhs, ti)) {
                return false;
            }
        }
        for (auto&& ti : rhs) {
            if (tallyOf (lhs, ti) != tallyOf (rhs, ti)) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    inline bool Iterable<T>::MultiSetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer) const
    {
        return MultiSetEquals (*this, rhs, equalsComparer);
    }
    template <typename T>
    template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    bool Iterable<T>::SequentialEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer)
    {
        auto li{lhs.begin ()};
        auto le{lhs.end ()};
        auto ri{rhs.begin ()};
        auto re{rhs.end ()};
        for (; li != le and ri != re; ++ri, ++li) {
            if (not equalsComparer (*li, *ri)) {
                return false;
            }
        }
        // one caused us to end (or more likely both)
        Assert (li == le or ri == re);
        // only true if we get to end at the same time
        return li == le and ri == re;
    }
    template <typename T>
    template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    inline bool Iterable<T>::SequentialEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer) const
    {
        return SequentialEquals (*this, rhs, equalsComparer);
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Iterable<T>::SequenceEquals (const Iterable& lhs, const Iterable& rhs, const EQUALS_COMPARER& equalsComparer)
    {
        if (lhs.GetLength () != rhs.GetLength ()) {
            return false;
        }
        auto li = lhs.MakeIterator ();
        auto ri = rhs.MakeIterator ();
        Assert (li.Done () == ri.Done ()); // cuz same length, and this requires GetLength cannot change during call
        while (not li.Done ()) {
            if (not equalsComparer (*li, *ri)) {
                return false;
            }
            ++li;
            ++ri;
            Assert (li.Done () == ri.Done ()); // cuz same length, and this requires GetLength cannot change during call
        }
        Assert (li.Done () and ri.Done ());
        return true;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline bool Iterable<T>::SequenceEquals (const Iterable& rhs, const EQUALS_COMPARER& equalsComparer) const
    {
        return SequenceEquals (*this, rhs, equalsComparer);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const
    {
        RequireNotNull (includeIfTrue);
        Iterable<T>              copyOfIterableSoRefCntBumpedInLambda = *this;
        Iterator<T>              tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
        function<optional<T> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, includeIfTrue] () mutable -> optional<T> {
            while (tmpIt and not includeIfTrue (*tmpIt)) {
                ++tmpIt;
            }
            return tmpIt ? *tmpIt++ : nullptr;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename RESULT_CONTAINER>
    inline RESULT_CONTAINER Iterable<T>::Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const
    {
        return Where<RESULT_CONTAINER> (includeIfTrue, RESULT_CONTAINER{});
    }
    template <typename T>
    template <typename RESULT_CONTAINER>
    RESULT_CONTAINER Iterable<T>::Where (const function<bool (ArgByValueType<T>)>& includeIfTrue, const RESULT_CONTAINER& emptyResult) const
    {
        Require (emptyResult.empty ());
        RESULT_CONTAINER result = emptyResult;
        for (auto i : Where (includeIfTrue)) {
            result.Add (i);
        }
        return result;
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    Iterable<T> Iterable<T>::Distinct (const EQUALS_COMPARER& equalsComparer) const
    {
        vector<T> tmp; // Simplistic/stupid/weak implementation
        if constexpr (is_same_v<equal_to<T>, EQUALS_COMPARER> and is_invocable_v<less<T>>) {
            set<T> t1 (begin (), end ());
            tmp = vector<T> (t1.begin (), t1.end ());
        }
        else {
            for (const T&& i : *this) {
                if (find_if (tmp.begin (), tmp.end (), [&] (ArgByValueType<T> n) { return equalsComparer (n, i); }) == tmp.end ()) {
                    tmp.push_back (i);
                }
            }
        }
        return Distinct_mkGenerator_<T> (tmp);
    }
    template <typename T>
    template <typename RESULT, typename EQUALS_COMPARER>
    Iterable<RESULT> Iterable<T>::Distinct (const function<RESULT (ArgByValueType<T>)>& extractElt, const EQUALS_COMPARER& equalsComparer) const
    {
        RequireNotNull (extractElt);
        vector<RESULT> tmp; // Simplistic/stupid/weak implementation
        if constexpr (is_same_v<equal_to<T>, EQUALS_COMPARER> and is_invocable_v<less<T>>) {
            set<RESULT> t1;
            for (T i : *this) {
                t1.add (extractElt (i));
            }
            tmp = vector<RESULT> (t1.begin (), t1.end ());
        }
        else {
            for (const T&& i : *this) {
                RESULT item2Test = extractElt (i);
                if (find_if (tmp.begin (), tmp.end (), [&] (ArgByValueType<T> n) { return equalsComparer (n, item2Test); }) == tmp.end ()) {
                    tmp.push_back (item2Test);
                }
            }
        }
        return Distinct_mkGenerator_<RESULT> (tmp);
    }
    template <typename T>
    template <typename RESULT>
    Iterable<RESULT> Iterable<T>::Distinct_mkGenerator_ (const vector<RESULT>& container)
    {
        size_t                        idx{0};
        function<optional<RESULT> ()> getNext = [container, idx] () mutable -> optional<RESULT> {
            if (idx < container.size ()) {
                return container[idx++];
            }
            else {
                return nullopt;
            }
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename T1, typename RESULT>
    Iterable<RESULT> Iterable<T>::Select (const function<T1 (const T&)>& extract1) const
    {
        RequireNotNull (extract1);
        Iterable<T>                   copyOfIterableSoRefCntBumpedInLambda = *this;
        Iterator<T>                   tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
        function<optional<RESULT> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, extract1] () mutable -> optional<RESULT> {
            if (tmpIt) {
                return RESULT (extract1 (*tmpIt++));
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename T1, typename T2, typename RESULT>
    Iterable<RESULT> Iterable<T>::Select (const function<T1 (const T&)>& extract1, const function<T2 (const T&)>& extract2) const
    {
        RequireNotNull (extract1);
        RequireNotNull (extract2);
        Iterable<T>                   copyOfIterableSoRefCntBumpedInLambda = *this;
        Iterator<T>                   tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
        function<optional<RESULT> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, extract1, extract2] () mutable -> optional<RESULT> {
            if (tmpIt) {
                RESULT result{extract1 (*tmpIt), extract2 (*tmpIt)};
                tmpIt++;
                return result;
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename T1, typename T2, typename T3, typename RESULT>
    Iterable<RESULT> Iterable<T>::Select (const function<T1 (const T&)>& extract1, const function<T2 (const T&)>& extract2, const function<T3 (const T&)>& extract3) const
    {
        Iterable<T>                   copyOfIterableSoRefCntBumpedInLambda = *this;
        Iterator<T>                   tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
        function<optional<RESULT> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, extract1, extract2, extract3] () mutable -> optional<RESULT> {
            if (tmpIt) {
                RESULT result{extract1 (*tmpIt), extract2 (*tmpIt), extract3 (*tmpIt)};
                tmpIt++;
                return result;
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Skip (size_t nItems) const
    {
        size_t                   nItemsToSkip                         = nItems;
        Iterable<T>              copyOfIterableSoRefCntBumpedInLambda = *this;
        Iterator<T>              tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
        function<optional<T> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, nItemsToSkip] () mutable -> optional<T> {
            while (tmpIt and nItemsToSkip > 0) {
                nItemsToSkip--;
                ++tmpIt;
            }
            return tmpIt ? *tmpIt++ : nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Take (size_t nItems) const
    {
        size_t                   nItemsToTake                         = nItems;
        Iterable<T>              copyOfIterableSoRefCntBumpedInLambda = *this;
        Iterator<T>              tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
        function<optional<T> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, nItemsToTake] () mutable -> optional<T> {
            if (nItemsToTake == 0) {
                return nullopt;
            }
            nItemsToTake--;
            return tmpIt ? *tmpIt++ : nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Slice (size_t from, size_t to) const
    {
        size_t                   nItemsToSkip                         = from;
        size_t                   nItemsToTake                         = to - from;
        Iterable<T>              copyOfIterableSoRefCntBumpedInLambda = *this;
        Iterator<T>              tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
        function<optional<T> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, nItemsToSkip, nItemsToTake] () mutable -> optional<T> {
            while (tmpIt and nItemsToSkip > 0) {
                nItemsToSkip--;
                ++tmpIt;
            }
            if (nItemsToTake == 0) {
                return nullopt;
            }
            nItemsToTake--;
            return tmpIt ? *tmpIt++ : nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename INORDER_COMPARER_TYPE>
    Iterable<T> Iterable<T>::OrderBy (const INORDER_COMPARER_TYPE& inorderComparer) const
    {
        vector<T> tmp (begin (), end ()); // Somewhat simplistic implementation
        stable_sort (tmp.begin (), tmp.end (), inorderComparer);
        size_t                   idx{0};
        function<optional<T> ()> getNext = [tmp, idx] () mutable -> optional<T> {
            if (idx < tmp.size ()) {
                return tmp[idx++];
            }
            else {
                return nullopt;
            }
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    inline optional<T> Iterable<T>::First () const
    {
        auto i = begin ();
        return i ? *i : nullopt;
    }
    template <typename T>
    inline optional<T> Iterable<T>::First (const function<bool (ArgByValueType<T>)>& that) const
    {
        RequireNotNull (that);
        for (auto i : *this) {
            if (that (i)) {
                return i;
            }
        }
        return nullopt;
    }
    template <typename T>
    inline T Iterable<T>::FirstValue (ArgByValueType<T> defaultValue) const
    {
        if (auto i = begin ()) {
            return *i;
        }
        else {
            return defaultValue;
        }
    }
    template <typename T>
    optional<T> Iterable<T>::Last () const
    {
        auto i = begin ();
        if (i) {
            auto prev = i;
            while (i) {
                prev = i;
                ++i;
            }
            return *prev;
        }
        return nullopt;
    }
    template <typename T>
    optional<T> Iterable<T>::Last (const function<bool (ArgByValueType<T>)>& that) const
    {
        RequireNotNull (that);
        optional<T> result;
        for (auto i : *this) {
            if (that (i)) {
                result = i;
            }
        }
        return result;
    }
    template <typename T>
    T Iterable<T>::LastValue (ArgByValueType<T> defaultValue) const
    {
        if (auto i = begin ()) {
            auto prev = i;
            while (i) {
                prev = i;
                ++i;
            }
            return *prev;
        }
        else {
            return defaultValue;
        }
    }
    template <typename T>
    bool Iterable<T>::All (const function<bool (ArgByValueType<T>)>& testEachElt) const
    {
        RequireNotNull (testEachElt);
        for (auto i : *this) {
            if (not testEachElt (i)) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    template <typename RESULT_TYPE>
    optional<RESULT_TYPE> Iterable<T>::Accumulate (const function<RESULT_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op) const
    {
        optional<RESULT_TYPE> result;
        for (T i : *this) {
            if (result) {
                result = op (i, *result);
            }
            else {
                result = i;
            }
        }
        return result;
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::AccumulateValue (const function<RESULT_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op, ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Accumulate<RESULT_TYPE> (op).Value (defaultValue);
    }
    template <typename T>
    optional<T> Iterable<T>::Min () const
    {
        return Accumulate<T> ([] (T lhs, T rhs) { return min (lhs, rhs); });
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::MinValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Min ().Value (defaultValue);
    }
    template <typename T>
    optional<T> Iterable<T>::Max () const
    {
        return Accumulate<T> ([] (T lhs, T rhs) -> T { return max (lhs, rhs); });
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::MaxValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Max ().Value (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE>
    optional<RESULT_TYPE> Iterable<T>::Mean () const
    {
        RESULT_TYPE  result{};
        unsigned int count{};
        for (T i : *this) {
            count++;
            result += i;
        }
        return (count == 0) ? optional<RESULT_TYPE>{} : (result / count);
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::MeanValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Mean ().value_or (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE>
    optional<RESULT_TYPE> Iterable<T>::Sum () const
    {
        return Accumulate<RESULT_TYPE> ([] (T lhs, T rhs) { return lhs + rhs; });
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::SumValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Sum ().Value (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE, typename INORDER_COMPARE_FUNCTION>
    optional<RESULT_TYPE> Iterable<T>::Median (const INORDER_COMPARE_FUNCTION& compare) const
    {
        vector<T> tmp (begin (), end ()); // Somewhat simplistic implementation
        sort (tmp.begin (), tmp.end (), compare);
        size_t sz{tmp.size ()};
        if (sz == 0)
            [[UNLIKELY_ATTR]]
            {
                return nullopt;
            }
        if ((sz % 2) == 0) {
            return (static_cast<RESULT_TYPE> (tmp[sz / 2]) + static_cast<RESULT_TYPE> (tmp[sz / 2 - 1])) / static_cast<RESULT_TYPE> (2);
        }
        else {
            return tmp[sz / 2];
        }
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::MedianValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Median ().Value (defaultValue);
    }
    template <typename T>
    inline bool Iterable<T>::empty () const
    {
        return IsEmpty ();
    }
    template <typename T>
    inline bool Iterable<T>::Any () const
    {
        return not IsEmpty ();
    }
    template <typename T>
    inline bool Iterable<T>::Any (const function<bool (ArgByValueType<T>)>& includeIfTrue) const
    {
        return not Where (includeIfTrue).IsEmpty ();
    }
    template <typename T>
    inline size_t Iterable<T>::length () const
    {
        return GetLength ();
    }
    template <typename T>
    inline size_t Iterable<T>::size () const
    {
        return GetLength ();
    }
    template <typename T>
    inline Iterator<T> Iterable<T>::begin () const
    {
        return MakeIterator ();
    }
    template <typename T>
    constexpr Iterator<T> Iterable<T>::end ()
    {
        return Iterator<T>::GetEmptyIterator ();
    }
    template <typename T>
    inline void Iterable<T>::Apply (const function<void (ArgByValueType<T> item)>& doToElement) const
    {
        RequireNotNull (doToElement);
        _SafeReadRepAccessor<>{this}._ConstGetRep ().Apply (doToElement);
    }
    template <typename T>
    inline Iterator<T> Iterable<T>::FindFirstThat (const function<bool (ArgByValueType<T> item)>& doToElement) const
    {
        RequireNotNull (doToElement);
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().FindFirstThat (doToElement, this);
    }
    template <typename T>
    inline Iterator<T> Iterable<T>::FindFirstThat (const Iterator<T>& startAt, const function<bool (ArgByValueType<T> item)>& doToElement) const
    {
        RequireNotNull (doToElement);
        for (Iterator<T> i = startAt; i != Iterable<T>::end (); ++i) {
            if ((doToElement) (*i)) {
                return i;
            }
        }
        return end ();
    }
    template <typename T>
    template <typename CONTAINER_OF_T>
    CONTAINER_OF_T Iterable<T>::As () const
    {
        return CONTAINER_OF_T (begin (), end ());
    }
    template <typename T>
    T Iterable<T>::Nth (size_t n) const
    {
        Require (n < size ());
        size_t idx = n;
        for (T i : *this) {
            if (idx == 0) {
                return i;
            }
            idx--;
        }
        AssertNotReached ();
        return *begin ();
    }
    template <typename T>
    T Iterable<T>::NthValue (size_t n, ArgByValueType<T> defaultValue) const
    {
        size_t idx = n;
        for (T i : *this) {
            if (idx == 0) {
                return i;
            }
            idx--;
        }
        return defaultValue;
    }

    template <typename T>
    template <typename T_THREEWAY_COMPARER>
    constexpr Iterable<T>::SequentialThreeWayComparer<T_THREEWAY_COMPARER>::SequentialThreeWayComparer (const T_THREEWAY_COMPARER& elementComparer)
        : fElementComparer{elementComparer}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_START (4701)
    template <typename T>
    template <typename T_THREEWAY_COMPARER>
    inline int Iterable<T>::SequentialThreeWayComparer<T_THREEWAY_COMPARER>::operator() (const Iterable& lhs, const Iterable& rhs) const
    {
        auto li = lhs.begin ();
        auto le = lhs.end ();
        auto ri = rhs.begin ();
        auto re = rhs.end ();
        DISABLE_COMPILER_MSC_WARNING_START (6001)
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
        // no need for c' initialization cuz only used in else return at end, but never get there
        // unless set at least once
        int c;
        while ((li != le) and (ri != re) and (c = fElementComparer (*li, *ri)) == 0) {
            ++li;
            ++ri;
        }
        if (li == le) {
            if (ri == re) {
                return 0; // all items same and loop ended with both things at end
            }
            else {
                return -1; // lhs shorter but an initial sequence of rhs
            }
        }
        else if (ri == re) {
            return 1; // rhs shorter but an initial sequence of lhs
        }
        else {
            Assert (li != le and ri != re);
            Assert (c == fElementComparer (*li, *ri));
            return c;
        }
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
        DISABLE_COMPILER_MSC_WARNING_END (6001)
    }
    DISABLE_COMPILER_MSC_WARNING_END (4701)
}

#endif /* _Stroika_Foundation_Traversal_Iterable_inl_ */
