/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_inl_
#define _Stroika_Foundation_Traversal_Iterable_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <execution>
#include <set>

#include "Stroika/Foundation/Containers/Adapters/Adder.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Math/Statistics.h"

#include "Generator.h"

namespace Stroika::Foundation::Traversal {

    constexpr bool kIterableUsesStroikaSharedPtr [[deprecated ("Since Stroika v3.0d1 - not used")]] = false;

    struct [[deprecated ("Since Stroika v3.0d1")]] IterableBase {
        template <typename SHARED_T>
        using PtrImplementationTemplate [[deprecated ("Since Stroika v3.0d1 - use shared_ptr directly")]] = shared_ptr<SHARED_T>;
        template <typename SHARED_T, typename... ARGS_TYPE>
        [[deprecated ("Since Stroika v3.0d1 - use Memory::MakeSharedPtr directly")]] static shared_ptr<SHARED_T> MakeSmartPtr (ARGS_TYPE&&... args)
        {
            return Memory::MakeSharedPtr<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
        template <typename SHARED_T>
        using enable_shared_from_this_PtrImplementationTemplate [[deprecated ("Since Stroika v3.0d1")]] = std::enable_shared_from_this<SHARED_T>;
    };

    /*
     ********************************************************************************
     ***************************** Iterable<T>::_IRep *******************************
     ********************************************************************************
     */
    template <typename T>
    inline size_t Iterable<T>::_IRep::size () const
    {
        /*
         * Default slow/weak implementation.
         */
        size_t sz{};
        if constexpr (true) {
            this->Apply ([&sz] (const T&) { ++sz; }, Execution::SequencePolicy::eDEFAULT);
        }
        else {
            for (Iterator<T> i = MakeIterator (); i != Iterator<T>::GetEmptyIterator (); ++i, ++sz)
                ;
        }
        return sz;
    }
    template <typename T>
    inline bool Iterable<T>::_IRep::empty () const
    {
        if (auto i = MakeIterator ()) {
            return false;
        }
        return true;
    }
    template <typename T>
    inline void Iterable<T>::_IRep::Apply (const function<void (ArgByValueType<T> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const
    {
        RequireNotNull (doToElement);
        for (Iterator<T> i = MakeIterator (); i != Iterator<T>::GetEmptyIterator (); ++i) {
            doToElement (*i);
        }
    }
    template <typename T>
    inline auto Iterable<T>::_IRep::Find (const function<bool (ArgByValueType<T> item)>& that, [[maybe_unused]] Execution::SequencePolicy seq) const
        -> Iterator<value_type>
    {
        RequireNotNull (that);
        for (Iterator<T> i = MakeIterator (); i != Iterator<T>::GetEmptyIterator (); ++i) {
            if (that (*i)) {
                return i;
            }
        }
        return Iterator<T>::GetEmptyIterator ();
    }
    template <typename T>
    inline auto Iterable<T>::_IRep::Find_equal_to (const ArgByValueType<T>& v, [[maybe_unused]] Execution::SequencePolicy seq) const
        -> Iterator<value_type>
    {
        if constexpr (Configuration::IEqualToOptimizable<T>) {
            /*
             *  This is the default implementation. It is only ever if there is a valid equal_to<> around, and
             *  that valid equal_to<> is stateless (verified by Configuration::IEqualToOptimizable).
             */
            if constexpr (true) {
                // simpler but not sure if faster; better though cuz by default leverages seq, which might
                // help. In 'size' testing, on windows, this was slightly larger, so
                // not 100% sure this is the best default -- LGP 2023-02-06
                return Find ([&v] (const T& rhs) { return equal_to<T>{}(v, rhs); }, seq);
            }
            else {
                for (Iterator<T> i = MakeIterator (); i != Iterator<T>::GetEmptyIterator (); ++i) {
                    if (equal_to<T>{}(v, *i)) {
                        return i;
                    }
                }
                return Iterator<T>::GetEmptyIterator ();
            }
        }
        else {
            RequireNotReached (); // cannot call if not IEqualToOptimizable
            return Iterator<T>::GetEmptyIterator ();
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
        : fConstRef_{static_cast<const REP_SUB_TYPE*> (it->_fRep.cget ())}
        , fIterableEnvelope_{it}
#if qDebug
        , fAssertReadLock_{it->_fThisAssertExternallySynchronized}
#endif
    {
        RequireNotNull (it);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const _SafeReadRepAccessor& src) noexcept
        : fConstRef_{src.fConstRef_}
        , fIterableEnvelope_{src.fIterableEnvelope_}
#if qDebug
        , fAssertReadLock_{src.fIterableEnvelope_}
#endif
    {
        RequireNotNull (fConstRef_);
        EnsureMember (fConstRef_, REP_SUB_TYPE);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (_SafeReadRepAccessor&& src) noexcept
        : fConstRef_{src.fConstRef_}
        , fIterableEnvelope_{src.fIterableEnvelope_}
#if qDebug
        , fAssertReadLock_{move (src.fAssertReadLock_)}
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
        fConstRef_               = rhs.fConstRef_;
        this->fIterableEnvelope_ = rhs.fIterableEnvelope_;
#if qDebug
        this->fAssertReadLock_ = rhs.fAssertReadLock_;
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
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline auto Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_ConstGetRepSharedPtr () const noexcept -> shared_ptr<REP_SUB_TYPE>
    {
        return Debug::UncheckedDynamicPointerCast<REP_SUB_TYPE> (this->fIterableEnvelope_->_fRep.cget_ptr ());
    }

    /*
     ********************************************************************************
     ************* Iterable<CONTAINER_OF_T, T>::_SafeReadWriteRepAccessor ***********
     ********************************************************************************
     */
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (Iterable<T>* iterableEnvelope)
        : fRepReference_{static_cast<REP_SUB_TYPE*> (iterableEnvelope->_fRep.rwget ())}
#if qDebug
        , fAssertWriteLock_{iterableEnvelope->_fThisAssertExternallySynchronized}
        , fIterableEnvelope_{iterableEnvelope}
#endif
    {
        RequireNotNull (iterableEnvelope);
    }
    template <typename T>
    template <typename REP_SUB_TYPE>
    inline Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (_SafeReadWriteRepAccessor&& from)
        : fRepReference_{from.fRepReference_}
#if qDebug
        , fAssertWriteLock_{move (from.fAssertWriteLock_)}
        , fIterableEnvelope_{from.fIterableEnvelope_}
#endif
    {
        RequireNotNull (fRepReference_);
        EnsureMember (fRepReference_, REP_SUB_TYPE);
#if qDebug
        from.fIterableEnvelope_ = nullptr;
#endif
        from.fRepReference_ = nullptr;
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
#if qDebug
        EnsureNotNull (fIterableEnvelope_);
        EnsureNotNull (fIterableEnvelope_->_fRep);
        Ensure (fIterableEnvelope_->_fRep.use_count () == 1);
#endif
        return *fRepReference_;
    }

    /*
     ********************************************************************************
     ********************************** Iterable<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    inline Iterable<T>::Iterable (const shared_ptr<typename Iterable<T>::_IRep>& rep) noexcept
        : _fRep{(RequireExpression (rep != nullptr), rep)}
    {
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
    }
    template <typename T>
    inline Iterable<T>::Iterable (shared_ptr<typename Iterable<T>::_IRep>&& rep) noexcept
        : _fRep{(RequireExpression (rep != nullptr), move (rep))}
    {
        Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
        Require (rep == nullptr); // after move (see https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr "After the construction, ... r is empty and its stored pointer is null"
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <ranges::range CONTAINER_OF_T>
    Iterable<T>::Iterable (CONTAINER_OF_T&& from)
        requires (not derived_from<remove_cvref_t<CONTAINER_OF_T>, Iterable<T>>)
        : _fRep{mk_ (forward<CONTAINER_OF_T> (from))._fRep}
    {
    }
#endif
    template <typename T>
    inline Iterable<T>::Iterable (const initializer_list<T>& from)
        : _fRep{mk_ (from)._fRep}
    {
    }
    template <typename T>
    inline Iterable<T>::operator bool () const
    {
        return not empty ();
    }
    template <typename T>
    inline shared_ptr<typename Iterable<T>::_IRep> Iterable<T>::Clone_ (const _IRep& rep)
    {
        return rep.Clone ();
    }
    template <typename T>
    template <typename CONTAINER_OF_T>
    Iterable<T> Iterable<T>::mk_ (CONTAINER_OF_T&& from)
    {
        using DECAYED_CONTAINER = remove_cvref_t<CONTAINER_OF_T>;
        // Most containers are safe to use copy-by-value, except not initializer_list<> - not sure how to check for that generically...
        using USE_CONTAINER_TYPE =
            conditional_t<is_copy_constructible_v<DECAYED_CONTAINER> and not is_same_v<DECAYED_CONTAINER, initializer_list<T>>, DECAYED_CONTAINER, vector<T>>;
        auto sharedCopyOfContainer = make_shared<USE_CONTAINER_TYPE> (forward<CONTAINER_OF_T> (from));
        // shared copy so if/when getNext copied, the container itself isn't (so not invalidating any iterators)
        function<optional<T> ()> getNext = [sharedCopyOfContainer, i = sharedCopyOfContainer->begin ()] () mutable -> optional<T> {
            if (i != sharedCopyOfContainer->end ()) {
                return *i++; // intentionally increment AFTER returning value
            }
            return nullopt;
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
        _SafeReadRepAccessor<> accessor{this};
        return accessor._ConstGetRep ().MakeIterator ();
    }
    template <typename T>
    inline size_t Iterable<T>::size () const
    {
        _SafeReadRepAccessor<> accessor{this};
        return accessor._ConstGetRep ().size ();
    }
    template <typename T>
    inline bool Iterable<T>::empty () const
    {
        _SafeReadRepAccessor<> accessor{this};
        return accessor._ConstGetRep ().empty ();
    }
    template <typename T>
    template <Common::IPotentiallyComparer<T> EQUALS_COMPARER>
    bool Iterable<T>::Contains (ArgByValueType<T> element, EQUALS_COMPARER&& equalsComparer) const
    {
        // grab iterator to first matching item, and contains if not at end; this is faster than using iterators
        return static_cast<bool> (this->Find ([&element, &equalsComparer] (T i) -> bool { return equalsComparer (i, element); }));
    }
    template <typename T>
    template <ranges::range LHS_CONTAINER_TYPE, ranges::range RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER>
    bool Iterable<T>::SetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer)
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
    template <ranges::range RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline bool Iterable<T>::SetEquals (const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer) const
    {
        return SetEquals (*this, rhs, forward<EQUALS_COMPARER> (equalsComparer));
    }
    template <typename T>
    template <ranges::range LHS_CONTAINER_TYPE, ranges::range RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER>
    bool Iterable<T>::MultiSetEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer)
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
    template <ranges::range RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline bool Iterable<T>::MultiSetEquals (const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer) const
    {
        return MultiSetEquals (*this, rhs, equalsComparer);
    }
    template <typename T>
    template <ranges::range LHS_CONTAINER_TYPE, ranges::range RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER>
    bool Iterable<T>::SequentialEquals (const LHS_CONTAINER_TYPE& lhs, const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer, bool useIterableSize)
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
            Assert ((li != le) == (ri != re)); // cuz same length, and this requires size cannot change during call
#endif
            while (li != le) {
                if (not equalsComparer (*li, *ri)) {
                    return false;
                }
                ++li;
                ++ri;
#if qDebug
                Assert ((li != le) == (ri != re)); // cuz same length, and this requires size cannot change during call
#endif
            }
#if qDebug
            Assert (li == le and ri == re);
#endif
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
    template <ranges::range RHS_CONTAINER_TYPE, Common::IEqualsComparer<T> EQUALS_COMPARER>
    inline bool Iterable<T>::SequentialEquals (const RHS_CONTAINER_TYPE& rhs, EQUALS_COMPARER&& equalsComparer, bool useIterableSize) const
    {
        return SequentialEquals (*this, rhs, forward<EQUALS_COMPARER> (equalsComparer), useIterableSize);
    }
    template <typename T>
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
#else
    template <derived_from<Iterable<T>> RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
#endif
    inline RESULT_CONTAINER Iterable<T>::Where (INCLUDE_PREDICATE&& includeIfTrue) const
    {
        //
        // LAZY evaluate Iterable<> and for concrete container types, explicitly create the object.
        //
        if constexpr (same_as<RESULT_CONTAINER, Iterable<T>>) {
            // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
            auto sharedContext = make_shared<Iterable<T>> (*this);
            // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
            // Both the 'sharedContext' and the i' get stored into the lambda closure so they get appropriately copied as you copy iterators
            function<optional<T> ()> getNext = [sharedContext, i = sharedContext->MakeIterator (), includeIfTrue] () mutable -> optional<T> {
                while (i and not includeIfTrue (*i)) {
                    ++i;
                }
                if (i) {
                    auto tmp = *i;
                    ++i;
                    return move (tmp);
                }
                return nullopt;
            };
            return CreateGenerator (getNext);
        }
        else {
            return Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{});
        }
    }
    template <typename T>
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
#else
    template <derived_from<Iterable<T>> RESULT_CONTAINER, predicate<T> INCLUDE_PREDICATE>
#endif
    RESULT_CONTAINER Iterable<T>::Where (INCLUDE_PREDICATE&& includeIfTrue, [[maybe_unused]] RESULT_CONTAINER&& emptyResult) const
    {
        if constexpr (same_as<RESULT_CONTAINER, Iterable<T>>) {
            // no point in emptyResult overload; vector to one spot for Iterable<> lazy implementation
            return Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue));
        }
        else {
            Require (emptyResult.empty ());
            RESULT_CONTAINER result = forward<RESULT_CONTAINER> (emptyResult);
            this->Apply ([&result, &includeIfTrue] (Configuration::ArgByValueType<T> arg) {
                if (includeIfTrue (arg)) {
                    Containers::Adapters::Adder<RESULT_CONTAINER>::Add (&result, arg);
                }
            });
            return result;
        }
    }
    template <typename T>
    template <Common::IPotentiallyComparer<T> EQUALS_COMPARER>
    Iterable<T> Iterable<T>::Distinct (EQUALS_COMPARER&& equalsComparer) const
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
        function<optional<T> ()> getNext = [container = move (tmp), idx = size_t{0}] () mutable -> optional<T> {
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
    template <typename RESULT, Common::IPotentiallyComparer<T> EQUALS_COMPARER>
    Iterable<RESULT> Iterable<T>::Distinct (const function<RESULT (ArgByValueType<T>)>& extractElt, EQUALS_COMPARER&& equalsComparer) const
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
        function<optional<RESULT> ()> getNext = [container = move (tmp), idx = size_t{0}] () mutable -> optional<RESULT> {
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
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    RESULT_CONTAINER Iterable<T>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        using RESULT_ELEMENT = typename RESULT_CONTAINER::value_type;
        constexpr bool kLazyEvaluateIteration_ = same_as<RESULT_CONTAINER, Iterable<RESULT_ELEMENT>>; // For now use vector and lazy not truly implemented
        constexpr bool kOptionalExtractor_ = not convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> and
                                             convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>;
        if constexpr (kLazyEvaluateIteration_) {
            // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
            auto sharedContext = make_shared<Iterable<T>> (*this);
            // Both the 'sharedContext' and the 'i' get stored into the lambda closure so they get appropriately copied as you copy iterators
            function<optional<RESULT_ELEMENT> ()> getNext = [sharedContext, i = sharedContext->MakeIterator (),
                                                             elementMapper] () mutable -> optional<RESULT_ELEMENT> {
                // tricky. The function we are defining returns nullopt as a sentinel to signal end of iteration. The function we are GIVEN returns nullopt
                // to signal skip this item. So adjust accordingly
                if constexpr (kOptionalExtractor_) {
                    while (i) {
                        optional<RESULT_ELEMENT> t = elementMapper (*i);
                        ++i;
                        if (t) {
                            return *t;
                        }
                    }
                    return nullopt;
                }
                else {
                    if (i) {
                        RESULT_ELEMENT result = elementMapper (*i);
                        ++i;
                        return move (result);
                    }
                    return nullopt;
                }
            };
            return CreateGenerator (getNext);
        }
        else {
            // subclasseers can replace this 'else' branch if RESULT_CONTAINER cannot or should not be default constructed
            return this->Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper), RESULT_CONTAINER{});
        }
    }
    template <typename T>
    template <typename RESULT_CONTAINER, invocable<T> ELEMENT_MAPPER>
    RESULT_CONTAINER Iterable<T>::Map (ELEMENT_MAPPER&& elementMapper, RESULT_CONTAINER&& emptyResult) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        using RESULT_ELEMENT = typename RESULT_CONTAINER::value_type;
        constexpr bool kLazyEvaluateIteration_ = same_as<RESULT_CONTAINER, Iterable<RESULT_ELEMENT>>; // For now use vector and lazy not truly implemented
        constexpr bool kOptionalExtractor_ = not convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, typename RESULT_CONTAINER::value_type> and
                                             convertible_to<invoke_result_t<ELEMENT_MAPPER, T>, optional<typename RESULT_CONTAINER::value_type>>;
        if constexpr (kLazyEvaluateIteration_) {
            return this->Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // ignore useless emptyResult if provided, and invoke other overload to share code
        }
        else {
            RESULT_CONTAINER c = forward<RESULT_CONTAINER> (emptyResult);
            // reserve iff we know the right size and container supports reserve
            if constexpr (not kOptionalExtractor_ and requires (RESULT_CONTAINER p) { p.reserve (3u); }) {
                c.reserve (this->size ());
            }
            this->Apply ([&c, &elementMapper] (Configuration::ArgByValueType<T> arg) {
                if constexpr (kOptionalExtractor_) {
                    if (auto oarg = elementMapper (arg)) {
                        Containers::Adapters::Adder<RESULT_CONTAINER>::Add (&c, *oarg);
                    }
                }
                else {
                    Containers::Adapters::Adder<RESULT_CONTAINER>::Add (&c, elementMapper (arg));
                }
            });
            return c;
        }
    }
    template <typename T>
    template <typename RESULT_T, invocable<T> CONVERT_TO_RESULT, invocable<RESULT_T, RESULT_T, bool> COMBINER>
    RESULT_T Iterable<T>::Join (const CONVERT_TO_RESULT& convertToResult, const COMBINER& combiner) const
        requires (convertible_to<invoke_result_t<CONVERT_TO_RESULT, T>, RESULT_T> and
                  convertible_to<invoke_result_t<COMBINER, RESULT_T, RESULT_T, bool>, RESULT_T>)
    {
        RESULT_T result{};
        size_t   idx{0};
        size_t   cnt = this->size ();
        for (auto i : *this) {
            if (idx == 0) {
                result = convertToResult (i);
            }
            else {
                result = combiner (result, convertToResult (i), idx + 1 == cnt);
            }
            ++idx;
        }
        return result;
    }
    template <typename T>
    Iterable<T> Iterable<T>::Skip (size_t nItems) const
    {
        // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same Iterable)
        auto sharedContext = make_shared<Iterable<T>> (*this);
        // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
        // Both the 'sharedContext' and the i' get stored into the lambda closure so they get appropriately copied as you copy iterators
        // perIteratorContextNItemsToSkip also must be cloned per iterator instance
        function<optional<T> ()> getNext = [sharedContext, i = sharedContext->MakeIterator (),
                                            perIteratorContextNItemsToSkip = nItems] () mutable -> optional<T> {
            while (i and perIteratorContextNItemsToSkip > 0) {
                --perIteratorContextNItemsToSkip;
                ++i;
            }
            if (i) {
                auto result = *i;
                ++i;
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
        // Both the 'sharedContext' and the i' get stored into the lambda closure so they get appropriately copied as you copy iterators
        // perIteratorContextNItemsToTake also must be cloned per iterator instance
        function<optional<T> ()> getNext = [sharedContext, i = sharedContext->MakeIterator (),
                                            perIteratorContextNItemsToTake = nItems] () mutable -> optional<T> {
            if (perIteratorContextNItemsToTake == 0) {
                return nullopt;
            }
            perIteratorContextNItemsToTake--;
            if (i) {
                auto result = *i;
                ++i;
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
        // Both the 'sharedContext' and the i' get stored into the lambda closure so they get appropriately copied as you copy iterators
        // perIteratorContextNItemsToSkip also must be cloned per iterator instance
        // perIteratorContextNItemsToTake also must be cloned per iterator instance
        function<optional<T> ()> getNext = [sharedContext, i = sharedContext->MakeIterator (), perIteratorContextNItemsToSkip = from,
                                            perIteratorContextNItemsToTake = to - from] () mutable -> optional<T> {
            while (i and perIteratorContextNItemsToSkip > 0) {
                --perIteratorContextNItemsToSkip;
                ++i;
            }
            if (perIteratorContextNItemsToTake == 0) {
                return nullopt;
            }
            perIteratorContextNItemsToTake--;
            if (i) {
                auto result = *i;
                ++i;
                return move (result);
            }
            return nullopt;
        };
        return CreateGenerator (getNext);
    }
    template <typename T>
    template <Common::IPotentiallyComparer<T> COMPARER>
    Iterable<T> Iterable<T>::Top (COMPARER&& cmp) const
    {
        // @todo https://stroika.atlassian.net/browse/STK-972 - optimize case where 'iterable' is already sortable
        vector<T> tmp{this->begin (), this->end ()};
#if __cpp_lib_execution >= 201603L
        sort (std::execution::par, tmp.begin (), tmp.end (), forward<COMPARER> (cmp));
#else
        sort (tmp.begin (), tmp.end (), forward<COMPARER> (cmp));
#endif
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
    template <Common::IPotentiallyComparer<T> COMPARER>
    Iterable<T> Iterable<T>::Top (size_t n, COMPARER&& cmp) const
    {
        if (n >= size ()) {
            return Top (cmp);
        }
        // @todo https://stroika.atlassian.net/browse/STK-972 - optimize case where 'iterable' is already sortable
        vector<T> tmp{this->begin (), this->end ()};
#if __cpp_lib_execution >= 201603L
        partial_sort (std::execution::par, tmp.begin (), tmp.begin () + n, tmp.end (), forward<COMPARER> (cmp));
#else
        partial_sort (tmp.begin (), tmp.begin () + n, tmp.end (), forward<COMPARER> (cmp));
#endif
        size_t idx{0};
        tmp.erase (tmp.begin () + n, tmp.end ());
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
    inline Iterable<T> Iterable<T>::Top () const
    {
        return Top (std::greater<T>{});
    }
    template <typename T>
    inline Iterable<T> Iterable<T>::Top (size_t n) const
    {
        return Top (n, std::greater<int>{});
    }
    template <typename T>
    template <Common::IPotentiallyComparer<T> INORDER_COMPARER_TYPE>
    Iterable<T> Iterable<T>::OrderBy (INORDER_COMPARER_TYPE&& inorderComparer, [[maybe_unused]] Execution::SequencePolicy seq) const
    {
        // @todo https://stroika.atlassian.net/browse/STK-972 - optimize case where 'iterable' is already sortable
        vector<T> tmp{begin (), end ()}; // Somewhat simplistic implementation (always over copy and index so no need to worry about iterator refereincing inside container)
#if __cpp_lib_execution >= 201603L
        if (seq == Execution::SequencePolicy::eSeq) {
            stable_sort (tmp.begin (), tmp.end (), inorderComparer);
        }
        else {
            stable_sort (std::execution::par, tmp.begin (), tmp.end (), inorderComparer);
        }
#else
        stable_sort (tmp.begin (), tmp.end (), inorderComparer);
#endif
        function<optional<T> ()> getNext = [tmp, idx = size_t{0}] () mutable -> optional<T> {
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
    template <invocable<T> F>
    inline optional<T> Iterable<T>::First (F&& that) const
        requires (convertible_to<invoke_result_t<F, T>, bool>)
    {
        constexpr bool kUseIterableRepIteration_ = true; // same semantics, but maybe faster cuz avoids Stroika iterator extra virtual calls overhead
        if (kUseIterableRepIteration_) {
            Iterator<T> t = this->_fRep->Find (forward<F> (that), Execution::SequencePolicy::eSeq);
            return t ? optional<T>{*t} : optional<T>{};
        }
        else {
            for (const auto& i : *this) {
                if (that (i)) {
                    return i;
                }
            }
            return nullopt;
        }
    }
    template <typename T>
    template <typename RESULT_T>
    inline optional<RESULT_T> Iterable<T>::First (const function<optional<RESULT_T> (ArgByValueType<T>)>& that) const
    {
        RequireNotNull (that);
        constexpr bool kUseIterableRepIteration_ = true; // same semantics, but maybe faster cuz avoids Stroika iterator extra virtual calls overhead
        if (kUseIterableRepIteration_) {
            optional<RESULT_T>          result; // actual result captured in side-effect of lambda
            auto                        f = [&that, &result] (ArgByValueType<T> i) { return (result = that (i)).has_value (); };
            _SafeReadRepAccessor<_IRep> accessor{this};
            Iterator<T>                 t = accessor._ConstGetRep ().Find (f, Execution::SequencePolicy::eSeq);
            return t ? result : optional<RESULT_T>{};
        }
        else {
            for (const auto& i : *this) {
                if (auto r = that (i)) {
                    return r;
                }
            }
            return nullopt;
        }
    }
    template <typename T>
    inline T Iterable<T>::FirstValue (ArgByValueType<T> defaultValue) const
    {
        return this->First ().value_or (defaultValue);
    }
    template <typename T>
    template <invocable<T> F>
    inline T Iterable<T>::FirstValue (F&& that, ArgByValueType<T> defaultValue) const
        requires (convertible_to<invoke_result_t<F, T>, bool>)
    {
        return this->First (forward<F> (that)).value_or (defaultValue);
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
    template <invocable<T> F>
    inline optional<T> Iterable<T>::Last (F&& that) const
        requires (convertible_to<invoke_result_t<F, T>, bool>)
    {
        optional<T> result;
        for (const auto& i : *this) {
            if (that (i)) {
                result = i;
            }
        }
        return result;
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
    inline T Iterable<T>::LastValue (ArgByValueType<T> defaultValue) const
    {
        return this->Last ().value_or (defaultValue);
    }
    template <typename T>
    template <invocable<T> F>
    inline T Iterable<T>::LastValue (F&& that, ArgByValueType<T> defaultValue) const
        requires (convertible_to<invoke_result_t<F, T>, bool>)
    {
        return this->Last (forward<F> (that)).value_or (defaultValue);
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
    template <typename REDUCED_TYPE>
    optional<REDUCED_TYPE> Iterable<T>::Reduce (const function<REDUCED_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op) const
    {
        optional<REDUCED_TYPE> result;
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
    template <typename REDUCED_TYPE>
    inline REDUCED_TYPE Iterable<T>::ReduceValue (const function<REDUCED_TYPE (ArgByValueType<T>, ArgByValueType<T>)>& op,
                                                  ArgByValueType<REDUCED_TYPE>                                         defaultValue) const
    {
        return Reduce<REDUCED_TYPE> (op).value_or (defaultValue);
    }
    template <typename T>
    inline optional<T> Iterable<T>::Min () const
    {
        return Reduce<T> ([] (T lhs, T rhs) -> T { return min (lhs, rhs); });
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::MinValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Min ().value_or (defaultValue);
    }
    template <typename T>
    inline optional<T> Iterable<T>::Max () const
    {
        return Reduce<T> ([] (T lhs, T rhs) -> T { return max (lhs, rhs); });
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::MaxValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Max ().value_or (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline optional<RESULT_TYPE> Iterable<T>::Mean () const
    {
        Iterator<T> i = begin ();
        if (i == end ()) [[unlikely]] {
            return nullopt;
        }
        return Math::Mean (i, end ());
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::MeanValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Mean ().value_or (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline optional<RESULT_TYPE> Iterable<T>::Sum () const
    {
        return Reduce<RESULT_TYPE> ([] (T lhs, T rhs) { return lhs + rhs; });
    }
    template <typename T>
    template <typename RESULT_TYPE>
    inline RESULT_TYPE Iterable<T>::SumValue (ArgByValueType<RESULT_TYPE> defaultValue) const
    {
        return Sum ().value_or (defaultValue);
    }
    template <typename T>
    template <typename RESULT_TYPE, Common::IPotentiallyComparer<RESULT_TYPE> INORDER_COMPARE_FUNCTION>
    inline optional<RESULT_TYPE> Iterable<T>::Median (const INORDER_COMPARE_FUNCTION& compare) const
    {
        Iterator<T> i = begin ();
        if (i == end ()) [[unlikely]] {
            return nullopt;
        }
        return Math::Median<Iterator<T>, RESULT_TYPE> (i, end (), compare);
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
                        if (innerIndex < origList.size ()) [[likely]] {
                            return origList[innerIndex++];
                        }
                        if (repeatCountIndex < count) [[likely]] {
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
    inline size_t Iterable<T>::Count () const
    {
        return size ();
    }
    template <typename T>
    inline size_t Iterable<T>::Count (const function<bool (ArgByValueType<T>)>& includeIfTrue) const
    {
        size_t cnt{};
        Apply ([&] (ArgByValueType<T> a) {
            if (includeIfTrue (a))
                ++cnt;
        });
        Ensure (cnt == Where (includeIfTrue).size ());
        return cnt;
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
    constexpr Iterator<T> Iterable<T>::end () const noexcept
    {
        return Iterator<T>::GetEmptyIterator ();
    }
    template <typename T>
    inline void Iterable<T>::Apply (const function<void (ArgByValueType<T> item)>& doToElement, Execution::SequencePolicy seq) const
    {
        RequireNotNull (doToElement);
        _SafeReadRepAccessor<> accessor{this};
        accessor._ConstGetRep ().Apply (doToElement, seq);
    }
    template <typename T>
    template <predicate<T> THAT_FUNCTION>
    inline Iterator<T> Iterable<T>::Find (THAT_FUNCTION&& that, Execution::SequencePolicy seq) const
    {
        // NB: This transforms perfectly forwarded 'THAT_FUNCTION' and converts it to std::function<> - preventing further inlining at this point -
        // just so it can be done
        _SafeReadRepAccessor<> accessor{this};
        return accessor._ConstGetRep ().Find (that, seq);
    }
    template <typename T>
    template <Common::IPotentiallyComparer<T> EQUALS_COMPARER>
    inline Iterator<T> Iterable<T>::Find (Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer, Execution::SequencePolicy seq) const
    {
        if constexpr (is_same_v<remove_cvref_t<EQUALS_COMPARER>, equal_to<T>> and Configuration::IEqualToOptimizable<T>) {
            // This CAN be much faster than the default implementation for this special (but common) case (often a tree structure will have been maintained making this find faster)
            _SafeReadRepAccessor<> accessor{this};
            return accessor._ConstGetRep ().Find_equal_to (v, seq);
        }
        else {
            return Find ([v, equalsComparer] (Configuration::ArgByValueType<T> arg) { return equalsComparer (v, arg); }, seq);
        }
    }
    template <typename T>
    template <predicate<T> THAT_FUNCTION>
    inline Iterator<T> Iterable<T>::Find (const Iterator<T>& startAt, THAT_FUNCTION&& that, [[maybe_unused]] Execution::SequencePolicy seq) const
    {
        for (Iterator<T> i = startAt; i != end (); ++i) {
            if (that (*i)) {
                return i;
            }
        }
        return end ();
    }
    template <typename T>
    template <Common::IPotentiallyComparer<T> EQUALS_COMPARER>
    Iterator<T> Iterable<T>::Find (const Iterator<T>& startAt, Configuration::ArgByValueType<T> v, EQUALS_COMPARER&& equalsComparer,
                                   [[maybe_unused]] Execution::SequencePolicy seq) const
    {
        for (Iterator<T> i = startAt; i != end (); ++i) {
            if (equalsComparer (v, *i)) {
                return i;
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
    template <
#if qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy
        typename
#else
        Common::IEqualsComparer<T>
#endif
        T_EQUALS_COMPARER>
    constexpr Iterable<T>::SequentialEqualsComparer<T_EQUALS_COMPARER>::SequentialEqualsComparer (const T_EQUALS_COMPARER& elementEqualsComparer,
                                                                                                  bool useIterableSize)
        : fElementComparer{elementEqualsComparer}
        , fUseIterableSize{useIterableSize}
    {
    }
    template <typename T>
    template <
#if qCompilerAndStdLib_template_ForwardDeclareWithConceptsInTypenameCrasher_Buggy
        typename
#else
        Common::IEqualsComparer<T>
#endif
        T_EQUALS_COMPARER>
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
        optional<strong_ordering> c;
        while ((li != le) and (ri != re) and (c = fElementComparer (*li, *ri)) == strong_ordering::equal) {
            ++li;
            ++ri;
        }
        if (li == le) {
            if (ri == re) {
                return strong_ordering::equal; // all items same and loop ended with both things at end
            }
            else {
                return strong_ordering::less; // lhs shorter but an initial sequence of rhs
            }
        }
        else if (ri == re) {
            return strong_ordering::greater; // rhs shorter but an initial sequence of lhs
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
