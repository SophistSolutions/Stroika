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
            inline  Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_SafeReadRepAccessor (const Iterable<T>& s)
                : fAccessor (s._GetReadOnlyIterableIRepReference ())
            {
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  const REP_SUB_TYPE&    Iterable<T>::_SafeReadRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
            {
                EnsureMember (fAccessor.get (), REP_SUB_TYPE);
                return static_cast<const REP_SUB_TYPE&> (*fAccessor.get ());   // static cast for performance sake - dynamic cast in Ensure
            }


            /*
             ********************************************************************************
             *************** Iterable<T>::_SafeReadWriteRepAccessor *************************
             ********************************************************************************
             */
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_SafeReadWriteRepAccessor (Iterable<T>* iterableEnvelope)
                : fAccessor (iterableEnvelope->fRep_)
                , fIterableEnvelope (iterableEnvelope)
            {
                RequireNotNull (iterableEnvelope);
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::~_SafeReadWriteRepAccessor ()
            {
                // @todo - CAREFUL ABOUT EXCEPTIONS HERE!
                //
                // Not as bad as it looks, since SharedByValue<>::operator= checks for no pointer change and does nothing
                fIterableEnvelope->fRep_ = fAccessor;
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  const REP_SUB_TYPE&    Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_ConstGetRep () const
            {
                EnsureMember (fAccessor.get (), REP_SUB_TYPE);
                return static_cast<const REP_SUB_TYPE&> (*fAccessor.get ());   // static cast for performance sake - dynamic cast in Ensure
            }
            template    <typename T>
            template <typename REP_SUB_TYPE>
            inline  REP_SUB_TYPE&    Iterable<T>::_SafeReadWriteRepAccessor<REP_SUB_TYPE>::_GetWriteableRep ()
            {
                // NOT QUITE RIGHT - THIS COPIES TOO AGGRESSIVELY. DONT COPY IF NO NEED!!!
                REP_SUB_TYPE*   r   =   static_cast<REP_SUB_TYPE*> (fAccessor.get (fIterableEnvelope));   // static cast for performance sake - dynamic cast in Ensure
                EnsureMember (r, REP_SUB_TYPE);
                return *r;
            }


            /*
             ********************************************************************************
             ********************************** Iterable<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Iterable<T>::Iterable (const _SharedPtrIRep& rep) noexcept
:
            fRep_ (rep)
            {
                Require (fRep_.GetSharingState () != Memory::SharedByValue_State::eNull);
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (const Iterable<T>& from) noexcept
:
            fRep_ (from.fRep_)
            {
                Require (fRep_.GetSharingState () != Memory::SharedByValue_State::eNull);
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (Iterable<T>&& from) noexcept
:
            fRep_ (std::move (from.fRep_))
            {
                Require (fRep_.GetSharingState () != Memory::SharedByValue_State::eNull);
#if     !qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr || qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
                Require (from.fRep_ == nullptr);    // after move
#endif
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (_SharedPtrIRep&& rep) noexcept
:
            fRep_ (std::move (rep))
            {
                Require (fRep_.GetSharingState () != Memory::SharedByValue_State::eNull);
#if     !qStroika_Foundation_Traveral_IterableUsesStroikaSharedPtr || qStroika_Foundation_Memory_SharedPtrSupportsRValueReferences_
                Require (rep == nullptr);   // after move
#endif
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (const Iterable<T>& rhs)
            {
                RequireNotNull (rhs.fRep_.get ());
                fRep_ = rhs.fRep_;
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
                return fRep_.GetSharingState ();
            }
            template    <typename T>
            inline  typename Iterable<T>::_IRep&         Iterable<T>::_GetRep ()
            {
                //EnsureNotNull (fRep_.get ());
                // subtle - but we must use the get () overload that allows passing in our Copy forward parameters
                // instead of using operator* (which doesnt allow passsing in the copy forward paramters).
                //
                // Note - our copy forward paramters are the container envelope pointer
                return *(fRep_.get (this));
            }
            template    <typename T>
            inline  const typename Iterable<T>::_IRep&   Iterable<T>::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            template    <typename T>
            inline  const typename Iterable<T>::_IRep&   Iterable<T>::_ConstGetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            template    <typename T>
            inline  Iterator<T>     Iterable<T>::MakeIterator () const
            {
#if 1
                // EXPERIMENTAL THREAD SAFETY SUPPORT
                return _SafeReadRepAccessor<> (*this)._ConstGetRep ().MakeIterator (this);
#else
                return _GetRep ().MakeIterator (this);
#endif
            }
            template    <typename T>
            inline  size_t  Iterable<T>::GetLength () const
            {
#if 1
                // EXPERIMENTAL THREAD SAFETY SUPPORT
                return _SafeReadRepAccessor<> (*this)._ConstGetRep ().GetLength ();
#else
                return _GetRep ().GetLength ();
#endif
            }
            template    <typename T>
            inline  bool    Iterable<T>::IsEmpty () const
            {
#if 1
                // EXPERIMENTAL THREAD SAFETY SUPPORT
                return _SafeReadRepAccessor<> (*this)._ConstGetRep ().IsEmpty ();
#else
                return _GetRep ().IsEmpty ();
#endif
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
#if 1
                // EXPERIMENTAL THREAD SAFETY SUPPORT
                _GetReadOnlyIterableIRepReference ().get ()->Apply (doToElement);
#else
                _GetRep ().Apply (doToElement);
#endif
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::FindFirstThat (const function<bool(const T& item)>& doToElement) const
            {
                RequireNotNull (doToElement);
#if 1
                // EXPERIMENTAL THREAD SAFETY SUPPORT
                return _GetReadOnlyIterableIRepReference ().get ()->FindFirstThat (doToElement, this);;
#else
                return _GetRep ().FindFirstThat (doToElement, this);
#endif
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
                return _ReadOnlyIterableIRepReference (fRep_);
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Iterable_inl_ */
