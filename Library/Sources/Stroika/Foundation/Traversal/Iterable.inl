/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_inl_
#define _Stroika_Foundation_Traversal_Iterable_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <set>

#include "../Debug/Assertions.h"

#include "Generator.h"

namespace Stroika {
    namespace Foundation {
        namespace Traversal {

            /*
             ********************************************************************************
             ********************************* IterableBase *********************************
             ********************************************************************************
             */
            template <typename SHARED_T, typename... ARGS_TYPE>
            inline auto IterableBase::MakeSharedPtr (ARGS_TYPE&&... args) -> SharedPtrImplementationTemplate<SHARED_T>
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
            inline auto Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::operator= (const _SafeReadRepAccessor& rhs) noexcept -> Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>&
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
            inline Iterable<T>::Iterable (const Iterable<T>& from) noexcept
                : _fRep (from._fRep)
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
            }
            template <typename T>
            inline Iterable<T>::Iterable (Iterable<T>&& from) noexcept
                : _fRep (move (from._fRep))
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
                if constexpr (!kIterableUsesStroikaSharedPtr) {
                    Require (from._fRep == nullptr); // after move
                }
            }
            template <typename T>
            template <typename CONTAINER_OF_T, enable_if_t<Configuration::has_beginend<CONTAINER_OF_T>::value and not is_convertible_v<const CONTAINER_OF_T*, const Iterable<T>*>>*>
            Iterable<T>::Iterable (const CONTAINER_OF_T& from)
                : _fRep (mk_ (from)._fRep)
            {
            }
            template <typename T>
            Iterable<T>::Iterable (const initializer_list<T>& from)
                : _fRep (mk_ (from)._fRep)
            {
            }
            template <typename T>
            inline Iterable<T>::Iterable (_IterableRepSharedPtr&& rep) noexcept
                : _fRep (move (rep))
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
                if constexpr (!kIterableUsesStroikaSharedPtr) {
                    Require (rep == nullptr); // after move
                }
            }
            template <typename T>
            inline Iterable<T>& Iterable<T>::operator= (const Iterable<T>& rhs)
            {
                RequireNotNull (rhs._fRep);
                _fRep = rhs._fRep;
                return *this;
            }
            template <typename T>
            inline Iterable<T>& Iterable<T>::operator= (Iterable<T>&& rhs)
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
            Iterable<T> Iterable<T>::mk_ (const CONTAINER_OF_T& from)
            {
                vector<T>                tmp (from.begin (), from.end ()); // Somewhat simplistic / inefficient implementation
                size_t                   idx{0};
                function<optional<T> ()> getNext = [tmp, idx]() mutable -> optional<T> {
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
                return static_cast<bool> (this->FindFirstThat ([&element, &equalsComparer](T i) -> bool {
                    return equalsComparer (i, element);
                }));
            }
            template <typename T>
            template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER>
            bool Iterable<T>::SetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer) const
            {
                /*
                 *  An extremely in-efficient but space-constant implementation. N^2 and check
                 *  a contains b and b contains a
                 */
                for (auto ti : *this) {
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
                for (auto ri : rhs) {
                    bool contained = false;
                    for (auto ti : *this) {
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
            template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER>
            bool Iterable<T>::MultiSetEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer) const
            {
                auto tallyOf = [equalsComparer](const Iterable<T>& c, T item) -> size_t {
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
                for (auto ti : *this) {
                    if (tallyOf (*this, ti) != tallyOf (rhs, ti)) {
                        return false;
                    }
                }
                for (auto ti : rhs) {
                    if (tallyOf (*this, ti) != tallyOf (rhs, ti)) {
                        return false;
                    }
                }
                return true;
            }
            template <typename T>
            template <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER>
            bool Iterable<T>::SequnceEquals (const RHS_CONTAINER_TYPE& rhs, const EQUALS_COMPARER& equalsComparer) const
            {
                Iterator<T> li{MakeIterator ()};
                Iterator<T> le{end ()};
                auto        ri = rhs.begin ();
                auto        re = rhs.end ();
                for (; li != le and ri != re; ++ri, ++li) {
                    if (not equalsComparer (*li, *ri)) {
                        return false;
                    }
                }
                // only true if we get to end at the same time
                return li == le and ri == re;
            }
            template <typename T>
            Iterable<T> Iterable<T>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
            {
                RequireNotNull (includeIfTrue);
                Iterable<T>              copyOfIterableSoRefCntBumpedInLambda = *this;
                Iterator<T>              tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
                function<optional<T> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, includeIfTrue]() mutable -> optional<T> {
                    while (tmpIt and not includeIfTrue (*tmpIt)) {
                        ++tmpIt;
                    }
                    return tmpIt ? *tmpIt++ : optional<T>{};
                };
                return CreateGenerator (getNext);
            }
            template <typename T>
            template <typename RESULT_CONTAINER>
            inline RESULT_CONTAINER Iterable<T>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
            {
                return Where<RESULT_CONTAINER> (includeIfTrue, RESULT_CONTAINER{});
            }
            template <typename T>
            template <typename RESULT_CONTAINER>
            RESULT_CONTAINER Iterable<T>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue, const RESULT_CONTAINER& emptyResult) const
            {
                Require (emptyResult.empty ());
                RESULT_CONTAINER result = emptyResult;
                for (auto i : Where (includeIfTrue)) {
                    result.Add (i);
                }
                return result;
            }
            template <typename T>
            Iterable<T> Iterable<T>::Distinct () const
            {
                set<T>                   t1 (begin (), end ()); // Somewhat simplistic/stupid/weak implementation
                vector<T>                tmp (t1.begin (), t1.end ());
                size_t                   idx{0};
                function<optional<T> ()> getNext = [tmp, idx]() mutable -> optional<T> {
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
            template <typename RESULT>
            Iterable<RESULT> Iterable<T>::Distinct (const function<RESULT (ArgByValueType<T>)>& extractElt) const
            {
                RequireNotNull (extractElt);
                set<RESULT> t1;
                for (T i : *this) {
                    t1.add (extractElt (i));
                }
                vector<RESULT>                tmp (t1.begin (), t1.end ());
                size_t                        idx{0};
                function<optional<RESULT> ()> getNext = [tmp, idx]() mutable -> optional<RESULT> {
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
            template <typename T1, typename RESULT>
            Iterable<RESULT> Iterable<T>::Select (const function<T1 (const T&)>& extract1) const
            {
                RequireNotNull (extract1);
                Iterable<T>                   copyOfIterableSoRefCntBumpedInLambda = *this;
                Iterator<T>                   tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
                function<optional<RESULT> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, extract1]() mutable -> optional<RESULT> {
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
                function<optional<RESULT> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, extract1, extract2]() mutable -> optional<RESULT> {
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
                function<optional<RESULT> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, extract1, extract2, extract3]() mutable -> optional<RESULT> {
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
                function<optional<T> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, nItemsToSkip]() mutable -> optional<T> {
                    while (tmpIt and nItemsToSkip > 0) {
                        nItemsToSkip--;
                        ++tmpIt;
                    }
                    return tmpIt ? *tmpIt++ : optional<T> ();
                };
                return CreateGenerator (getNext);
            }
            template <typename T>
            Iterable<T> Iterable<T>::Take (size_t nItems) const
            {
                size_t                   nItemsToTake                         = nItems;
                Iterable<T>              copyOfIterableSoRefCntBumpedInLambda = *this;
                Iterator<T>              tmpIt{copyOfIterableSoRefCntBumpedInLambda.MakeIterator ()};
                function<optional<T> ()> getNext = [copyOfIterableSoRefCntBumpedInLambda, tmpIt, nItemsToTake]() mutable -> optional<T> {
                    if (nItemsToTake == 0) {
                        return nullopt;
                    }
                    nItemsToTake--;
                    return tmpIt ? *tmpIt++ : optional<T> ();
                };
                return CreateGenerator (getNext);
            }
            template <typename T>
            template <typename INORDER_COMPARER_TYPE>
            Iterable<T> Iterable<T>::OrderBy (const INORDER_COMPARER_TYPE& inorderComparer) const
            {
                vector<T> tmp (begin (), end ()); // Somewhat simplistic implementation
                sort (tmp.begin (), tmp.end (), inorderComparer);
                size_t                   idx{0};
                function<optional<T> ()> getNext = [tmp, idx]() mutable -> optional<T> {
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
            inline optional<T> Iterable<T>::First (const function<bool(ArgByValueType<T>)>& that) const
            {
                RequireNotNull (that);
                for (auto i : *this) {
                    if (that (i)) {
                        return i;
                    }
                }
                return optional<T>{};
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
                return optional<T>{};
            }
            template <typename T>
            optional<T> Iterable<T>::Last (const function<bool(ArgByValueType<T>)>& that) const
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
                return Accumulate<T> ([](T lhs, T rhs) { return min (lhs, rhs); });
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
                return Accumulate<T> ([](T lhs, T rhs) -> T { return max (lhs, rhs); });
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
                return Accumulate<RESULT_TYPE> ([](T lhs, T rhs) { return lhs + rhs; });
            }
            template <typename T>
            template <typename RESULT_TYPE>
            inline RESULT_TYPE Iterable<T>::SumValue (ArgByValueType<RESULT_TYPE> defaultValue) const
            {
                return Sum ().Value (defaultValue);
            }
            template <typename T>
            template <typename RESULT_TYPE>
            optional<RESULT_TYPE> Iterable<T>::Median (const function<bool(T, T)>& compare) const
            {
                vector<T> tmp (begin (), end ()); // Somewhat simplistic implementation
                sort (tmp.begin (), tmp.end (), compare);
                size_t sz{tmp.size ()};
                if (sz == 0) {
                    return optional<RESULT_TYPE>{};
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
            inline bool Iterable<T>::Any (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
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
            inline void Iterable<T>::Apply (const function<void(ArgByValueType<T> item)>& doToElement) const
            {
                RequireNotNull (doToElement);
                _SafeReadRepAccessor<>{this}._ConstGetRep ().Apply (doToElement);
            }
            template <typename T>
            inline Iterator<T> Iterable<T>::FindFirstThat (const function<bool(ArgByValueType<T> item)>& doToElement) const
            {
                RequireNotNull (doToElement);
                return _SafeReadRepAccessor<>{this}._ConstGetRep ().FindFirstThat (doToElement, this);
            }
            template <typename T>
            inline Iterator<T> Iterable<T>::FindFirstThat (const Iterator<T>& startAt, const function<bool(ArgByValueType<T> item)>& doToElement) const
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
        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Iterable_inl_ */
