/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
        else if constexpr (Memory::UsesBlockAllocation<SHARED_T> ()) {
            // almost as good, but still does two allocs, above does one shared alloc of the block allocated controlblock+SHARED_T
            //return shared_ptr<SHARED_T> (new SHARED_T (forward<ARGS_TYPE> (args)...));
            return allocate_shared<SHARED_T> (Memory::BlockAllocator<SHARED_T>{}, forward<ARGS_TYPE> (args)...);
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
        return size () == 0;
    }
    template <typename T>
    inline void Iterable<T>::_IRep::_Apply (const function<void (ArgByValueType<T> item)>& doToElement) const
    {
        RequireNotNull (doToElement);
        for (Iterator<T> i = MakeIterator (); i != Iterable<T>::end (); ++i) {
            doToElement (*i);
        }
    }
    template <typename T>
    template <typename THAT_FUNCTION, enable_if_t<Configuration::IsTPredicate<T, THAT_FUNCTION> ()>*>
    inline Iterator<T> Iterable<T>::_IRep::_Find (THAT_FUNCTION&& that) const
    {
        RequireNotNull (that);
        for (Iterator<T> i = MakeIterator (); i != end (); ++i) {
            if (that (*i)) {
                return i;
            }
        }
        return end ();
    }
    template <typename T>
    auto Iterable<T>::_IRep::_Find_equal_to_default_implementation ([[maybe_unused]] const ArgByValueType<value_type>& v) const -> Iterator<value_type>
    {
        if constexpr (Configuration::HasUsableEqualToOptimization<T> ()) {
            /*
             *  This is the default implementation. It is only ever compiled if there is a valid equal_to<> around, and
             *  that valid equal_to<> is stateless (verified by Configuration::HasUsableEqualToOptimization).
             */
            for (Iterator<T> i = MakeIterator (); i != end (); ++i) {
                if (equal_to<T>{}(v, *i)) {
                    return i;
                }
            }
            return end ();
        }
        else {
            RequireNotReached ();
            return end ();
        }
    }

    /*
     ********************************************************************************
     ******************* Iterable<T>::_SafeReadRepAccessor **************************
     ********************************************************************************
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const Iterable<T>* it) noexcept
        : shared_lock<const Debug::AssertExternallySynchronizedMutex>{*it}
        // clang-format off
        , fConstRef_{static_cast<const REP_SUB_TYPE*> (it->_fRep.cget ())}
#if qDebug
        , fIterableEnvelope_{it}
#endif
    // clang-format on
    {
        RequireNotNull (it);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const _SafeReadRepAccessor& src) noexcept
    // clang-format off
#if qDebug
        : shared_lock<const Debug::AssertExternallySynchronizedMutex>{*src.fIterableEnvelope_}
#else
        : shared_lock<const Debug::AssertExternallySynchronizedMutex>{*(const Iterable<T>*)nullptr}
#endif
        , fConstRef_{src.fConstRef_}
#if qDebug
        , fIterableEnvelope_{src.fIterableEnvelope_}
#endif
    // clang-format on
    {
        RequireNotNull (fConstRef_);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (_SafeReadRepAccessor&& src) noexcept
        : shared_lock<const Debug::AssertExternallySynchronizedMutex> (move<const Debug::AssertExternallySynchronizedMutex> (src))
        // clang-format off
        , fConstRef_{src.fConstRef_}
#if qDebug
        , fIterableEnvelope_{src.fIterableEnvelope_}
#endif
    // clang-format on
    {
        RequireNotNull (fConstRef_);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
        src.fConstRef_ = nullptr;
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline auto Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::operator= (const _SafeReadRepAccessor& rhs) noexcept -> _SafeReadRepAccessor&
    {
        shared_lock<const Debug::AssertExternallySynchronizedMutex>::operator= (rhs);
        fConstRef_                                                           = rhs.fConstRef_;
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
        : lock_guard<const Debug::AssertExternallySynchronizedMutex>{*iterableEnvelope}
        , fIterableEnvelope_{iterableEnvelope}
        , fRepReference_{static_cast<REP_SUB_TYPE*> (iterableEnvelope->_fRep.rwget ())}
    {
        RequireNotNull (iterableEnvelope);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (_SafeReadWriteRepAccessor&& from)
        : lock_guard<const Debug::AssertExternallySynchronizedMutex>{move<const Debug::AssertExternallySynchronizedMutex> (from)}
        , fIterableEnvelope_{from.fIterableEnvelope_}
        , fRepReference_{from.fRepReference_}
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
        EnsureNotNull (fIterableEnvelope_);
        EnsureNotNull (fIterableEnvelope_->_fRep);
        Ensure (fIterableEnvelope_->_fRep.use_count () == 1);
        return *fRepReference_;
    }

    /*
     ********************************************************************************
     ********************************** Iterable<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    inline Iterable<T>::Iterable (const _IterableRepSharedPtr& rep) noexcept
        : _fRep{(RequireNotNull (rep), rep)}
    {
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
    }
    template <typename T>
    inline Iterable<T>::Iterable (_IterableRepSharedPtr&& rep) noexcept
        : _fRep{(RequireNotNull (rep), move (rep))}
    {
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
        if constexpr (!kIterableUsesStroikaSharedPtr) {
            Require (rep == nullptr); // after move (see https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr "After the construction, ... r is empty and its stored pointer is null"
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_T, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_T> and not is_base_of_v<Iterable<T>, decay_t<CONTAINER_OF_T>>>*>
    Iterable<T>::Iterable (CONTAINER_OF_T&& from)
        : _fRep{mk_ (forward<CONTAINER_OF_T> (from))._fRep}
    {
    }
    template <typename T>
    Iterable<T>::Iterable (const initializer_list<T>& from)
        : _fRep{mk_ (from)._fRep}
    {
    }
    template <typename T>
    inline Iterable<T>::operator bool () const
    {
        return not empty ();
    }
    template <typename T>
    inline typename Iterable<T>::_IterableRepSharedPtr Iterable<T>::Clone_ (const _IRep& rep)
    {
        return rep.Clone ();
    }
    template <typename T>
    template <typename CONTAINER_OF_T>
    Iterable<T> Iterable<T>::mk_ (CONTAINER_OF_T&& from)
    {
#if 1
        using DECAYED_CONTAINER = decay_t<CONTAINER_OF_T>;
        // Most containers are safe to use copy-by-value, except not initializer_list<> - not sure how to check for that generically...
        using USE_CONTAINER_TYPE                             = conditional_t<is_copy_constructible_v<DECAYED_CONTAINER> and not is_same_v<DECAYED_CONTAINER, initializer_list<T>>, DECAYED_CONTAINER, vector<T>>;
        shared_ptr<USE_CONTAINER_TYPE> sharedCopyOfContainer = make_shared<USE_CONTAINER_TYPE> (forward<CONTAINER_OF_T> (from));
        auto                           currentI              = sharedCopyOfContainer->begin ();
        function<optional<T> ()>       getNext               = [sharedCopyOfContainer, currentI] () mutable -> optional<T> {
            // Capture a SHARED reference to the container (so if this container is copied, we still refer to that same data
            // And capture a by-value refrence to the iterator so if it gets updated and copied we are referring to that point in the original iterable
            if (currentI != sharedCopyOfContainer->end ()) {
                return *currentI++; // intentionally increment AFTER returning value
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
#else
        // @todo consider if this should use forward_list<> and stroika blockallocator? Just have to be more careful on the indexing (using iterator carefully)
        // Or even better, maybe just capture 'from' itself (without references) in a shared_context(make_shared) and then do a regular iterator pointing
        // to that. THat should work fine, and be much cheaper
        // -- LGP 2021-12-03
        vector<T>                tmp{from.begin (), from.end ()}; // Somewhat simplistic / inefficient implementation
        size_t                   idx{0};
        function<optional<T> ()> getNext = [tmp, idx] () mutable -> optional<T> {
            if (idx < tmp.size ()) [[LIKELY_ATTR]] {
                return tmp[idx++];
            }
            else {
                return nullopt;
            }
        };
        return CreateGenerator (getNext);
#endif
    }
    template <typename T>
    inline Memory::SharedByValue_State Iterable<T>::_GetSharingState () const
    {
        return _fRep.GetSharingState ();
    }
    template <typename T>
    inline Iterator<T> Iterable<T>::MakeIterator () const
    {
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().MakeIterator ();
    }
    template <typename T>
    inline size_t Iterable<T>::size () const
    {
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().size ();
    }
    template <typename T>
    inline bool Iterable<T>::empty () const
    {
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().empty ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Iterable<T>::Contains (ArgByValueType<T> element, const EQUALS_COMPARER& equalsComparer) const
    {
        // grab iterator to first matching item, and contains if not at end; this is faster than using iterators
        return static_cast<bool> (this->Find ([&element, &equalsComparer] (T i) -> bool {
            return equalsComparer (i, element);
        }));
    }
    template <typename T>
    template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    bool Iterable<T>::SetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer)
    {
        // @todo OPTIMIZATION - check if contexpr EQUALS_COMPARE == equal_to and if less<> defined - and if so - construct a std::set<> to lookup/compare (do on shorter side)
        /*
         *  An extremely inefficient but space-constant implementation. N^2 and check
         *  a contains b and b contains a
         */
        for (const auto& ti : lhs) {
            bool contained = false;
            for (const auto& ri : rhs) {
                if (equalsComparer (ti, ri)) {
                    contained = true;
                    break;
                }
            }
            if (not contained) {
                return false;
            }
        }
        for (const auto& ri : rhs) {
            bool contained = false;
            for (const auto& ti : lhs) {
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
        auto tallyOf = [&equalsComparer] (const auto& c, Configuration::ArgByValueType<T> item) -> size_t {
            size_t total = 0;
            for (const auto& ti : c) {
                if (equalsComparer (ti, item)) {
                    ++total;
                }
            }
            return total;
        };
        /*
         *  An extremely in-efficient but space-constant implementation. N^3 and check
         *  a contains b and b contains a
         */
        for (const auto& ti : lhs) {
            if (tallyOf (lhs, ti) != tallyOf (rhs, ti)) {
                return false;
            }
        }
        for (const auto& ti : rhs) {
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
    template <typename LHS_CONTAINER_TYPE, typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> /* and Common::IsEqualsComparer<EQUALS_COMPARER> ()*/>*>
    bool Iterable<T>::SequentialEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer, bool useIterableSize)
    {
        if (useIterableSize) {
            if (lhs.size () != rhs.size ()) {
                return false;
            }
        }
        auto li{lhs.begin ()};
        auto ri{rhs.begin ()};
        auto le{lhs.end ()};
        if (useIterableSize) {
#if qDebug
            auto re{rhs.end ()};
#endif
            Assert ((li != le) == (ri != re)); // cuz same length, and this requires size cannot change during call
            while (li != le) {
                if (not equalsComparer (*li, *ri)) {
                    return false;
                }
                ++li;
                ++ri;
                Assert ((li != le) == (ri != re)); // cuz same length, and this requires size cannot change during call
            }
            Assert (li == le and ri == re);
            return true;
        }
        else {
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
    }
    template <typename T>
    template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<RHS_CONTAINER_TYPE> and Common::IsEqualsComparer<EQUALS_COMPARER> ()>*>
    inline bool Iterable<T>::SequentialEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer, bool useIterableSize) const
    {
        return SequentialEquals (*this, rhs, equalsComparer, useIterableSize);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const
    {
        RequireNotNull (includeIfTrue);
        // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
        auto sharedContext = make_shared<Iterable<T>> (*this);
        // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
        // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
        function<optional<T> ()> getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator (), includeIfTrue] () mutable -> optional<T> {
            while (perIteratorContextBaseIterator and not includeIfTrue (*perIteratorContextBaseIterator)) {
                ++perIteratorContextBaseIterator;
            }
            if (perIteratorContextBaseIterator) {
                auto tmp = *perIteratorContextBaseIterator;
                ++perIteratorContextBaseIterator;
                return move (tmp);
            }
            return nullopt;
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
        for (const auto& i : Where (includeIfTrue)) {
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
            set<T> t1{begin (), end ()};
            tmp = vector<T>{t1.begin (), t1.end ()};
        }
        else {
            for (const auto& i : *this) {
                if (find_if (tmp.begin (), tmp.end (), [&] (ArgByValueType<T> n) { return equalsComparer (n, i); }) == tmp.end ()) {
                    tmp.push_back (i);
                }
            }
        }
        size_t                   idx{0};
        function<optional<T> ()> getNext = [container = move (tmp), idx] () mutable -> optional<T> {
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
    template <typename RESULT, typename EQUALS_COMPARER>
    Iterable<RESULT> Iterable<T>::Distinct (const function<RESULT (ArgByValueType<T>)>& extractElt, const EQUALS_COMPARER& equalsComparer) const
    {
        RequireNotNull (extractElt);
        vector<RESULT> tmp; // Simplistic/stupid/weak implementation
        if constexpr (is_same_v<equal_to<T>, EQUALS_COMPARER> and is_invocable_v<less<T>>) {
            set<RESULT> t1;
            for (const T& i : *this) {
                t1.add (extractElt (i));
            }
            tmp = vector<RESULT>{t1.begin (), t1.end ()};
        }
        else {
            for (const T& i : *this) {
                RESULT item2Test = extractElt (i);
                if (find_if (tmp.begin (), tmp.end (), [&] (ArgByValueType<T> n) { return equalsComparer (n, item2Test); }) == tmp.end ()) {
                    tmp.push_back (item2Test);
                }
            }
        }
        size_t                        idx{0};
        function<optional<RESULT> ()> getNext = [container = move (tmp), idx] () mutable -> optional<RESULT> {
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
    template <typename RESULT>
    Iterable<RESULT> Iterable<T>::Select (const function<RESULT (const T&)>& extract) const
    {
        RequireNotNull (extract);
        // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
        auto sharedContext = make_shared<Iterable<T>> (*this);
        // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
        // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
        function<optional<RESULT> ()> getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator (), extract] () mutable -> optional<RESULT> {
            if (perIteratorContextBaseIterator) {
                RESULT result = extract (*perIteratorContextBaseIterator);
                ++perIteratorContextBaseIterator;
                return move (result);
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename RESULT>
    Iterable<RESULT> Iterable<T>::Select (const function<optional<RESULT> (const T&)>& extract) const
    {
        RequireNotNull (extract);
        // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
        auto sharedContext = make_shared<Iterable<T>> (*this);
        // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
        // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
        function<optional<RESULT> ()> getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator (), extract] () mutable -> optional<RESULT> {
            // tricky. The funtion we are defining returns nullopt as a sentinal to signal end of iteration. The function we are GIVEN returns nullopt
            // to signal skip this item. So adjust accordingly
            while (perIteratorContextBaseIterator) {
                optional<RESULT> t = extract (*perIteratorContextBaseIterator);
                ++perIteratorContextBaseIterator;
                if (t) {
                    return *t;
                }
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename RESULT>
    nonvirtual RESULT Iterable<T>::Join (const function<RESULT (const T&)>& convertToT, const function<RESULT (const RESULT&, const RESULT&)>& combine) const
    {
        RESULT result{};
        bool   firstTime{true};
        for (const auto& i : *this) {
            if (firstTime) {
                result    = convertToT (i);
                firstTime = false;
            }
            else {
                result = combine (result, convertToT (i));
            }
        }
        return result;
    }
#if 0
    /*
     *  LOGICALLY, we should put Iterable<T>::Join () String overloads here, but they must be defined in
     *  String.inl to avoid #include problems (that includes this anyhow, and this cannot include that because that includes this)
     */
    template <typename T>
    Characters::String Iterable<T>::Join (const function<Characters::String (const T&)>& convertToT, const function<Characters::String (const Characters::String&, const Characters::String&)>& combine) const
    template <typename T>
    inline Characters::String Iterable<T>::Join (const function<Characters::String (const T&)>& convertToT, const Characters::String& separator) const
    template <typename T>
    inline Characters::String Iterable<T>::Join (const function<Characters::String (const T&)>& convertToT) const
    template <typename T>
    inline Characters::String Iterable<T>::Join (const Characters::String& separator) const
    template <typename T>
    inline Characters::String Iterable<T>::Join () const
#endif
    template <typename T>
    Iterable<T> Iterable<T>::Skip (size_t nItems) const
    {
        // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
        auto sharedContext = make_shared<Iterable<T>> (*this);
        // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
        // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
        // perIteratorContextNItemsToSkip also must be cloned per iterator instance
        function<optional<T> ()> getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator (), perIteratorContextNItemsToSkip = nItems] () mutable -> optional<T> {
            while (perIteratorContextBaseIterator and perIteratorContextNItemsToSkip > 0) {
                --perIteratorContextNItemsToSkip;
                ++perIteratorContextBaseIterator;
            }
            if (perIteratorContextBaseIterator) {
                auto result = *perIteratorContextBaseIterator;
                ++perIteratorContextBaseIterator;
                return move (result);
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Take (size_t nItems) const
    {
        // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
        auto sharedContext = make_shared<Iterable<T>> (*this);
        // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
        // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
        // perIteratorContextNItemsToTake also must be cloned per iterator instance
        function<optional<T> ()> getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator (), perIteratorContextNItemsToTake = nItems] () mutable -> optional<T> {
            if (perIteratorContextNItemsToTake == 0) {
                return nullopt;
            }
            perIteratorContextNItemsToTake--;
            if (perIteratorContextBaseIterator) {
                auto result = *perIteratorContextBaseIterator;
                ++perIteratorContextBaseIterator;
                return move (result);
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Slice (size_t from, size_t to) const
    {
        // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
        auto sharedContext = make_shared<Iterable<T>> (*this);
        // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
        // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
        // perIteratorContextNItemsToSkip also must be cloned per iterator instance
        // perIteratorContextNItemsToTake also must be cloned per iterator instance
        function<optional<T> ()> getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator (), perIteratorContextNItemsToSkip = from, perIteratorContextNItemsToTake = to - from] () mutable -> optional<T> {
            while (perIteratorContextBaseIterator and perIteratorContextNItemsToSkip > 0) {
                --perIteratorContextNItemsToSkip;
                ++perIteratorContextBaseIterator;
            }
            if (perIteratorContextNItemsToTake == 0) {
                return nullopt;
            }
            perIteratorContextNItemsToTake--;
            if (perIteratorContextBaseIterator) {
                auto result = *perIteratorContextBaseIterator;
                ++perIteratorContextBaseIterator;
                return move (result);
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <typename INORDER_COMPARER_TYPE>
    Iterable<T> Iterable<T>::OrderBy (const INORDER_COMPARER_TYPE& inorderComparer) const
    {
        vector<T> tmp{begin (), end ()}; // Somewhat simplistic implementation (always over copy and index so no need to worry about iterator refereincing inside container)
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
        return i ? *i : optional<T>{};
    }
    template <typename T>
    template <typename RESULT_T>
    inline optional<RESULT_T> Iterable<T>::First (const function<optional<RESULT_T> (ArgByValueType<T>)>& that) const
    {
        RequireNotNull (that);
        for (const auto& i : *this) {
            if (auto r = that (i)) {
                return r;
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
    template <typename RESULT_T>
    inline optional<RESULT_T> Iterable<T>::Last (const function<optional<RESULT_T> (ArgByValueType<T>)>& that) const
    {
        RequireNotNull (that);
        optional<T> result;
        for (const auto& i : *this) {
            if (auto o = that (i)) {
                result = *o;
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
        for (const auto& i : *this) {
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
        for (const auto& i : *this) {
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
        return Accumulate<RESULT_TYPE> (op).value_or (defaultValue);
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
        return Min ().value_or (defaultValue);
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
        return Max ().value_or (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE>
    optional<RESULT_TYPE> Iterable<T>::Mean () const
    {
        RESULT_TYPE  result{};
        unsigned int count{};
        for (const T& i : *this) {
            ++count;
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
        return Sum ().value_or (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE, typename INORDER_COMPARE_FUNCTION>
    optional<RESULT_TYPE> Iterable<T>::Median (const INORDER_COMPARE_FUNCTION& compare) const
    {
        vector<T> tmp{begin (), end ()}; // Somewhat simplistic implementation
        sort (tmp.begin (), tmp.end (), compare);
        size_t sz{tmp.size ()};
        if (sz == 0) [[UNLIKELY_ATTR]] {
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
        return Median ().value_or (defaultValue);
    }
    template <typename T>
    Iterable<T> Iterable<T>::Repeat (size_t count) const
    {
        switch (count) {
            case 0:
                return Iterable<T>{};
            case 1:
                return *this;
            default: {
                // Somewhat simplistic / inefficient implementation
                vector<T>                origList{begin (), end ()};
                size_t                   repeatCountIndex{1}; // start at one, cuz we don't copy the zeroth time
                size_t                   innerIndex{0};
                function<optional<T> ()> getNext = [origList, repeatCountIndex, innerIndex, count] () mutable -> optional<T> {
                    Again:
                        if (innerIndex < origList.size ()) [[LIKELY_ATTR]] {
                            return origList[innerIndex++];
                        }
                        if (repeatCountIndex < count) [[LIKELY_ATTR]] {
                            ++repeatCountIndex;
                            innerIndex = 0;
                            goto Again;
                        }
                        return nullopt;
                };
                return CreateGenerator (getNext);
            }
        }
    }
    template <typename T>
    inline bool Iterable<T>::Any () const
    {
        return not empty ();
    }
    template <typename T>
    inline bool Iterable<T>::Any (const function<bool (ArgByValueType<T>)>& includeIfTrue) const
    {
        return not Where (includeIfTrue).empty ();
    }
    template <typename T>
    inline size_t Iterable<T>::length () const
    {
        return size ();
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
    template <typename THAT_FUNCTION, enable_if_t<Configuration::IsTPredicate<T, THAT_FUNCTION> ()>*>
    inline Iterator<T> Iterable<T>::Find (THAT_FUNCTION&& that) const
    {
        // NB: This transforms perfectly forwarded 'THAT_FUNCTION' and converts it to std::function<> - preventing further inlining at this point -
        // just so it can be done
        return _SafeReadRepAccessor<>{this}._ConstGetRep ().Find (that);
    }
    template <typename T>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<EQUALS_COMPARER, T> ()>*>
    inline Iterator<T> Iterable<T>::Find (Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer) const
    {
        if constexpr (is_same_v<Configuration::remove_cvref_t<EQUALS_COMPARER>, equal_to<T>> and Configuration::HasUsableEqualToOptimization<T> ()) {
            // This CAN be much faster than the default implementation for this special (but common) case (often a tree structure will have been maintained making this find faster)
            return _SafeReadRepAccessor<>{this}._ConstGetRep ().Find_equal_to (v);
        }
        else {
            return Find ([v, equalsComparer] (Configuration::ArgByValueType<T> arg) { return equalsComparer (v, arg); });
        }
    }
    template <typename T>
    template <typename THAT_FUNCTION, enable_if_t<Configuration::IsTPredicate<T, THAT_FUNCTION> ()>*>
    inline Iterator<T> Iterable<T>::Find (const Iterator<T>& startAt, THAT_FUNCTION&& that) const
    {
        for (Iterator<T> i = startAt; i != end (); ++i) {
            if (that (*i)) {
                return move (i);
            }
        }
        return end ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<EQUALS_COMPARER, T> ()>*>
    Iterator<T> Iterable<T>::Find (const Iterator<T>& startAt, Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer) const
    {
        for (Iterator<T> i = startAt; i != end (); ++i) {
            if (equalsComparer (v, *i)) {
                return move (i);
            }
        }
        return end ();
    }
    template <typename T>
    template <typename CONTAINER_OF_T>
    CONTAINER_OF_T Iterable<T>::As () const
    {
        return CONTAINER_OF_T (begin (), end ()); // use () instead of {} because we do want to allow coersion here - since use explictly called As<>
    }
    template <typename T>
    T Iterable<T>::Nth (size_t n) const
    {
        Require (n < size ());
        size_t idx = n;
        for (const T& i : *this) {
            if (idx == 0) {
                return i;
            }
            --idx;
        }
        AssertNotReached ();
        return *begin ();
    }
    template <typename T>
    T Iterable<T>::NthValue (size_t n, ArgByValueType<T> defaultValue) const
    {
        size_t idx = n;
        for (const T& i : *this) {
            if (idx == 0) {
                return i;
            }
            --idx;
        }
        return defaultValue;
    }

    /*
     ********************************************************************************
     ******************** Iterable<T>::SequentialEqualsComparer *********************
     ********************************************************************************
     */
    template <typename T>
    template <typename T_EQUALS_COMPARER>
    constexpr Iterable<T>::SequentialEqualsComparer<T_EQUALS_COMPARER>::SequentialEqualsComparer (const T_EQUALS_COMPARER& elementEqualsComparer, bool useIterableSize)
        : fElementComparer{elementEqualsComparer}
        , fUseIterableSize{useIterableSize}
    {
    }
    template <typename T>
    template <typename T_EQUALS_COMPARER>
    inline bool Iterable<T>::SequentialEqualsComparer<T_EQUALS_COMPARER>::operator() (const Iterable& lhs, const Iterable& rhs) const
    {
        return SequentialEquals (lhs, rhs, fElementComparer, fUseIterableSize);
    }

    /*
     ********************************************************************************
     ******************** Iterable<T>::SequentialThreeWayComparer *******************
     ********************************************************************************
     */
    template <typename T>
    template <typename T_THREEWAY_COMPARER>
    constexpr Iterable<T>::SequentialThreeWayComparer<T_THREEWAY_COMPARER>::SequentialThreeWayComparer (const T_THREEWAY_COMPARER& elementComparer)
        : fElementComparer{elementComparer}
    {
    }
    DISABLE_COMPILER_MSC_WARNING_START (4701)
    template <typename T>
    template <typename T_THREEWAY_COMPARER>
    inline auto Iterable<T>::SequentialThreeWayComparer<T_THREEWAY_COMPARER>::operator() (const Iterable& lhs, const Iterable& rhs) const
    {
        auto li = lhs.begin ();
        auto le = lhs.end ();
        auto ri = rhs.begin ();
        auto re = rhs.end ();
        DISABLE_COMPILER_MSC_WARNING_START (6001)
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
        // no need for c' initialization cuz only used in else return at end, but never get there
        // unless set at least once
        optional<Common::strong_ordering> c;
        while ((li != le) and (ri != re) and (c = fElementComparer (*li, *ri)) == Common::kEqual) {
            ++li;
            ++ri;
        }
        if (li == le) {
            if (ri == re) {
                return Common::kEqual; // all items same and loop ended with both things at end
            }
            else {
                return Common::kLess; // lhs shorter but an initial sequence of rhs
            }
        }
        else if (ri == re) {
            return Common::kGreater; // rhs shorter but an initial sequence of lhs
        }
        else {
            Assert (li != le and ri != re);
            Assert (c == fElementComparer (*li, *ri));
            return c.value ();
        }
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
        DISABLE_COMPILER_MSC_WARNING_END (6001)
    }
    DISABLE_COMPILER_MSC_WARNING_END (4701)
}

#endif /* _Stroika_Foundation_Traversal_Iterable_inl_ */
