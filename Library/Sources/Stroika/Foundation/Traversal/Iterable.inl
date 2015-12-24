/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_inl_
#define _Stroika_Foundation_Traversal_Iterable_inl_


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <set>

#include    "../Debug/Assertions.h"

#include    "Generator.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             *************************** Iterable<T>::_IRep *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  bool    Iterable<T>::_IRep::_IsEmpty () const
            {
                return GetLength () == 0;
            }
            template    <typename T>
            inline  void    Iterable<T>::_IRep::_Apply (_APPLY_ARGTYPE doToElement) const
            {
                RequireNotNull (doToElement);
                for (Iterator<T> i = MakeIterator (this); i != Iterable<T>::end (); ++i) {
                    (doToElement) (*i);
                }
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::_IRep::_FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
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
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const Iterable<T>* it)
                : shared_lock<const Debug::AssertExternallySynchronizedLock> (*it)
                , fConstRef_ (static_cast<const REP_SUB_TYPE*> (it->_fRep.cget ()))
            {
                RequireNotNull (it);
                EnsureMember (fConstRef_, REP_SUB_TYPE);
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (_SafeReadRepAccessor&& from)
                : shared_lock<const Debug::AssertExternallySynchronizedLock> (move<const Debug::AssertExternallySynchronizedLock> (from))
                , fConstRef_ (from.fConstRef_)
            {
                RequireNotNull (fConstRef_);
                EnsureMember (fConstRef_, REP_SUB_TYPE);
                from.fConstRef_ = nullptr;
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  const REP_SUB_TYPE&    Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
            {
                EnsureMember (fConstRef_, REP_SUB_TYPE);
                return *fConstRef_;
            }


            /*
             ********************************************************************************
             ************* Iterable<CONTAINER_OF_T, T>::_SafeReadWriteRepAccessor ***********
             ********************************************************************************
             */
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (Iterable<T>* iterableEnvelope)
                : lock_guard<const Debug::AssertExternallySynchronizedLock> (*iterableEnvelope)
                , fIterableEnvelope_ (iterableEnvelope)
                , fRepReference_ (static_cast<REP_SUB_TYPE*> (iterableEnvelope->_fRep.get (iterableEnvelope)))
            {
                RequireNotNull (iterableEnvelope);
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (_SafeReadWriteRepAccessor&& from)
                : lock_guard<const Debug::AssertExternallySynchronizedLock> (move<const Debug::AssertExternallySynchronizedLock> (from))
                , fIterableEnvelope_ (from.fIterableEnvelope_)
                , fRepReference_ (from.fRepReference_)
            {
                RequireNotNull (fRepReference_);
                EnsureMember (fRepReference_, REP_SUB_TYPE);
                from.fIterableEnvelope_ = nullptr;
                from.fRepReference_ = nullptr;
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  const REP_SUB_TYPE&    Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
            {
                EnsureNotNull (fRepReference_);
                return *fRepReference_;
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  REP_SUB_TYPE&    Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_GetWriteableRep ()
            {
                EnsureNotNull (fRepReference_);
                return *fRepReference_;
            }
            template    <typename T>
            template    <typename REP_SUB_TYPE>
            inline  void    Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_UpdateRep (const typename _SharedByValueRepType::shared_ptr_type& sp)
            {
                EnsureNotNull (fIterableEnvelope_);
                fIterableEnvelope_->_fRep = sp;
            }


            /*
             ********************************************************************************
             ********************************** Iterable<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Iterable<T>::Iterable (const _SharedPtrIRep& rep) noexcept
:
            _fRep (rep)
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (const Iterable<T>& from) noexcept
:
            _fRep (from._fRep)
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (Iterable<T>&& from) noexcept
:
            _fRep (std::move (from._fRep))
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
#if     !qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr
                Require (from._fRep == nullptr);    // after move
#endif
            }
            template    <typename T>
            Iterable<T>::Iterable (const initializer_list<T>& from)
                :
                _fRep (mk_(from)._fRep)
            {
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (_SharedPtrIRep&& rep) noexcept
:
            _fRep (std::move (rep))
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
#if     !qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr
                Require (rep == nullptr);   // after move
#endif
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (const Iterable<T>& rhs)
            {
                RequireNotNull (rhs._fRep.cget ());
                _fRep = rhs._fRep;
                return *this;
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (Iterable<T> && rhs)
            {
                RequireNotNull (rhs._fRep.cget ());
                _fRep = move (rhs._fRep);
                return *this;
            }
            template    <typename T>
            inline  typename Iterable<T>::_SharedPtrIRep  Iterable<T>::Clone_ (const _IRep& rep, IteratorOwnerID forIterableEnvelope)
            {
                return rep.Clone (forIterableEnvelope);
            }
            template    <typename T>
            Iterable<T> Iterable<T>::mk_ (const initializer_list<T>& from)
            {
                using   Memory::Optional;
                vector<T>   tmp (from.begin (), from.end ());     // Somewhat simplistic / inefficient implementation
                size_t idx { 0 };
                function<Optional<T>()> getNext = [tmp, idx] () mutable -> Memory::Optional<T> {
                    if (idx < tmp.size ())
                    {
                        return tmp[idx++];
                    }
                    else {
                        return Optional<T> ();
                    }
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            inline  Memory::SharedByValue_State     Iterable<T>::_GetSharingState () const
            {
                return _fRep.GetSharingState ();
            }
            template    <typename T>
            inline  const typename Iterable<T>::_IRep&   Iterable<T>::_ConstGetRep () const
            {
                EnsureNotNull (_fRep.cget ());
                return *_fRep;
            }
            template    <typename T>
            inline  Iterator<T>     Iterable<T>::MakeIterator () const
            {
                return _SafeReadRepAccessor<> { this } ._ConstGetRep ().MakeIterator (this);
            }
            template    <typename T>
            inline  size_t  Iterable<T>::GetLength () const
            {
                return _SafeReadRepAccessor<> { this } ._ConstGetRep ().GetLength ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::IsEmpty () const
            {
                return _SafeReadRepAccessor<> { this } ._ConstGetRep ().IsEmpty ();
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            bool    Iterable<T>::Contains (ArgByValueType<T> element) const
            {
                for (T i : *this) {
                    if (EQUALS_COMPARER::Equals (i, element)) {
                        return true;
                    }
                }
                return false;
            }
            template    <typename T>
            template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER>
            bool    Iterable<T>::SetEquals (const RHS_CONTAINER_TYPE& rhs) const
            {
                /*
                 *  An extremely in-efficient but space-constant implementation. N^2 and check
                 *  a contains b and b contains a
                 */
                for (auto ti : *this) {
                    bool contained  =   false;
                    for (auto ri : rhs) {
                        if (EQUALS_COMPARER::Equals (ti, ri)) {
                            contained = true;
                            break;
                        }
                    }
                    if (not contained) {
                        return false;
                    }
                }
                for (auto ri : rhs) {
                    bool contained  =   false;
                    for (auto ti : *this) {
                        if (EQUALS_COMPARER::Equals (ti, ri)) {
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
            template    <typename T>
            template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER>
            bool    Iterable<T>::MultiSetEquals (const RHS_CONTAINER_TYPE& rhs) const
            {
                auto    tallyOf = [] (const Iterable<T>& c, T item) -> size_t {
                    size_t  total = 0;
                    for (auto ti : c)
                    {
                        if (EQUALS_COMPARER::Equals (ti, item)) {
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
            template    <typename T>
            template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER>
            bool    Iterable<T>::SequnceEquals (const RHS_CONTAINER_TYPE& rhs) const
            {
                Iterator<T> li  { MakeIterator () };
                Iterator<T> le  { end () };
                auto        ri  = rhs.begin ();
                auto        re  = rhs.end ();
                for (; li != le and ri != re; ++ri, ++li) {
                    if (not EQUALS_COMPARER::Equals (*li, *ri)) {
                        return false;
                    }
                }
                // only true if we get to end at the same time
                return li == le and ri == re;
            }
            template    <typename T>
            Iterable<T>     Iterable<T>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
            {
                using   Memory::Optional;
                Iterator<T> tmpIt { this->MakeIterator () };
                function<Optional<T>()> getNext = [tmpIt, includeIfTrue] () mutable -> Memory::Optional<T> {
                    if (tmpIt)
                    {
                        ++tmpIt;
                    }
                    while (tmpIt and not includeIfTrue (*tmpIt))
                    {
                        ++tmpIt;
                    }
                    return tmpIt ?* tmpIt : Optional<T> ();
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            template    <typename RESULT_CONTAINER>
            inline  RESULT_CONTAINER    Iterable<T>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
            {
                return Where<RESULT_CONTAINER> (includeIfTrue, RESULT_CONTAINER {});
            }
            template    <typename T>
            template    <typename RESULT_CONTAINER>
            RESULT_CONTAINER    Iterable<T>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue, const RESULT_CONTAINER& emptyResult) const
            {
                Require (emptyResult.empty ());
                RESULT_CONTAINER    result  =   emptyResult;
                for (auto i : Where (includeIfTrue)) {
                    result.Add (i);
                }
                return result;
            }
            template    <typename T>
            Iterable<T>     Iterable<T>::Distinct () const
            {
                using   Memory::Optional;
                set<T>   t1 (begin (), end ());     // Somewhat simplistic/stupid/weak implementation
                vector<T>   tmp (t1.begin (), t1.end ());
                size_t idx { 0 };
                function<Optional<T>()> getNext = [tmp, idx] () mutable -> Optional<T> {
                    if (idx < tmp.size ())
                    {
                        return tmp[idx++];
                    }
                    else {
                        return Optional<T> ();
                    }
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            template    <typename RESULT>
            Iterable<RESULT>     Iterable<T>::Distinct (const function<RESULT(ArgByValueType<T>)>& extractElt) const
            {
                using   Memory::Optional;
                set<RESULT>   t1;
                for (T i : *this) {
                    t1.add (extractElt (i));
                }
                vector<RESULT>  tmp (t1.begin (), t1.end ());
                size_t idx { 0 };
                function<Optional<RESULT>()> getNext = [tmp, idx] () mutable -> Optional<RESULT> {
                    if (idx < tmp.size ())
                    {
                        return tmp[idx++];
                    }
                    else {
                        return Optional<T> ();
                    }
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            template    <typename   T1, typename RESULT>
            Iterable<RESULT>    Iterable<T>::Select (const function<T1(const T&)>& extract1) const
            {
                using   Memory::Optional;
                Iterator<T> tmpIt { this->MakeIterator () };
                function<Optional<RESULT>()> getNext = [tmpIt, extract1] () mutable -> Memory::Optional<RESULT> {
                    if (tmpIt)
                    {
                        return RESULT (extract1 (*tmpIt++));
                    }
                    return Optional<RESULT> ();
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            template    <typename   T1, typename   T2, typename RESULT>
            Iterable<RESULT>    Iterable<T>::Select (const function<T1(const T&)>& extract1, const function<T2(const T&)>& extract2) const
            {
                using   Memory::Optional;
                Iterator<T> tmpIt { this->MakeIterator () };
                function<Optional<RESULT>()> getNext = [tmpIt, extract1, extract2] () mutable -> Memory::Optional<RESULT> {
                    if (tmpIt)
                    {
                        RESULT result { extract1 (*tmpIt), extract2 (*tmpIt) };
                        tmpIt++;
                        return result;
                    }
                    return Optional<RESULT> ();
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            template    <typename   T1, typename   T2, typename   T3, typename RESULT>
            Iterable<RESULT>    Iterable<T>::Select (const function<T1(const T&)>& extract1, const function<T2(const T&)>& extract2, const function<T3(const T&)>& extract3) const
            {
                using   Memory::Optional;
                Iterator<T> tmpIt { this->MakeIterator () };
                function<Optional<RESULT>()> getNext = [tmpIt, extract1, extract2, extract3] () mutable -> Memory::Optional<RESULT> {
                    if (tmpIt)
                    {
                        RESULT result { extract1 (*tmpIt), extract2 (*tmpIt), extract3 (*tmpIt) };
                        tmpIt++;
                        return result;
                    }
                    return Optional<RESULT> ();
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            Iterable<T> Iterable<T>::Skip (size_t nItems) const
            {
                using   Memory::Optional;
                size_t  nItemsToSkip = nItems;
                Iterator<T> tmpIt { this->MakeIterator () };
                function<Optional<T>()> getNext = [tmpIt, nItemsToSkip] () mutable -> Memory::Optional<T> {
                    while (tmpIt and nItemsToSkip > 0)
                    {
                        nItemsToSkip--;
                        ++tmpIt;
                    }
                    return tmpIt ?* tmpIt++ : Optional<T> ();
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            Iterable<T> Iterable<T>::Take (size_t nItems) const
            {
                using   Memory::Optional;
                size_t  nItemsToTake = nItems;
                Iterator<T> tmpIt { this->MakeIterator () };
                function<Optional<T>()> getNext = [tmpIt, nItemsToTake] () mutable -> Memory::Optional<T> {
                    if (nItemsToTake == 0)
                    {
                        return Optional<T> ();
                    }
                    nItemsToTake--;
                    return tmpIt ?* tmpIt++ : Optional<T> ();
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            Iterable<T> Iterable<T>::OrderBy (const function<bool(T, T)>& compare) const
            {
                using   Memory::Optional;
                vector<T>   tmp (begin (), end ());     // Somewhat simplistic implementation
#if     qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy
                sort (tmp.begin (), tmp.end (), [compare] (const T & l, const T & r) -> bool { return compare (l, r); });
#else
                sort (tmp.begin (), tmp.end (), compare);
#endif
                size_t idx { 0 };
                function<Optional<T>()> getNext = [tmp, idx] () mutable -> Optional<T> {
                    if (idx < tmp.size ())
                    {
                        return tmp[idx++];
                    }
                    else {
                        return Optional<T> ();
                    }
                };
                return CreateGenerator (getNext);
            }
            template    <typename T>
            T   Iterable<T>::Min () const
            {
                using   Memory::Optional;
                Optional<T> m;
                for (T i : *this) {
                    if (m) {
                        m = min (i, *m);
                    }
                    else {
                        m = i;
                    }
                }
                if (m) {
                    return *m;
                }
                else {
                    throw range_error ("Cannot take Min() on empty list");
                }
            }
            template    <typename T>
            T   Iterable<T>::Max () const
            {
                using   Memory::Optional;
                Optional<T> m;
                for (T i : *this) {
                    if (m) {
                        m = max (i, *m);
                    }
                    else {
                        m = i;
                    }
                }
                if (m) {
                    return *m;
                }
                else {
                    throw range_error ("Cannot take Max() on empty list");
                }
            }
            template    <typename T>
            template    <typename   RESULT_TYPE>
            RESULT_TYPE   Iterable<T>::Mean () const
            {
                using   Memory::Optional;
                Optional<RESULT_TYPE> m;
                unsigned int count {};
                for (T i : *this) {
                    if (m) {
                        m = *m + i;
                    }
                    else {
                        m = i;
                    }
                    ++count;
                }
                if (m) {
                    return *m / count;
                }
                else {
                    throw range_error ("Cannot take Mean() on empty list");
                }
            }
            template    <typename T>
            template    <typename   RESULT_TYPE>
            RESULT_TYPE   Iterable<T>::Median (const function<bool(T, T)>& compare
#if     qCompilerAndStdLib_TemplateDiffersOnReturnTypeOnly_Buggy
                                               , RESULT_TYPE ignored
#endif
                                              ) const
            {
                vector<T>   tmp (begin (), end ());     // Somewhat simplistic implementation
#if     qCompilerAndStdLib_TemplateCompareIndirectionLevelCPP14_Buggy
                sort (tmp.begin (), tmp.end (), [compare] (const T & l, const T & r) -> bool { return compare (l, r); });
#else
                sort (tmp.begin (), tmp.end (), compare);
#endif
                size_t  sz { tmp.size () };
                if (sz == 0) {
                    throw range_error ("Cannot take Median () on empty list");
                }
                if ((sz % 2) == 0) {
                    return (static_cast<RESULT_TYPE> (tmp[sz / 2])  + static_cast<RESULT_TYPE> (tmp[sz / 2 - 1])) / static_cast<RESULT_TYPE> (2);
                }
                else {
                    return tmp[sz / 2];
                }
            }
            template    <typename T>
            inline  bool    Iterable<T>::empty () const
            {
                return IsEmpty ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::Any () const
            {
                return not IsEmpty ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::Any (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
            {
                return not Where (includeIfTrue).IsEmpty ();
            }
            template    <typename T>
            inline  size_t  Iterable<T>::length () const
            {
                return GetLength ();
            }
            template    <typename T>
            inline  size_t  Iterable<T>::size () const
            {
                return GetLength ();
            }
            template    <typename T>
            inline  Iterator<T> Iterable<T>::begin () const
            {
                return MakeIterator ();
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::end ()
            {
                return Iterator<T>::GetEmptyIterator ();
            }
            template    <typename T>
            inline  void    Iterable<T>::Apply (const function<void(ArgByValueType<T> item)>& doToElement) const
            {
                RequireNotNull (doToElement);
                _SafeReadRepAccessor<> { this } ._ConstGetRep ().Apply (doToElement);
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::FindFirstThat (const function<bool(ArgByValueType<T> item)>& doToElement) const
            {
                RequireNotNull (doToElement);
                return _SafeReadRepAccessor<> { this } ._ConstGetRep ().FindFirstThat (doToElement, this);
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::FindFirstThat (const Iterator<T>& startAt, const function<bool(ArgByValueType<T> item)>& doToElement) const
            {
                for (Iterator<T> i = startAt; i != Iterable<T>::end (); ++i) {
                    if ((doToElement) (*i)) {
                        return i;
                    }
                }
                return end ();
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            CONTAINER_OF_T    Iterable<T>::As () const
            {
                return CONTAINER_OF_T (begin (), end ());
            }
#if 0
            template    <typename T>
            inline  typename Iterable<T>::_ReadOnlyIterableIRepReference   Iterable<T>::_GetReadOnlyIterableIRepReference () const
            {
                return _ReadOnlyIterableIRepReference (_fRep);
            }
#endif


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Iterable_inl_ */
