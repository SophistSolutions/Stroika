/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterable_inl_
#define _Stroika_Foundation_Traversal_Iterable_inl_

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
            inline  Iterable<T>::_IRep::_IRep ()
            {
            }
            template    <typename T>
            inline  Iterable<T>::_IRep::~_IRep ()
            {
            }
            template    <typename T>
            inline  bool    Iterable<T>::_IRep::_IsEmpty () const
            {
                return GetLength () == 0;
            }
            template    <typename T>
            inline  void    Iterable<T>::_IRep::_Apply (_APPLY_ARGTYPE doToElement) const
            {
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                RequireNotNull (doToElement);
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                RequireNotNull (doToElement.second);
#endif
                for (Iterator<T> i = MakeIterator (); i != Iterable<T>::end (); ++i) {
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                    (doToElement) (*i);
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                    (doToElement.second) (doToElement.first, *i);
#else
                    AssertNotImplemented ();
#endif
                }
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::_IRep::_ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
            {
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                RequireNotNull (doToElement);
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                RequireNotNull (doToElement.second);
#endif
                for (Iterator<T> i = MakeIterator (); i != Iterable<T>::end (); ++i) {
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                    if ((doToElement) (*i)) {
                        return i;
                    }
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                    if ((doToElement.second) (doToElement.first, *i)) {
                        return i;
                    }
#else
                    AssertNotImplemented ();
#endif
                }
                return end ();
            }





            /*
             ********************************************************************************
             ********************************** Iterable<T> *********************************
             ********************************************************************************
             */
            template    <typename T>
            inline  Iterable<T>::Iterable (const _SharedPtrIRep& rep)
                : fRep_ (rep)
            {
                RequireNotNull (fRep_.get ());
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (const Iterable<T>& from)
                : fRep_ (from.fRep_)
            {
                RequireNotNull (fRep_.get ());
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (Iterable<T>&& from)
                : fRep_ (std::move (from.fRep_))
            {
                RequireNotNull (fRep_.get ());
            }
            template    <typename T>
            inline  Iterable<T>::Iterable (_SharedPtrIRep&& rep)
                : fRep_ (std::move (rep))
            {
                RequireNotNull (fRep_.get ());
            }
            template    <typename T>
            inline  Iterable<T>::~Iterable ()
            {
            }
            template    <typename T>
            inline  Iterable<T>&    Iterable<T>::operator= (const Iterable<T>& rhs)
            {
                RequireNotNull (rhs.fRep_.get ());
                fRep_ = rhs.fRep_;
                return *this;
            }
            template    <typename T>
            inline  typename Iterable<T>::_SharedPtrIRep  Iterable<T>::Clone_ (const typename Iterable<T>::_IRep& rep)
            {
                return (rep.Clone ());
            }
            template    <typename T>
            inline  typename Iterable<T>::_IRep&         Iterable<T>::_GetRep ()
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            template    <typename T>
            inline  const typename Iterable<T>::_IRep&   Iterable<T>::_GetRep () const
            {
                EnsureNotNull (fRep_.get ());
                return *fRep_;
            }
            template    <typename T>
            inline  typename Iterable<T>::_SharedPtrIRep   Iterable<T>::_GetSharedPtrIRep () const
            {
                EnsureNotNull (fRep_.get ());
                return fRep_.GetSharedPtr ();
            }
            template    <typename T>
            inline  Iterator<T>     Iterable<T>::MakeIterator () const
            {
                return _GetRep ().MakeIterator ();
            }
            template    <typename T>
            inline  size_t  Iterable<T>::GetLength () const
            {
                return _GetRep ().GetLength ();
            }
            template    <typename T>
            inline  bool    Iterable<T>::IsEmpty () const
            {
                return _GetRep ().IsEmpty ();
            }
            template    <typename T>
            template    <typename EQUALS_COMPARER>
            bool    Iterable<T>::Contains (const T& element) const
            {
                for (T i : *this) {
                    if (typename EQUALS_COMPARER::Equals (i, element)) {
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
                        if (typename EQUALS_COMPARER::Equals (ti, ri)) {
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
                        if (typename EQUALS_COMPARER::Equals (ti, ri)) {
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
            bool    Iterable<T>::TallyEquals (const RHS_CONTAINER_TYPE& rhs) const
            {
                AssertNotImplemented ();
                return false;
            }
            template    <typename T>
            template    <typename RHS_CONTAINER_TYPE, typename EQUALS_COMPARER>
            bool    Iterable<T>::ExactEquals (const RHS_CONTAINER_TYPE& rhs) const
            {
                Iterator<T> li  =   MakeIterator ();
                Iterator<T> le  =   end ();
                auto        ri  =   rhs.begin ();
                auto        re  =   rhs.end ();
                for (; li != le and ri != re; ++ri, ++li) {
                    if (not typename EQUALS_COMPARER::Equals (*li, *ri)) {
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
            inline  void    Iterable<T>::ApplyStatic (void (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                _GetRep ().Apply (doToElement);
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                struct CheapLambda_ {
                    CheapLambda_ (void (*doToElement) (const T& item))
                        : fToDoItem (doToElement)
                    {
                    }
                    void (*fToDoItem) (const T& item);
                    static void DoToItem (const void* cookie, const T& item)
                    {
                        (reinterpret_cast<const CheapLambda_*> (cookie)->fToDoItem) (item);
                    }
                };
                CheapLambda_    tmp =   CheapLambda_(doToElement);
                _GetRep ().Apply (typename Iterable<T>::_IRep::_APPLY_ARGTYPE (&tmp, &CheapLambda_::DoToItem));
#endif
            }
            template    <typename T>
            inline  void    Iterable<T>::Apply (const std::function<void(const T& item)>& doToElement) const
            {
                RequireNotNull (doToElement);
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                _GetRep ().Apply (doToElement);
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                struct CheapLambda_ {
                    CheapLambda_ (const std::function<void(const T& item)>& doToElement)
                        : fDoToItem (doToElement)
                    {
                    }
                    const std::function<void(const T& item)>& fDoToItem;
                    static void DoToItem (const void* cookie, const T& item)
                    {
                        (reinterpret_cast<const CheapLambda_*> (cookie)->fDoToItem) (item);
                    }
                };
                return _GetRep ().Apply (typename _IRep::_APPLY_ARGTYPE (&CheapLambda_ (doToElement), &CheapLambda_::DoToItem));
#endif
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::ApplyUntilTrue (const std::function<bool(const T& item)>& doToElement) const
            {
                RequireNotNull (doToElement);
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                return _GetRep ().ApplyUntilTrue (doToElement);
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                struct CheapLambda_ {
                    CheapLambda_ (bool (*doToElement) (const T& item))
                        : fToDoItem (doToElement)
                    {
                    }
                    bool (*fToDoItem) (const T& item);
                    static bool DoToItem (const void* cookie, const T& item)
                    {
                        return (reinterpret_cast<const CheapLambda_*> (cookie)->fToDoItem) (item);
                    }
                };
                return _GetRep ().Apply (typename Iterable<T>::_IRep::_APPLYUNTIL_ARGTYPE (&CheapLambda_ (doToElement), &CheapLambda_::DoToItem));
#endif
            }
            template    <typename T>
            inline  Iterator<T>    Iterable<T>::ApplyUntilTrueStatic (bool (*doToElement) (const T& item)) const
            {
                RequireNotNull (doToElement);
#if     qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_STDFUNCTION
                return _GetRep ().ApplyUntilTrue (doToElement);
#elif   qAPPLY_IMPL_STRATEGY==qAPPLY_IMPL_STRATEGY_COOKIE
                struct CheapLambda_ {
                    CheapLambda_ (bool (*doToElement) (const T& item))
                        : fToDoItem (doToElement)
                    {
                    }
                    bool (*fToDoItem) (const T& item);
                    static bool DoToItem (const void* cookie, const T& item)
                    {
                        return (reinterpret_cast<const CheapLambda_*> (cookie)->fToDoItem) (item);
                    }
                };
                return _GetRep ().Apply (typename Iterable<T>::_IRep::_APPLYUNTIL_ARGTYPE (&CheapLambda_ (doToElement), &CheapLambda_::DoToItem));
#endif
            }
            template    <typename T>
            template    <typename CONTAINER_OF_T>
            CONTAINER_OF_T    Iterable<T>::As () const
            {
                return CONTAINER_OF_T (this->begin (), this->end ());
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_Iterable_inl_ */
