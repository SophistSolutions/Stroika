/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_inl_
#define _Stroika_Foundation_Traversal_Iterable_inl_


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"



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
             *************** Iterable<T>::_SafeReadRepAccessor ******************************
             ********************************************************************************
             */
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const Iterable<T>* it)
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                : fAccessor_ (it->_GetReadOnlyIterableIRepReference ())
#else
                : fConstRef_ (*static_cast<const REP_SUB_TYPE*> (it->_fRep.cget ()))
#endif
            {
                RequireNotNull (it);
#if     !qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                EnsureMember (&fConstRef_, REP_SUB_TYPE);
#endif
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  const REP_SUB_TYPE&    Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
            {
#if     qStroika_Foundation_Traveral_Iterator_SafeRepAccessorIsSafe_
                EnsureMember (fAccessor_.get (), REP_SUB_TYPE);
                return static_cast<const REP_SUB_TYPE&> (*fAccessor_.get ());   // static cast for performance sake - dynamic cast in Ensure
#else
                EnsureMember (&fConstRef_, REP_SUB_TYPE);
                return fConstRef_;
#endif
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
#if     !qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr || qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
                Require (from._fRep == nullptr);    // after move
#endif
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (_SharedPtrIRep&& rep) noexcept
:
            _fRep (std::move (rep))
            {
                Require (_fRep.GetSharingState () != Memory::SharedByValue_State::eNull);
#if     !qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr || qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
                Require (rep == nullptr);   // after move
#endif
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (const Iterable<T>& rhs)
            {
                RequireNotNull (rhs._fRep.get ());
                _fRep = rhs._fRep;
                return *this;
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (Iterable<T>&& rhs)
            {
                RequireNotNull (rhs._fRep.get ());
                _fRep = move (rhs._fRep);
                return *this;
            }
            template    <typename T>
            inline  typename Iterable<T>::_SharedPtrIRep  Iterable<T>::Clone_ (const _IRep& rep, IteratorOwnerID forIterableEnvelope)
            {
                return rep.Clone (forIterableEnvelope);
            }
            template    <typename T>
            inline  Memory::SharedByValue_State     Iterable<T>::_GetSharingState () const
            {
                return _fRep.GetSharingState ();
            }
            template    <typename T>
            inline  const typename Iterable<T>::_IRep&   Iterable<T>::_ConstGetRep () const
            {
                EnsureNotNull (_fRep.get ());
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
            bool    Iterable<T>::Contains (const T& element) const
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
            bool    Iterable<T>::ExactEquals (const RHS_CONTAINER_TYPE& rhs) const
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
            inline  bool    Iterable<T>::empty () const
            {
                return IsEmpty ();
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
                return (Iterator<T>::GetEmptyIterator ());
            }
            template    <typename T>
            inline  void    Iterable<T>::Apply (const function<void(const T& item)>& doToElement) const
            {
                RequireNotNull (doToElement);
                _SafeReadRepAccessor<> { this } ._ConstGetRep ().Apply (doToElement);
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::FindFirstThat (const function<bool(const T& item)>& doToElement) const
            {
                RequireNotNull (doToElement);
                return _SafeReadRepAccessor<> { this } ._ConstGetRep ().FindFirstThat (doToElement, this);
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            CONTAINER_OF_T    Iterable<T>::As () const
            {
                return CONTAINER_OF_T (this->begin (), this->end ());
            }
            template    <typename T>
            inline  typename Iterable<T>::_ReadOnlyIterableIRepReference   Iterable<T>::_GetReadOnlyIterableIRepReference () const
            {
                return _ReadOnlyIterableIRepReference (_fRep);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Iterable_inl_ */
